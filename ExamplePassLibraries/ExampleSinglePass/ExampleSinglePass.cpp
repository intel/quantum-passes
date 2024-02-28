//===- ExampleSinglePass.cpp ----------------------------------------------===//
//
// Copyright (C) 2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//
//===----------------------------------------------------------------------===//
//
// This is a single pass and plugin implementation of a pass that prints
// the human readable version of all the gates in a circuit.
//
//===----------------------------------------------------------------------===//

#include "IntelQuantumPasses/QuantumAnalysis/QuantumCompilerLinkage.h"
#include "IntelQuantumPasses/QuantumAnalysis/QuantumGateIdentifiers.h"
#include "IntelQuantumPasses/QuantumAnalysis/QuantumIterator.h"
#include "IntelQuantumPasses/QuantumAnalysis/QuantumModule.h"
#include "llvm/IR/Function.h"
#include "llvm/Pass.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

static cl::opt<std::string>
    ExampleCommandLineOpt("example-pass-opt", cl::init("default"),
                          cl::desc("Example command line Option"));

#define DEBUG_TYPE "print_all_gates"

static void printAllGates(BasicBlock &QBB) {
  aqcc::QBBIter qit(QBB);
  errs() << "Gates in " << QBB.getName() << ":\n";
  while (!qit.isEnd()) {
    int gateIdentifier = qit.getIdentifier();
    errs() << GetNameFromGateIdentifier(gateIdentifier) << "\n";
    ++qit;
  }
}

class PrintAllGatesExamplePass
    : public PassInfoMixin<PrintAllGatesExamplePass> {
public:
  PreservedAnalyses run(Module &M, ModuleAnalysisManager &MAM) {
    llvm::aqcc::QuantumModuleProxy QMP =
        MAM.getResult<QuantumCompilerLinkageAnalysis>(M);
    errs() << "Running Example Pass, using \"" << ExampleCommandLineOpt
           << "\" as the command line argument.\n";
    for (auto &QK : *QMP.QM) {
      for (auto &QBB : QK) {
        printAllGates(QBB);
      }
    }

    return PreservedAnalyses::all();
  }
};

llvm::PassPluginLibraryInfo getExampleSinglePassPluginInfo() {
  return {LLVM_PLUGIN_API_VERSION, "ExampleSinglePassPlugin",
          LLVM_VERSION_STRING, [](PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
                [](StringRef Name, ModulePassManager &MPM,
                   ArrayRef<PassBuilder::PipelineElement>) {
                  if (Name == "print-all-gates-example") {
                    MPM.addPass(PrintAllGatesExamplePass());
                    return true;
                  }
                });
          }};
}

extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo() {
  return getExampleSinglePassPluginInfo();
}

#undef DEBUG_TYPE