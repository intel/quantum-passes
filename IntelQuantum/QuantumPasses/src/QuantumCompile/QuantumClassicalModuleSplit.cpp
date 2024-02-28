//===- ClassicalQuantumModuleSplit.h -------------------------------------====//
//
// Copyright (C) 2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//
//===----------------------------------------------------------------------===//
//
// This file implements a Module pass which replaces simple QBB definitions with
// a declaration for that function while moving the defintions to a new Module.
//
//===----------------------------------------------------------------------===//

#include "IntelQuantumPasses/QuantumCompile/QuantumClassicalModuleSplit.h"
#include "IntelQuantumPasses/InitializeQuantumPasses.h"
#include "IntelQuantumPasses/QuantumAnalysis/QuantumAnnotationGetters.h"
#include "IntelQuantumPasses/QuantumAnalysis/QuantumCompilerLinkage.h"
#include "IntelQuantumPasses/QuantumAnalysis/QuantumGateIdentifiers.h"
#include "IntelQuantumPasses/QuantumAnalysis/QuantumIterator.h"
#include "IntelQuantumPasses/QuantumAnalysis/QuantumModule.h"
#include "IntelQuantumPasses/QuantumAnalysis/QuantumRefs.h"
#include "IntelQuantumPasses/QuantumAnalysis/QuantumSpinNativeToJson.h"
#include "IntelQuantumPasses/QuantumUtils/QuantumGeneralUtils.h"
#include "llvm/ADT/SCCIterator.h"
#include "llvm/Analysis/CFG.h"
#include "llvm/IR/CFG.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/GlobalVariable.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/Operator.h"
#include "llvm/Pass.h"
#include "llvm/Support/raw_ostream.h"

#define DEBUG_TYPE "classical-quantum-module-split"

using namespace llvm;
using namespace aqcc;

static cl::opt<std::string> OutputDir("qdir", cl::Hidden);

void updateInstructionCount(Module &M, QuantumModule &QM) {

  for (auto &pair : QM.QBB_link_map) {

    // count the number of instructions in the QBB
    unsigned cnt = (pair.first->getEntryBlock()).size();

    // create the new ConstantInt argument
    Value *n_arg = dyn_cast<Value>(
        ConstantInt::getSigned(Type::getInt32Ty(M.getContext()), cnt));

    // find QRTCall and replace second argument
    for (auto &inst : *pair.second) {
      if (CallInst *CI = dyn_cast<CallInst>(&inst)) {
        if (CI->getCalledFunction() == QM.QRT_run_qbb)
          CI->setArgOperand(1, n_arg);
      }
    }
  };
}

void printQModuleToFile(QuantumModule &QM) {
  std::unique_ptr<Module> q_split =
      separateQuantumFromClassicalModule(*QM.base);

  std::error_code EC;
  raw_fd_ostream outfile(q_split->getModuleIdentifier(), EC);

  q_split->print(outfile, nullptr);
}

namespace {

struct QuantumClassicalModuleSplitLegacyPass : public ModulePass {

public:
  static char ID; // Pass identification, replacement for typeid
  QuantumModule &QM = QuantumCompilerLinkageLegacyPass::QM;

  QuantumClassicalModuleSplitLegacyPass() : ModulePass(ID) {
    initializeQuantumClassicalModuleSplitLegacyPassPass(
        *PassRegistry::getPassRegistry());
  }

  void getAnalysisUsage(AnalysisUsage &AU) const override {

    // AU.addRequired<QuantumSpinNativeToJsonWrapperPass>();
    // AU.addRequired<QuantumCompilerLinkageWrapperPass>();
    AU.setPreservesAll();
  }

  bool runOnModule(Module &M) override {
    // updateInstructionCount(M);
    printQModuleToFile(QM);
    return false;
  }
};

} // end of anonymous namespace

char QuantumClassicalModuleSplitLegacyPass::ID = 0;

INITIALIZE_PASS_BEGIN(QuantumClassicalModuleSplitLegacyPass,
                      "q-classical-module-split",
                      "Quantum classical module split legacy pass", false,
                      false)
INITIALIZE_PASS_END(QuantumClassicalModuleSplitLegacyPass,
                    "q-classical-module-split",
                    "Quantum classical module split legacy pass", false, false)

PreservedAnalyses
QuantumClassicalModuleSplitPass::run(Module &M, ModuleAnalysisManager &MAM) {
  QuantumModuleProxy QMP = MAM.getResult<QuantumCompilerLinkageAnalysis>(M);
  printQModuleToFile(*QMP.QM);

  return PreservedAnalyses::all();
}

#undef DEBUG_TYPE
