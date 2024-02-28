//===- SpinNativeAnglesInRange.h ------------------------------------------===//
//
// Updates out of range angles to in range equivalent
//
//===----------------------------------------------------------------------===//
//
// Copyright (C) 2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//
//===----------------------------------------------------------------------===//
//
// This file describes a Module pass which replaces out-of-range parametric
// angles to their [0,2Pi) in-range equivalent assuming these are periodic such
// such that angle ~= angle + 2n Pi. This is reasonble since this pass is
// primary run after all all gates are reduced to native gates for which this
// should hold.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_QUANTUM_SPIN_NATIVE_ANGLES_IN_RANGE_H
#define LLVM_QUANTUM_SPIN_NATIVE_ANGLES_IN_RANGE_H

#include "IntelQuantumPasses/QuantumAnalysis/QuantumModule.h"
#include "llvm/IR/PassManager.h"

namespace llvm {
namespace aqcc {

class SpinNativeAnglesInRangePass
    : public PassInfoMixin<SpinNativeAnglesInRangePass> {
public:
  PreservedAnalyses run(Module &M, ModuleAnalysisManager &MAM);
};

} // end namespace aqcc
} // end namespace llvm

#endif // LLVM_QUANTUM_SPIN_NATIVE_ANGLES_IN_RANGE_H