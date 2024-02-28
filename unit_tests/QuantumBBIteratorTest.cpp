//===- QuantumBBIteratorTest.cpp -------------------------------*- C++ -*-===//
//
// Copyright (C) 2022 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//
//===----------------------------------------------------------------------===//
/// \file "QuantumBBIteratorTest.cpp"
///
/// This file provides a basic unit test of the functionality of the QBBIter
/// class.
///
//===----------------------------------------------------------------------===//

#include "IntelQuantumPasses/QuantumAnalysis/QuantumAnnotationsToJson.h"
#include "IntelQuantumPasses/QuantumAnalysis/QuantumCompilerLinkage.h"
#include "IntelQuantumPasses/QuantumAnalysis/QuantumGateIdentifiers.h"
#include "IntelQuantumPasses/QuantumAnalysis/QuantumIterator.h"
#include "IntelQuantumPasses/QuantumAnalysis/QuantumRefs.h"

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
#include "llvm/Support/JSON.h" //JSON PACKAGE
#include "llvm/Support/SourceMgr.h"
#include "llvm/Support/raw_ostream.h"

#include "gtest/gtest.h"

#include <iostream>
#include <map>

#include "QuantumPasses/QuantumAnalysis/QuantumBBIteratorTest.hpp"

using namespace llvm;
using namespace aqcc;

static std::unique_ptr<Module> parseIR(LLVMContext &C, const char *IR) {
  SMDiagnostic Err;
  std::unique_ptr<Module> Mod = parseAssemblyString(IR, Err, C);
  if (!Mod)
    Err.print("QuantumIteratorTest", errs());
  return Mod;
}

//////////////////////////////////////////////////////////////////////////////
/*
TEST_F(QuantumBBIteratorTest, <template>)
{
  LLVMContext C;
  std::unique_ptr<Module> M = parseIR(C, *&Code);

//run if you are using annotation getters or QIter////////////////////////////

  legacy::PassManager PM;
  PM.add( new QuantumCompilerLinkageWrapperPass() );
  PM.add( new QuantumAnnotationsToJsonPass() );
  PM.run(*M);

///////////////////////////////////////////////////////////////////////////////

  for (Function &F : *M) {
    if (F.getName() == "<function of interest>") {
      //Do some things
    }

  };
// run if you desire to print IR to screen upon
failure///////////////////////////

  legacy::PassManager PMprint;
  PMprint.add( llvm::createPrintModulePass(errs()) );
  PMprint.run(*M);

/////////////////////////////////////////////////////////////////////////////

}
*/

class QuantumIteratorTest : public ::testing::Test {

  void TearDown() override {
    // clean up occurs when test completes or an exception is thrown
    ParaRef::deleteAllOwnedParaRefs();
  }
};

class QuantumBBIteratorTest : public ::testing::Test {

  void TearDown() override {
    // clean up occurs when test completes or an exception is thrown
    ParaRef::deleteAllOwnedParaRefs();
  }
};

