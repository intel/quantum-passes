//===- QuantumLoopUnrolling.cpp ------------------------------------------====//
//
// Copyright (C) 2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//
//===----------------------------------------------------------------------===//
//
// This file implements a loop pass which performs loop unrolling and resolvable
// branching only for quantum kernel. The pass takes a subset of the base loop
// unrolling pass that is defined in LoopUnrollPass.cpp and adds check if the
// parent function is a quantum function before unrolling.
//
//===----------------------------------------------------------------------===//

#include "llvm/Analysis/AssumptionCache.h"
#include "llvm/Analysis/CodeMetrics.h"
#include "llvm/Analysis/LoopAnalysisManager.h"
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/Analysis/LoopPass.h"
#include "llvm/Analysis/OptimizationRemarkEmitter.h"
#include "llvm/Analysis/ScalarEvolution.h"
#include "llvm/Analysis/TargetTransformInfo.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Dominators.h"
#include "llvm/IR/Function.h"
#include "llvm/InitializePasses.h"
#include "llvm/Pass.h"
#include "llvm/Support/Casting.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Transforms/Scalar.h"
#include "llvm/Transforms/Scalar/LoopPassManager.h"
#include "llvm/Transforms/Utils.h"
#include "llvm/Transforms/Utils/LoopSimplify.h"
#include "llvm/Transforms/Utils/LoopUtils.h"
#include "llvm/Transforms/Utils/SizeOpts.h"
#include "llvm/Transforms/Utils/UnrollLoop.h"
#include <optional>

// for isQkernel() check
#include "IntelQuantumPasses/InitializeQuantumPasses.h"
#include "IntelQuantumPasses/QuantumUtils/QuantumFLEQUtils.h"
#include "IntelQuantumPasses/QuantumUtils/QuantumGeneralUtils.h"

#include "IntelQuantumPasses/QuantumCompile/QuantumLoopUnrolling.h"
#include "llvm/Transforms/Utils/LoopPeel.h"

using namespace llvm;

#define DEBUG_TYPE "q-loop-unroll"

