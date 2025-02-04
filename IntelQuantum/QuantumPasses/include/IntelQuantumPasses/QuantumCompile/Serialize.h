//===- Serialize.h ----------------------------- ---===//
//
// Lowers Canonical to Spin native
//
//===----------------------------------------------------------------------===//
//
// Copyright (C) 2024 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//
//===----------------------------------------------------------------------===//
//
// This file defines a module pass that puts each gate operation in its
// own slice.
//
//===----------------------------------------------------------------------===//

#ifndef SERIALIZE_H
#define SERIALIZE_H

#include "IntelQuantumPasses/QuantumAnalysis/QuantumAnnotationGetters.h"
#include "IntelQuantumPasses/QuantumAnalysis/QuantumCompilerLinkage.h"
#include "IntelQuantumPasses/QuantumAnalysis/QuantumGateIdentifiers.h"
#include "IntelQuantumPasses/QuantumAnalysis/QuantumIterator.h"
#include "IntelQuantumPasses/QuantumAnalysis/QuantumModule.h"

#include "llvm/ADT/StringRef.h"
#include "llvm/IR/PassManager.h"
#include "llvm/Passes/PassBuilder.h"

#include <string>

using namespace llvm;
using namespace aqcc;

void serializeKernels(Module &M, QuantumModule &QM);

namespace llvm {

class SerializeKernelsPass : public PassInfoMixin<SerializeKernelsPass> {

public:
  PreservedAnalyses run(Module &M, ModuleAnalysisManager &MAM);
};

} // namespace llvm

#endif // SERIALIZE_H
