//===- QuantumPassPipeline.cpp -----------------------------------*- C++
//-*-===//
//
//===----------------------------------------------------------------------===//
//
// Copyright (C) 2024 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//
//===----------------------------------------------------------------------===//
/// \file "QuantumPassPipeline.cpp"
///
/// Implementation of the custom quantum compilation pipelines.
///
//===----------------------------------------------------------------------===//

#include "llvm/ADT/StringRef.h"
#include "llvm/Analysis/AliasAnalysis.h"
#include "llvm/Analysis/BasicAliasAnalysis.h"
#include "llvm/Analysis/CGSCCPassManager.h"
#include "llvm/Analysis/CallGraph.h"
#include "llvm/Analysis/CallGraphSCCPass.h"
#include "llvm/Analysis/GlobalsModRef.h"
#include "llvm/Analysis/InlineAdvisor.h"
#include "llvm/Analysis/LoopPass.h"
#include "llvm/Analysis/ProfileSummaryInfo.h"
#include "llvm/Analysis/RegionPass.h"
#include "llvm/Analysis/ScopedNoAliasAA.h"
#include "llvm/Analysis/TargetLibraryInfo.h"
#include "llvm/Analysis/TargetTransformInfo.h"
#include "llvm/Analysis/TypeBasedAliasAnalysis.h"
#include "llvm/CodeGen/TargetPassConfig.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/IR/DebugInfo.h"
#include "llvm/IR/Dominators.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/LLVMRemarkStreamer.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/LegacyPassNameParser.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/ModuleSummaryIndex.h"
#include "llvm/IR/PassManager.h"
#include "llvm/IR/Verifier.h"
#include "llvm/IRPrinter/IRPrintingPasses.h"
#include "llvm/IRReader/IRReader.h"
#include "llvm/InitializePasses.h"
#include "llvm/LinkAllIR.h"
#include "llvm/LinkAllPasses.h"
#include "llvm/MC/TargetRegistry.h"
#include "llvm/Passes/OptimizationLevel.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Passes/StandardInstrumentations.h"
#include "llvm/Remarks/HotnessThresholdParser.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/InitLLVM.h"
#include "llvm/Support/PGOOptions.h"
#include "llvm/Support/SourceMgr.h"
#include "llvm/Support/SystemUtils.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Support/ToolOutputFile.h"
#include "llvm/Support/VirtualFileSystem.h"
#include "llvm/Support/YAMLTraits.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/TargetParser/Host.h"
#include "llvm/TargetParser/SubtargetFeature.h"
#include "llvm/TargetParser/Triple.h"
#include "llvm/Transforms/AggressiveInstCombine/AggressiveInstCombine.h"
#include "llvm/Transforms/Coroutines/CoroCleanup.h"
#include "llvm/Transforms/Coroutines/CoroConditionalWrapper.h"
#include "llvm/Transforms/Coroutines/CoroEarly.h"
#include "llvm/Transforms/Coroutines/CoroElide.h"
#include "llvm/Transforms/Coroutines/CoroSplit.h"
#include "llvm/Transforms/IPO/AlwaysInliner.h"
#include "llvm/Transforms/IPO/Annotation2Metadata.h"
#include "llvm/Transforms/IPO/ArgumentPromotion.h"
#include "llvm/Transforms/IPO/Attributor.h"
#include "llvm/Transforms/IPO/CalledValuePropagation.h"
#include "llvm/Transforms/IPO/ConstantMerge.h"
#include "llvm/Transforms/IPO/CrossDSOCFI.h"
#include "llvm/Transforms/IPO/DeadArgumentElimination.h"
#include "llvm/Transforms/IPO/ElimAvailExtern.h"
#include "llvm/Transforms/IPO/EmbedBitcodePass.h"
#include "llvm/Transforms/IPO/ForceFunctionAttrs.h"
#include "llvm/Transforms/IPO/FunctionAttrs.h"
#include "llvm/Transforms/IPO/GlobalDCE.h"
#include "llvm/Transforms/IPO/GlobalOpt.h"
#include "llvm/Transforms/IPO/GlobalSplit.h"
#include "llvm/Transforms/IPO/HotColdSplitting.h"
#include "llvm/Transforms/IPO/InferFunctionAttrs.h"
#include "llvm/Transforms/IPO/Inliner.h"
#include "llvm/Transforms/IPO/LowerTypeTests.h"
#include "llvm/Transforms/IPO/MemProfContextDisambiguation.h"
#include "llvm/Transforms/IPO/MergeFunctions.h"
#include "llvm/Transforms/IPO/ModuleInliner.h"
#include "llvm/Transforms/IPO/OpenMPOpt.h"
#include "llvm/Transforms/IPO/PartialInlining.h"
#include "llvm/Transforms/IPO/SCCP.h"
#include "llvm/Transforms/IPO/SampleProfile.h"
#include "llvm/Transforms/IPO/SampleProfileProbe.h"
#include "llvm/Transforms/IPO/SyntheticCountsPropagation.h"
#include "llvm/Transforms/IPO/ThinLTOBitcodeWriter.h"
#include "llvm/Transforms/IPO/WholeProgramDevirt.h"
#include "llvm/Transforms/InstCombine/InstCombine.h"
#include "llvm/Transforms/Scalar/ADCE.h"
#include "llvm/Transforms/Scalar/AlignmentFromAssumptions.h"
#include "llvm/Transforms/Scalar/AnnotationRemarks.h"
#include "llvm/Transforms/Scalar/BDCE.h"
#include "llvm/Transforms/Scalar/CallSiteSplitting.h"
#include "llvm/Transforms/Scalar/ConstraintElimination.h"
#include "llvm/Transforms/Scalar/CorrelatedValuePropagation.h"
#include "llvm/Transforms/Scalar/DCE.h"
#include "llvm/Transforms/Scalar/DFAJumpThreading.h"
#include "llvm/Transforms/Scalar/DeadStoreElimination.h"
#include "llvm/Transforms/Scalar/DivRemPairs.h"
#include "llvm/Transforms/Scalar/EarlyCSE.h"
#include "llvm/Transforms/Scalar/Float2Int.h"
#include "llvm/Transforms/Scalar/GVN.h"
#include "llvm/Transforms/Scalar/IndVarSimplify.h"
#include "llvm/Transforms/Scalar/InferAlignment.h"
#include "llvm/Transforms/Scalar/InstSimplifyPass.h"
#include "llvm/Transforms/Scalar/JumpThreading.h"
#include "llvm/Transforms/Scalar/LICM.h"
#include "llvm/Transforms/Scalar/LoopDeletion.h"
#include "llvm/Transforms/Scalar/LoopDistribute.h"
#include "llvm/Transforms/Scalar/LoopFlatten.h"
#include "llvm/Transforms/Scalar/LoopIdiomRecognize.h"
#include "llvm/Transforms/Scalar/LoopInstSimplify.h"
#include "llvm/Transforms/Scalar/LoopInterchange.h"
#include "llvm/Transforms/Scalar/LoopLoadElimination.h"
#include "llvm/Transforms/Scalar/LoopPassManager.h"
#include "llvm/Transforms/Scalar/LoopRotation.h"
#include "llvm/Transforms/Scalar/LoopSimplifyCFG.h"
#include "llvm/Transforms/Scalar/LoopSink.h"
#include "llvm/Transforms/Scalar/LoopUnrollAndJamPass.h"
#include "llvm/Transforms/Scalar/LoopUnrollPass.h"
#include "llvm/Transforms/Scalar/LoopVersioningLICM.h"
#include "llvm/Transforms/Scalar/LowerConstantIntrinsics.h"
#include "llvm/Transforms/Scalar/LowerExpectIntrinsic.h"
#include "llvm/Transforms/Scalar/LowerMatrixIntrinsics.h"
#include "llvm/Transforms/Scalar/MemCpyOptimizer.h"
#include "llvm/Transforms/Scalar/MergedLoadStoreMotion.h"
#include "llvm/Transforms/Scalar/NewGVN.h"
#include "llvm/Transforms/Scalar/Reassociate.h"
#include "llvm/Transforms/Scalar/SCCP.h"
#include "llvm/Transforms/Scalar/SROA.h"
#include "llvm/Transforms/Scalar/SimpleLoopUnswitch.h"
#include "llvm/Transforms/Scalar/SimplifyCFG.h"
#include "llvm/Transforms/Scalar/SpeculativeExecution.h"
#include "llvm/Transforms/Scalar/TailRecursionElimination.h"
#include "llvm/Transforms/Scalar/WarnMissedTransforms.h"
#include "llvm/Transforms/Utils/AddDiscriminators.h"
#include "llvm/Transforms/Utils/AssumeBundleBuilder.h"
#include "llvm/Transforms/Utils/CanonicalizeAliases.h"
#include "llvm/Transforms/Utils/Cloning.h"
#include "llvm/Transforms/Utils/CountVisits.h"
#include "llvm/Transforms/Utils/Debugify.h"
#include "llvm/Transforms/Utils/EntryExitInstrumenter.h"
#include "llvm/Transforms/Utils/InjectTLIMappings.h"
#include "llvm/Transforms/Utils/LibCallsShrinkWrap.h"
#include "llvm/Transforms/Utils/LowerInvoke.h"
#include "llvm/Transforms/Utils/Mem2Reg.h"
#include "llvm/Transforms/Utils/MoveAutoInit.h"
#include "llvm/Transforms/Utils/NameAnonGlobals.h"
#include "llvm/Transforms/Utils/RelLookupTableConverter.h"
#include "llvm/Transforms/Utils/SimplifyCFGOptions.h"
#include "llvm/Transforms/Vectorize/LoopVectorize.h"
#include "llvm/Transforms/Vectorize/SLPVectorizer.h"
#include "llvm/Transforms/Vectorize/VectorCombine.h"
#include <algorithm>
#include <memory>
#include <optional>

