//===- SimpleQbitPlacement.cpp -------------------------------------------====//
//
// Copyright (C) 2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//
//===----------------------------------------------------------------------===//
//
// This file implements a module pass for the simplest qubit placement. It
// creates a single placement for all qubits with a first-come first assigned
// protocol
//
//===----------------------------------------------------------------------===//

#include "IntelQuantumPasses/QuantumCompile/SimpleQbitPlacement.h"
#include "IntelQuantumPasses/InitializeQuantumPasses.h"
#include "IntelQuantumPasses/QuantumAnalysis/QuantumAnnotationGetters.h"
#include "IntelQuantumPasses/QuantumAnalysis/QuantumCompilerLinkage.h"
#include "IntelQuantumPasses/QuantumAnalysis/QuantumGateIdentifiers.h"
#include "IntelQuantumPasses/QuantumAnalysis/QuantumIterator.h"
#include "IntelQuantumPasses/QuantumAnalysis/QuantumModule.h"
#include "IntelQuantumPasses/QuantumAnalysis/QuantumRefs.h"
#include "IntelQuantumPasses/QuantumAnalysis/QuantumSpinNativeToJson.h"
#include "IntelQuantumPasses/QuantumUtils/QuantumGeneralUtils.h"

#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/GlobalVariable.h"
#include "llvm/IR/Intrinsics.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/Operator.h"
#include "llvm/Pass.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;
using namespace aqcc;

#define DEBUG_TYPE "simple-qbit-placement"

namespace {

struct SimpleQbitPlacementLegacyPass : public ModulePass {

public:
  static char ID; // Pass identification, replacement for typeid
  QuantumModule &QM = QuantumCompilerLinkageLegacyPass::QM;

  SimpleQbitPlacementLegacyPass() : ModulePass(ID) {
    initializeSimpleQbitPlacementLegacyPassPass(
        *PassRegistry::getPassRegistry());
  }

  void getAnalysisUsage(AnalysisUsage &AU) const override {

    AU.setPreservesAll();
    // AU.addRequired<QuantumSpinNativeToJsonWrapperPass>();
    // AU.addRequired<QuantumCompilerLinkageWrapperPass>();
  }

  bool runOnModule(Module &M) override {

    if (!addTrivialQubitPlacement(QM))
      displayErrorAndExit("SimpleQbitPlacementPass",
                          "Unable to add a qubit mapping.\n");

    return false;

  } // runOnModule

}; // end of SimpleQbitPlacementPass

} // end of anonymous namespace

char SimpleQbitPlacementLegacyPass::ID = 0;

static RegisterPass<SimpleQbitPlacementLegacyPass>
    X("simple-qbit-placement", "SimpleQbitPlacementLegacyPass",
      false /* Only looks at CFG */, false /* Analysis Pass */);

INITIALIZE_PASS_BEGIN(SimpleQbitPlacementLegacyPass, "simple-qubit-placement",
                      "SimpleQbitPlacementLegacyPass", false, false)
INITIALIZE_PASS_END(SimpleQbitPlacementLegacyPass, "simple-qubit-placement",
                    "SimpleQbitPlacementLegacyPass", false, false)

PreservedAnalyses SimpleQbitPlacementPass::run(Module &M,
                                               ModuleAnalysisManager &MAM) {

  QuantumModuleProxy QMP = MAM.getResult<QuantumCompilerLinkageAnalysis>(M);
  if (!addTrivialQubitPlacement(*QMP.QM)) {
    displayErrorAndExit("SimpleQbitPlacementPass",
                        "Unable to add a qubit mapping.\n");
  }
  return PreservedAnalyses::all();
}

#undef DEBUG_TYPE
