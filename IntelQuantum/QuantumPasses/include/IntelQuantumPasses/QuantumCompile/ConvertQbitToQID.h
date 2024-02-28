//===- ConvertQbitToQID.h ------------------------------------------------====//
//
// Copyright (C) 2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//
//===----------------------------------------------------------------------===//
//
// This file defines a module pass which converts all qubit gate arguments to
// QID values.
// ASSUMPTION: this assumes a previous pass (namely scheduling) has converted
// all qbits to a single qubit register.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_QUANTUM_CONVERT_QBIT_TO_QID_H
#define LLVM_QUANTUM_CONVERT_QBIT_TO_QID_H

#include "IntelQuantumPasses/QuantumAnalysis/QuantumModule.h"
#include "llvm/IR/PassManager.h"

namespace llvm {
namespace aqcc {

class ConvertQbitToQIDPass : public PassInfoMixin<ConvertQbitToQIDPass> {
public:
  PreservedAnalyses run(Module &M, ModuleAnalysisManager &MAM);
};

} // end namespace aqcc
} // end namespace llvm

#endif // LLVM_QUANTUM_CONVERT_QBIT_TO_QID_H