//===- SerializeKernel.cpp - Serialize kernels into single slices --------===//
//
// Copyright (C) 2024 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//
//===----------------------------------------------------------------------===//
//
// This file implements a pass which serializes circuits so there is only
// one operation per time slice.
//
//===----------------------------------------------------------------------===//

#include "IntelQuantumPasses/QuantumCompile/Serialize.h"
#include "IntelQuantumPasses/QuantumAnalysis/QuantumAnnotationGetters.h"
#include "IntelQuantumPasses/QuantumAnalysis/QuantumAnnotationsToJson.h"
#include "IntelQuantumPasses/QuantumAnalysis/QuantumGateIdentifiers.h"
#include "IntelQuantumPasses/QuantumAnalysis/QuantumIterator.h"
#include "IntelQuantumPasses/QuantumUtils/QuantumGeneralUtils.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/InstIterator.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Intrinsics.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/PassManager.h"
#include "llvm/IRReader/IRReader.h"
#include "llvm/InitializePasses.h"
#include "llvm/Pass.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/SourceMgr.h"
#include "llvm/Support/raw_ostream.h"

#include <cassert>
#include <cmath>   // std::sqrt, std::pow
#include <complex> // std::conj
#include <fstream>
#include <iostream>
#include <set>
#include <sstream>
#include <string>

using namespace llvm;

#define DEBUG_TYPE "serialize-kernel"

void serializeKernels(Module &M, QuantumModule &QM) {
  for (auto &QK : QM)
    for (auto &QBB : QK)
      setSerializedSlicing(QBB);
}

namespace {

class QuantumSerializeKernelLegacyPass : public ModulePass {

public:
  static char ID;
  QuantumModule &QM = QuantumCompilerLinkageLegacyPass::QM;

  /// Minimal constructor.
  QuantumSerializeKernelLegacyPass() : ModulePass(ID) {}
  /// Destructor.
  ~QuantumSerializeKernelLegacyPass() {};

  bool doInitialization(Module &M) override {
    // add 0 const to pass Context later on.
    aqcc::ParaRef temp(0., Type::getDoubleTy(M.getContext()));
    return false;
  }

  void getAnalysisUsage(AnalysisUsage &AU) const override {

    AU.setPreservesAll();
    // AU.addRequired<QuantumAnnotationsToJsonWrapperPass>();
  }

  //
  bool runOnModule(Module &M) override {
    serializeKernels(M, QM);

    return false;
  }
};

} // namespace

////////////////////////////////////////////////////////////////////////////////

char QuantumSerializeKernelLegacyPass::ID = 0;

INITIALIZE_PASS(QuantumSerializeKernelLegacyPass, "serialize-kernel",
                "Serialize Quantum Kernels.", false, false)

////////////////////////////////////////////////////////////////////////////////

PreservedAnalyses SerializeKernelsPass::run(Module &M,
                                            ModuleAnalysisManager &MAM) {
  // duplicating action from QuantumKernelToPoPRLegacyPass::doInitialization
  QuantumModuleProxy QMP = MAM.getResult<QuantumCompilerLinkageAnalysis>(M);

  serializeKernels(M, *QMP.QM);

  return PreservedAnalyses::all();
}

#undef DEBUG_TYPE