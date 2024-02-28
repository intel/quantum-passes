//===- FlattenQuantumKernel.h --------------------------------------------====//
//
// Copyright (C) 2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//
//===----------------------------------------------------------------------===//
//
// This file defines a module pass which inlines quantum kernel calls which
// are inside other quantum kernels.
//
//===----------------------------------------------------------------------===//

#include "llvm/IR/PassManager.h"

namespace llvm {

class InsertQuantumIntrinsicsPass
    : public PassInfoMixin<InsertQuantumIntrinsicsPass> {
public:
  PreservedAnalyses run(Module &M, ModuleAnalysisManager &AM);
};

} // namespace llvm
