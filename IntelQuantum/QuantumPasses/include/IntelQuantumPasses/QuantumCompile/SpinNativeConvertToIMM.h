//===- SpinNativeConvertToIMM.h ------------------------------------------====//
//
// Copyright (C) 2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//
//===----------------------------------------------------------------------===//
//
// This file defines a module pass which converts all spin native gates to
// their IMM version.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_QUANTUM_SPIN_NATIVE_CONVERT_TO_IMM_H
#define LLVM_QUANTUM_SPIN_NATIVE_CONVERT_TO_IMM_H

#include "IntelQuantumPasses/QuantumAnalysis/QuantumModule.h"
#include "llvm/IR/PassManager.h"

namespace llvm {
namespace aqcc {

class SpinNativeConvertToIMMPass
    : public PassInfoMixin<SpinNativeConvertToIMMPass> {
public:
  PreservedAnalyses run(Module &M, ModuleAnalysisManager &MAM);
};

} // end namespace aqcc
} // end namespace llvm

#endif // LLVM_QUANTUM_SPIN_NATIVE_CONVERT_TO_IMM_H