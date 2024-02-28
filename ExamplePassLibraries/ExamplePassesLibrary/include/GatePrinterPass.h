//===- GatePrinterPass.h --------------------------------------------------===//
//
// Copyright (C) 2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_PRINTER_PASS_H
#define LLVM_PRINTER_PASS_H

#include "llvm/IR/PassManager.h"

namespace llvm {

class PrintAllGatesPass : public PassInfoMixin<PrintAllGatesPass> {
public:
  PreservedAnalyses run(Module &M, ModuleAnalysisManager &MAM);
};

} // end namespace llvm

#endif // LLVM_PRINTER_PASS_H