//===- QuantumClassicalModuleSplit.h -------------------------------------====//
//
// Copyright (C) 2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//
//===----------------------------------------------------------------------===//
//
// This file defines a Module pass which replaces simple QBB definitions with
// a declaration for that function while moving the defintions to a new Module.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_QUANTUM_CLASSICAL_MODULE_SPLIT_H
#define LLVM_QUANTUM_CLASSICAL_MODULE_SPLIT_H

#include "IntelQuantumPasses/QuantumAnalysis/QuantumModule.h"
#include "llvm/IR/PassManager.h"

namespace llvm {
namespace aqcc {

class QuantumClassicalModuleSplitPass
    : public PassInfoMixin<QuantumClassicalModuleSplitPass> {
public:
  PreservedAnalyses run(Module &M, ModuleAnalysisManager &MAM);
};

} // end of namespace aqcc
} // end of namespace llvm

#endif