#include "IntelQuantumPasses/QuantumAnalysis/QuantumAnnotationsToJson.h"
#include "IntelQuantumPasses/QuantumAnalysis/QuantumCompilerLinkage.h"
#include "IntelQuantumPasses/QuantumAnalysis/QuantumInitPlacement.h"
#include "IntelQuantumPasses/QuantumAnalysis/QuantumModule.h"
#include "IntelQuantumPasses/QuantumAnalysis/QuantumSpinNativeToJson.h"

#include "IntelQuantumPasses/QuantumCompile/ConvertQbitToQID.h"
#include "IntelQuantumPasses/QuantumCompile/DecomposeToffoli.h"
#include "IntelQuantumPasses/QuantumCompile/FinalizeQuantumCompilation.h"
#include "IntelQuantumPasses/QuantumCompile/FlattenQuantumKernel.h"
#include "IntelQuantumPasses/QuantumCompile/InsertQuantumAttributes.h"
#include "IntelQuantumPasses/QuantumCompile/InsertQuantumIntrinsics.h"
#include "IntelQuantumPasses/QuantumCompile/IntelQuantumPlugin.h"
#include "IntelQuantumPasses/QuantumCompile/PrintCircuitQBB.h"
#include "IntelQuantumPasses/QuantumCompile/Printer.h"
#include "IntelQuantumPasses/QuantumCompile/PrinterASCII.h"
#include "IntelQuantumPasses/QuantumCompile/PrinterInterface.h"
#include "IntelQuantumPasses/QuantumCompile/PrinterJson.h"
#include "IntelQuantumPasses/QuantumCompile/PrinterLaTex.h"
#include "IntelQuantumPasses/QuantumCompile/ProofReader.h"
#include "IntelQuantumPasses/QuantumCompile/QuantumClassicalModuleSplit.h"
#include "IntelQuantumPasses/QuantumCompile/QuantumKernelStatsPrint.h"
#include "IntelQuantumPasses/QuantumCompile/QuantumLoopUnrolling.h"
#include "IntelQuantumPasses/QuantumCompile/QuantumPassPipeline.h"
#include "IntelQuantumPasses/QuantumCompile/Scheduler.h"
#include "IntelQuantumPasses/QuantumCompile/SeparateAndReplaceQBB.h"
#include "IntelQuantumPasses/QuantumCompile/Serialize.h"
#include "IntelQuantumPasses/QuantumCompile/SimpleQbitPlacement.h"
#include "IntelQuantumPasses/QuantumCompile/SpinNativeAnglesInRange.h"
#include "IntelQuantumPasses/QuantumCompile/SpinNativeConvertToIMM.h"
#include "IntelQuantumPasses/QuantumCompile/SpinNativeLowerFromCanonical.h"
#include "IntelQuantumPasses/QuantumCompile/ValidateAndConditionQBB.h"

