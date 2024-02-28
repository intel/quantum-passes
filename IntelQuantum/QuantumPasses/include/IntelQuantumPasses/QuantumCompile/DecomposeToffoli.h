//===- DecomposeToffoli.h - reduces Toffoli to 1 and 2 qubit gates     ----===//
//
// Copyright (C) 2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//
//===----------------------------------------------------------------------===//
//
// This describes a pass to decompose Toffoli gates to CNOT, H, and T(dag)
// gates.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_QUANTUM_DECOMPOSE_TOFFOLI_H
#define LLVM_QUANTUM_DECOMPOSE_TOFFOLI_H

#include "IntelQuantumPasses/QuantumAnalysis/QuantumModule.h"
#include "llvm/IR/PassManager.h"

namespace llvm {
namespace aqcc {

class DecomposeToffoliPass : public PassInfoMixin<DecomposeToffoliPass> {
public:
  PreservedAnalyses run(Module &M, ModuleAnalysisManager &MAM);
};

} // end namespace aqcc
} // end namespace llvm

#endif // LLVM_QUANTUM_DECOMPOSE_TOFFOLI_H