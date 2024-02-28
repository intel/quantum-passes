//===- SimpleQbitPlacement.h ---------------------------------------------====//
//
// Copyright (C) 2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//
//===----------------------------------------------------------------------===//
//
// This file defines a module pass for the simplest qubit placement. It creates
// a single placement for all qubits with a first-come first assigned protocol
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_QUANTUM_SIMPLE_QBIT_PLACEMENT_H
#define LLVM_QUANTUM_SIMPLE_QBIT_PLACEMENT_H

#include "IntelQuantumPasses/QuantumAnalysis/QuantumModule.h"
#include "IntelQuantumPasses/QuantumUtils/QuantumGeneralUtils.h"
#include "llvm/IR/PassManager.h"

namespace llvm {
namespace aqcc {

class SimpleQbitPlacementPass : public PassInfoMixin<SimpleQbitPlacementPass> {
public:
  PreservedAnalyses run(Module &M, ModuleAnalysisManager &MAM);
};

} // namespace aqcc
} // namespace llvm

#endif // LLVM_QUANTUM_SIMPLE_QBIT_PLACEMENT_H