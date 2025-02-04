//===- DecomposeToffoli.cpp - reduces Toffoli to 1 and 2 qubit gates     --===//
//
// Copyright (C) 2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//
//===----------------------------------------------------------------------===//
//
// This implements a pass to decompose Toffoli gates to CNOT, H, and T(dag)
// gates.
//
//===----------------------------------------------------------------------===//

#include "IntelQuantumPasses/QuantumCompile/DecomposeToffoli.h"
#include "IntelQuantumPasses/QuantumAnalysis/QuantumAnnotationsToJson.h"
#include "IntelQuantumPasses/QuantumAnalysis/QuantumGateIdentifiers.h"
#include "IntelQuantumPasses/QuantumAnalysis/QuantumIterator.h"
#include "IntelQuantumPasses/QuantumAnalysis/QuantumRefs.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Pass.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

#define DEBUG_TYPE "decompose_toffoli"

static void decomposeAllToffoli(BasicBlock &QBB) {

  aqcc::QBBIter qit(QBB);

  if (!qit.isEnd()) {

    // This implements the following decomposition
    //
    // ctrl 1  --------------*--------------*-----*---T----*--     --*--
    //                       |              |     |        |         |
    // ctrl 2  -----*--------)-----*--------)--T--X--Tdag--X--  =  --*--
    //              |        |     |        |                        |
    // target  --H--X--Tdag--X--T--X--Tdag--X--T--H-----------     --X--

    if (qit.gotoFirstGateInstance(kTOFF))
      do {

        std::vector<aqcc::QbitRef> toff_qbits = qit.getQubitOperands();
        std::vector<aqcc::QbitRef> n_qbit1(1);
        std::vector<aqcc::QbitRef> n_qbit2(2);

        bool flag = true;
        // We will insert after to avoid ordering problems for qubit indices
        ++qit;

        // recall we need to insert the gates backward

        // CX ctl1 ctrl2
        n_qbit2[0] = toff_qbits[0];
        n_qbit2[1] = toff_qbits[1];
        flag = flag && qit.insertGate(kCX, n_qbit2);

        // Tdag ctrl2
        n_qbit1[0] = toff_qbits[1];
        flag = flag && qit.insertGate(kTDAG, n_qbit1);

        // T ctrl1
        n_qbit1[0] = toff_qbits[0];
        flag = flag && qit.insertGate(kT, n_qbit1);

        // CX ctl1 ctrl2
        n_qbit2[0] = toff_qbits[0];
        n_qbit2[1] = toff_qbits[1];
        flag = flag && qit.insertGate(kCX, n_qbit2);

        // H target
        n_qbit1[0] = toff_qbits[2];
        flag = flag && qit.insertGate(kH, n_qbit1);

        // T ctrl2
        n_qbit1[0] = toff_qbits[1];
        flag = flag && qit.insertGate(kT, n_qbit1);

        // T target
        n_qbit1[0] = toff_qbits[2];
        flag = flag && qit.insertGate(kT, n_qbit1);

        // CX ctl1 target
        n_qbit2[0] = toff_qbits[0];
        n_qbit2[1] = toff_qbits[2];
        flag = flag && qit.insertGate(kCX, n_qbit2);

        // Tdag target
        flag = flag && qit.insertGate(kTDAG, n_qbit1);

        // CX ctl2 target
        n_qbit2[0] = toff_qbits[1];
        n_qbit2[1] = toff_qbits[2];
        flag = flag && qit.insertGate(kCX, n_qbit2);

        // T target
        flag = flag && qit.insertGate(kT, n_qbit1);

        // CX ctl1 target
        n_qbit2[0] = toff_qbits[0];
        n_qbit2[1] = toff_qbits[2];
        flag = flag && qit.insertGate(kCX, n_qbit2);

        // Tdag target
        flag = flag && qit.insertGate(kTDAG, n_qbit1);

        // CX ctl2 target
        n_qbit2[0] = toff_qbits[1];
        n_qbit2[1] = toff_qbits[2];
        flag = flag && qit.insertGate(kCX, n_qbit2);

        // H target
        flag = flag && qit.insertGate(kH, n_qbit1);

        // delete the canonical Toffoli

        --qit;

        if (flag && qit.getIdentifier() == kTOFF)
          qit.removeGate();
        else {
          errs() << "Decompose Toffoli Pass Error: Lost the canonical "
                    "Toffoli Gate.\n";
          assert(false);
        }
      } while (qit.gotoNextGateInstance(kTOFF));
  }
} // decomposeAllToffoli

namespace {

struct DecomposeToffoliLegacyPass : public ModulePass {

  static char ID; // Pass identification, replacement for typeid
  QuantumModule &QM = QuantumCompilerLinkageLegacyPass::QM;

  DecomposeToffoliLegacyPass() : ModulePass(ID) {}

  void getAnalysisUsage(AnalysisUsage &AU) const override {

    AU.setPreservesAll();
    AU.addRequired<QuantumAnnotationsToJsonWrapperPass>();
  }

  bool runOnModule(Module &M) override {
    for (auto &QK : QM) {
      for (auto &QBB : QK) {
        decomposeAllToffoli(QBB);
      }
    }
    return false;
  }

}; // end of struct DecomposeToffoliLegacyPass

} // end of anonymous namespace

char DecomposeToffoliLegacyPass::ID = 0;

INITIALIZE_PASS(DecomposeToffoliLegacyPass, "decompose-toff",
                "DecomposeToffoliLegacyPass", false, false)

PreservedAnalyses DecomposeToffoliPass::run(Module &M,
                                            ModuleAnalysisManager &MAM) {
  QuantumModuleProxy &QMP = MAM.getResult<QuantumCompilerLinkageAnalysis>(M);
  for (auto &QK : *QMP.QM) {
    for (auto &QBB : QK) {
      decomposeAllToffoli(QBB);
    }
  }

  return PreservedAnalyses::all();
}

#undef DEBUG_TYPE
