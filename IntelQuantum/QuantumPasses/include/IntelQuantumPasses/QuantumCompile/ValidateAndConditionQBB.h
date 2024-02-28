//===- ValidateAndConditionQBB.cpp -------------------------------------====//
//
// Copyright (C) 2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//
//===----------------------------------------------------------------------===//
//
// This file defines a function pass which checks for validity of each quantum
// kernels and condition each hybrid quantum basic block (QBB) into three
// segments:
//      1) input classical data (gate parameter) manipulation
//      2) the simple QBB which is a uninterupted sequence of quantum gates
//      3) output classical data (measurements) manipulation
//
// Validity is determined by current version of validateQuantumKernel function
// in the file "QuantumUtils/QuantumGeneralUtils.h"
//
//  Transformation dependency:
//      - quantum kernel unrolling and inlining to resolve qubit arguments
//  Analysis dependency:
//      - QuantumAnnotationsToJsonPass
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_VALIDATE_AND_CONDITION_QBB_H
#define LLVM_VALIDATE_AND_CONDITION_QBB_H

#include "IntelQuantumPasses/InitializeQuantumPasses.h"
#include "IntelQuantumPasses/QuantumAnalysis/QuantumModule.h"
#include "IntelQuantumPasses/QuantumAnalysis/QuantumRefs.h"
#include "IntelQuantumPasses/QuantumAnalysis/QuantumSpinNativeToJson.h"
#include "IntelQuantumPasses/QuantumUtils/QuantumGeneralUtils.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/GlobalVariable.h"
#include "llvm/IR/Intrinsics.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/Operator.h"

#include "llvm/Pass.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;
using namespace aqcc;

#define DEBUG_TYPE "validate-and-condition-qbb"

struct ValidateAndConditionQBB {

public:
  SmallVector<Function *, 2> meas_ctrl_functs;
  std::map<Function *, Instruction *> release_calls;

  void conditionIntoQBBs(Function &F, std::vector<QBBIter> &F_gates);

  /// locates and store both the measurement control intrinsic functions
  /// and all their calls.
  void locateMeasurementControlInst(Module &M);

  /// This function removes all calls to measurement control intrinsics
  // TODO: if these intrinsics have backend QuISA support, remove the use
  //      of this fuction.
  void cleanUpMeasurementControlInst(Module &M);

  bool conditionQBB(Module &M, QuantumModule &QM);

}; // end of struct ValidateAndConditionQBBPass

class ValidateAndConditionQBBPass
    : public PassInfoMixin<ValidateAndConditionQBBPass> {
public:
  ValidateAndConditionQBB QBB;

  PreservedAnalyses run(Module &M, ModuleAnalysisManager &MAM);
};

struct ValidateAndConditionQBBLegacyPass : public ModulePass {

public:
  static char ID; // Pass identification, replacement for typeid

  QuantumModule &QM = QuantumCompilerLinkageLegacyPass::QM;
  ValidateAndConditionQBB QBB;

  ValidateAndConditionQBBLegacyPass() : ModulePass(ID) {
    initializeValidateAndConditionQBBLegacyPassPass(
        *PassRegistry::getPassRegistry());
  }

  ~ValidateAndConditionQBBLegacyPass() {}

  void getAnalysisUsage(AnalysisUsage &AU) const override {

    AU.setPreservesAll();
    AU.addRequired<QuantumAnnotationsToJsonWrapperPass>();
  }

  bool runOnModule(Module &M) override;

}; // end of struct ValidateAndConditionQBBLegacyPass

#undef DEBUG_TYPE
#endif