// The pass borrows directly from the base LoopUrollPass.cpp with an added
// check if the parent function is a quantum kernel function. It does not
// call tryToUnrollLoop() from that pass directly as file as that is a
// static function, but the functionality is identical as the code below
// is a subset of that function/pass.
static LoopUnrollResult
tryToRun(Loop *L, DominatorTree &DT, LoopInfo *LI, ScalarEvolution &SE,
         const TargetTransformInfo &TTI, AssumptionCache &AC,
         OptimizationRemarkEmitter &ORE, BlockFrequencyInfo *BFI,
         ProfileSummaryInfo *PSI, bool PreserveLCSSA, int OptLevel,
         bool OnlyWhenForced, bool ForgetAllSCEV,
         std::optional<unsigned> ProvidedCount,
         std::optional<unsigned> ProvidedThreshold,
         std::optional<bool> ProvidedAllowPartial,
         std::optional<bool> ProvidedRuntime,
         std::optional<bool> ProvidedUpperBound,
         std::optional<bool> ProvidedAllowPeeling,
         std::optional<bool> ProvidedAllowProfileBasedPeeling,
         std::optional<unsigned> ProvidedFullUnrollMaxCount) {

  Function &F = *L->getHeader()->getParent();

  if (isQKernel(F) || llvm::fleq::isQExprFunc(&F)) {

    // These parameters are the same ones that are passed into
    // tryToUnrollLoop in the base unrolling pass

    if (!L->isLoopSimplifyForm()) {
      LLVM_DEBUG(
          dbgs()
          << "  Not unrolling loop which is not in loop-simplify form.\n");
      errs() << "  Not unrolling loop which is not in loop-simplify form.\n";
      return LoopUnrollResult::Unmodified;
    }

    // The definition of gatherUnrollingPreferences and other functions
    // used to get values for the UnrollLoop call are foudn in the base
    // loop unroll pass file
    unsigned NumInlineCandidates;
    bool NotDuplicatable;
    bool Convergent;
    TargetTransformInfo::PeelingPreferences PP = gatherPeelingPreferences(
        L, SE, TTI, ProvidedAllowPeeling, ProvidedAllowProfileBasedPeeling);
    TargetTransformInfo::UnrollingPreferences UP = gatherUnrollingPreferences(
        L, SE, TTI, nullptr, nullptr, ORE, OptLevel, ProvidedThreshold,
        ProvidedCount, ProvidedAllowPartial, ProvidedRuntime,
        ProvidedUpperBound, ProvidedFullUnrollMaxCount);

    // Hard-coding to match the value set in the quantum compiler script
    UP.Threshold = 100000000;

    SmallPtrSet<const Value *, 32> EphValues;
    CodeMetrics::collectEphemeralValues(L, &AC, EphValues);

    llvm::InstructionCost LoopSizeIC =
        ApproximateLoopSize(L, NumInlineCandidates, NotDuplicatable, Convergent,
                            TTI, EphValues, UP.BEInsns);
    unsigned LoopSize = *LoopSizeIC.getValue();
    LLVM_DEBUG(dbgs() << "  Loop Size = " << LoopSize << "\n");
    if (NotDuplicatable) {
      LLVM_DEBUG(
          dbgs() << "  Not unrolling loop which contains non-duplicatable"
                 << " instructions.\n");
      errs() << "  Not unrolling loop which contains non-duplicatable"
             << " instructions.\n";
      return LoopUnrollResult::Unmodified;
      ;
    }

    if (NumInlineCandidates != 0) {
      LLVM_DEBUG(dbgs() << "  Not unrolling loop with inlinable calls.\n");
      errs() << "  Not unrolling loop with inlinable calls.\n";
      return LoopUnrollResult::Unmodified;
      ;
    }

    // Find trip count and trip multiple if count is not available
    unsigned TripCount = 0;
    unsigned TripMultiple = 1;
    // If there are multiple exiting blocks but one of them is the latch, use
    // the latch for the trip count estimation. Otherwise insist on a single
    // exiting block for the trip count estimation.
    BasicBlock *ExitingBlock = L->getLoopLatch();
    if (!ExitingBlock || !L->isLoopExiting(ExitingBlock))
      ExitingBlock = L->getExitingBlock();
    if (ExitingBlock) {
      TripCount = SE.getSmallConstantTripCount(L, ExitingBlock);
      TripMultiple = SE.getSmallConstantTripMultiple(L, ExitingBlock);
    }

    // Try to find the trip count upper bound if we cannot find the exact trip
    // count.
    unsigned MaxTripCount = 0;
    bool MaxOrZero = false;
    if (!TripCount) {
      MaxTripCount = SE.getSmallConstantMaxTripCount(L);
      MaxOrZero = SE.isBackedgeTakenCountMaxOrZero(L);
    }

    // computeUnrollCount() decides whether it is beneficial to use upper bound
    // to fully unroll the loop.
    bool UseUpperBound = false;
    bool IsCountSetExplicitly = computeUnrollCount(
        L, TTI, DT, LI, &AC, SE, EphValues, &ORE, TripCount, MaxTripCount,
        MaxOrZero, TripMultiple, LoopSize, UP, PP, UseUpperBound);

    // Do not unroll if conputeUnrollCount returns a 0 for Count
    if (!UP.Count) {
      return LoopUnrollResult::Unmodified;
    }
    // Unroll factor (Count) must be less or equal to TripCount.
    if (TripCount && UP.Count > TripCount)
      UP.Count = TripCount;

    // Save loop properties before it is transformed.
    MDNode *OrigLoopID = L->getLoopID();

    // Unroll the loop.
    Loop *RemainderLoop = nullptr;
    LoopUnrollResult UnrollResult = UnrollLoop(
        L,
        {UP.Count, UP.Force, UP.Runtime, UP.AllowExpensiveTripCount,
         UP.UnrollRemainder, ForgetAllSCEV},
        LI, &SE, &DT, &AC, &TTI, &ORE, PreserveLCSSA, &RemainderLoop);
    if (UnrollResult == LoopUnrollResult::Unmodified) {
      return LoopUnrollResult::Unmodified;
    }

    if (RemainderLoop) {
      errs() << "Remainder Loop\n";
      std::optional<MDNode *> RemainderLoopID =
          makeFollowupLoopID(OrigLoopID, {LLVMLoopUnrollFollowupAll,
                                          LLVMLoopUnrollFollowupRemainder});
      if (RemainderLoopID.has_value())
        RemainderLoop->setLoopID(RemainderLoopID.value());
    }
    if (UnrollResult != LoopUnrollResult::FullyUnrolled) {
      errs() << "NewLoopID Loop\n";
      std::optional<MDNode *> NewLoopID =
          makeFollowupLoopID(OrigLoopID, {LLVMLoopUnrollFollowupAll,
                                          LLVMLoopUnrollFollowupUnrolled});
      if (NewLoopID.has_value()) {
        L->setLoopID(NewLoopID.value());
      }
    }

    if (UnrollResult == LoopUnrollResult::FullyUnrolled)
      return LoopUnrollResult::FullyUnrolled;

    return LoopUnrollResult::Unmodified;
  }

  // Return false if parent function is not a quantum function
  return LoopUnrollResult::Unmodified;
}

