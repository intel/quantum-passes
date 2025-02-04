//===- XToHZHPass.cpp  ----------------------------------------------------===//
//
// Copyright (C) 2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//
//===----------------------------------------------------------------------===//
//
// This is an example pass which converts all X gates with a sequence of HZH
// gates.
//
//===----------------------------------------------------------------------===//

#include "XToHZHPass.h"
#include "IntelQuantumPasses/QuantumAnalysis/QuantumCompilerLinkage.h"
#include "IntelQuantumPasses/QuantumAnalysis/QuantumGateIdentifiers.h"
#include "IntelQuantumPasses/QuantumAnalysis/QuantumIterator.h"
#include "IntelQuantumPasses/QuantumAnalysis/QuantumModule.h"
#include "IntelQuantumPasses/QuantumUtils/QuantumGeneralUtils.h"
#include "llvm/IR/Function.h"
#include "llvm/Pass.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

static cl::opt<std::string>
    ExampleCommandLineOpt("example-pass-opt", cl::init("default"),
                          cl::desc("Example command line Option"));

#define DEBUG_TYPE "print_all_gates"

static void ConvertXToHZH(BasicBlock &QBB) {
  aqcc::QBBIter qit(QBB);

  if (!qit.isEnd()) {

    if (qit.gotoFirstGateInstance(kX))
      do {

        std::vector<aqcc::QbitRef> x_qubits = qit.getQubitOperands();
        std::vector<aqcc::QbitRef> n_qbit1(1);

        bool flag = true;
        // We will insert after to avoid ordering problems for qubit indices
        ++qit;

        // recall we need to insert the gates backward

        // H
        n_qbit1[0] = x_qubits[0];
        flag = flag && qit.insertGate(kH, n_qbit1);

        // Z
        n_qbit1[0] = x_qubits[0];
        flag = flag && qit.insertGate(kZ, n_qbit1);

        // H
        n_qbit1[0] = x_qubits[0];
        flag = flag && qit.insertGate(kH, n_qbit1);

        // delete the canonical X gate

        --qit;

        if (flag && qit.getIdentifier() == kX)
          qit.removeGate();
        else {
          displayErrorAndExit("XToHZHPass", "Lost the initial X gate.");
        }
      } while (qit.gotoNextGateInstance(kX));
  }
}

PreservedAnalyses XToHZHPass::run(Module &M, ModuleAnalysisManager &MAM) {
  llvm::aqcc::QuantumModuleProxy &QMP =
      MAM.getResult<QuantumCompilerLinkageAnalysis>(M);
  for (auto &QK : *QMP.QM) {
    for (auto &QBB : QK) {
      ConvertXToHZH(QBB);
    }
  }

  return PreservedAnalyses::all();
}

#undef DEBUG_TYPE