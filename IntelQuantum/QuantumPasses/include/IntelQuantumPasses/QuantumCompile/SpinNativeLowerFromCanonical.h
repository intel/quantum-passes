//===- SpinNativeLowerFromCanonicalPass.h ----------------------------- ---===//
//
// Lowers Canonical to Spin native
//
//===----------------------------------------------------------------------===//
//
// Copyright (C) 2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//
//===----------------------------------------------------------------------===//
//
// This file defines a module pass which replaces canonical gates with their
// spin native equivalent (non-IMM).
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_SPIN_NATIVE_LOWER_FROM_CANONICAL_H
#define LLVM_SPIN_NATIVE_LOWER_FROM_CANONICAL_H

#include "IntelQuantumPasses/QuantumAnalysis/QuantumAnnotationGetters.h"
#include "IntelQuantumPasses/QuantumAnalysis/QuantumCompilerLinkage.h"
#include "IntelQuantumPasses/QuantumAnalysis/QuantumGateIdentifiers.h"
#include "IntelQuantumPasses/QuantumAnalysis/QuantumIterator.h"
#include "IntelQuantumPasses/QuantumAnalysis/QuantumModule.h"
#include "llvm/ADT/StringRef.h"

#include "llvm/IR/PassManager.h"
#include "llvm/Passes/PassBuilder.h"

#include <string>

using namespace llvm;
using namespace aqcc;

namespace llvm {

class SpinNativeLowerFromCanonical {
  const unsigned SLICE_BEG = 1;
  const unsigned SLICE_MID = 0;
  const unsigned SLICE_END = 2;

public:
  void addDecomposedRz(double angle, std::vector<aqcc::ParaRef> &NParas,
                       Type *TY, bool &Flag, aqcc::QBBIter &Qit,
                       std::vector<aqcc::QbitRef> &Qubits,
                       aqcc::ParaRef MidSlice);
  bool lowerFromCanonical(BasicBlock &BB, QuantumModule &QM);
  void cleanUpGateDefs(QuantumModule &QM);
};

class SpinNativeLowerFromCanonicalPass
    : public PassInfoMixin<SpinNativeLowerFromCanonicalPass> {

  SpinNativeLowerFromCanonical SN;

public:
  PreservedAnalyses run(Module &M, ModuleAnalysisManager &MAM);
};

} // namespace llvm

#endif // LLVM_SPIN_NATIVE_LOWER_FROM_CANONICAL_H