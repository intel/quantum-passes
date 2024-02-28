//===- FinalizeQuantumCompilation.h -------------------------------------====//
//
// Copyright (C) 2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//
//===----------------------------------------------------------------------===//
//
// This file implements a module pass which adds the final QRT call to pass:
//    -QSDA
//    -QBBS section
//    -QBBS_TEXT section
//    -JSON file name (through command line)
//
// This should be the last pass called before sending IR into the rest of Clang
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_TRANSFORMS_INTELQUANTUM_FINALIZEQUANTUMCOMPILATION_H
#define LLVM_TRANSFORMS_INTELQUANTUM_FINALIZEQUANTUMCOMPILATION_H

#include "IntelQuantumPasses/QuantumAnalysis/QuantumModule.h"
#include "llvm/IR/PassManager.h"

namespace llvm {
namespace aqcc {

class FinalizeQuantumCompilationPass
    : public PassInfoMixin<FinalizeQuantumCompilationPass> {
public:
  PreservedAnalyses run(Module &M, ModuleAnalysisManager &MAM);
};

} // end of namespace aqcc
} // end of namespace llvm

#endif
