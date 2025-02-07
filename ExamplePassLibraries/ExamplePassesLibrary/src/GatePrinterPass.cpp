//===- GatePrinterPass.cpp ------------------------------------------------===//
//
// Copyright (C) 2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//
//===----------------------------------------------------------------------===//
//
// This implements a pass to print all gates in human readable names.
//
//===----------------------------------------------------------------------===//

#include "GatePrinterPass.h"
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
    ExampleCommandLineOpt("print_all_gates-opt", cl::init("default"),
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

PreservedAnalyses PrintAllGatesPass::run(Module &M,
                                         ModuleAnalysisManager &MAM) {
  llvm::aqcc::QuantumModuleProxy &QMP =
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

#undef DEBUG_TYPE