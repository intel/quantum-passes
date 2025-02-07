//===- QuantumCompilerLinkageTest.cpp -----------------------------*- C++
//-*-===//
//
//===----------------------------------------------------------------------===//
//
// Copyright (C) 2022 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//
//===----------------------------------------------------------------------===//
/// \file "QuantumCompilerLinkageTest.cpp"
///
/// This file implements unit test for the QuantumCompilerLinkagePass
///
//===----------------------------------------------------------------------===//

#include "IntelQuantumPasses/QuantumAnalysis/QuantumCompilerLinkage.h"

#include "IntelQuantumPasses/QuantumAnalysis/QuantumModule.h"
#include "llvm/AsmParser/Parser.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRPrintingPasses.h"
#include "llvm/IR/InstIterator.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/PassManager.h"
#include "llvm/IRPrinter/IRPrintingPasses.h"
#include "llvm/IRReader/IRReader.h"
#include "llvm/InitializePasses.h"
#include "llvm/Pass.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/SourceMgr.h"
#include "llvm/Support/raw_ostream.h"

#include "gtest/gtest.h"

#include <map>

#include "QuantumPasses/QuantumAnalysis/QuantumCompilerLinkageTest.hpp"

using namespace llvm;

static std::unique_ptr<Module> parseIR(LLVMContext &C, const char *IR) {
  SMDiagnostic Err;
  std::unique_ptr<Module> Mod = parseAssemblyString(IR, Err, C);
  if (!Mod)
    Err.print("QuantumCompilerLinkageTest", errs());
  return Mod;
}

static void runPostPasses(Module &M) {
  ModuleAnalysisManager MAM;
  ModulePassManager MPM;
  PassBuilder PB;
  LoopAnalysisManager LAM;
  FunctionAnalysisManager FAM;
  CGSCCAnalysisManager CGSSCAM;

  PB.registerModuleAnalyses(MAM);
  PB.registerCGSCCAnalyses(CGSSCAM);
  PB.registerFunctionAnalyses(FAM);
  PB.registerLoopAnalyses(LAM);
  PB.crossRegisterProxies(LAM, FAM, CGSSCAM, MAM);
  MPM.addPass(PrintModulePass(errs()));
  MPM.run(M, MAM);
}

///////////////////////////////////////////////////////////////////////////////////

TEST(QuantumCompilerLinkageTest, BasicTest) {

  LLVMContext C;
  std::unique_ptr<Module> M = parseIR(C, *&Code);

  QuantumCompilerLinkage QCL;
  QuantumModule qm;
  QCL.doInitialization(*M, qm);
  QCL.locateOrDeclareQRTFunctions(*M, qm);
  qm.setSure();
  QCL.checkForConditioning(*M, qm);
  QCL.checkForSeparation(*M, qm);
  QCL.findMaxQSDAUsage(*M, qm);

  for (auto &F : *M) {

    if (F.getName() == "_ZN4qrt021initializeQrtSystem64EPdPcS1_j")
      EXPECT_TRUE(&F == qm.QRT_initialize);
    else if (F.getName() == "_ZN4qrt013isInitializedEv")
      EXPECT_TRUE(&F == qm.QRT_is_initialized);
    else if (F.getName() == "_ZN4qrt020updateMeasurementMapEPbmb")
      EXPECT_TRUE(&F == qm.QRT_measure_move);
    else if (F.getName() == "_ZN4qrt014scheduleKernelEPFvvEj")
      EXPECT_TRUE(&F == qm.QRT_run_qbb);
    else if (F.getName() == "_ZN4qrt018dismantleQrtSystemEv")
      EXPECT_TRUE(&F == qm.QRT_dismantle);
  };

  runPostPasses(*M);
}
