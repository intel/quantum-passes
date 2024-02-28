//===- FlattenQuantumKernel.cpp ------------------------------------------====//
//
// Copyright (C) 2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//
//===----------------------------------------------------------------------===//
//
// This file implements a module pass which inlines quantum kernel calls which
// are inside other quantum kernels.
//
//===----------------------------------------------------------------------===//

#include "IntelQuantumPasses/QuantumCompile/FlattenQuantumKernel.h"
#include "IntelQuantumPasses/InitializeQuantumPasses.h"
#include "llvm/Analysis/CallGraph.h"
#include "llvm/InitializePasses.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"

#define DEBUG_TYPE "flatten-module"

using namespace llvm;
using namespace aqcc;

static bool FlattenQuantumKernel(Module &M, CallGraph &CG) {
  InlineFunctionInfo InlineInfo;

  std::vector<CallBase *> CB_to_inline;

  // Iterate over all SCCs in the module in bottom-up order
  for (scc_iterator<CallGraph *> si = scc_begin(&CG), se = scc_end(&CG);
       si != se; ++si) {

    // we will check for loops and make sure QK are not an element of the loop
    // as this implies function regression in a way that makes no sense given
    // the current conditions on QKs.
    if (si.hasCycle()) {
      auto &scc = *si;
      for (size_t i = 0; i < scc.size(); i++) {
        Function *F = scc[i]->getFunction();
        if (!F)
          displayErrorAndExit("FlattenQuantumKernelPass", "Not a function");
        if (isQKernel(*F))
          displayErrorAndExit(
              "FlattenQuantumKernelPass",
              "Cannot have function recursion with quantum kernels: " +
                  DemangleQuantumKernelName(F->getName()));
      }
    } else {
      // guaranteed to only be one element of in the scc
      Function *F = si->front()->getFunction();

      if (!F)
        continue;
      if (isQKernel(*F)) {
        // check the user list for CallInst, then check its called function
        // to see if it is a QK

        // Check if this QK is passing in qbit by value
        if (passesQbitsByValue(*F))
          displayErrorAndExit(
              "FlattenQuantumKernelPass",
              "Error in " + DemangleQuantumKernelName(F->getName()) +
                  ". Quantum kernels cannot have qbit type as arguments. "
                  "Please use qbit& instead.");

        for (auto user : F->users()) {
          if (CallBase *CB = dyn_cast<CallBase>(user)) {
            Function *Caller = CB->getParent()->getParent();

            if (!Caller)
              displayErrorAndExit("FlattenQuantumKernelPass",
                                  "something went wrong...");

            if (isQKernel(*Caller) || fleq::isQExprFunc(Caller)) {
              CB_to_inline.push_back(CB);
            }
          }
        } // for loop end - function usage
      }   // isQKernel
    }     // else of hasLoop()
  }       // for loop end -scc

  // iterate and Inline elements of CB_to_inline
  for (size_t i = 0; i < CB_to_inline.size(); i++) {
    CallBase *CB = CB_to_inline[i];
    InlineResult result = InlineFunction(*CB, InlineInfo);
    if (!result.isSuccess()) {
      Function *F = CB->getCalledFunction();
      if (nullptr == F) {
        displayErrorAndExit("FlattenQuantumKernelPass", "inlining failed.");
      }
      Function *Caller = CB->getParent()->getParent();
      if (nullptr == Caller) {
        displayErrorAndExit("FlattenQuantumKernelPass",
                            "inlining of" +
                                DemangleQuantumKernelName(F->getName()) +
                                "failed.");
      }
      displayErrorAndExit("FlattenQuantumKernelPass",
                          "inlining of " +
                              DemangleQuantumKernelName(F->getName()) +
                              "failed in " + (Caller->getName()).str() + ": " +
                              std::string(result.getFailureReason()));
    }
  }
  return false;
}

struct FlattenQuantumKernelLegacyPass : public ModulePass {

  static char ID; // Pass identification
  FlattenQuantumKernelLegacyPass() : ModulePass(ID) {
    initializeFlattenQuantumKernelLegacyPassPass(
        *PassRegistry::getPassRegistry());
  }

  bool runOnModule(Module &M) override;

  bool run(Module &M, CallGraph &CG);

  virtual void getAnalysisUsage(AnalysisUsage &AU) const {
    AU.setPreservesCFG();
    AU.addRequired<CallGraphWrapperPass>();
  }

}; // End of struct FlattenQuantumKernelPass

char FlattenQuantumKernelLegacyPass::ID = 0;
static RegisterPass<FlattenQuantumKernelLegacyPass>
    X("flatten-qk", "FlattenQuantumKernelLegacyPass", false, false);

INITIALIZE_PASS_BEGIN(FlattenQuantumKernelLegacyPass, "flatten-qk",
                      "FlattenQuantumKernelLegacyPass", false, false)
INITIALIZE_PASS_END(FlattenQuantumKernelLegacyPass, "flatten-qk",
                    "FlattenQuantumKernelLegacyPass", false, false)

bool FlattenQuantumKernelLegacyPass::runOnModule(Module &M) {
  // We will use the call graph scc ordering to order the inlining
  CallGraph &CG = getAnalysis<CallGraphWrapperPass>().getCallGraph();
  return FlattenQuantumKernel(M, CG);
}

PreservedAnalyses FlattenQuantumKernelPass::run(Module &M,
                                                ModuleAnalysisManager &AM) {
  CallGraph &CG = AM.getResult<CallGraphAnalysis>(M);
  FlattenQuantumKernel(M, CG);

  PreservedAnalyses PA;
  PA.preserveSet<CFGAnalyses>();
  return PA;
}

#undef DEBUG_TYPE
