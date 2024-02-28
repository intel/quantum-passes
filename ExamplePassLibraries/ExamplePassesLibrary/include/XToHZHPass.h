//===- XToHZHPass.h -------------------------------------------------------===//
//
// Copyright (C) 2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_XTOHZH_PASS_H
#define LLVM_XTOHZH_PASS_H

#include "llvm/IR/PassManager.h"

namespace llvm {

class XToHZHPass : public PassInfoMixin<XToHZHPass> {
public:
  PreservedAnalyses run(Module &M, ModuleAnalysisManager &MAM);
};

} // end namespace llvm

#endif // LLVM_XTOHZH_PASS_H