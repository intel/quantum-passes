
//===- PrintCircuitQBB.h - Read and Print Quantum Circuits ----------------===//
//
// Copyright (C) 2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//
//===----------------------------------------------------------------------===//
//
// This file describes a module pass to read quantum IR and print the circuit
//   representation. Printing is done on a per QBB basis.
//
// The purpose is to offer an example of using the Intel QIter class to treat
//   code that represents a quantum circuit, as well as to offer examples of
//   llvm idioms.
//
// Printing options available:
//   * ASCII - this has the merit of giving immediate feedback with no extra
//       implementation, redirectable at commandline, and comes with a retro
//       computing vibe.
//   * Tex - this has the merit of producing publication ready diagrams. We
//       produce the tex instructions which the user can render into pdf or
//       use in their paper as they see fit.
//===----------------------------------------------------------------------===//

#ifndef LLVM_QUANTUM_PRINT_CIRCUIT_QBB_H
#define LLVM_QUANTUM_PRINT_CIRCUIT_QBB_H

#include "IntelQuantumPasses/QuantumAnalysis/QuantumModule.h"
#include "llvm/IR/PassManager.h"

namespace llvm {
namespace aqcc {

class PrintCircuitQBBPass : public PassInfoMixin<PrintCircuitQBBPass> {
private:
  std::string comp_pos = "";

public:
  PrintCircuitQBBPass(std::string position = "") { comp_pos = position; }
  PreservedAnalyses run(Module &M, ModuleAnalysisManager &MAM);
};

} // end namespace aqcc
} // end namespace llvm

#endif // LLVM_QUANTUM_PRINT_CIRCUIT_QBB_H