#include "IntelQuantumPasses/QuantumUtils/QuantumGeneralUtils.h"

namespace llvm {
namespace aqcc {

void addPreQuantumLoopUnrollingModulePasses(PassBuilder &PB,
                                            ModulePassManager &MPM) {
  OptimizationLevel Level = OptimizationLevel::O3;
  ThinOrFullLTOPhase Phase = ThinOrFullLTOPhase::None;
  MPM.addPass(ReversePostOrderFunctionAttrsPass());
  MPM.addPass(GlobalOptPass());
  MPM.addPass(GlobalDCEPass(false));
  MPM.addPass(CalledValuePropagationPass());
  MPM.addPass(AlwaysInlinerPass(/*InsertLifetimeIntrinsics=*/true));
  PB.buildInlinerPipeline(Level, Phase);
  MPM.addPass(GlobalOptPass());
  MPM.addPass(GlobalDCEPass(false));
}

void addPostQuantumLoopUnrollingModulePasses(PassBuilder &PB,
                                             ModulePassManager &MPM) {
  MPM.addPass(GlobalOptPass());
  MPM.addPass(GlobalDCEPass(false));
  MPM.addPass(ConstantMergePass());
}

// Adapted from addVectorPasses from
// llvm/lib/Passes/PassBuilderPipelines.cpp:addVectorPasses.
void addUnrollingVectorPasses(PassBuilder &PB, OptimizationLevel Level,
                              FunctionPassManager &FPM,
                              PipelineTuningOptions &PTO,
                              std::optional<PGOOptions> &PGOOpt,
                              bool IsFullLTO) {
  FPM.addPass(LoopVectorizePass(
      LoopVectorizeOptions(!PTO.LoopInterleaving, !PTO.LoopVectorization)));

  bool ExtraVectorizerPasses = false;
  bool EnableInferAlignmentPass = false;
  bool EnableUnrollAndJam = false;

  if (EnableInferAlignmentPass)
    FPM.addPass(InferAlignmentPass());

  // Eliminate loads by forwarding stores from the previous iteration to loads
  // of the current iteration.
  FPM.addPass(LoopLoadEliminationPass());

  // Cleanup after the loop optimization passes.
  FPM.addPass(InstCombinePass());

  // Now that we've formed fast to execute loop structures, we do further
  // optimizations. These are run afterward as they might block doing complex
  // analyses and transforms such as what are needed for loop vectorization.

  // Cleanup after loop vectorization, etc. Simplification passes like CVP and
  // GVN, loop transforms, and others have already run, so it's now better to
  // convert to more optimized IR using more aggressive simplify CFG options.
  // The extra sinking transform can create larger basic blocks, so do this
  // before SLP vectorization.
  FPM.addPass(SimplifyCFGPass(SimplifyCFGOptions()
                                  .forwardSwitchCondToPhi(true)
                                  .convertSwitchRangeToICmp(true)
                                  .convertSwitchToLookupTable(true)
                                  .needCanonicalLoops(false)
                                  .hoistCommonInsts(true)
                                  .sinkCommonInsts(true)));

  // Optimize parallel scalar instruction chains into SIMD instructions.
  if (PTO.SLPVectorization) {
    FPM.addPass(SLPVectorizerPass());
    if (Level.getSpeedupLevel() > 1 && ExtraVectorizerPasses) {
      FPM.addPass(EarlyCSEPass());
    }
  }
  // Enhance/cleanup vector code.
  FPM.addPass(VectorCombinePass());

  FPM.addPass(InstCombinePass());
  // Unroll small loops to hide loop backedge latency and saturate any
  // parallel execution resources of an out-of-order processor. We also then
  // need to clean up redundancies and loop invariant code.
  // FIXME: It would be really good to use a loop-integrated instruction
  // combiner for cleanup here so that the unrolling and LICM can be pipelined
  // across the loop nests.
  // We do UnrollAndJam in a separate LPM to ensure it happens before unroll
  FPM.addPass(createFunctionToLoopPassAdaptor(QuantumLoopUnrollPass(
      Level.getSpeedupLevel(), /*OnlyWhenForced=*/!PTO.LoopUnrolling,
      PTO.ForgetAllSCEVInLoopUnroll)));
  FPM.addPass(WarnMissedTransformationsPass());

  // Now that we are done with loop unrolling, be it either by LoopVectorizer,
  // or LoopUnroll passes, some variable-offset GEP's into alloca's could have
  // become constant-offset, thus enabling SROA and alloca promotion. Do so.
  // NOTE: we are very late in the pipeline, and we don't have any LICM
  // or SimplifyCFG passes scheduled after us, that would cleanup
  // the CFG mess this may created if allowed to modify CFG, so forbid that.
  FPM.addPass(SROAPass(SROAOptions::PreserveCFG));

  if (EnableInferAlignmentPass)
    FPM.addPass(InferAlignmentPass());
  FPM.addPass(InstCombinePass());

  // This is needed for two reasons:
  //   1. It works around problems that instcombine introduces, such as sinking
  //      expensive FP divides into loops containing multiplications using the
  //      divide result.
  //   2. It helps to clean up some loop-invariant code created by the loop
  //      unroll pass when IsFullLTO=false.
  FPM.addPass(createFunctionToLoopPassAdaptor(
      LICMPass(PTO.LicmMssaOptCap, PTO.LicmMssaNoAccForPromotionCap,
               /*AllowSpeculation=*/true),
      /*UseMemorySSA=*/true, /*UseBlockFrequencyInfo=*/false));

  // Now that we've vectorized and unrolled loops, we may have more refined
  // alignment information, try to re-derive it here.
  FPM.addPass(AlignmentFromAssumptionsPass());
}

// Adapted from addVectorPasses from
// llvm/lib/Passes/PassBuilderPipelines.cpp:addFunctionSimplificationPipeline
// and llvm/lib/Passes/PassBuilderPipelines.cpp:addFunctionOptimizationPipeline.
void addQuantumLoopUnrollingFunctionPasses(PassBuilder &PB,
                                           FunctionPassManager &FPM,
                                           PipelineTuningOptions &PTO,
                                           std::optional<PGOOptions> &PGOOpt) {
  OptimizationLevel Level = OptimizationLevel::O3;
  ThinOrFullLTOPhase Phase = ThinOrFullLTOPhase::None;

  // Form SSA out of local memory accesses after breaking apart aggregates into
  // scalars.
  FPM.addPass(SROAPass(SROAOptions::ModifyCFG));

  // Catch trivial redundancies
  FPM.addPass(EarlyCSEPass(true /* Enable mem-ssa. */));

  // Speculative execution if the target has divergent branches; otherwise nop.
  FPM.addPass(SpeculativeExecutionPass(/* OnlyIfDivergentTarget =*/true));

  // Optimize based on known information about branches, and cleanup afterward.
  FPM.addPass(JumpThreadingPass());
  FPM.addPass(CorrelatedValuePropagationPass());

  // Hoisting of scalars and load expressions.
  FPM.addPass(
      SimplifyCFGPass(SimplifyCFGOptions().convertSwitchRangeToICmp(true)));
  FPM.addPass(InstCombinePass());
  FPM.addPass(AggressiveInstCombinePass());

  FPM.addPass(LibCallsShrinkWrapPass());

  PB.invokePeepholeEPCallbacks(FPM, OptimizationLevel::O3);
  FPM.addPass(Float2IntPass());
  FPM.addPass(LowerConstantIntrinsicsPass());

  FPM.addPass(LowerInvokePass());

  FPM.addPass(TailCallElimPass());
  FPM.addPass(
      SimplifyCFGPass(SimplifyCFGOptions().convertSwitchRangeToICmp(true)));

  // Form canonically associated expression trees, and simplify the trees using
  // basic mathematical properties. For example, this will form (nearly)
  // minimal multiplication trees.
  FPM.addPass(ReassociatePass());
  FPM.addPass(ConstraintEliminationPass());

  // Add the primary loop simplification pipeline.
  // FIXME: Currently this is split into two loop pass pipelines because we run
  // some function passes in between them. These can and should be removed
  // and/or replaced by scheduling the loop pass equivalents in the correct
  // positions. But those equivalent passes aren't powerful enough yet.
  // Specifically, `SimplifyCFGPass` and `InstCombinePass` are currently still
  // used. We have `LoopSimplifyCFGPass` which isn't yet powerful enough yet to
  // fully replace `SimplifyCFGPass`, and the closest to the other we have is
  // `LoopInstSimplify`.
  LoopPassManager LPM1, LPM2;

  // Simplify the loop body. We do this initially to clean up after other loop
  // passes run, either when iterating on a loop or on inner loops wi.h"
  // implications on the outer loop.
  LPM1.addPass(LoopInstSimplifyPass());
  LPM1.addPass(LoopSimplifyCFGPass());

  // Try to remove as much code from the loop header as possible,
  // to reduce amount of IR that will have to be duplicated. However,
  // do not perform speculative hoisting the first time as LICM
  // will destroy metadata that may not need to be destroyed if run
  // after loop rotation.
  // TODO: Investigate promotion cap for O1.
  LPM1.addPass(LICMPass(PTO.LicmMssaOptCap, PTO.LicmMssaNoAccForPromotionCap,
                        /*AllowSpeculation=*/false));

  LPM1.addPass(LoopRotatePass(/* Disable header duplication */ true, false));
  // TODO: Investigate promotion cap for O1.
  LPM1.addPass(LICMPass(PTO.LicmMssaOptCap, PTO.LicmMssaNoAccForPromotionCap,
                        /*AllowSpeculation=*/true));
  LPM1.addPass(SimpleLoopUnswitchPass());
  // if (EnableLoopFlatten)
  //   LPM1.addPass(LoopFlattenPass());

  LPM2.addPass(LoopIdiomRecognizePass());
  LPM2.addPass(IndVarSimplifyPass());

  PB.invokeLateLoopOptimizationsEPCallbacks(LPM2, Level);

  LPM2.addPass(LoopDeletionPass());

  LPM2.addPass(QuantumLoopUnrollPass(Level.getSpeedupLevel(),
                                     /* OnlyWhenForced= */ !PTO.LoopUnrolling,
                                     PTO.ForgetAllSCEVInLoopUnroll));

  PB.invokeLoopOptimizerEndEPCallbacks(LPM2, Level);

  FPM.addPass(createFunctionToLoopPassAdaptor(std::move(LPM1),
                                              /*UseMemorySSA=*/true,
                                              /*UseBlockFrequencyInfo=*/true));
  FPM.addPass(
      SimplifyCFGPass(SimplifyCFGOptions().convertSwitchRangeToICmp(true)));
  FPM.addPass(InstCombinePass());
  // The loop passes in LPM2 (LoopFullUnrollPass) do not preserve MemorySSA.
  // *All* loop passes must preserve it, in order to be able to use it.
  FPM.addPass(createFunctionToLoopPassAdaptor(std::move(LPM2),
                                              /*UseMemorySSA=*/false,
                                              /*UseBlockFrequencyInfo=*/false));

  // Delete small array after loop unroll.
  FPM.addPass(SROAPass(SROAOptions::ModifyCFG));

  FPM.addPass(GVNPass());

  // Specially optimize memory movement as it doesn't look like dataflow in SSA.
  FPM.addPass(MemCpyOptPass());

  // Sparse conditional constant propagation.
  // FIXME: It isn't clear why we do this *after* loop passes rather than
  // before...
  FPM.addPass(SCCPPass());

  // Delete dead bit computations (instcombine runs after to fold away the dead
  // computations, and then ADCE will run later to exploit any new DCE
  // opportunities that creates).
  FPM.addPass(BDCEPass());

  // Run instcombine after redundancy and dead bit elimination to exploit
  // opportunities opened up by them.
  FPM.addPass(InstCombinePass());
  PB.invokePeepholeEPCallbacks(FPM, Level);

  FPM.addPass(JumpThreadingPass());
  FPM.addPass(CorrelatedValuePropagationPass());

  // Finally, do an expensive DCE pass to catch all the dead code exposed by
  // the simplifications and basic cleanup after all the simplifications.
  // TODO: Investigate if this is too expensive.
  FPM.addPass(ADCEPass());

  // Specially optimize memory movement as it doesn't look like dataflow in SSA.
  FPM.addPass(MemCpyOptPass());

  FPM.addPass(DSEPass());
  FPM.addPass(MoveAutoInitPass());

  FPM.addPass(createFunctionToLoopPassAdaptor(
      LICMPass(PTO.LicmMssaOptCap, PTO.LicmMssaNoAccForPromotionCap,
               /*AllowSpeculation=*/true),
      /*UseMemorySSA=*/true, /*UseBlockFrequencyInfo=*/false));

  FPM.addPass(CoroElidePass());

  PB.invokeScalarOptimizerLateEPCallbacks(FPM, Level);

  // Finally, do an expensive DCE pass to catch all the dead code exposed by
  // the simplifications and basic cleanup after all the simplifications.
  // TODO: Investigate if this is too expensive.
  FPM.addPass(ADCEPass());
  FPM.addPass(
      SimplifyCFGPass(SimplifyCFGOptions().convertSwitchRangeToICmp(true)));
  FPM.addPass(InstCombinePass());
  PB.invokePeepholeEPCallbacks(FPM, Level);

  LoopPassManager LPM;
  // First rotate loops that may have been un-rotated by prior passes.
  // Disable header duplication at -Oz.
  LPM.addPass(LoopRotatePass(false || Level != OptimizationLevel::Oz, false));

  // Some loops may have become dead by now. Try to delete them.
  // FIXME: see discussion in https://reviews.llvm.org/D112851,
  //        this may need to be revisited once we run GVN before loop deletion
  //        in the simplification pipeline.
  LPM.addPass(LoopDeletionPass());

  FPM.addPass(createFunctionToLoopPassAdaptor(
      std::move(LPM), /*UseMemorySSA=*/false, /*UseBlockFrequencyInfo=*/false));

  // Distribute loops to allow partial vectorization.  I.e. isolate dependences
  // into separate loop that would otherwise inhibit vectorization.  This is
  // currently only performed for loops marked with the metadata
  // llvm.loop.distribute=true or when -enable-loop-distribute is specified.
  FPM.addPass(LoopDistributePass());

  // Populates the VFABI attribute with the scalar-to-vector mappings
  // from the TargetLibraryInfo.
  FPM.addPass(InjectTLIMappings());

  addUnrollingVectorPasses(PB, Level, FPM, PTO, PGOOpt, /* IsFullLTO */ false);

  // LoopSink pass sinks instructions hoisted by LICM, which serves as a
  // canonicalization pass that enables other optimizations. As a result,
  // LoopSink pass needs to be a very late IR pass to avoid undoing LICM
  // result too early.
  FPM.addPass(LoopSinkPass());

  // And finally clean up LCSSA form before generating code.
  FPM.addPass(InstSimplifyPass());

  // This hoists/decomposes div/rem ops. It should run after other sink/hoist
  // passes to avoid re-sinking, but before SimplifyCFG because it can allow
  // flattening of blocks.
  FPM.addPass(DivRemPairsPass());

  // Try to annotate calls that were created during optimization.
  FPM.addPass(TailCallElimPass());

  // LoopSink (and other loop passes since the last simplifyCFG) might have
  // resulted in single-entry-single-exit or empty blocks. Clean up the CFG.
  FPM.addPass(
      SimplifyCFGPass(SimplifyCFGOptions().convertSwitchRangeToICmp(true)));

  FPM.addPass(createFunctionToLoopPassAdaptor(std::move(LPM1),
                                              /*UseMemorySSA=*/true,
                                              /*UseBlockFrequencyInfo=*/true));
  FPM.addPass(
      SimplifyCFGPass(SimplifyCFGOptions().convertSwitchRangeToICmp(true)));
  FPM.addPass(InstCombinePass());
  FPM.addPass(EarlyCSEPass());
  FPM.addPass(
      SimplifyCFGPass(SimplifyCFGOptions().convertSwitchRangeToICmp(true)));
}

void runFunctionPassesQuantumKernels(ModulePassManager &PreMPM,
                                     ModulePassManager &PostMPM,
                                     FunctionPassManager &FPM, Module &M,
                                     ModuleAnalysisManager &MAM,
                                     FunctionAnalysisManager &FAM,
                                     unsigned AttemptLimit) {
  std::set<Function *> Complete;
  bool FuncsFound = true;
  unsigned Iter = 0;
  std::vector<QBBIter> F_gates;
  std::map<Function *, unsigned> Attempts;
  std::map<Function *, unsigned>::iterator It;

  // Keep running the passes as long as we have a function that has not been
  // simplified yet.
  while (FuncsFound) {
    FuncsFound = false;
    // Before we run function passes, run the set up by running the pre
    // unrolling passes.
    PreMPM.run(M, MAM);
    for (Function &F : M) {
      // If the function has be validated, we can skip it.
      if (Complete.find(&F) != Complete.end()) {
        continue;
      }

      // If the function is not a quantum kernel, we can skip it.
      if ((!isQKernel(F) && !llvm::fleq::isQExprFunc(&F)) || F.isIntrinsic()) {
        continue;
      }

      // Check to make sure the kernel is not already valid.
      F_gates.clear();
      int result = validateQuantumKernel(F, F_gates);

      // If it is valid already, add it to the complete set, and
      // continue.
      if (result > 0 ||
          (llvm::fleq::isQExprFunc(&F) && (result == -3 || result == 0))) {
        Complete.insert(&F);
        continue;
      }

      // Add to the attempt count.
      It = Attempts.find(&F);
      if (It == Attempts.end()) {
        Attempts[&F] = 0;
      }

      Attempts[&F]++;

      // If the attempt limit has been met, skip it.
      if (Attempts[&F] > AttemptLimit) {
        if (!F.hasFnAttribute("quantum_inlined")) {
          std::string ErrorString = "";
          ErrorString += "When simplifying non-fully inlined function " +
                         F.getName().str() +
                         " optimizations have exceeded the unrolling attempt "
                         "limit due to ";
          switch (result) {
          case -1:
          case -2:
            ErrorString += "invalid loops that cannot be fully unrolled.";
            break;
          case -3:
            ErrorString += "unresolved qubit arguments to the kernel.";
            break;
          case -4:
            ErrorString +=
                "a multi-qubit gate with non-unique qubit arguments.";
            break;
          }
          if (!F.hasNUsesOrMore(1)) {
            ErrorString +=
                " While this function is not used, if referenced by an "
                "external interface you may encounter errors and "
                "undefined behavior.\n";
            errs() << "Quantum Kernel Simplification Warning: " << ErrorString;
          } else {
            ErrorString +=
                " This kernel is called and not a top level kernel. Exiting as "
                "this will cause issues later in compilation.\n";
            displayErrorAndExit("Quantum Kernel Simplification", ErrorString);
          }
        }
        continue;
      }

      // Run the function passes for the quantum kernel.
      FPM.run(F, FAM);
      F_gates.clear();

      // Check if the quantum kernel is valid.
      result = validateQuantumKernel(F, F_gates);
      if (result > 0 ||
          (llvm::fleq::isQExprFunc(&F) && (result == -3 || result == 0))) {
        Complete.insert(&F);
      } else
        FuncsFound = FuncsFound || true;
    }
    // Run the cleanup/ending passes after the function passes have been run.
    PostMPM.run(M, MAM);
  }
}

void addQuantumModulePasses(ModulePassManager &MPM) {
  MPM.addPass(QuantumAnnotationsToJsonPass());
  MPM.addPass(QuantumSpinNativeToJsonPass());
}

void addPrinterPass(ModulePassManager &MPM, std::string Stage,
                    bool PrintCircuit) {
  if (PrintCircuit)
    MPM.addPass(PrintCircuitQBBPass(Stage));
}

void addVerbosePasses(ModulePassManager &MPM, bool PrintVerbose,
                      bool AddPlacement) {
  if (PrintVerbose) {
    if (AddPlacement)
      MPM.addPass(SimpleQbitPlacementPass());
    MPM.addPass(QuantumKernelStatsPrintPass());
  }
}

void addQuantumAttributePasses(ModulePassManager &MPM) {
  MPM.addPass(InsertQuantumIntrinsicsPass());
  MPM.addPass(InsertQuantumArgumentAttributesPass());
}

void addFlattenQuantumKernelPasses(ModulePassManager &MPM, bool PrintVerbose,
                                   bool PrintCircuit) {
  MPM.addPass(FlattenQuantumKernelPass());
  MPM.addPass(createModuleToFunctionPassAdaptor(DCEPass()));
}

void addValidatePasses(ModulePassManager &MPM, bool PrintVerbose,
                       bool PrintCircuit) {
  MPM.addPass(ValidateAndConditionQBBPass());
  addVerbosePasses(MPM, PrintVerbose);
  addPrinterPass(MPM, "validate", PrintCircuit);
}

void addSynthesisPasses(ModulePassManager &MPM, bool PrintVerbose,
                        bool PrintCircuit) {
  addVerbosePasses(MPM, PrintVerbose);
  addPrinterPass(MPM, "synthesis", PrintCircuit);
}

void addLoweringPasses(ModulePassManager &MPM, std::string DotType,
                       bool CanonicalScheduling, bool SkipFLEQ,
                       bool PrintVerbose, bool PrintCircuit) {
  MPM.addPass(SpinNativeLowerFromCanonicalPass());
  MPM.addPass(createModuleToFunctionPassAdaptor(DCEPass()));
  addVerbosePasses(MPM, PrintVerbose);
  addPrinterPass(MPM, "lowering", PrintCircuit);
}

void addSchedulingPasses(ModulePassManager &MPM, std::string DotType,
                         bool UsePulse, bool Serialize, bool PrintVerbose,
                         bool PrintCircuit) {
  MPM.addPass(SchedulerPass());
  MPM.addPass(ConvertQbitToQIDPass());
  if (Serialize)
    MPM.addPass(SerializeKernelsPass());
  MPM.addPass(SpinNativeAnglesInRangePass());
  MPM.addPass(createModuleToFunctionPassAdaptor(DCEPass()));
  addVerbosePasses(MPM, PrintVerbose, false);
  addPrinterPass(MPM, "scheduling", PrintCircuit);
}

void addSeparationPasses(ModulePassManager &MPM, bool PrintVerbose,
                         bool PrintCircuit) {
  MPM.addPass(SpinNativeConvertToIMMPass());
  MPM.addPass(SeparateAndReplaceQBBPass());
  addVerbosePasses(MPM, PrintVerbose, true);
  addPrinterPass(MPM, "separated", PrintCircuit);
  MPM.addPass(createModuleToFunctionPassAdaptor(DCEPass()));
}

void addSplitPasses(ModulePassManager &MPM) {
  MPM.addPass(QuantumClassicalModuleSplitPass());
  MPM.addPass(FinalizeQuantumCompilationPass());
}

void addCleanupPasses(ModulePassManager &MPM) {
  MPM.addPass(createModuleToFunctionPassAdaptor(DCEPass()));
}

} // namespace aqcc
} // namespace llvm
