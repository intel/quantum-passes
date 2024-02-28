//===- QuantumKernelStatsPrint.h - Print approx depth to screen -----------===//
//
// Copyright (C) 2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//
//===----------------------------------------------------------------------===//
//
// This file describes a pass which prints an approximate depth to screen.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_QUANTUM_QUANTUM_KERNEL_STATS_PRINT_H
#define LLVM_QUANTUM_QUANTUM_KERNEL_STATS_PRINT_H

#include "IntelQuantumPasses/QuantumAnalysis/QuantumModule.h"
#include "llvm/IR/PassManager.h"


namespace llvm {
namespace aqcc {

class QuantumKernelStatsPrintPass : public PassInfoMixin<QuantumKernelStatsPrintPass> {
public:
  PreservedAnalyses run(Module &M, ModuleAnalysisManager &MAM);
};

} // end namespace aqcc
} // end namespace llvm



#endif // LLVM_QUANTUM_QUANTUM_KERNEL_STATS_PRINT_H