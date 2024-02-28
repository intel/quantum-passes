//===- quantum-module-stage.cpp ---------------------------*- C++ -*-===//
//
//
// Copyright (C) 2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//
//===----------------------------------------------------------------------===//
/// \file "quantum-module-stage.cpp"
///
/// This tool uses the QuantumModule to detect which stage of compilation
/// the program is currently in.  It reports it as a value relating to the
/// enum in QuantumModule.h
///
//===----------------------------------------------------------------------===//

#include "llvm/IRReader/IRReader.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/InitLLVM.h"
#include "llvm/Support/SourceMgr.h"
#include "llvm/Support/ToolOutputFile.h"

#include "IntelQuantumPasses/QuantumAnalysis/QuantumAnnotationsToJson.h"
#include "IntelQuantumPasses/QuantumAnalysis/QuantumCompilerLinkage.h"
#include "IntelQuantumPasses/QuantumAnalysis/QuantumModule.h"
#include "IntelQuantumPasses/QuantumAnalysis/QuantumSpinNativeToJson.h"

#include "IntelQuantumPasses/QuantumUtils/QuantumGeneralUtils.h"

static cl::opt<std::string> InputSourceFile(cl::Positional,
                                            cl::desc("<Source file>"),
                                            cl::init("-"),
                                            cl::value_desc("filename"));

static cl::opt<int> CheckStage("s", cl::desc("<stage>"), cl::init(-1));

int main(int argc, const char *argv[]) {
  InitLLVM X(argc, argv);

  cl::ParseCommandLineOptions(argc, argv,
                              "Intel Quantum Module Compilation Stage\n");

  LLVMContext CurrContext;
  SMDiagnostic Err;
  std::unique_ptr<Module> ModuleToAnalyze =
      parseIRFile(InputSourceFile, Err, CurrContext);
  if (!ModuleToAnalyze) {
    Err.print(argv[0], errs());
    return -1;
  }

  // Intialize the module and determine the stage.
  QuantumCompilerLinkage QCL;
  QuantumSpinNativeToJson QSNJ;
  QuantumModule QM;
  QuantumAnnotationsToJson QAJ;
  QAJ.doInitialization(*ModuleToAnalyze, QM);
  QSNJ.addSpinNativeFunctions(*ModuleToAnalyze, QM);
  try {
    llvm::updateCompilerLinkage(*ModuleToAnalyze, QM, true);
  } catch (...) {
    displayErrorAndExit("Quantum Compilation Stage Detection",
                        "Unable to parse JSON files or Module passed in.");
  }

  std::error_code EC;
  std::unique_ptr<ToolOutputFile> Out(
      new ToolOutputFile("-", EC, sys::fs::OF_None));
  if (EC) {
    errs() << argv[0] << ": " << EC.message() << '\n';
    return -1;
  }

  // Output the position or output a 1 if the current stage
  // is past the requested stage.
  if (CheckStage < 0)
    Out->os() << QM.pos << "\n";
  else
    Out->os() << (CheckStage >= QM.pos) << "\n";
}