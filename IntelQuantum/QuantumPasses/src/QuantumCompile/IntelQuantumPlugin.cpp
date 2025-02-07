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

#include "IntelQuantumPasses/QuantumCompile/IntelQuantumPlugin.h"
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
#include "IntelQuantumPasses/QuantumCompile/Serialize.h"
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

#define QUANTUM_MODULE_PASS(NAME, CREATE_PASS)                                 \
  if (Name == NAME) {                                                          \
    MPM.addPass(CREATE_PASS);                                                  \
    return true;                                                               \
  }
#define QUANTUM_LOOP_PASS(NAME, CREATE_PASS)                                   \
  if (Name == NAME) {                                                          \
    MPM.addPass(createModuleToFunctionPassAdaptor(                             \
        createFunctionToLoopPassAdaptor(CREATE_PASS)));                        \
    return true;                                                               \
  }

void registerIntelQuantumPassesCallBacks(PassBuilder &PB) {
  PB.registerAnalysisRegistrationCallback([](ModuleAnalysisManager &MAM) {
    MAM.registerPass([] { return QuantumCompilerLinkageAnalysis(); });
  });
  PB.registerPipelineParsingCallback(
      [](StringRef Name, ModulePassManager &MPM,
         ArrayRef<PassBuilder::PipelineElement>) {
#include "QuantumPasses.def"
        return false;
      });

#define QUANTUM_LOOP_PASS(NAME, CREATE_PASS)                                   \
  if (Name == NAME) {                                                          \
    FPM.addPass(createFunctionToLoopPassAdaptor(CREATE_PASS));                 \
    return true;                                                               \
  }

  PB.registerPipelineParsingCallback(
      [](StringRef Name, FunctionPassManager &FPM,
         ArrayRef<PassBuilder::PipelineElement>) {
#include "QuantumPasses.def"
        return false;
      });
#define QUANTUM_LOOP_PASS(NAME, CREATE_PASS)                                   \
  if (Name == NAME) {                                                          \
    LPM.addPass(QuantumLoopUnrollPass(CREATE_PASS));                           \
    return true;                                                               \
  }
  PB.registerPipelineParsingCallback(
      [](StringRef Name, LoopPassManager &LPM,
         ArrayRef<PassBuilder::PipelineElement>) {
#include "QuantumPasses.def"
        return false;
      });
}

llvm::PassPluginLibraryInfo getIntelQuantumPassesPluginInfo() {
  llvm::setBugReportMsg(IntelMsg);
  sys::AddSignalHandler(ErrorHandler, nullptr);
  return {LLVM_PLUGIN_API_VERSION, "IntelQuantumPassesPlugin",
          LLVM_VERSION_STRING, registerIntelQuantumPassesCallBacks};
}

extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo() {
  return getIntelQuantumPassesPluginInfo();
}