static void runInitialPasses(Module &M) {
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
  MAM.registerPass([] { return QuantumCompilerLinkageAnalysis(); });
  MPM.addPass(QuantumAnnotationsToJsonPass());

  MPM.run(M, MAM);
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

TEST_F(QuantumBBIteratorTest, BeginEndTest) {
  LLVMContext C;
  std::unique_ptr<Module> M = parseIR(C, *&Code);

  // run if you are using annotation getters or
  // QIter////////////////////////////

  runInitialPasses(*M);

  ///////////////////////////////////////////////////////////////////////////////

  for (Function &F : *M) {
    if (F.getName() == "_Z15MultipleQBBTestv.stub") {

      QIter qit(F);

      // should be three BBs
      auto bb_iter = F.begin();

      QBBIter qbb1(*bb_iter);

      EXPECT_TRUE(&*qit == &*qbb1);
      qit.gotoEnd();
      qbb1.gotoEnd();
      EXPECT_FALSE(&*qit == &*qbb1);

      ++bb_iter;
      qit.gotoBegin();
      QBBIter qbb2(*bb_iter);

      EXPECT_FALSE(&*qit == &*qbb2);
      qit.gotoEnd();
      qbb2.gotoEnd();
      EXPECT_TRUE(&*qit == &*qbb2);

      ++bb_iter;
      QBBIter qbb3(*bb_iter);

      inst_iterator end = qbb3.getGateInstIterator();

      EXPECT_TRUE(&*end == bb_iter->getTerminator());
    }
  };

  runPostPasses(*M);
  ParaRef::clearValueOwnedMap();
  QbitRef::clearIndexOwnedMap();
  /////////////////////////////////////////////////////////////////////////////
}

TEST_F(QuantumBBIteratorTest, InsertGateTest) {
  LLVMContext C;
  std::unique_ptr<Module> M = parseIR(C, *&Code);

  // run if you are using annotation getters or
  // QIter////////////////////////////

  runInitialPasses(*M);

  ///////////////////////////////////////////////////////////////////////////////

  for (Function &F : *M) {
    if (F.getName() == "_Z15MultipleQBBTestv.stub") {

      // mostly concerned with insertion at the beginning of BBs

      auto bb_iter = F.begin();
      unsigned cnt = 0;

      QBBIter qbb1(*bb_iter);

      auto qubits = qbb1.getQubitOperands();

      EXPECT_TRUE(qbb1.insertGate(kS, qubits));

      EXPECT_EQ(qbb1.getIdentifier(), kS);
      EXPECT_EQ(qubits, qbb1.getQubitOperands());

      for (qbb1.gotoBegin(); !qbb1.isEnd(); ++qbb1)
        cnt++;
      EXPECT_EQ(cnt, 5);
      cnt = 0;

      ++bb_iter;
      QBBIter qbb2(*bb_iter);

      EXPECT_TRUE(qbb2.insertGate(kS, qubits));

      EXPECT_EQ(qbb2.getIdentifier(), kS);
      EXPECT_EQ(qubits, qbb2.getQubitOperands());

      for (qbb2.gotoBegin(); !qbb2.isEnd(); ++qbb2)
        cnt++;
      EXPECT_EQ(cnt, 3);
      cnt = 0;

      ++bb_iter;
      QBBIter qbb3(*bb_iter);

      EXPECT_TRUE(qbb3.insertGate(kS, qubits));

      EXPECT_EQ(qbb3.getIdentifier(), kS);
      EXPECT_EQ(qubits, qbb3.getQubitOperands());

      for (qbb3.gotoBegin(); !qbb3.isEnd(); ++qbb3)
        cnt++;
      EXPECT_EQ(cnt, 1);
    }
  };

  runPostPasses(*M);
  ParaRef::clearValueOwnedMap();
  QbitRef::clearIndexOwnedMap();
  /////////////////////////////////////////////////////////////////////////////
}

TEST_F(QuantumBBIteratorTest, InsertGateInEmptyBBTest) {
  LLVMContext C;
  std::unique_ptr<Module> M = parseIR(C, *&Code);

  // run if you are using annotation getters or
  // QIter////////////////////////////

  runInitialPasses(*M);
  ///////////////////////////////////////////////////////////////////////////////

  for (Function &F : *M) {
    if (F.getName() == "_Z15MultipleQBBTestv.stub") {

      // mostly concerned with insertion at the beginning of empty BBs

      auto bb_iter = F.begin();
      unsigned cnt = 0;

      QBBIter qbb1(*bb_iter);
      qbb1.translateIterator(2);

      auto qubits = qbb1.getQubitOperands();
      auto paras = qbb1.getParametricOperands();

      ++bb_iter;

      // remove all gates but the terminator
      auto I_iter = bb_iter->end();
      --I_iter;
      Instruction *term = bb_iter->getTerminator();

      while (&*bb_iter->begin() != term) {
        --I_iter;
        if (&*I_iter != (bb_iter->getTerminator()))
          I_iter->eraseFromParent();
        I_iter = bb_iter->end();
        --I_iter;
      }
      QBBIter qbb2(*bb_iter);

      EXPECT_TRUE(qbb2.insertGate(kRX, qubits, paras));

      EXPECT_EQ(qbb2.getIdentifier(), kRX);
      EXPECT_EQ(qubits, qbb2.getQubitOperands());
      // because Qiter now makes a copy of of previously used instructions
      // we need to check the GEP is the same, not the instruciton itself
      // EXPECT_EQ(paras, qbb2.getParametricOperands());
      ParaRef temp = qbb2.getParametricOperands().front();
      auto gep1 = dyn_cast<LoadInst>(paras[0].getValue())->getPointerOperand();
      auto gep2 = dyn_cast<LoadInst>(temp.getValue())->getPointerOperand();
      EXPECT_EQ(gep1, gep2);
      // Check gate added to correct BB
      EXPECT_TRUE(qbb2.getGateInstIterator()->getParent() == &*bb_iter);

      for (qbb2.gotoBegin(); !qbb2.isEnd(); ++qbb2)
        cnt++;
      EXPECT_EQ(cnt, 1);
    }
  };

  runPostPasses(*M);
  ParaRef::clearValueOwnedMap();
  QbitRef::clearIndexOwnedMap();
  /////////////////////////////////////////////////////////////////////////////
}

TEST_F(QuantumBBIteratorTest, ChangeGateTest) {
  LLVMContext C;
  std::unique_ptr<Module> M = parseIR(C, *&Code);

  // run if you are using annotation getters or
  // QIter////////////////////////////

  runInitialPasses(*M);

  ///////////////////////////////////////////////////////////////////////////////

  for (Function &F : *M) {
    if (F.getName() == "_Z15MultipleQBBTestv.stub") {

      // mostly concerned with change at the beginning of BBs

      auto bb_iter = F.begin();
      unsigned cnt = 0;

      QBBIter qbb1(*bb_iter);

      auto qubits = qbb1.getQubitOperands();

      EXPECT_TRUE(qbb1.changeGate(kS));

      EXPECT_EQ(qbb1.getIdentifier(), kS);
      EXPECT_EQ(qubits, qbb1.getQubitOperands());

      for (qbb1.gotoBegin(); !qbb1.isEnd(); ++qbb1)
        cnt++;
      EXPECT_EQ(cnt, 4);
      cnt = 0;

      ++bb_iter;
      QBBIter qbb2(*bb_iter);

      qubits = qbb2.getQubitOperands();

      EXPECT_TRUE(qbb2.changeGate(kCZ));

      EXPECT_EQ(qbb2.getIdentifier(), kCZ);
      EXPECT_EQ(qubits, qbb2.getQubitOperands());

      for (qbb2.gotoBegin(); !qbb2.isEnd(); ++qbb2)
        cnt++;
      EXPECT_EQ(cnt, 2);
      cnt = 0;

      ++bb_iter;
      QBBIter qbb3(*bb_iter);

      EXPECT_FALSE(qbb3.changeGate(kS));

      for (qbb3.gotoBegin(); !qbb3.isEnd(); ++qbb3)
        cnt++;
      EXPECT_EQ(cnt, 0);
    }
  };

  runPostPasses(*M);
  ParaRef::clearValueOwnedMap();
  QbitRef::clearIndexOwnedMap();
  /////////////////////////////////////////////////////////////////////////////
}