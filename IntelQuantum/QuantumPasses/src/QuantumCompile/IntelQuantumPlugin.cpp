//===- IntelQuantumPlugin.cpp --------------------------------------------====//
//
// Copyright (C) 2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//
//===----------------------------------------------------------------------===//
//
// This file implements the passes required to run the passes as a plugin.
//
//===----------------------------------------------------------------------===//

#include "IntelQuantumPasses/QuantumAnalysis/QuantumAnnotationsToJson.h"
#include "IntelQuantumPasses/QuantumAnalysis/QuantumCompilerLinkage.h"
#include "IntelQuantumPasses/QuantumAnalysis/QuantumInitPlacement.h"
#include "IntelQuantumPasses/QuantumAnalysis/QuantumSpinNativeToJson.h"
#include "IntelQuantumPasses/QuantumCompile/ConvertQbitToQID.h"
#include "IntelQuantumPasses/QuantumCompile/DecomposeToffoli.h"
#include "IntelQuantumPasses/QuantumCompile/FinalizeQuantumCompilation.h"
#include "IntelQuantumPasses/QuantumCompile/FlattenQuantumKernel.h"
#include "IntelQuantumPasses/QuantumCompile/InsertQuantumAttributes.h"
#include "IntelQuantumPasses/QuantumCompile/InsertQuantumIntrinsics.h"
#include "IntelQuantumPasses/QuantumCompile/PrintCircuitQBB.h"
#include "IntelQuantumPasses/QuantumCompile/QuantumClassicalModuleSplit.h"
#include "IntelQuantumPasses/QuantumCompile/QuantumKernelStatsPrint.h"
#include "IntelQuantumPasses/QuantumCompile/QuantumLoopUnrolling.h"
#include "IntelQuantumPasses/QuantumCompile/Scheduler.h"
#include "IntelQuantumPasses/QuantumCompile/SeparateAndReplaceQBB.h"
#include "IntelQuantumPasses/QuantumCompile/SimpleQbitPlacement.h"
#include "IntelQuantumPasses/QuantumCompile/SpinNativeAnglesInRange.h"
#include "IntelQuantumPasses/QuantumCompile/SpinNativeConvertToIMM.h"
#include "IntelQuantumPasses/QuantumCompile/SpinNativeLowerFromCanonical.h"
#include "IntelQuantumPasses/QuantumCompile/ValidateAndConditionQBB.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/PrettyStackTrace.h"
#include "llvm/Support/Signals.h"

using namespace llvm;

static const char *IntelMsg =
    "\nPlease report bugs to  " BUG_REPORT_URL ".\n\n";

cl::list<std::string> quantum_debug("quantum-debug",
                                    cl::desc("Debug flag for intel quantum"));

static void ErrorHandler(void *) { errs() << getBugReportMsg(); }

llvm::PassPluginLibraryInfo getIntelQuantumPassesPluginInfo() {
  llvm::setBugReportMsg(IntelMsg);
  sys::AddSignalHandler(ErrorHandler, nullptr);
  return {
      LLVM_PLUGIN_API_VERSION, "IntelQuantumPassesPlugin", LLVM_VERSION_STRING,
      [](PassBuilder &PB) {
        PB.registerAnalysisRegistrationCallback([](ModuleAnalysisManager &MAM) {
          MAM.registerPass([] { return QuantumCompilerLinkageAnalysis(); });
        });
        PB.registerPipelineParsingCallback(
            [](StringRef Name, ModulePassManager &MPM,
               ArrayRef<PassBuilder::PipelineElement>) {
              if (Name == "flatten-qk") {
                MPM.addPass(FlattenQuantumKernelPass());
                return true;
              } else if (Name == "insert-q-intrinsics") {
                MPM.addPass(InsertQuantumIntrinsicsPass());
                return true;
              } else if (Name == "insert-q-attrs") {
                MPM.addPass(InsertQuantumArgumentAttributesPass());
                return true;
              } else if (Name == "q-annotations-to-json") {
                MPM.addPass(QuantumAnnotationsToJsonPass());
                return true;
              } else if (Name == "validate-and-condition-qbb") {
                MPM.addPass(ValidateAndConditionQBBPass());
                return true;
              } else if (Name == "spin-native-annotate") {
                MPM.addPass(QuantumSpinNativeToJsonPass());
                return true;
              } else if (Name == "spin-lower-from-canonical") {
                MPM.addPass(SpinNativeLowerFromCanonicalPass());
                return true;
              } else if (Name == "simple-qbit-placement") {
                MPM.addPass(SimpleQbitPlacementPass());
                return true;
              } else if (Name == "q-init-placement") {
                MPM.addPass(QuantumInitPlacementPass());
                return true;
              } else if (Name == "decompose-toff") {
                MPM.addPass(DecomposeToffoliPass());
                return true;
              } else if (Name == "q-scheduler") {
                MPM.addPass(SchedulerPass());
                return true;
              } else if (Name == "convert-qbit-to-qid") {
                MPM.addPass(ConvertQbitToQIDPass());
                return true;
              } else if (Name == "spin-angles-in-range") {
                MPM.addPass(SpinNativeAnglesInRangePass());
                return true;
              } else if (Name == "spin-convert-to-imm") {
                MPM.addPass(SpinNativeConvertToIMMPass());
                return true;
              } else if (Name == "separate-and-replace-qbb") {
                MPM.addPass(SeparateAndReplaceQBBPass());
                return true;
              } else if (Name == "q-classical-module-split") {
                MPM.addPass(QuantumClassicalModuleSplitPass());
                return true;
              } else if (Name == "finalize-quantum-compilation") {
                MPM.addPass(FinalizeQuantumCompilationPass());
                return true;
              } else if (Name == "q-stats-print") {
                MPM.addPass(QuantumKernelStatsPrintPass());
                return true;
              } else if (Name == "print-circuit-qbb") {
                MPM.addPass(PrintCircuitQBBPass());
                return true;
              } else if (Name == "q-loop-unroll") {
                MPM.addPass(createModuleToFunctionPassAdaptor(
                    createFunctionToLoopPassAdaptor(QuantumLoopUnrollPass(),
                                                    false, false)));
                return true;
              }
              return false;
            });
        PB.registerPipelineParsingCallback(
            [](StringRef Name, FunctionPassManager &FPM,
               ArrayRef<PassBuilder::PipelineElement>) { return false; });
        PB.registerPipelineParsingCallback(
            [](StringRef Name, LoopPassManager &LPM,
               ArrayRef<PassBuilder::PipelineElement>) {
              if (Name == "q-loop-unroll") {
                LPM.addPass(QuantumLoopUnrollPass());
                return true;
              }
              return false;
            });
      }};
}

extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo() {
  return getIntelQuantumPassesPluginInfo();
}