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
/// This file implements unit test for the QuantumCompilerLinkagePass after
/// finalization.
///
//===----------------------------------------------------------------------===//

#include "IntelQuantumPasses/QuantumAnalysis/QuantumCompilerLinkage.h"
#include "IntelQuantumPasses/QuantumUtils/QuantumGeneralUtils.h"

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

#include "QuantumPasses/QuantumAnalysis/QuantumCompilerLinkageFinalTest.hpp"

using namespace llvm;

static std::unique_ptr<Module> parseIR(LLVMContext &C, const char *IR) {
  SMDiagnostic Err;
  std::unique_ptr<Module> Mod = parseAssemblyString(IR, Err, C);
  if (!Mod)
    Err.print("QuantumCompilerLinkageFinalTest", errs());
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

// Minimum working .ll file for quantum annotations

///////////////////////////////////////////////////////////////////////////////////

TEST(QuantumCompilerLinkageFinalTest, BasicTest) {

  LLVMContext C;
  std::unique_ptr<Module> M = parseIR(C, *&Code);

  // // auto &link = QuantumCompilerLinkagePass::module_linkage;
  QuantumCompilerLinkage QCL;
  QuantumModule qm;
  QCL.doInitialization(*M, qm);
  QCL.locateOrDeclareQRTFunctions(*M, qm);
  qm.setSure();
  QCL.checkForConditioning(*M, qm);
  QCL.checkForSeparation(*M, qm);
  QCL.findMaxQSDAUsage(*M, qm);

  for (auto &F : *M) {

    if (F.getName() == "_ZN5_qrt024_initialize_qrt_system64EPmS0_PdPc")
      EXPECT_TRUE(&F == qm.QRT_initialize);
    else if (F.getName() == "_ZN4qrt013isInitializedEv")
      EXPECT_TRUE(&F == qm.QRT_is_initialized);
    else if (F.getName() == "_ZN5_qrt022update_measurement_mapEPmb")
      EXPECT_TRUE(&F == qm.QRT_measure_move);
    else if (F.getName() == "_ZN5_qrt015schedule_kernelEm")
      EXPECT_TRUE(&F == qm.QRT_run_qbb);

    // check the quantum kernel functions
    else if (F.getName() == "_Z7PrepAllv.stub") {

      // find the aqcc.quantum BB
      for (auto &BB : F) {
        if (isQBasicBlock(BB)) {
          Function *qbb = M->getFunction("_Z16PrepAll().QBB.0.v.stub");
          EXPECT_EQ(qm.QBB_link_map[qbb], &BB);
          EXPECT_EQ(qm.QSDA_used_map[&BB], 4);
        }
      }

    } else if (F.getName() == "_Z10VQE_anzatzPd.stub") {

      // find the aqcc.quantum.gates BB
      for (auto &BB : F) {
        if (isQBasicBlock(BB)) {
          Function *qbb =
              M->getFunction("_Z26VQE_anzatz(double*).QBB.1.v.stub");
          EXPECT_EQ(qm.QBB_link_map[qbb], &BB);
          EXPECT_EQ(qm.QSDA_used_map[&BB], 9);
        }
      }

    } else if (F.getName() == "_Z5MeasHv.stub") {

      // find the aqcc.quantum.gates BB
      for (auto &BB : F) {
        if (isQBasicBlock(BB)) {
          Function *qbb = M->getFunction("_Z14MeasH().QBB.2.v.stub");
          EXPECT_EQ(qm.QBB_link_map[qbb], &BB);
          EXPECT_EQ(qm.QSDA_used_map[&BB], 4);
        }
      }
    }
  };

  // check for globals
  for (auto &global : (*M).globals()) {
    if (global.getName() == "aqcc.qsda.final")
      EXPECT_EQ(dyn_cast<Value>(&global), qm.QSDA);
    // else if(global.getName() == "qbb_section")
    //   EXPECT_EQ(dyn_cast<Value>(&global), qm.QBBS);
    // else if(global.getName() == "qbb_section_text")
    //   EXPECT_EQ(dyn_cast<Value>(&global), qm.QBBS_TEXT);
  };

  // Check max used
  EXPECT_EQ(qm.max_QSDA_used, 9);

  runPostPasses(*M);
}
