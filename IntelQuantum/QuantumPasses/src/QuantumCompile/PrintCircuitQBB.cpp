//===--------- PrintCircuitQBB.cpp - Read and Print Quantum Circuits ------===//
//
// Copyright (C) 2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//
//===----------------------------------------------------------------------===//
//
// This file implements a module pass to read quantum IR and print the circuit
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

#include "IntelQuantumPasses/QuantumCompile/PrintCircuitQBB.h"
#include "IntelQuantumPasses/QuantumAnalysis/QuantumAnnotationsToJson.h"
#include "IntelQuantumPasses/QuantumAnalysis/QuantumGateIdentifiers.h"
#include "IntelQuantumPasses/QuantumAnalysis/QuantumIterator.h"
#include "IntelQuantumPasses/QuantumAnalysis/QuantumRefs.h"
#include "IntelQuantumPasses/QuantumCompile/PrinterInterface.h"
#include "IntelQuantumPasses/QuantumCompile/ProofReader.h"
#include "llvm/IR/Module.h"
#include "llvm/IRReader/IRReader.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/SourceMgr.h"
#include "llvm/Support/raw_ostream.h"

#include <list>

#define DEBUG_TYPE "callProofReader"

using namespace llvm;

cl::list<std::string> PrintFormat("print-format", cl::desc("Printing Format"),
                                  cl::ZeroOrMore);
cl::list<std::string> OutputDir("odir", cl::desc("Output Directory"),
                                cl::ZeroOrMore);

static void PrintCircuit(QuantumModule &QM, std::string comp_pos) {
  QuantumModule::QCompPosition pos = QM.getCompilationPosition();

  if (pos < QuantumModule::kConditioned)
    errs() << "WARNING: Quantum SDK - cannot print circuit this early in the "
              "compilation: "
           << pos << "\n";
  else {
    for (auto &QK : QM) {
      for (auto &QBB : QK) {
        proofreader::ProofReader reader(QBB);
        PrinterInterface interface(&reader, comp_pos);
        reader.setCompilationPosition(pos);
        // Default print to the console
        if (PrintFormat.size() == 0) {
          interface.addOptions("console");
        } else {
          for (int i = 0; i < PrintFormat.size(); i++) {
            if (PrintFormat[i] != "console" && PrintFormat[i] != "tex" &&
                PrintFormat[i] != "json") {
              errs() << "WARNING: Quantum SDK - " << PrintFormat[i]
                     << " is an undefined printing format. Skipping ... \n";
            } else
              interface.addOptions(PrintFormat[i]);
          }
        }

        if (OutputDir.size() == 0) {
          interface.setOutputDir("");
        } else {
          interface.setOutputDir(OutputDir[0]);
        }

        interface.run();

        // The following code is for debug only
        DEBUG_WITH_TYPE("callProofReader", dbgs()
                                               << "Number of qubits in this QBB"
                                               << reader.getNumQubits() << "\n"
                                               << "Qubits used in this QBB:\n");
        for (auto &s : reader.getListOfQubits()) {
          DEBUG_WITH_TYPE("callProofReader", dbgs() << s << "\n");
        }
        DEBUG_WITH_TYPE("callProofReader", dbgs() << "\n");
      }
    }
  }
}

namespace {
struct PrintCircuitQBBLegacyPass : public ModulePass {
  static char ID;
  PrintCircuitQBBLegacyPass() : ModulePass(ID) {}
  QuantumModule &QM = QuantumCompilerLinkageLegacyPass::QM;

  bool runOnModule(Module &M) override {
    PrintCircuit(QM, "");

    return true;
  }

}; // end of struct PrinterCircuitQBB
} // end of anonymous namespace

char PrintCircuitQBBLegacyPass::ID = 0;

INITIALIZE_PASS_BEGIN(PrintCircuitQBBLegacyPass, "print-circuit-qbb",
                      "Print Quantum Circuit Per QBB Pass", false, false)
INITIALIZE_PASS_END(PrintCircuitQBBLegacyPass, "print-circuit-qbb",
                    "Print Quantum Circuit Per QBB Pass", false, false)

PreservedAnalyses PrintCircuitQBBPass::run(Module &M,
                                           ModuleAnalysisManager &MAM) {
  QuantumModuleProxy &QMP = MAM.getResult<QuantumCompilerLinkageAnalysis>(M);
  PrintCircuit(*QMP.QM, comp_pos);

  return PreservedAnalyses::all();
}

#undef DEBUG_TYPE
