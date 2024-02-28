//===- QuantumLoopUnrolling.cpp -----------------------------------------====//
//
// Copyright (C) 2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//
//===----------------------------------------------------------------------===//
//
// This file provides a loop pass which performs loop unrolling and resolvable
// branching only for quantum kernel
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_TRANSFORMS_INTELQUANTUM_QUANTUMLOOPUNROLLPASS_H
#define LLVM_TRANSFORMS_INTELQUANTUM_QUANTUMLOOPUNROLLPASS_H

#include "IntelQuantumPasses/QuantumAnalysis/QuantumModule.h"
#include "llvm/IR/PassManager.h"
#include "llvm/Transforms/Scalar/LoopPassManager.h"

namespace llvm {
namespace aqcc {

class QuantumLoopUnrollPass : public PassInfoMixin<QuantumLoopUnrollPass> {
  const int OptLevel;

  /// If false, use a cost model to determine whether unrolling of a loop is
  /// profitable. If true, only loops that explicitly request unrolling via
  /// metadata are considered. All other loops are skipped.
  const bool OnlyWhenForced;

  /// If true, forget all loops when unrolling. If false, forget top-most loop
  /// of the currently processed loops, which removes one entry at a time from
  /// the internal SCEV records. For large loops, the former is faster.
  const bool ForgetSCEV;

public:
  explicit QuantumLoopUnrollPass(int OptLevel = 2, bool OnlyWhenForced = false,
                                 bool ForgetSCEV = false)
      : OptLevel(OptLevel), OnlyWhenForced(OnlyWhenForced),
        ForgetSCEV(ForgetSCEV) {}

  PreservedAnalyses run(Loop &L, LoopAnalysisManager &LAM,
                        LoopStandardAnalysisResults &AR, LPMUpdater &Updater);
};

} // end namespace aqcc
} // end namespace llvm

#endif // for header guard LLVM_TRANSFORMS_INTELQUANTUM_QUANTUMLOOPUNROLLPASS_H
