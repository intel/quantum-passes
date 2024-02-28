//===- SeparateAndReplaceQBB.h -------------------------------------====//
//
// Copyright (C) 2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//
//===----------------------------------------------------------------------===//
//
// This file defines a Module pass which extracts the simple QBBs from the
// quantum kernels, inserts them into a new stand-alone function, and replaces
// the simple QBB with a QRT call. The qbb_link_map is then populated to link
// the two.
//
//  Transformation dependency:
//      - quantum kernel unrolling and inlining to resolve qubit arguments
//      - SplitUnresolvedParameterPass
//      - ValidateAndConditionQBBPass
//  Analysis dependency:
//      - QuantumCompilerLinkagePass
//===----------------------------------------------------------------------===//

#ifndef LLVM_SEPARATE_AND_REPLACE_QBB_H
#define LLVM_SEPARATE_AND_REPLACE_QBB_H

#include "IntelQuantumPasses/QuantumAnalysis/QuantumModule.h"
#include "llvm/IR/PassManager.h"

namespace llvm {
namespace aqcc {

class SeparateAndReplaceQBBPass
    : public PassInfoMixin<SeparateAndReplaceQBBPass> {
public:
  PreservedAnalyses run(Module &M, ModuleAnalysisManager &MAM);
};

} // end namespace aqcc
} // end namespace llvm

#endif
