//===- SpinNativeAnglesInRange.cpp - Updates out of range angles ----------===//
//
// Copyright (C) 2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//
//===----------------------------------------------------------------------===//
//
// This file implements a Module pass which replaces out-of-range parametric
// angles to their [0,2Pi) in-range equivalent assuming these are periodic such
// such that angle ~= angle + 2n Pi. This is reasonable since this pass is
// primary run after all all gates are reduced to native gates for which this
// should hold.
//
//===----------------------------------------------------------------------===//

#include "IntelQuantumPasses/QuantumCompile/SpinNativeAnglesInRange.h"
#include "IntelQuantumPasses/InitializeQuantumPasses.h"
#include "IntelQuantumPasses/QuantumAnalysis/QuantumCompilerLinkage.h"
#include "IntelQuantumPasses/QuantumAnalysis/QuantumGateIdentifiers.h"
#include "IntelQuantumPasses/QuantumAnalysis/QuantumIterator.h"
#include "IntelQuantumPasses/QuantumAnalysis/QuantumModule.h"
#include "IntelQuantumPasses/QuantumAnalysis/QuantumSpinNativeToJson.h"
#include "IntelQuantumPasses/QuantumUtils/QuantumGeneralUtils.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Pass.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;
using namespace aqcc;

#define DEBUG_TYPE "spin_angles_in_range"

namespace {

static void spinNativeAnglesInRange(Module &M, QuantumModule &QM) {
  for (auto &QK : QM) {
    for (auto &QBB : QK) {

      for (QBBIter qit(QBB); !qit.isEnd(); ++qit) {
        // kSpinBegin is max non-native identifier value
        if (qit.getIdentifier() >= kSpinBegin) {

          std::vector<ParaRef> angles = qit.getParametricOperands();

          for (int i = 0; i < angles.size(); i++) {

            if (angles[i].isResolved()) {
              if (angles[i].isFloatingPoint()) { // only need for FP angles
                double value = angles[i].getValueResolved().value();
                double multi = std::floor(value / FP_2PI);
                angles[i] -= multi * FP_2PI;
              }
            }
          };

          // Special case for RXY gate
          if ((qit.getIdentifier() == kSpinRotXY)) {
            // If either of the two angles is not resolved, let runtime put
            // the angles in range
            if (angles[0].isResolved() && angles[1].isResolved()) {
              if (angles[0].getValueResolved().value() > FP_PI) {
                // need angles[0] = FP_2PI- angles[0]
                angles[0] -= FP_2PI;
                angles[0] = -angles[0];
                if (angles[1].getValueResolved().value() < FP_PI)
                  angles[1] += FP_PI;
                else
                  angles[1] -= FP_PI;
              }
            }
          }

          qit.setParametricOperands(angles);

        } else { // is native?
          displayErrorAndExit(
              "Spin Native Angles In Range Pass",
              "Found an unrecognized gate " +
                  std::to_string(qit.getIdentifier()) + " in function '" +
                  DemangleQuantumKernelName(QK.getFunction()->getName()) +
                  "'.");
        }
      } // for qit
    } // for QBBs
  }

  QM.setAnglesInRange();
}

struct SpinNativeAnglesInRangeLegacyPass : public ModulePass {

  static char ID; // Pass identification, replacement for typeid

  QuantumModule &QM = QuantumCompilerLinkageLegacyPass::QM;

  SpinNativeAnglesInRangeLegacyPass() : ModulePass(ID) {
    initializeSpinNativeAnglesInRangeLegacyPassPass(
        *PassRegistry::getPassRegistry());
  }

  void getAnalysisUsage(AnalysisUsage &AU) const override {

    AU.setPreservesAll();
    // AU.addRequired<QuantumSpinNativeToJsonWrapperPass>();
    // AU.addRequired<QuantumCompilerLinkageWrapperPass>();
  }

  bool runOnModule(Module &M) override {
    spinNativeAnglesInRange(M, QM);
    return false;

  } // runOnFunction

}; // end of struct SpinNativeAnglesInRangePass

} // end of anonymous namespace

char SpinNativeAnglesInRangeLegacyPass::ID = 0;

INITIALIZE_PASS_BEGIN(SpinNativeAnglesInRangeLegacyPass, "spin-angles-in-range",
                      "Spin native angles in range legacy pass", false, false)
INITIALIZE_PASS_END(SpinNativeAnglesInRangeLegacyPass, "spin-angles-in-range",
                    "Spin native angles in range legacy pass", false, false)

PreservedAnalyses SpinNativeAnglesInRangePass::run(Module &M,
                                                   ModuleAnalysisManager &MAM) {
  QuantumModuleProxy &QMP = MAM.getResult<QuantumCompilerLinkageAnalysis>(M);
  spinNativeAnglesInRange(M, *QMP.QM);
  return PreservedAnalyses::all();
}

#undef DEBUG_TYPE