namespace {
class QuantumLoopUnrollLegacyPass : public LoopPass {

public:
  static char ID;
  QuantumModule &QM = QuantumCompilerLinkageLegacyPass::QM;

  int OptLevel;

  bool OnlyWhenForced;

  bool ForgetAllSCEV;

  std::optional<unsigned> ProvidedCount;
  std::optional<unsigned> ProvidedThreshold;
  std::optional<bool> ProvidedAllowPartial;
  std::optional<bool> ProvidedRuntime;
  std::optional<bool> ProvidedUpperBound;
  std::optional<bool> ProvidedAllowPeeling;
  std::optional<bool> ProvidedAllowProfileBasedPeeling;
  std::optional<unsigned> ProvidedFullUnrollMaxCount;

  QuantumLoopUnrollLegacyPass(
      int OptLevel = 2, bool OnlyWhenForced = false, bool ForgetAllSCEV = false,
      std::optional<unsigned> Threshold = {},
      std::optional<unsigned> Count = {},
      std::optional<bool> AllowPartial = false,
      std::optional<bool> Runtime = false,
      std::optional<bool> UpperBound = false,
      std::optional<bool> AllowPeeling = false,
      std::optional<bool> AllowProfileBasedPeeling = {},
      std::optional<unsigned> ProvidedFullUnrollMaxCount = {})
      : LoopPass(ID), OptLevel(OptLevel), OnlyWhenForced(OnlyWhenForced),
        ForgetAllSCEV(ForgetAllSCEV), ProvidedCount(std::move(Count)),
        ProvidedThreshold(Threshold), ProvidedAllowPartial(AllowPartial),
        ProvidedRuntime(Runtime), ProvidedUpperBound(UpperBound),
        ProvidedAllowPeeling(AllowPeeling),
        ProvidedAllowProfileBasedPeeling(AllowProfileBasedPeeling),
        ProvidedFullUnrollMaxCount(ProvidedFullUnrollMaxCount) {
    initializeQuantumLoopUnrollLegacyPassPass(*PassRegistry::getPassRegistry());
  }

  void getAnalysisUsage(AnalysisUsage &AU) const override {
    AU.addRequired<AssumptionCacheTracker>();
    AU.addRequired<TargetTransformInfoWrapperPass>();
    // FIXME: Loop passes are required to preserve domtree, and for now we just
    // recreate dom info if anything gets unrolled.
    getLoopAnalysisUsage(AU);
  }

  bool runOnLoop(Loop *L, LPPassManager &LPM) override {
    if (skipLoop(L))
      return false;

    Function &F = *L->getHeader()->getParent();

    auto &DT = getAnalysis<DominatorTreeWrapperPass>().getDomTree();
    LoopInfo *LI = &getAnalysis<LoopInfoWrapperPass>().getLoopInfo();
    ScalarEvolution &SE = getAnalysis<ScalarEvolutionWrapperPass>().getSE();
    const TargetTransformInfo &TTI =
        getAnalysis<TargetTransformInfoWrapperPass>().getTTI(F);
    auto &AC = getAnalysis<AssumptionCacheTracker>().getAssumptionCache(F);
    // For the old PM, we can't use OptimizationRemarkEmitter as an analysis
    // pass.  Function analyses need to be preserved across loop transformations
    // but ORE cannot be preserved (see comment before the pass definition).
    OptimizationRemarkEmitter ORE(&F);
    bool PreserveLCSSA = mustPreserveAnalysisID(LCSSAID);

    LoopUnrollResult Result =
        tryToRun(L, DT, LI, SE, TTI, AC, ORE, nullptr, nullptr, PreserveLCSSA,
                 OptLevel, OnlyWhenForced, ForgetAllSCEV, ProvidedCount,
                 ProvidedThreshold, ProvidedAllowPartial, ProvidedRuntime,
                 ProvidedUpperBound, ProvidedAllowPeeling,
                 ProvidedAllowProfileBasedPeeling, ProvidedFullUnrollMaxCount);
    if (Result == LoopUnrollResult::FullyUnrolled)
      LPM.markLoopAsDeleted(*L);

    return Result != LoopUnrollResult::Unmodified;
  }
};

} // namespace

char QuantumLoopUnrollLegacyPass::ID = 0;

INITIALIZE_PASS_BEGIN(QuantumLoopUnrollLegacyPass, "q-loop-unroll",
                      "Quantum Loop Unroll Pass", false, false)
INITIALIZE_PASS_DEPENDENCY(AssumptionCacheTracker)
INITIALIZE_PASS_DEPENDENCY(LoopPass)
INITIALIZE_PASS_DEPENDENCY(TargetTransformInfoWrapperPass)
INITIALIZE_PASS_END(QuantumLoopUnrollLegacyPass, "q-loop-unroll",
                    "Quantum Loop Unroll Pass", false, false)

PreservedAnalyses QuantumLoopUnrollPass::run(Loop &L, LoopAnalysisManager &AM,
                                             LoopStandardAnalysisResults &AR,
                                             LPMUpdater &Updater) {
  // For the new PM, we can't use OptimizationRemarkEmitter as an analysis
  // pass. Function analyses need to be preserved across loop transformations
  // but ORE cannot be preserved (see comment before the pass definition).
  OptimizationRemarkEmitter ORE(L.getHeader()->getParent());

  // Keep track of the previous loop structure so we can identify new loops
  // created by unrolling.
  Loop *ParentL = L.getParentLoop();
  Updater.setParentLoop(ParentL);
  SmallPtrSet<Loop *, 4> OldLoops;
  if (ParentL)
    OldLoops.insert(ParentL->begin(), ParentL->end());
  else
    OldLoops.insert(AR.LI.begin(), AR.LI.end());
  std::string LoopName = std::string(L.getName());

  bool Changed = tryToRun(&L, AR.DT, &AR.LI, AR.SE, AR.TTI, AR.AC, ORE,
                          /*BFI*/ nullptr, /*PSI*/ nullptr,
                          /*PreserveLCSSA*/ true, OptLevel, OnlyWhenForced,
                          ForgetSCEV, /*Count*/ std::nullopt,
                          /*Threshold*/ std::nullopt, /*AllowPartial*/ false,
                          /*Runtime*/ false, /*UpperBound*/ false,
                          /*AllowPeeling*/ true,
                          /*AllowProfileBasedPeeling*/ false,
                          /*FullUnrollMaxCount*/ std::nullopt) !=
                 LoopUnrollResult::Unmodified;
  if (!Changed)
    return PreservedAnalyses::all();

  // Included from LoopUnrollPass.cpp
  bool IsCurrentLoopValid = false;
  SmallVector<Loop *, 4> SibLoops;
  if (ParentL)
    SibLoops.append(ParentL->begin(), ParentL->end());
  else
    SibLoops.append(AR.LI.begin(), AR.LI.end());
  erase_if(SibLoops, [&](Loop *SibLoop) {
    if (SibLoop == &L) {
      IsCurrentLoopValid = true;
      return true;
    }

    // Otherwise erase the loop from the list if it was in the old loops.
    return OldLoops.contains(SibLoop);
  });
  Updater.addSiblingLoops(SibLoops);

  if (!IsCurrentLoopValid) {
    Updater.markLoopAsDeleted(L, LoopName);
  }

  return getLoopPassPreservedAnalyses();
}

#undef DEBUG_TYPE
