//===- QuantumIteratorTest.cpp ---------------------------------*- C++ -*-===//
//
// Copyright (C) 2022 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//
//===----------------------------------------------------------------------===//
/// \file "QuantumIteratorTest.cpp"
///
/// This file provides a basic unit test of the functionality of the QIter
/// class.
///
//===----------------------------------------------------------------------===//

#include "IntelQuantumPasses/QuantumAnalysis/QuantumIterator.h"
#include "IntelQuantumPasses/QuantumAnalysis/QuantumAnnotationsToJson.h"
#include "IntelQuantumPasses/QuantumAnalysis/QuantumCompilerLinkage.h"
#include "IntelQuantumPasses/QuantumAnalysis/QuantumGateIdentifiers.h"
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

#include "QuantumPasses/QuantumAnalysis/QuantumIteratorTest.hpp"

using namespace llvm;
using namespace aqcc;

static std::unique_ptr<Module> parseIR(LLVMContext &C, const char *IR) {
  SMDiagnostic Err;
  std::unique_ptr<Module> Mod = parseAssemblyString(IR, Err, C);
  if (!Mod)
    Err.print("QuantumIteratorTest", errs());
  return Mod;
}

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

////////////////////////////////////////////////////////////////////////////////////////////
/*
TEST_F(QuantumIteratorTest, <template>)
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

TEST_F(QuantumIteratorTest, CtorTest) {
  LLVMContext C;
  std::unique_ptr<Module> M = parseIR(C, *&Code);

  runInitialPasses(*M);

  Function *F1;
  Function *F2;
  Function *F3;
  Function *F4;

  for (Function &F : *M) {
    if (F.getName() == "_Z9CtorTest1v")
      F1 = &F;
    else if (F.getName() == "_Z9CtorTest2v")
      F2 = &F;
    else if (F.getName() == "_Z9CtorTest3v")
      F3 = &F;
    else if (F.getName() == "_Z9CtorTest4v")
      F4 = &F;
  };

  QIter qit1(*F1);

  inst_iterator iter = inst_begin(*F1);
  QIter qit2(*F1, iter);

  EXPECT_TRUE(qit1 == qit2);

  QIter qit3(*F2);

  EXPECT_TRUE(qit1 != qit3);

  QIter qit4(*F3);
  iter = inst_end(*F3);
  --iter; // should be terminator

  EXPECT_TRUE(qit4.getGateInstIterator() == iter);

  QIter qit5(*F4);
  iter = inst_end(*F4);
  --iter; // should be terminator

  EXPECT_TRUE(qit5.getGateInstIterator() == iter);
  ParaRef::clearValueOwnedMap();
  QbitRef::clearIndexOwnedMap();
}

TEST_F(QuantumBBIteratorTest, CtorTest) {
  LLVMContext C;
  std::unique_ptr<Module> M = parseIR(C, *&Code);

  runInitialPasses(*M);

  Function *F1;
  Function *F2;
  Function *F3;
  Function *F4;

  for (Function &F : *M) {
    if (F.getName() == "_Z9CtorTest1v")
      F1 = &F;
    else if (F.getName() == "_Z9CtorTest2v")
      F2 = &F;
    else if (F.getName() == "_Z9CtorTest3v")
      F3 = &F;
    else if (F.getName() == "_Z9CtorTest4v")
      F4 = &F;
  };

  QBBIter qit1((F1->getEntryBlock()));

  inst_iterator iter = inst_bb_begin(&(F1->getEntryBlock()));
  QBBIter qit2((F1->getEntryBlock()), iter);

  EXPECT_TRUE(qit1 == qit2);

  QBBIter qit3((F2->getEntryBlock()));

  EXPECT_TRUE(qit1 != qit3);

  QBBIter qit4((F3->getEntryBlock()));
  iter = inst_bb_end(&(F3->getEntryBlock()));
  --iter; // should be terminator

  EXPECT_TRUE(qit4.getGateInstIterator() == iter);

  QBBIter qit5((F4->getEntryBlock()));
  iter = inst_bb_end(&(F4->getEntryBlock()));
  --iter; // should be terminator

  EXPECT_TRUE(qit5.getGateInstIterator() == iter);
  ParaRef::clearValueOwnedMap();
  QbitRef::clearIndexOwnedMap();
}

////////////////////////////////////////////////////////////////////////////

TEST_F(QuantumIteratorTest, MoversTest) {
  LLVMContext C;
  std::unique_ptr<Module> M = parseIR(C, *&Code);

  runInitialPasses(*M);

  for (Function &F : *M) {
    if (F.getName() == "_Z10MoversTestv") {
      /*
       * Source code:
        void MoversTEST_F() {

           qbit q1;
           qbit q2;
           qbit q3;
           qbit q4;

           HGate(q1);
           CZGate(q1, q2);
           HGate(q2);
           HGate(q4);
           CZGate(q1, q4);
           CZGate(q2, q3);
           HGate(q3);
           HGate(q1);
           HGate(q4);
           CZGate(q2, q1);
           HGate(q3);

         }
      */

      QIter qit(F);

      // Basic increment and decrement
      unsigned cnt1 = 0;

      for (qit.gotoBegin(); !qit.isEnd(); ++qit) {
        cnt1++;
      }
      EXPECT_EQ(cnt1, 11);

      unsigned cnt2 = 0;
      for (qit.gotoEnd(); !qit.isBegin(); --qit)
        cnt2++;
      EXPECT_EQ(cnt2, 11);

      // translate by fixed amounts
      unsigned cnt3 = 0;
      for (qit.gotoBegin(); !qit.isEnd(); qit.translateIterator(3))
        cnt3++;
      EXPECT_EQ(cnt3, 4);

      unsigned cnt4 = 0;
      for (qit.gotoEnd(); !qit.isBegin(); qit.translateIterator(-3))
        cnt4++;
      EXPECT_EQ(cnt4, 4);

      // translate via gate instance
      unsigned cnt5 = 0;
      if (qit.gotoFirstGateInstance(kH))
        do {
          EXPECT_EQ(qit.getIdentifier(), kH);
          cnt5++;
        } while (qit.gotoNextGateInstance(kH));
      EXPECT_EQ(cnt5, 7);

      unsigned cnt6 = 0;
      if (qit.gotoFirstGateInstance(kCZ))
        do {
          EXPECT_EQ(qit.getIdentifier(), kCZ);
          cnt6++;
        } while (qit.gotoNextGateInstance(kCZ));
      EXPECT_EQ(cnt6, 4);

      unsigned cnt7 = 0;
      if (qit.gotoFirstGateInstance(kCX))
        do {
          EXPECT_EQ(qit.getIdentifier(), kCX);
          cnt7++;
        } while (qit.gotoNextGateInstance(kCX));
      EXPECT_EQ(cnt7, 0);

      unsigned cnt8 = 0;
      if (qit.gotoLastGateInstance(kH))
        do {
          EXPECT_EQ(qit.getIdentifier(), kH);
          cnt8++;
        } while (qit.gotoPreviousGateInstance(kH));
      EXPECT_EQ(cnt8, 7);

      unsigned cnt9 = 0;
      if (qit.gotoLastGateInstance(kCZ))
        do {
          EXPECT_EQ(qit.getIdentifier(), kCZ);
          cnt9++;
        } while (qit.gotoPreviousGateInstance(kCZ));
      EXPECT_EQ(cnt9, 4);

      unsigned cnt10 = 0;
      if (qit.gotoLastGateInstance(kCX))
        do {
          EXPECT_EQ(qit.getIdentifier(), kCX);
          cnt10++;
        } while (qit.gotoPreviousGateInstance(kCX));
      EXPECT_EQ(cnt10, 0);

      // translate via qubit list
      inst_iterator iter = inst_begin(F);
      std::vector<QbitRef> qlist1(1);
      std::vector<QbitRef> qlist2(2);

      qlist1[0].setRegister(iter);
      qlist2[0].setRegister(iter);
      ++iter;
      ++iter;
      qlist2[1].setRegister(iter);

      unsigned cnt11 = 0;
      if (qit.gotoFirstQubitInstance(qlist1))
        do {
          EXPECT_EQ(qit.isSupportedOnQubit(qlist1[0]), 1);
          cnt11++;
        } while (qit.gotoNextQubitInstance(qlist1));
      EXPECT_EQ(cnt11, 5);

      unsigned cnt12 = 0;
      if (qit.gotoFirstQubitInstance(qlist2))
        do {
          EXPECT_TRUE((qit.isSupportedOnQubit(qlist2[0]) +
                       qit.isSupportedOnQubit(qlist2[1])) > 0);
          cnt12++;
        } while (qit.gotoNextQubitInstance(qlist2));
      EXPECT_EQ(cnt12, 8);

      unsigned cnt13 = 0;
      if (qit.gotoLastQubitInstance(qlist1))
        do {
          EXPECT_EQ(qit.isSupportedOnQubit(qlist1[0]), 1);
          cnt13++;
        } while (qit.gotoPreviousQubitInstance(qlist1));
      EXPECT_EQ(cnt13, 5);

      unsigned cnt14 = 0;
      if (qit.gotoLastQubitInstance(qlist2))
        do {
          EXPECT_TRUE((qit.isSupportedOnQubit(qlist2[0]) +
                       qit.isSupportedOnQubit(qlist2[1])) > 0);
          cnt14++;
        } while (qit.gotoPreviousQubitInstance(qlist2));
      EXPECT_EQ(cnt14, 8);
    }
  };
  ParaRef::clearValueOwnedMap();
  QbitRef::clearIndexOwnedMap();
}

TEST_F(QuantumBBIteratorTest, MoversTest) {
  LLVMContext C;
  std::unique_ptr<Module> M = parseIR(C, *&Code);

  runInitialPasses(*M);

  for (Function &F : *M) {
    if (F.getName() == "_Z10MoversTestv") {
      /*
       * Source code:
        void MoversTEST_F() {

           qbit q1;
           qbit q2;
           qbit q3;
           qbit q4;

           HGate(q1);
           CZGate(q1, q2);
           HGate(q2);
           HGate(q4);
           CZGate(q1, q4);
           CZGate(q2, q3);
           HGate(q3);
           HGate(q1);
           HGate(q4);
           CZGate(q2, q1);
           HGate(q3);

         }
      */

      QBBIter qit(F.getEntryBlock());

      // Basic increment and decrement
      unsigned cnt1 = 0;

      for (qit.gotoBegin(); !qit.isEnd(); ++qit) {
        cnt1++;
      }
      EXPECT_EQ(cnt1, 11);

      unsigned cnt2 = 0;
      for (qit.gotoEnd(); !qit.isBegin(); --qit)
        cnt2++;
      EXPECT_EQ(cnt2, 11);

      // translate by fixed amounts
      unsigned cnt3 = 0;
      for (qit.gotoBegin(); !qit.isEnd(); qit.translateIterator(3))
        cnt3++;
      EXPECT_EQ(cnt3, 4);

      unsigned cnt4 = 0;
      for (qit.gotoEnd(); !qit.isBegin(); qit.translateIterator(-3))
        cnt4++;
      EXPECT_EQ(cnt4, 4);

      // translate via gate instance
      unsigned cnt5 = 0;
      if (qit.gotoFirstGateInstance(kH))
        do {
          EXPECT_EQ(qit.getIdentifier(), kH);
          cnt5++;
        } while (qit.gotoNextGateInstance(kH));
      EXPECT_EQ(cnt5, 7);

      unsigned cnt6 = 0;
      if (qit.gotoFirstGateInstance(kCZ))
        do {
          EXPECT_EQ(qit.getIdentifier(), kCZ);
          cnt6++;
        } while (qit.gotoNextGateInstance(kCZ));
      EXPECT_EQ(cnt6, 4);

      unsigned cnt7 = 0;
      if (qit.gotoFirstGateInstance(kCX))
        do {
          EXPECT_EQ(qit.getIdentifier(), kCX);
          cnt7++;
        } while (qit.gotoNextGateInstance(kCX));
      EXPECT_EQ(cnt7, 0);

      unsigned cnt8 = 0;
      if (qit.gotoLastGateInstance(kH))
        do {
          EXPECT_EQ(qit.getIdentifier(), kH);
          cnt8++;
        } while (qit.gotoPreviousGateInstance(kH));
      EXPECT_EQ(cnt8, 7);

      unsigned cnt9 = 0;
      if (qit.gotoLastGateInstance(kCZ))
        do {
          EXPECT_EQ(qit.getIdentifier(), kCZ);
          cnt9++;
        } while (qit.gotoPreviousGateInstance(kCZ));
      EXPECT_EQ(cnt9, 4);

      unsigned cnt10 = 0;
      if (qit.gotoLastGateInstance(kCX))
        do {
          EXPECT_EQ(qit.getIdentifier(), kCX);
          cnt10++;
        } while (qit.gotoPreviousGateInstance(kCX));
      EXPECT_EQ(cnt10, 0);

      // translate via qubit list
      inst_iterator iter = inst_begin(F);
      std::vector<QbitRef> qlist1(1);
      std::vector<QbitRef> qlist2(2);

      qlist1[0].setRegister(iter);
      qlist2[0].setRegister(iter);
      ++iter;
      ++iter;
      qlist2[1].setRegister(iter);

      unsigned cnt11 = 0;
      if (qit.gotoFirstQubitInstance(qlist1))
        do {
          EXPECT_EQ(qit.isSupportedOnQubit(qlist1[0]), 1);
          cnt11++;
        } while (qit.gotoNextQubitInstance(qlist1));
      EXPECT_EQ(cnt11, 5);

      unsigned cnt12 = 0;
      if (qit.gotoFirstQubitInstance(qlist2))
        do {
          EXPECT_TRUE((qit.isSupportedOnQubit(qlist2[0]) +
                       qit.isSupportedOnQubit(qlist2[1])) > 0);
          cnt12++;
        } while (qit.gotoNextQubitInstance(qlist2));
      EXPECT_EQ(cnt12, 8);

      unsigned cnt13 = 0;
      if (qit.gotoLastQubitInstance(qlist1))
        do {
          EXPECT_EQ(qit.isSupportedOnQubit(qlist1[0]), 1);
          cnt13++;
        } while (qit.gotoPreviousQubitInstance(qlist1));
      EXPECT_EQ(cnt13, 5);

      unsigned cnt14 = 0;
      if (qit.gotoLastQubitInstance(qlist2))
        do {
          EXPECT_TRUE((qit.isSupportedOnQubit(qlist2[0]) +
                       qit.isSupportedOnQubit(qlist2[1])) > 0);
          cnt14++;
        } while (qit.gotoPreviousQubitInstance(qlist2));
      EXPECT_EQ(cnt14, 8);
    }
  };
  ParaRef::clearValueOwnedMap();
  QbitRef::clearIndexOwnedMap();
}

TEST_F(QuantumIteratorTest, GetterTest) {
  LLVMContext C;
  std::unique_ptr<Module> M = parseIR(C, *&Code);

  runInitialPasses(*M);

  for (Function &F : *M) {
    if (F.getName() == "_Z11GettersTestv") {
      // Source code:
      /*
        void GettersTEST_F() {

          qbit q1;
          qbit q2;

          CNOTGate(q1, q2);
          RZGate(q2, 10.1);
          SdagGate(q2);
          HGate(q1);

        }

      */

      inst_iterator iter = inst_begin(F);
      QbitRef qbit1(iter);
      ++iter;
      QbitRef qbit2(iter);
      ParaRef angle;
      Type *FTy = Type::getDoubleTy(F.getContext());
      angle.setValueResolved(10.1, FTy);

      std::vector<QbitRef> qbit_ops;
      std::vector<QbitRef> ctl_ops;
      std::vector<ParaRef> para_ops;
      QIter qit(F);

      // CNOTGate
      EXPECT_FALSE(qit.isParametric());
      EXPECT_TRUE(qit.isUnitary());
      EXPECT_TRUE(qit.isHermitian());
      EXPECT_TRUE(qit.isMutable());
      EXPECT_TRUE(qit.isCanonicalGate());
      EXPECT_EQ(qit.getIdentifier(), kCX);
      ASSERT_EQ(qit.getNumQubitOperands(), 2);
      qbit_ops = qit.getQubitOperands();
      EXPECT_EQ(qbit_ops[0].isEqual(qbit1), 1);
      EXPECT_EQ(qbit_ops[1].isEqual(qbit2), 1);
      ASSERT_EQ(qit.getNumParametricOperands(), 0);
      para_ops = qit.getParametricOperands();
      EXPECT_EQ(para_ops.size(), 0);
      ctl_ops = qit.getControlQubits();
      EXPECT_EQ(ctl_ops[0].isEqual(qbit1), 1);
      ++qit;

      // RZGate
      EXPECT_TRUE(qit.isParametric());
      EXPECT_TRUE(qit.isUnitary());
      EXPECT_FALSE(qit.isHermitian());
      EXPECT_TRUE(qit.isMutable());
      EXPECT_TRUE(qit.isCanonicalGate());
      EXPECT_EQ(qit.getIdentifier(), kRZ);
      ASSERT_EQ(qit.getNumQubitOperands(), 1);
      qbit_ops = qit.getQubitOperands();
      EXPECT_EQ(qbit_ops[0].isEqual(qbit2), 1);
      ASSERT_EQ(qit.getNumParametricOperands(), 1);
      para_ops = qit.getParametricOperands();
      EXPECT_EQ(para_ops[0].isEqual(angle), 1);
      ctl_ops = qit.getControlQubits();
      EXPECT_EQ(ctl_ops.size(), 0);
      ++qit;

      // SdagGate
      EXPECT_FALSE(qit.isParametric());
      EXPECT_TRUE(qit.isUnitary());
      EXPECT_FALSE(qit.isHermitian());
      EXPECT_TRUE(qit.isMutable());
      EXPECT_TRUE(qit.isCanonicalGate());
      EXPECT_EQ(qit.getIdentifier(), kSDAG);
      ASSERT_EQ(qit.getNumQubitOperands(), 1);
      qbit_ops = qit.getQubitOperands();
      EXPECT_EQ(qbit_ops[0].isEqual(qbit2), 1);
      ASSERT_EQ(qit.getNumParametricOperands(), 0);
      para_ops = qit.getParametricOperands();
      EXPECT_EQ(para_ops.size(), 0);
      ctl_ops = qit.getControlQubits();
      EXPECT_EQ(ctl_ops.size(), 0);
      ++qit;

      // HGate
      EXPECT_FALSE(qit.isParametric());
      EXPECT_TRUE(qit.isUnitary());
      EXPECT_TRUE(qit.isHermitian());
      EXPECT_TRUE(qit.isMutable());
      EXPECT_TRUE(qit.isCanonicalGate());
      EXPECT_EQ(qit.getIdentifier(), kH);
      ASSERT_EQ(qit.getNumQubitOperands(), 1);
      qbit_ops = qit.getQubitOperands();
      EXPECT_EQ(qbit_ops[0].isEqual(qbit1), 1);
      ASSERT_EQ(qit.getNumParametricOperands(), 0);
      para_ops = qit.getParametricOperands();
      EXPECT_EQ(para_ops.size(), 0);
      ctl_ops = qit.getControlQubits();
      EXPECT_EQ(ctl_ops.size(), 0);
    }
  };
  ParaRef::clearValueOwnedMap();
  QbitRef::clearIndexOwnedMap();
}

TEST_F(QuantumBBIteratorTest, GetterTest) {
  LLVMContext C;
  std::unique_ptr<Module> M = parseIR(C, *&Code);

  runInitialPasses(*M);

  for (Function &F : *M) {
    if (F.getName() == "_Z11GettersTestv") {
      // Source code:
      /*
        void GettersTEST_F() {

          qbit q1;
          qbit q2;

          CNOTGate(q1, q2);
          RZGate(q2, 10.1);
          SdagGate(q2);
          HGate(q1);

        }

      */

      inst_iterator iter = inst_begin(F);
      QbitRef qbit1(iter);
      ++iter;
      QbitRef qbit2(iter);
      ParaRef angle;
      Type *FTy = Type::getDoubleTy(F.getContext());
      angle.setValueResolved(10.1, FTy);

      std::vector<QbitRef> qbit_ops;
      std::vector<QbitRef> ctl_ops;
      std::vector<ParaRef> para_ops;
      QBBIter qit(F.getEntryBlock());

      // CNOTGate
      EXPECT_FALSE(qit.isParametric());
      EXPECT_TRUE(qit.isUnitary());
      EXPECT_TRUE(qit.isHermitian());
      EXPECT_TRUE(qit.isMutable());
      EXPECT_TRUE(qit.isCanonicalGate());
      EXPECT_EQ(qit.getIdentifier(), kCX);
      ASSERT_EQ(qit.getNumQubitOperands(), 2);
      qbit_ops = qit.getQubitOperands();
      EXPECT_EQ(qbit_ops[0].isEqual(qbit1), 1);
      EXPECT_EQ(qbit_ops[1].isEqual(qbit2), 1);
      ASSERT_EQ(qit.getNumParametricOperands(), 0);
      para_ops = qit.getParametricOperands();
      EXPECT_EQ(para_ops.size(), 0);
      ctl_ops = qit.getControlQubits();
      EXPECT_EQ(ctl_ops[0].isEqual(qbit1), 1);
      ++qit;

      // RZGate
      EXPECT_TRUE(qit.isParametric());
      EXPECT_TRUE(qit.isUnitary());
      EXPECT_FALSE(qit.isHermitian());
      EXPECT_TRUE(qit.isMutable());
      EXPECT_TRUE(qit.isCanonicalGate());
      EXPECT_EQ(qit.getIdentifier(), kRZ);
      ASSERT_EQ(qit.getNumQubitOperands(), 1);
      qbit_ops = qit.getQubitOperands();
      EXPECT_EQ(qbit_ops[0].isEqual(qbit2), 1);
      ASSERT_EQ(qit.getNumParametricOperands(), 1);
      para_ops = qit.getParametricOperands();
      EXPECT_EQ(para_ops[0].isEqual(angle), 1);
      ctl_ops = qit.getControlQubits();
      EXPECT_EQ(ctl_ops.size(), 0);
      ++qit;

      // SdagGate
      EXPECT_FALSE(qit.isParametric());
      EXPECT_TRUE(qit.isUnitary());
      EXPECT_FALSE(qit.isHermitian());
      EXPECT_TRUE(qit.isMutable());
      EXPECT_TRUE(qit.isCanonicalGate());
      EXPECT_EQ(qit.getIdentifier(), kSDAG);
      ASSERT_EQ(qit.getNumQubitOperands(), 1);
      qbit_ops = qit.getQubitOperands();
      EXPECT_EQ(qbit_ops[0].isEqual(qbit2), 1);
      ASSERT_EQ(qit.getNumParametricOperands(), 0);
      para_ops = qit.getParametricOperands();
      EXPECT_EQ(para_ops.size(), 0);
      ctl_ops = qit.getControlQubits();
      EXPECT_EQ(ctl_ops.size(), 0);
      ++qit;

      // HGate
      EXPECT_FALSE(qit.isParametric());
      EXPECT_TRUE(qit.isUnitary());
      EXPECT_TRUE(qit.isHermitian());
      EXPECT_TRUE(qit.isMutable());
      EXPECT_TRUE(qit.isCanonicalGate());
      EXPECT_EQ(qit.getIdentifier(), kH);
      ASSERT_EQ(qit.getNumQubitOperands(), 1);
      qbit_ops = qit.getQubitOperands();
      EXPECT_EQ(qbit_ops[0].isEqual(qbit1), 1);
      ASSERT_EQ(qit.getNumParametricOperands(), 0);
      para_ops = qit.getParametricOperands();
      EXPECT_EQ(para_ops.size(), 0);
      ctl_ops = qit.getControlQubits();
      EXPECT_EQ(ctl_ops.size(), 0);
    }
  };
  ParaRef::clearValueOwnedMap();
  QbitRef::clearIndexOwnedMap();
}

////////////////////////////////////////////////////////////////////////////////////////////

TEST_F(QuantumIteratorTest, CommutesTest) {
  LLVMContext C;
  std::unique_ptr<Module> M = parseIR(C, *&Code);

  runInitialPasses(*M);

  for (Function &F : *M) {
    if (F.getName() == "_Z12CommutesTestv") {
      // Source code:
      /*
        void CommutesTEST_F() {

          qbit q1;
          qbit q2;
          qbit q3;

          HGate(q1);
          HGate(q2);
          HGate(q1);
          CNOTGate(q1, q2);
          RXGate(q1, 1.234);
          XGate(q2);
          RZGate(q2, 1.1234);
          ZGate(q1);
          CZGate(q1, q2);
          CXGate(q1, q3);
          CXGate(q3, q2);
          ToffoliGate(q1, q2, q3);
        }

     */

      std::vector<int> g1 = {1, 1, -1, -1, 1, 1, -1, -1, -1, 1, -1};
      std::vector<int> g2 = {1, -1, 1, -1, -1, 1, -1, 1, -1, -1};
      std::vector<int> g3 = {-1, -1, 1, 1, -1, -1, -1, 1, -1};
      std::vector<int> g4 = {-1, 1, -1, 1, -1, 1, 1, -1};
      std::vector<int> g5 = {1, 1, -1, -1, -1, 1, -1};
      std::vector<int> g6 = {-1, 1, -1, 1, 1, -1};
      std::vector<int> g7 = {1, 1, 1, -1, 1};
      std::vector<int> g8 = {1, 1, 1, 1};
      std::vector<int> g9 = {1, -1, 1};
      std::vector<int> g10 = {-1, 1};
      std::vector<int> g11 = {-1};

      std::vector<std::vector<int>> com = {g1, g2, g3, g4,  g5, g6,
                                           g7, g8, g9, g10, g11};

      QIter qit1(F);
      QIter qit2(F);
      unsigned cnt1 = 0, cnt2 = 0;

      for (qit1.gotoBegin(); !qit1.isEnd(); ++qit1) {
        for (qit2 = qit1, ++qit2; !qit2.isEnd(); ++qit2) {
          ASSERT_EQ(qit1.commutesWith(qit2), com[cnt1][cnt2]);
          cnt2++;
        };
        cnt1++;
        cnt2 = 0;
      };
    }
  };
  ParaRef::clearValueOwnedMap();
  QbitRef::clearIndexOwnedMap();
}

TEST_F(QuantumBBIteratorTest, CommutesTest) {
  LLVMContext C;
  std::unique_ptr<Module> M = parseIR(C, *&Code);

  runInitialPasses(*M);

  for (Function &F : *M) {
    if (F.getName() == "_Z12CommutesTestv") {
      // Source code:
      /*
        void CommutesTEST_F() {

          qbit q1;
          qbit q2;
          qbit q3;

          HGate(q1);
          HGate(q2);
          HGate(q1);
          CNOTGate(q1, q2);
          RXGate(q1, 1.234);
          XGate(q2);
          RZGate(q2, 1.1234);
          ZGate(q1);
          CZGate(q1, q2);
          CXGate(q1, q3);
          CXGate(q3, q2);
          ToffoliGate(q1, q2, q3);
        }

     */

      std::vector<int> g1 = {1, 1, -1, -1, 1, 1, -1, -1, -1, 1, -1};
      std::vector<int> g2 = {1, -1, 1, -1, -1, 1, -1, 1, -1, -1};
      std::vector<int> g3 = {-1, -1, 1, 1, -1, -1, -1, 1, -1};
      std::vector<int> g4 = {-1, 1, -1, 1, -1, 1, 1, -1};
      std::vector<int> g5 = {1, 1, -1, -1, -1, 1, -1};
      std::vector<int> g6 = {-1, 1, -1, 1, 1, -1};
      std::vector<int> g7 = {1, 1, 1, -1, 1};
      std::vector<int> g8 = {1, 1, 1, 1};
      std::vector<int> g9 = {1, -1, 1};
      std::vector<int> g10 = {-1, 1};
      std::vector<int> g11 = {-1};

      std::vector<std::vector<int>> com = {g1, g2, g3, g4,  g5, g6,
                                           g7, g8, g9, g10, g11};

      QBBIter qit1(F.getEntryBlock());
      QBBIter qit2(F.getEntryBlock());
      unsigned cnt1 = 0, cnt2 = 0;

      for (qit1.gotoBegin(); !qit1.isEnd(); ++qit1) {
        for (qit2 = qit1, ++qit2; !qit2.isEnd(); ++qit2) {
          ASSERT_EQ(qit1.commutesWith(qit2), com[cnt1][cnt2]);
          cnt2++;
        };
        cnt1++;
        cnt2 = 0;
      };
    }
  };
  ParaRef::clearValueOwnedMap();
  QbitRef::clearIndexOwnedMap();
}

////////////////////////////////////////////////////////////////////////////////////////////

TEST_F(QuantumIteratorTest, IRManipulationTest) {
  LLVMContext C;
  std::unique_ptr<Module> M = parseIR(C, *&Code);

  runInitialPasses(*M);

  for (Function &F : *M) {
    if (F.getName() == "_Z18IRManipulationTestRqPq") {

      // get qbits
      QbitRef qbit1(dyn_cast<Value>(F.getArg(0)));
      QbitRef qbit20(dyn_cast<Value>(F.getArg(1)));
      QbitRef qbit21 = qbit20;
      qbit21.setIndexResolved(1);
      inst_iterator iter = inst_begin(F);
      ++iter;
      ++iter;
      QbitRef qbit3(iter);
      ++iter;
      QbitRef qbit40(iter);
      QbitRef qbit41 = qbit40;
      qbit41.setIndexResolved(1);

      // get parameters
      Type *FTy = Type::getDoubleTy(F.getContext());
      ParaRef angle1(1.234, FTy);
      ParaRef angle2(2.345, FTy);
      ++iter;
      Value *angle3_alloc = dyn_cast<Value>(&*iter);
      Type *angle3_ty = dyn_cast<AllocaInst>(angle3_alloc)->getAllocatedType();
      ++iter;
      Value *angle4_alloc = dyn_cast<Value>(&*iter);

      // get index register
      ++iter;
      Value *idx_alloc = dyn_cast<Value>(&*iter);

      QIter qit(F);
      QIter qqit(F);
      // Add 5 no-para SQ gates
      std::vector<QbitRef> qbit_ops1(1);
      std::vector<QbitRef> qbit_ops2(1);
      std::vector<QbitRef> qbit_ops3(1);
      std::vector<QbitRef> qbit_ops4(1);
      std::vector<QbitRef> qbit_ops5(1);
      qbit_ops1[0] = qbit1;
      EXPECT_TRUE(qit.insertGate(kH, qbit_ops1));
      qbit_ops2[0] = qbit20;
      EXPECT_TRUE(qit.insertGate(kS, qbit_ops2));
      qbit_ops3[0] = qbit21;
      EXPECT_TRUE(qit.insertGate(kZ, qbit_ops3));
      qbit_ops4[0] = qbit3;
      EXPECT_TRUE(qit.insertGate(kY, qbit_ops4));
      qbit_ops5[0] = qbit41;
      EXPECT_TRUE(qit.insertGate(kTDAG, qbit_ops5));

      // Add 5 Rotation gates
      std::vector<QbitRef> qbit_ops6(1);
      std::vector<QbitRef> qbit_ops7(1);
      std::vector<QbitRef> qbit_ops8(1);
      std::vector<QbitRef> qbit_ops9(1);
      std::vector<QbitRef> qbit_ops10(1);
      std::vector<ParaRef> para_ops1(1);
      std::vector<ParaRef> para_ops2(1);
      std::vector<ParaRef> para_ops3(1);
      std::vector<ParaRef> para_ops4(1);
      std::vector<ParaRef> para_ops5(1);

      qbit_ops6[0] = qbit1;
      para_ops1[0] = angle1;
      EXPECT_TRUE(qit.insertGate(kRX, qbit_ops6, para_ops1));

      qbit_ops7[0] = qbit20;
      para_ops2[0] = angle2;
      EXPECT_TRUE(qit.insertGate(kRY, qbit_ops7, para_ops2));

      qbit_ops8[0] = qbit21;
      // create LoadInst for use in ParaRef
      // Note insertGate will add this to the Function
      Instruction *Empty = nullptr;
      LoadInst *angle3_load = new LoadInst(angle3_ty, angle3_alloc, "", Empty);
      para_ops3[0].setValue(dyn_cast<Value>(angle3_load));
      EXPECT_TRUE(qit.insertGate(kRZ, qbit_ops8, para_ops3));

      qbit_ops9[0] = qbit3;
      // Need to create both a LoadInst and GetElementPtrInst
      // we do this by first adding the load to Function via insertGate
      // We then add the GEPInst before load to insure proper placement
      Type *IntTy = Type::getInt64Ty(F.getContext());
      std::vector<Value *> gep_array = {
          dyn_cast<Value>(ConstantInt::get(IntTy, 0, false)),
          dyn_cast<Value>(ConstantInt::get(IntTy, 1, false))};
      Type *angle4_ty = dyn_cast<AllocaInst>(angle4_alloc)->getAllocatedType();
      GetElementPtrInst *angle4_gep = GetElementPtrInst::CreateInBounds(
          angle4_ty, angle4_alloc, gep_array, "arrayidx");
      LoadInst *angle4_load =
          new LoadInst(angle4_ty, dyn_cast<Value>(angle4_gep), "", Empty);
      para_ops4[0].setValue(dyn_cast<Value>(angle4_load));
      EXPECT_TRUE(qit.insertGate(kRX, qbit_ops9, para_ops4));
      angle4_gep->insertBefore(dyn_cast<Instruction>(angle4_load));

      qbit_ops10[0] = qbit20;
      para_ops5[0] = angle1;
      // want to use idx value as an index on q2
      // This time I will add the new load
      // after that store, before using insertGate
      Type *idx_type =
          (dyn_cast<PointerType>(idx_alloc->getType()))->getArrayElementType();
      LoadInst *idx_load = new LoadInst(idx_type, idx_alloc, "", Empty);
      idx_load->insertAfter(dyn_cast<Instruction>(idx_alloc));
      qbit_ops10[0].setIndex(dyn_cast<Value>(idx_load));
      EXPECT_TRUE(qit.insertGate(kRY, qbit_ops10, para_ops5));

      // Add 3 two qubit gates
      std::vector<QbitRef> qbit_ops11(2);
      std::vector<QbitRef> qbit_ops12(2);
      std::vector<QbitRef> qbit_ops13(2);

      qbit_ops11[0] = qbit1;
      qbit_ops11[1] = qbit20;
      EXPECT_TRUE(qit.insertGate(kCX, qbit_ops11));

      qbit_ops12[0] = qbit21;
      qbit_ops12[1] = qbit3;
      EXPECT_TRUE(qit.insertGate(kCZ, qbit_ops12));

      qbit_ops13[0] = qbit40;
      qbit_ops13[1] = qbit41;
      LoadInst *idx_load2 = new LoadInst(idx_type, idx_alloc, "", Empty);
      qbit_ops13[0].setIndex(dyn_cast<Value>(idx_load2));
      EXPECT_TRUE(qit.insertGate(kCX, qbit_ops13));

      // Add 1 three qubit gate
      std::vector<QbitRef> qbit_ops14(3);

      qbit_ops14[0] = qbit3;
      qbit_ops14[1] = qbit20;
      qbit_ops14[2] = qbit41;
      LoadInst *idx_load3 = new LoadInst(idx_type, idx_alloc, "", Empty);
      qbit_ops14[1].setIndex(dyn_cast<Value>(idx_load3));
      EXPECT_TRUE(qit.insertGate(kTOFF, qbit_ops14));

      ///////////////////////////////////////////////////////////
      // check added gates

      qqit.gotoBegin();

      // TOFF
      EXPECT_EQ(qqit.getIdentifier(), kTOFF);
      std::vector<QbitRef> qbit_chk14 = qqit.getQubitOperands();
      EXPECT_EQ(qbit_chk14[0].isEqual(qbit_ops14[0]), 1);
      EXPECT_EQ(qbit_chk14[1].isEqual(qbit_ops14[1]), 1);
      EXPECT_EQ(qbit_chk14[2].isEqual(qbit_ops14[2]), 1);
      ++qqit;
      // CX
      EXPECT_EQ(qqit.getIdentifier(), kCX);
      std::vector<QbitRef> qbit_chk13 = qqit.getQubitOperands();
      EXPECT_EQ(qbit_chk13[0].isEqual(qbit_ops13[0]), 1);
      EXPECT_EQ(qbit_chk13[1].isEqual(qbit_ops13[1]), 1);
      ++qqit;
      // CZ
      EXPECT_EQ(qqit.getIdentifier(), kCZ);
      std::vector<QbitRef> qbit_chk12 = qqit.getQubitOperands();
      EXPECT_EQ(qbit_chk12[0].isEqual(qbit_ops12[0]), 1);
      EXPECT_EQ(qbit_chk12[1].isEqual(qbit_ops12[1]), 1);
      ++qqit;
      // CX
      EXPECT_EQ(qqit.getIdentifier(), kCX);
      std::vector<QbitRef> qbit_chk11 = qqit.getQubitOperands();
      EXPECT_EQ(qbit_chk11[0].isEqual(qbit_ops11[0]), 1);
      EXPECT_EQ(qbit_chk11[1].isEqual(qbit_ops11[1]), 1);
      ++qqit;
      // RY
      EXPECT_EQ(qqit.getIdentifier(), kRY);
      std::vector<QbitRef> qbit_chk10 = qqit.getQubitOperands();
      std::vector<ParaRef> para_chk5 = qqit.getParametricOperands();
      EXPECT_EQ(qbit_chk10[0].isEqual(qbit_ops10[0]), 1);
      EXPECT_EQ(para_chk5[0].isEqual(para_ops5[0]), 1);
      ++qqit;
      // RX
      EXPECT_EQ(qqit.getIdentifier(), kRX);
      std::vector<QbitRef> qbit_chk9 = qqit.getQubitOperands();
      std::vector<ParaRef> para_chk4 = qqit.getParametricOperands();
      EXPECT_EQ(qbit_chk9[0].isEqual(qbit_ops9[0]), 1);
      EXPECT_EQ(para_chk4[0].isEqual(para_ops4[0]), 1);
      ++qqit;
      // RZ
      EXPECT_EQ(qqit.getIdentifier(), kRZ);
      std::vector<QbitRef> qbit_chk8 = qqit.getQubitOperands();
      std::vector<ParaRef> para_chk3 = qqit.getParametricOperands();
      EXPECT_EQ(qbit_chk8[0].isEqual(qbit_ops8[0]), 1);
      EXPECT_EQ(para_chk5[0].isEqual(para_ops5[0]), 1);
      ++qqit;
      // RY
      EXPECT_EQ(qqit.getIdentifier(), kRY);
      std::vector<QbitRef> qbit_chk7 = qqit.getQubitOperands();
      std::vector<ParaRef> para_chk2 = qqit.getParametricOperands();
      EXPECT_EQ(qbit_chk7[0].isEqual(qbit_ops7[0]), 1);
      EXPECT_EQ(para_chk2[0].isEqual(para_ops2[0]), 1);
      ++qqit;
      // RX
      EXPECT_EQ(qqit.getIdentifier(), kRX);
      std::vector<QbitRef> qbit_chk6 = qqit.getQubitOperands();
      std::vector<ParaRef> para_chk1 = qqit.getParametricOperands();
      EXPECT_EQ(qbit_chk6[0].isEqual(qbit_ops6[0]), 1);
      EXPECT_EQ(para_chk1[0].isEqual(para_ops1[0]), 1);
      ++qqit;
      // TDAG
      EXPECT_EQ(qqit.getIdentifier(), kTDAG);
      std::vector<QbitRef> qbit_chk5 = qqit.getQubitOperands();
      EXPECT_EQ(qbit_chk5[0].isEqual(qbit_ops5[0]), 1);
      ++qqit;
      // Y
      EXPECT_EQ(qqit.getIdentifier(), kY);
      std::vector<QbitRef> qbit_chk4 = qqit.getQubitOperands();
      EXPECT_EQ(qbit_chk4[0].isEqual(qbit_ops4[0]), 1);
      ++qqit;
      // Z
      EXPECT_EQ(qqit.getIdentifier(), kZ);
      std::vector<QbitRef> qbit_chk3 = qqit.getQubitOperands();
      EXPECT_EQ(qbit_chk3[0].isEqual(qbit_ops3[0]), 1);
      ++qqit;
      // S
      EXPECT_EQ(qqit.getIdentifier(), kS);
      std::vector<QbitRef> qbit_chk2 = qqit.getQubitOperands();
      EXPECT_EQ(qbit_chk2[0].isEqual(qbit_ops2[0]), 1);
      ++qqit;
      // H
      EXPECT_EQ(qqit.getIdentifier(), kH);
      std::vector<QbitRef> qbit_chk1 = qqit.getQubitOperands();
      EXPECT_EQ(qbit_chk1[0].isEqual(qbit_ops1[0]), 1);

      ///////////////////////////////////////////////////////////////////
      // ChangeGate tests

      // try to change all gates to Hgate
      qqit.gotoBegin();

      EXPECT_FALSE(qqit.changeGate(kH));
      ++qqit;
      EXPECT_FALSE(qqit.changeGate(kH));
      ++qqit;
      EXPECT_FALSE(qqit.changeGate(kH));
      ++qqit;
      EXPECT_FALSE(qqit.changeGate(kH));
      ++qqit;
      EXPECT_FALSE(qqit.changeGate(kH));
      ++qqit;
      EXPECT_FALSE(qqit.changeGate(kH));
      ++qqit;
      EXPECT_FALSE(qqit.changeGate(kH));
      ++qqit;
      EXPECT_FALSE(qqit.changeGate(kH));
      ++qqit;
      EXPECT_FALSE(qqit.changeGate(kH));
      ++qqit;
      EXPECT_TRUE(qqit.changeGate(kH));
      ++qqit;
      EXPECT_TRUE(qqit.changeGate(kH));
      ++qqit;
      EXPECT_TRUE(qqit.changeGate(kH));
      ++qqit;
      EXPECT_TRUE(qqit.changeGate(kH));
      ++qqit;
      EXPECT_TRUE(qqit.changeGate(kH));

      // try to change all gates to RX
      qqit.gotoBegin();

      EXPECT_FALSE(qqit.changeGate(kRX));
      ++qqit;
      EXPECT_FALSE(qqit.changeGate(kRX));
      ++qqit;
      EXPECT_FALSE(qqit.changeGate(kRX));
      ++qqit;
      EXPECT_FALSE(qqit.changeGate(kRX));
      ++qqit;
      EXPECT_TRUE(qqit.changeGate(kRX));
      ++qqit;
      EXPECT_TRUE(qqit.changeGate(kRX));
      ++qqit;
      EXPECT_TRUE(qqit.changeGate(kRX));
      ++qqit;
      EXPECT_TRUE(qqit.changeGate(kRX));
      ++qqit;
      EXPECT_TRUE(qqit.changeGate(kRX));
      ++qqit;
      EXPECT_FALSE(qqit.changeGate(kRX));
      ++qqit;
      EXPECT_FALSE(qqit.changeGate(kRX));
      ++qqit;
      EXPECT_FALSE(qqit.changeGate(kRX));
      ++qqit;
      EXPECT_FALSE(qqit.changeGate(kRX));
      ++qqit;
      EXPECT_FALSE(qqit.changeGate(kRX));

      // try to change all gates to CZ
      qqit.gotoBegin();

      EXPECT_FALSE(qqit.changeGate(kCZ));
      ++qqit;
      EXPECT_TRUE(qqit.changeGate(kCZ));
      ++qqit;
      EXPECT_TRUE(qqit.changeGate(kCZ));
      ++qqit;
      EXPECT_TRUE(qqit.changeGate(kCZ));
      ++qqit;
      EXPECT_FALSE(qqit.changeGate(kCZ));
      ++qqit;
      EXPECT_FALSE(qqit.changeGate(kCZ));
      ++qqit;
      EXPECT_FALSE(qqit.changeGate(kCZ));
      ++qqit;
      EXPECT_FALSE(qqit.changeGate(kCZ));
      ++qqit;
      EXPECT_FALSE(qqit.changeGate(kCZ));
      ++qqit;
      EXPECT_FALSE(qqit.changeGate(kCZ));
      ++qqit;
      EXPECT_FALSE(qqit.changeGate(kCZ));
      ++qqit;
      EXPECT_FALSE(qqit.changeGate(kCZ));
      ++qqit;
      EXPECT_FALSE(qqit.changeGate(kCZ));
      ++qqit;
      EXPECT_FALSE(qqit.changeGate(kCZ));

      // Verify the result
      qqit.gotoBegin();

      // TOFF
      EXPECT_EQ(qqit.getIdentifier(), kTOFF);
      qbit_chk14 = qqit.getQubitOperands();
      EXPECT_EQ(qbit_chk14[0].isEqual(qbit_ops14[0]), 1);
      EXPECT_EQ(qbit_chk14[1].isEqual(qbit_ops14[1]), 1);
      EXPECT_EQ(qbit_chk14[2].isEqual(qbit_ops14[2]), 1);
      ++qqit;
      // CZ
      EXPECT_EQ(qqit.getIdentifier(), kCZ);
      qbit_chk13 = qqit.getQubitOperands();
      EXPECT_EQ(qbit_chk13[0].isEqual(qbit_ops13[0]), 1);
      EXPECT_EQ(qbit_chk13[1].isEqual(qbit_ops13[1]), 1);
      ++qqit;
      // CZ
      EXPECT_EQ(qqit.getIdentifier(), kCZ);
      qbit_chk12 = qqit.getQubitOperands();
      EXPECT_EQ(qbit_chk12[0].isEqual(qbit_ops12[0]), 1);
      EXPECT_EQ(qbit_chk12[1].isEqual(qbit_ops12[1]), 1);
      ++qqit;
      // CZ
      EXPECT_EQ(qqit.getIdentifier(), kCZ);
      qbit_chk11 = qqit.getQubitOperands();
      EXPECT_EQ(qbit_chk11[0].isEqual(qbit_ops11[0]), 1);
      EXPECT_EQ(qbit_chk11[1].isEqual(qbit_ops11[1]), 1);
      ++qqit;
      // RX
      EXPECT_EQ(qqit.getIdentifier(), kRX);
      qbit_chk10 = qqit.getQubitOperands();
      para_chk5 = qqit.getParametricOperands();
      EXPECT_EQ(qbit_chk10[0].isEqual(qbit_ops10[0]), 1);
      EXPECT_EQ(para_chk5[0].isEqual(para_ops5[0]), 1);
      ++qqit;
      // RX
      EXPECT_EQ(qqit.getIdentifier(), kRX);
      qbit_chk9 = qqit.getQubitOperands();
      para_chk4 = qqit.getParametricOperands();
      EXPECT_EQ(qbit_chk9[0].isEqual(qbit_ops9[0]), 1);
      EXPECT_EQ(para_chk4[0].isEqual(para_ops4[0]), 1);
      ++qqit;
      // RX
      EXPECT_EQ(qqit.getIdentifier(), kRX);
      qbit_chk8 = qqit.getQubitOperands();
      para_chk3 = qqit.getParametricOperands();
      EXPECT_EQ(qbit_chk8[0].isEqual(qbit_ops8[0]), 1);
      EXPECT_EQ(para_chk5[0].isEqual(para_ops5[0]), 1);
      ++qqit;
      // RX
      EXPECT_EQ(qqit.getIdentifier(), kRX);
      qbit_chk7 = qqit.getQubitOperands();
      para_chk2 = qqit.getParametricOperands();
      EXPECT_EQ(qbit_chk7[0].isEqual(qbit_ops7[0]), 1);
      EXPECT_EQ(para_chk2[0].isEqual(para_ops2[0]), 1);
      ++qqit;
      // RX
      EXPECT_EQ(qqit.getIdentifier(), kRX);
      qbit_chk6 = qqit.getQubitOperands();
      para_chk1 = qqit.getParametricOperands();
      EXPECT_EQ(qbit_chk6[0].isEqual(qbit_ops6[0]), 1);
      EXPECT_EQ(para_chk1[0].isEqual(para_ops1[0]), 1);
      ++qqit;
      // H
      EXPECT_EQ(qqit.getIdentifier(), kH);
      qbit_chk5 = qqit.getQubitOperands();
      EXPECT_EQ(qbit_chk5[0].isEqual(qbit_ops5[0]), 1);
      ++qqit;
      // H
      EXPECT_EQ(qqit.getIdentifier(), kH);
      qbit_chk4 = qqit.getQubitOperands();
      EXPECT_EQ(qbit_chk4[0].isEqual(qbit_ops4[0]), 1);
      ++qqit;
      // H
      EXPECT_EQ(qqit.getIdentifier(), kH);
      qbit_chk3 = qqit.getQubitOperands();
      EXPECT_EQ(qbit_chk3[0].isEqual(qbit_ops3[0]), 1);
      ++qqit;
      // H
      EXPECT_EQ(qqit.getIdentifier(), kH);
      qbit_chk2 = qqit.getQubitOperands();
      EXPECT_EQ(qbit_chk2[0].isEqual(qbit_ops2[0]), 1);
      ++qqit;
      // H
      EXPECT_EQ(qqit.getIdentifier(), kH);
      qbit_chk1 = qqit.getQubitOperands();
      EXPECT_EQ(qbit_chk1[0].isEqual(qbit_ops1[0]), 1);

      /////////////////////////////////////////////////////////////////
      // setQubit and Parametric Operands
      qqit.gotoBegin();

      // reverse order of TOFF gate
      std::vector<QbitRef> qbit_ops15(3);
      qbit_ops15[0] = qbit_ops14[2];
      qbit_ops15[1] = qbit_ops14[0];
      qbit_ops15[2] = qbit_ops14[1];
      EXPECT_TRUE(qqit.setQubitOperands(qbit_ops15));
      ++qqit;

      // cyclic permute of CX gate arguments
      EXPECT_TRUE(qqit.setQubitOperands(qbit_ops11));
      ++qqit;
      EXPECT_TRUE(qqit.setQubitOperands(qbit_ops13));
      ++qqit;
      EXPECT_TRUE(qqit.setQubitOperands(qbit_ops12));
      ++qqit;

      // cyclic permute of RX gates
      EXPECT_TRUE(qqit.setQubitOperands(qbit_ops6));
      EXPECT_TRUE(qqit.setParametricOperands(para_ops1));
      ++qqit;
      EXPECT_TRUE(qqit.setQubitOperands(qbit_ops10));
      EXPECT_TRUE(qqit.setParametricOperands(para_ops5));
      ++qqit;
      EXPECT_TRUE(qqit.setQubitOperands(qbit_ops9));
      EXPECT_TRUE(qqit.setParametricOperands(para_ops4));
      ++qqit;
      EXPECT_TRUE(qqit.setQubitOperands(qbit_ops8));
      EXPECT_TRUE(qqit.setParametricOperands(para_ops3));
      ++qqit;
      EXPECT_TRUE(qqit.setQubitOperands(qbit_ops7));
      EXPECT_TRUE(qqit.setParametricOperands(para_ops2));
      ++qqit;

      // cyclic permute of H gates
      EXPECT_TRUE(qqit.setQubitOperands(qbit_ops1));
      ++qqit;
      EXPECT_TRUE(qqit.setQubitOperands(qbit_ops5));
      ++qqit;
      EXPECT_TRUE(qqit.setQubitOperands(qbit_ops4));
      ++qqit;
      EXPECT_TRUE(qqit.setQubitOperands(qbit_ops3));
      ++qqit;
      EXPECT_TRUE(qqit.setQubitOperands(qbit_ops2));

      // Verify the result
      qqit.gotoBegin();

      // TOFF
      EXPECT_EQ(qqit.getIdentifier(), kTOFF);
      qbit_chk14 = qqit.getQubitOperands();
      EXPECT_EQ(qbit_chk14[0].isEqual(qbit_ops15[0]), 1);
      EXPECT_EQ(qbit_chk14[1].isEqual(qbit_ops15[1]), 1);
      EXPECT_EQ(qbit_chk14[2].isEqual(qbit_ops15[2]), 1);
      ++qqit;
      // CZ
      EXPECT_EQ(qqit.getIdentifier(), kCZ);
      qbit_chk13 = qqit.getQubitOperands();
      EXPECT_EQ(qbit_chk13[0].isEqual(qbit_ops11[0]), 1);
      EXPECT_EQ(qbit_chk13[1].isEqual(qbit_ops11[1]), 1);
      ++qqit;
      // CZ
      EXPECT_EQ(qqit.getIdentifier(), kCZ);
      qbit_chk12 = qqit.getQubitOperands();
      EXPECT_EQ(qbit_chk12[0].isEqual(qbit_ops13[0]), 1);
      EXPECT_EQ(qbit_chk12[1].isEqual(qbit_ops13[1]), 1);
      ++qqit;
      // CZ
      EXPECT_EQ(qqit.getIdentifier(), kCZ);
      qbit_chk11 = qqit.getQubitOperands();
      EXPECT_EQ(qbit_chk11[0].isEqual(qbit_ops12[0]), 1);
      EXPECT_EQ(qbit_chk11[1].isEqual(qbit_ops12[1]), 1);
      ++qqit;
      // RX
      EXPECT_EQ(qqit.getIdentifier(), kRX);
      qbit_chk10 = qqit.getQubitOperands();
      para_chk5 = qqit.getParametricOperands();
      EXPECT_EQ(qbit_chk10[0].isEqual(qbit_ops6[0]), 1);
      EXPECT_EQ(para_chk5[0].isEqual(para_ops1[0]), 1);
      ++qqit;
      // RX
      EXPECT_EQ(qqit.getIdentifier(), kRX);
      qbit_chk9 = qqit.getQubitOperands();
      para_chk4 = qqit.getParametricOperands();
      EXPECT_EQ(qbit_chk9[0].isEqual(qbit_ops10[0]), 1);
      EXPECT_EQ(para_chk4[0].isEqual(para_ops5[0]), 1);
      ++qqit;
      // RX
      EXPECT_EQ(qqit.getIdentifier(), kRX);
      qbit_chk8 = qqit.getQubitOperands();
      para_chk3 = qqit.getParametricOperands();
      EXPECT_EQ(qbit_chk8[0].isEqual(qbit_ops9[0]), 1);
      EXPECT_EQ(para_chk3[0].isEqual(para_ops4[0]), 1);
      ++qqit;
      // RX
      EXPECT_EQ(qqit.getIdentifier(), kRX);
      qbit_chk7 = qqit.getQubitOperands();
      para_chk2 = qqit.getParametricOperands();
      EXPECT_EQ(qbit_chk7[0].isEqual(qbit_ops8[0]), 1);
      EXPECT_EQ(para_chk2[0].isEqual(para_ops3[0]), 1);
      ++qqit;
      // RX
      EXPECT_EQ(qqit.getIdentifier(), kRX);
      qbit_chk6 = qqit.getQubitOperands();
      para_chk1 = qqit.getParametricOperands();
      EXPECT_EQ(qbit_chk6[0].isEqual(qbit_ops7[0]), 1);
      EXPECT_EQ(para_chk1[0].isEqual(para_ops2[0]), 1);
      ++qqit;
      // H
      EXPECT_EQ(qqit.getIdentifier(), kH);
      qbit_chk5 = qqit.getQubitOperands();
      EXPECT_EQ(qbit_chk5[0].isEqual(qbit_ops1[0]), 1);
      ++qqit;
      // H
      EXPECT_EQ(qqit.getIdentifier(), kH);
      qbit_chk4 = qqit.getQubitOperands();
      EXPECT_EQ(qbit_chk4[0].isEqual(qbit_ops5[0]), 1);
      ++qqit;
      // H
      EXPECT_EQ(qqit.getIdentifier(), kH);
      qbit_chk3 = qqit.getQubitOperands();
      EXPECT_EQ(qbit_chk3[0].isEqual(qbit_ops4[0]), 1);
      ++qqit;
      // H
      EXPECT_EQ(qqit.getIdentifier(), kH);
      qbit_chk2 = qqit.getQubitOperands();
      EXPECT_EQ(qbit_chk2[0].isEqual(qbit_ops3[0]), 1);
      ++qqit;
      // H
      EXPECT_EQ(qqit.getIdentifier(), kH);
      qbit_chk1 = qqit.getQubitOperands();
      EXPECT_EQ(qbit_chk1[0].isEqual(qbit_ops2[0]), 1);

      /////////////////////////////////////////////////////
      // remove all gates

      qit.gotoBegin();
      qqit.gotoEnd();

      while (!qit.isEnd())
        EXPECT_TRUE(qit.removeGate());

      ASSERT_TRUE(qqit.isBegin());
      ASSERT_TRUE(qqit.isEnd());
    }
  };

  runPostPasses(*M);
  ParaRef::clearValueOwnedMap();
  QbitRef::clearIndexOwnedMap();
}

TEST_F(QuantumBBIteratorTest, IRManipulationTest) {
  LLVMContext C;
  std::unique_ptr<Module> M = parseIR(C, *&Code);

  runInitialPasses(*M);

  for (Function &F : *M) {
    if (F.getName() == "_Z18IRManipulationTestRqPq") {

      // get qbits
      QbitRef qbit1(dyn_cast<Value>(F.getArg(0)));
      QbitRef qbit20(dyn_cast<Value>(F.getArg(1)));
      QbitRef qbit21 = qbit20;
      qbit21.setIndexResolved(1);
      inst_iterator iter = inst_begin(F);
      ++iter;
      ++iter;
      QbitRef qbit3(iter);
      ++iter;
      QbitRef qbit40(iter);
      QbitRef qbit41 = qbit40;
      qbit41.setIndexResolved(1);

      // get parameters
      Type *FTy = Type::getDoubleTy(F.getContext());
      ParaRef angle1(1.234, FTy);
      ParaRef angle2(2.345, FTy);
      ++iter;
      Value *angle3_alloc = dyn_cast<Value>(&*iter);
      Type *angle3_ty = dyn_cast<AllocaInst>(angle3_alloc)->getAllocatedType();
      ++iter;
      Value *angle4_alloc = dyn_cast<Value>(&*iter);

      // get index register
      ++iter;
      Value *idx_alloc = dyn_cast<Value>(&*iter);

      QBBIter qit(F.getEntryBlock());
      QBBIter qqit(F.getEntryBlock());
      // Add 5 no-para SQ gates
      std::vector<QbitRef> qbit_ops1(1);
      std::vector<QbitRef> qbit_ops2(1);
      std::vector<QbitRef> qbit_ops3(1);
      std::vector<QbitRef> qbit_ops4(1);
      std::vector<QbitRef> qbit_ops5(1);
      qbit_ops1[0] = qbit1;
      EXPECT_TRUE(qit.insertGate(kH, qbit_ops1));
      qbit_ops2[0] = qbit20;
      EXPECT_TRUE(qit.insertGate(kS, qbit_ops2));
      qbit_ops3[0] = qbit21;
      EXPECT_TRUE(qit.insertGate(kZ, qbit_ops3));
      qbit_ops4[0] = qbit3;
      EXPECT_TRUE(qit.insertGate(kY, qbit_ops4));
      qbit_ops5[0] = qbit41;
      EXPECT_TRUE(qit.insertGate(kTDAG, qbit_ops5));

      // Add 5 Rotation gates
      std::vector<QbitRef> qbit_ops6(1);
      std::vector<QbitRef> qbit_ops7(1);
      std::vector<QbitRef> qbit_ops8(1);
      std::vector<QbitRef> qbit_ops9(1);
      std::vector<QbitRef> qbit_ops10(1);
      std::vector<ParaRef> para_ops1(1);
      std::vector<ParaRef> para_ops2(1);
      std::vector<ParaRef> para_ops3(1);
      std::vector<ParaRef> para_ops4(1);
      std::vector<ParaRef> para_ops5(1);

      qbit_ops6[0] = qbit1;
      para_ops1[0] = angle1;
      EXPECT_TRUE(qit.insertGate(kRX, qbit_ops6, para_ops1));

      qbit_ops7[0] = qbit20;
      para_ops2[0] = angle2;
      EXPECT_TRUE(qit.insertGate(kRY, qbit_ops7, para_ops2));

      qbit_ops8[0] = qbit21;
      // create LoadInst for use in ParaRef
      // Note insertGate will add this to the Function
      Instruction *Empty = nullptr;
      LoadInst *angle3_load = new LoadInst(angle3_ty, angle3_alloc, "", Empty);
      para_ops3[0].setValue(dyn_cast<Value>(angle3_load));
      EXPECT_TRUE(qit.insertGate(kRZ, qbit_ops8, para_ops3));

      qbit_ops9[0] = qbit3;
      // Need to create both a LoadInst and GetElementPtrInst
      // we do this by first adding the load to Function via insertGate
      // We then add the GEPInst before load to insure proper placement
      Type *IntTy = Type::getInt64Ty(F.getContext());
      std::vector<Value *> gep_array = {
          dyn_cast<Value>(ConstantInt::get(IntTy, 0, false)),
          dyn_cast<Value>(ConstantInt::get(IntTy, 1, false))};
      Type *angle4_ty = dyn_cast<AllocaInst>(angle4_alloc)->getAllocatedType();
      GetElementPtrInst *angle4_gep = GetElementPtrInst::CreateInBounds(
          angle4_ty, angle4_alloc, gep_array, "arrayidx");
      LoadInst *angle4_load =
          new LoadInst(angle4_ty, dyn_cast<Value>(angle4_gep), "", Empty);
      para_ops4[0].setValue(dyn_cast<Value>(angle4_load));
      EXPECT_TRUE(qit.insertGate(kRX, qbit_ops9, para_ops4));
      angle4_gep->insertBefore(dyn_cast<Instruction>(angle4_load));

      qbit_ops10[0] = qbit20;
      para_ops5[0] = angle1;
      // want to use idx value as an index on q2
      // This time I will add the new load
      // after that store, before using insertGate
      Type *idx_type = dyn_cast<AllocaInst>(idx_alloc)->getAllocatedType();
      LoadInst *idx_load = new LoadInst(idx_type, idx_alloc, "", Empty);
      idx_load->insertAfter(dyn_cast<Instruction>(idx_alloc));
      qbit_ops10[0].setIndex(dyn_cast<Value>(idx_load));
      EXPECT_TRUE(qit.insertGate(kRY, qbit_ops10, para_ops5));

      // Add 3 two qubit gates
      std::vector<QbitRef> qbit_ops11(2);
      std::vector<QbitRef> qbit_ops12(2);
      std::vector<QbitRef> qbit_ops13(2);

      qbit_ops11[0] = qbit1;
      qbit_ops11[1] = qbit20;
      EXPECT_TRUE(qit.insertGate(kCX, qbit_ops11));

      qbit_ops12[0] = qbit21;
      qbit_ops12[1] = qbit3;
      EXPECT_TRUE(qit.insertGate(kCZ, qbit_ops12));

      qbit_ops13[0] = qbit40;
      qbit_ops13[1] = qbit41;
      LoadInst *idx_load2 = new LoadInst(idx_type, idx_alloc, "", Empty);
      qbit_ops13[0].setIndex(dyn_cast<Value>(idx_load2));
      EXPECT_TRUE(qit.insertGate(kCX, qbit_ops13));

      // Add 1 three qubit gate
      std::vector<QbitRef> qbit_ops14(3);

      qbit_ops14[0] = qbit3;
      qbit_ops14[1] = qbit20;
      qbit_ops14[2] = qbit41;
      LoadInst *idx_load3 = new LoadInst(idx_type, idx_alloc, "", Empty);
      qbit_ops14[1].setIndex(dyn_cast<Value>(idx_load3));
      EXPECT_TRUE(qit.insertGate(kTOFF, qbit_ops14));

      ///////////////////////////////////////////////////////////
      // check added gates

      qqit.gotoBegin();

      // TOFF
      EXPECT_EQ(qqit.getIdentifier(), kTOFF);
      std::vector<QbitRef> qbit_chk14 = qqit.getQubitOperands();
      EXPECT_EQ(qbit_chk14[0].isEqual(qbit_ops14[0]), 1);
      EXPECT_EQ(qbit_chk14[1].isEqual(qbit_ops14[1]), 1);
      EXPECT_EQ(qbit_chk14[2].isEqual(qbit_ops14[2]), 1);
      ++qqit;
      // CX
      EXPECT_EQ(qqit.getIdentifier(), kCX);
      std::vector<QbitRef> qbit_chk13 = qqit.getQubitOperands();
      EXPECT_EQ(qbit_chk13[0].isEqual(qbit_ops13[0]), 1);
      EXPECT_EQ(qbit_chk13[1].isEqual(qbit_ops13[1]), 1);
      ++qqit;
      // CZ
      EXPECT_EQ(qqit.getIdentifier(), kCZ);
      std::vector<QbitRef> qbit_chk12 = qqit.getQubitOperands();
      EXPECT_EQ(qbit_chk12[0].isEqual(qbit_ops12[0]), 1);
      EXPECT_EQ(qbit_chk12[1].isEqual(qbit_ops12[1]), 1);
      ++qqit;
      // CX
      EXPECT_EQ(qqit.getIdentifier(), kCX);
      std::vector<QbitRef> qbit_chk11 = qqit.getQubitOperands();
      EXPECT_EQ(qbit_chk11[0].isEqual(qbit_ops11[0]), 1);
      EXPECT_EQ(qbit_chk11[1].isEqual(qbit_ops11[1]), 1);
      ++qqit;
      // RY
      EXPECT_EQ(qqit.getIdentifier(), kRY);
      std::vector<QbitRef> qbit_chk10 = qqit.getQubitOperands();
      std::vector<ParaRef> para_chk5 = qqit.getParametricOperands();
      EXPECT_EQ(qbit_chk10[0].isEqual(qbit_ops10[0]), 1);
      EXPECT_EQ(para_chk5[0].isEqual(para_ops5[0]), 1);
      ++qqit;
      // RX
      EXPECT_EQ(qqit.getIdentifier(), kRX);
      std::vector<QbitRef> qbit_chk9 = qqit.getQubitOperands();
      std::vector<ParaRef> para_chk4 = qqit.getParametricOperands();
      EXPECT_EQ(qbit_chk9[0].isEqual(qbit_ops9[0]), 1);
      EXPECT_EQ(para_chk4[0].isEqual(para_ops4[0]), 1);
      ++qqit;
      // RZ
      EXPECT_EQ(qqit.getIdentifier(), kRZ);
      std::vector<QbitRef> qbit_chk8 = qqit.getQubitOperands();
      std::vector<ParaRef> para_chk3 = qqit.getParametricOperands();
      EXPECT_EQ(qbit_chk8[0].isEqual(qbit_ops8[0]), 1);
      EXPECT_EQ(para_chk5[0].isEqual(para_ops5[0]), 1);
      ++qqit;
      // RY
      EXPECT_EQ(qqit.getIdentifier(), kRY);
      std::vector<QbitRef> qbit_chk7 = qqit.getQubitOperands();
      std::vector<ParaRef> para_chk2 = qqit.getParametricOperands();
      EXPECT_EQ(qbit_chk7[0].isEqual(qbit_ops7[0]), 1);
      EXPECT_EQ(para_chk2[0].isEqual(para_ops2[0]), 1);
      ++qqit;
      // RX
      EXPECT_EQ(qqit.getIdentifier(), kRX);
      std::vector<QbitRef> qbit_chk6 = qqit.getQubitOperands();
      std::vector<ParaRef> para_chk1 = qqit.getParametricOperands();
      EXPECT_EQ(qbit_chk6[0].isEqual(qbit_ops6[0]), 1);
      EXPECT_EQ(para_chk1[0].isEqual(para_ops1[0]), 1);
      ++qqit;
      // TDAG
      EXPECT_EQ(qqit.getIdentifier(), kTDAG);
      std::vector<QbitRef> qbit_chk5 = qqit.getQubitOperands();
      EXPECT_EQ(qbit_chk5[0].isEqual(qbit_ops5[0]), 1);
      ++qqit;
      // Y
      EXPECT_EQ(qqit.getIdentifier(), kY);
      std::vector<QbitRef> qbit_chk4 = qqit.getQubitOperands();
      EXPECT_EQ(qbit_chk4[0].isEqual(qbit_ops4[0]), 1);
      ++qqit;
      // Z
      EXPECT_EQ(qqit.getIdentifier(), kZ);
      std::vector<QbitRef> qbit_chk3 = qqit.getQubitOperands();
      EXPECT_EQ(qbit_chk3[0].isEqual(qbit_ops3[0]), 1);
      ++qqit;
      // S
      EXPECT_EQ(qqit.getIdentifier(), kS);
      std::vector<QbitRef> qbit_chk2 = qqit.getQubitOperands();
      EXPECT_EQ(qbit_chk2[0].isEqual(qbit_ops2[0]), 1);
      ++qqit;
      // H
      EXPECT_EQ(qqit.getIdentifier(), kH);
      std::vector<QbitRef> qbit_chk1 = qqit.getQubitOperands();
      EXPECT_EQ(qbit_chk1[0].isEqual(qbit_ops1[0]), 1);

      ///////////////////////////////////////////////////////////////////
      // ChangeGate tests

      // try to change all gates to Hgate
      qqit.gotoBegin();

      EXPECT_FALSE(qqit.changeGate(kH));
      ++qqit;
      EXPECT_FALSE(qqit.changeGate(kH));
      ++qqit;
      EXPECT_FALSE(qqit.changeGate(kH));
      ++qqit;
      EXPECT_FALSE(qqit.changeGate(kH));
      ++qqit;
      EXPECT_FALSE(qqit.changeGate(kH));
      ++qqit;
      EXPECT_FALSE(qqit.changeGate(kH));
      ++qqit;
      EXPECT_FALSE(qqit.changeGate(kH));
      ++qqit;
      EXPECT_FALSE(qqit.changeGate(kH));
      ++qqit;
      EXPECT_FALSE(qqit.changeGate(kH));
      ++qqit;
      EXPECT_TRUE(qqit.changeGate(kH));
      ++qqit;
      EXPECT_TRUE(qqit.changeGate(kH));
      ++qqit;
      EXPECT_TRUE(qqit.changeGate(kH));
      ++qqit;
      EXPECT_TRUE(qqit.changeGate(kH));
      ++qqit;
      EXPECT_TRUE(qqit.changeGate(kH));

      // try to change all gates to RX
      qqit.gotoBegin();

      EXPECT_FALSE(qqit.changeGate(kRX));
      ++qqit;
      EXPECT_FALSE(qqit.changeGate(kRX));
      ++qqit;
      EXPECT_FALSE(qqit.changeGate(kRX));
      ++qqit;
      EXPECT_FALSE(qqit.changeGate(kRX));
      ++qqit;
      EXPECT_TRUE(qqit.changeGate(kRX));
      ++qqit;
      EXPECT_TRUE(qqit.changeGate(kRX));
      ++qqit;
      EXPECT_TRUE(qqit.changeGate(kRX));
      ++qqit;
      EXPECT_TRUE(qqit.changeGate(kRX));
      ++qqit;
      EXPECT_TRUE(qqit.changeGate(kRX));
      ++qqit;
      EXPECT_FALSE(qqit.changeGate(kRX));
      ++qqit;
      EXPECT_FALSE(qqit.changeGate(kRX));
      ++qqit;
      EXPECT_FALSE(qqit.changeGate(kRX));
      ++qqit;
      EXPECT_FALSE(qqit.changeGate(kRX));
      ++qqit;
      EXPECT_FALSE(qqit.changeGate(kRX));

      // try to change all gates to CZ
      qqit.gotoBegin();

      EXPECT_FALSE(qqit.changeGate(kCZ));
      ++qqit;
      EXPECT_TRUE(qqit.changeGate(kCZ));
      ++qqit;
      EXPECT_TRUE(qqit.changeGate(kCZ));
      ++qqit;
      EXPECT_TRUE(qqit.changeGate(kCZ));
      ++qqit;
      EXPECT_FALSE(qqit.changeGate(kCZ));
      ++qqit;
      EXPECT_FALSE(qqit.changeGate(kCZ));
      ++qqit;
      EXPECT_FALSE(qqit.changeGate(kCZ));
      ++qqit;
      EXPECT_FALSE(qqit.changeGate(kCZ));
      ++qqit;
      EXPECT_FALSE(qqit.changeGate(kCZ));
      ++qqit;
      EXPECT_FALSE(qqit.changeGate(kCZ));
      ++qqit;
      EXPECT_FALSE(qqit.changeGate(kCZ));
      ++qqit;
      EXPECT_FALSE(qqit.changeGate(kCZ));
      ++qqit;
      EXPECT_FALSE(qqit.changeGate(kCZ));
      ++qqit;
      EXPECT_FALSE(qqit.changeGate(kCZ));

      // Verify the result
      qqit.gotoBegin();

      // TOFF
      EXPECT_EQ(qqit.getIdentifier(), kTOFF);
      qbit_chk14 = qqit.getQubitOperands();
      EXPECT_EQ(qbit_chk14[0].isEqual(qbit_ops14[0]), 1);
      EXPECT_EQ(qbit_chk14[1].isEqual(qbit_ops14[1]), 1);
      EXPECT_EQ(qbit_chk14[2].isEqual(qbit_ops14[2]), 1);
      ++qqit;
      // CZ
      EXPECT_EQ(qqit.getIdentifier(), kCZ);
      qbit_chk13 = qqit.getQubitOperands();
      EXPECT_EQ(qbit_chk13[0].isEqual(qbit_ops13[0]), 1);
      EXPECT_EQ(qbit_chk13[1].isEqual(qbit_ops13[1]), 1);
      ++qqit;
      // CZ
      EXPECT_EQ(qqit.getIdentifier(), kCZ);
      qbit_chk12 = qqit.getQubitOperands();
      EXPECT_EQ(qbit_chk12[0].isEqual(qbit_ops12[0]), 1);
      EXPECT_EQ(qbit_chk12[1].isEqual(qbit_ops12[1]), 1);
      ++qqit;
      // CZ
      EXPECT_EQ(qqit.getIdentifier(), kCZ);
      qbit_chk11 = qqit.getQubitOperands();
      EXPECT_EQ(qbit_chk11[0].isEqual(qbit_ops11[0]), 1);
      EXPECT_EQ(qbit_chk11[1].isEqual(qbit_ops11[1]), 1);
      ++qqit;
      // RX
      EXPECT_EQ(qqit.getIdentifier(), kRX);
      qbit_chk10 = qqit.getQubitOperands();
      para_chk5 = qqit.getParametricOperands();
      EXPECT_EQ(qbit_chk10[0].isEqual(qbit_ops10[0]), 1);
      EXPECT_EQ(para_chk5[0].isEqual(para_ops5[0]), 1);
      ++qqit;
      // RX
      EXPECT_EQ(qqit.getIdentifier(), kRX);
      qbit_chk9 = qqit.getQubitOperands();
      para_chk4 = qqit.getParametricOperands();
      EXPECT_EQ(qbit_chk9[0].isEqual(qbit_ops9[0]), 1);
      EXPECT_EQ(para_chk4[0].isEqual(para_ops4[0]), 1);
      ++qqit;
      // RX
      EXPECT_EQ(qqit.getIdentifier(), kRX);
      qbit_chk8 = qqit.getQubitOperands();
      para_chk3 = qqit.getParametricOperands();
      EXPECT_EQ(qbit_chk8[0].isEqual(qbit_ops8[0]), 1);
      EXPECT_EQ(para_chk5[0].isEqual(para_ops5[0]), 1);
      ++qqit;
      // RX
      EXPECT_EQ(qqit.getIdentifier(), kRX);
      qbit_chk7 = qqit.getQubitOperands();
      para_chk2 = qqit.getParametricOperands();
      EXPECT_EQ(qbit_chk7[0].isEqual(qbit_ops7[0]), 1);
      EXPECT_EQ(para_chk2[0].isEqual(para_ops2[0]), 1);
      ++qqit;
      // RX
      EXPECT_EQ(qqit.getIdentifier(), kRX);
      qbit_chk6 = qqit.getQubitOperands();
      para_chk1 = qqit.getParametricOperands();
      EXPECT_EQ(qbit_chk6[0].isEqual(qbit_ops6[0]), 1);
      EXPECT_EQ(para_chk1[0].isEqual(para_ops1[0]), 1);
      ++qqit;
      // H
      EXPECT_EQ(qqit.getIdentifier(), kH);
      qbit_chk5 = qqit.getQubitOperands();
      EXPECT_EQ(qbit_chk5[0].isEqual(qbit_ops5[0]), 1);
      ++qqit;
      // H
      EXPECT_EQ(qqit.getIdentifier(), kH);
      qbit_chk4 = qqit.getQubitOperands();
      EXPECT_EQ(qbit_chk4[0].isEqual(qbit_ops4[0]), 1);
      ++qqit;
      // H
      EXPECT_EQ(qqit.getIdentifier(), kH);
      qbit_chk3 = qqit.getQubitOperands();
      EXPECT_EQ(qbit_chk3[0].isEqual(qbit_ops3[0]), 1);
      ++qqit;
      // H
      EXPECT_EQ(qqit.getIdentifier(), kH);
      qbit_chk2 = qqit.getQubitOperands();
      EXPECT_EQ(qbit_chk2[0].isEqual(qbit_ops2[0]), 1);
      ++qqit;
      // H
      EXPECT_EQ(qqit.getIdentifier(), kH);
      qbit_chk1 = qqit.getQubitOperands();
      EXPECT_EQ(qbit_chk1[0].isEqual(qbit_ops1[0]), 1);

      /////////////////////////////////////////////////////////////////
      // setQubit and Parametric Operands
      qqit.gotoBegin();

      // reverse order of TOFF gate
      std::vector<QbitRef> qbit_ops15(3);
      qbit_ops15[0] = qbit_ops14[2];
      qbit_ops15[1] = qbit_ops14[0];
      qbit_ops15[2] = qbit_ops14[1];
      EXPECT_TRUE(qqit.setQubitOperands(qbit_ops15));
      ++qqit;

      // cyclic permute of CX gate arguments
      EXPECT_TRUE(qqit.setQubitOperands(qbit_ops11));
      ++qqit;
      EXPECT_TRUE(qqit.setQubitOperands(qbit_ops13));
      ++qqit;
      EXPECT_TRUE(qqit.setQubitOperands(qbit_ops12));
      ++qqit;

      // cyclic permute of RX gates
      EXPECT_TRUE(qqit.setQubitOperands(qbit_ops6));
      EXPECT_TRUE(qqit.setParametricOperands(para_ops1));
      ++qqit;
      EXPECT_TRUE(qqit.setQubitOperands(qbit_ops10));
      EXPECT_TRUE(qqit.setParametricOperands(para_ops5));
      ++qqit;
      EXPECT_TRUE(qqit.setQubitOperands(qbit_ops9));
      EXPECT_TRUE(qqit.setParametricOperands(para_ops4));
      ++qqit;
      EXPECT_TRUE(qqit.setQubitOperands(qbit_ops8));
      EXPECT_TRUE(qqit.setParametricOperands(para_ops3));
      ++qqit;
      EXPECT_TRUE(qqit.setQubitOperands(qbit_ops7));
      EXPECT_TRUE(qqit.setParametricOperands(para_ops2));
      ++qqit;

      // cyclic permute of H gates
      EXPECT_TRUE(qqit.setQubitOperands(qbit_ops1));
      ++qqit;
      EXPECT_TRUE(qqit.setQubitOperands(qbit_ops5));
      ++qqit;
      EXPECT_TRUE(qqit.setQubitOperands(qbit_ops4));
      ++qqit;
      EXPECT_TRUE(qqit.setQubitOperands(qbit_ops3));
      ++qqit;
      EXPECT_TRUE(qqit.setQubitOperands(qbit_ops2));

      // Verify the result
      qqit.gotoBegin();

      // TOFF
      EXPECT_EQ(qqit.getIdentifier(), kTOFF);
      qbit_chk14 = qqit.getQubitOperands();
      EXPECT_EQ(qbit_chk14[0].isEqual(qbit_ops15[0]), 1);
      EXPECT_EQ(qbit_chk14[1].isEqual(qbit_ops15[1]), 1);
      EXPECT_EQ(qbit_chk14[2].isEqual(qbit_ops15[2]), 1);
      ++qqit;
      // CZ
      EXPECT_EQ(qqit.getIdentifier(), kCZ);
      qbit_chk13 = qqit.getQubitOperands();
      EXPECT_EQ(qbit_chk13[0].isEqual(qbit_ops11[0]), 1);
      EXPECT_EQ(qbit_chk13[1].isEqual(qbit_ops11[1]), 1);
      ++qqit;
      // CZ
      EXPECT_EQ(qqit.getIdentifier(), kCZ);
      qbit_chk12 = qqit.getQubitOperands();
      EXPECT_EQ(qbit_chk12[0].isEqual(qbit_ops13[0]), 1);
      EXPECT_EQ(qbit_chk12[1].isEqual(qbit_ops13[1]), 1);
      ++qqit;
      // CZ
      EXPECT_EQ(qqit.getIdentifier(), kCZ);
      qbit_chk11 = qqit.getQubitOperands();
      EXPECT_EQ(qbit_chk11[0].isEqual(qbit_ops12[0]), 1);
      EXPECT_EQ(qbit_chk11[1].isEqual(qbit_ops12[1]), 1);
      ++qqit;
      // RX
      EXPECT_EQ(qqit.getIdentifier(), kRX);
      qbit_chk10 = qqit.getQubitOperands();
      para_chk5 = qqit.getParametricOperands();
      EXPECT_EQ(qbit_chk10[0].isEqual(qbit_ops6[0]), 1);
      EXPECT_EQ(para_chk5[0].isEqual(para_ops1[0]), 1);
      ++qqit;
      // RX
      EXPECT_EQ(qqit.getIdentifier(), kRX);
      qbit_chk9 = qqit.getQubitOperands();
      para_chk4 = qqit.getParametricOperands();
      EXPECT_EQ(qbit_chk9[0].isEqual(qbit_ops10[0]), 1);
      EXPECT_EQ(para_chk4[0].isEqual(para_ops5[0]), 1);
      ++qqit;
      // RX
      EXPECT_EQ(qqit.getIdentifier(), kRX);
      qbit_chk8 = qqit.getQubitOperands();
      para_chk3 = qqit.getParametricOperands();
      EXPECT_EQ(qbit_chk8[0].isEqual(qbit_ops9[0]), 1);
      EXPECT_EQ(para_chk3[0].isEqual(para_ops4[0]), 1);
      ++qqit;
      // RX
      EXPECT_EQ(qqit.getIdentifier(), kRX);
      qbit_chk7 = qqit.getQubitOperands();
      para_chk2 = qqit.getParametricOperands();
      EXPECT_EQ(qbit_chk7[0].isEqual(qbit_ops8[0]), 1);
      EXPECT_EQ(para_chk2[0].isEqual(para_ops3[0]), 1);
      ++qqit;
      // RX
      EXPECT_EQ(qqit.getIdentifier(), kRX);
      qbit_chk6 = qqit.getQubitOperands();
      para_chk1 = qqit.getParametricOperands();
      EXPECT_EQ(qbit_chk6[0].isEqual(qbit_ops7[0]), 1);
      EXPECT_EQ(para_chk1[0].isEqual(para_ops2[0]), 1);
      ++qqit;
      // H
      EXPECT_EQ(qqit.getIdentifier(), kH);
      qbit_chk5 = qqit.getQubitOperands();
      EXPECT_EQ(qbit_chk5[0].isEqual(qbit_ops1[0]), 1);
      ++qqit;
      // H
      EXPECT_EQ(qqit.getIdentifier(), kH);
      qbit_chk4 = qqit.getQubitOperands();
      EXPECT_EQ(qbit_chk4[0].isEqual(qbit_ops5[0]), 1);
      ++qqit;
      // H
      EXPECT_EQ(qqit.getIdentifier(), kH);
      qbit_chk3 = qqit.getQubitOperands();
      EXPECT_EQ(qbit_chk3[0].isEqual(qbit_ops4[0]), 1);
      ++qqit;
      // H
      EXPECT_EQ(qqit.getIdentifier(), kH);
      qbit_chk2 = qqit.getQubitOperands();
      EXPECT_EQ(qbit_chk2[0].isEqual(qbit_ops3[0]), 1);
      ++qqit;
      // H
      EXPECT_EQ(qqit.getIdentifier(), kH);
      qbit_chk1 = qqit.getQubitOperands();
      EXPECT_EQ(qbit_chk1[0].isEqual(qbit_ops2[0]), 1);

      /////////////////////////////////////////////////////
      // remove all gates

      qit.gotoBegin();
      qqit.gotoEnd();

      while (!qit.isEnd())
        EXPECT_TRUE(qit.removeGate());

      ASSERT_TRUE(qqit.isBegin());
      ASSERT_TRUE(qqit.isEnd());
    }
  };

  runPostPasses(*M);
  ParaRef::clearValueOwnedMap();
  QbitRef::clearIndexOwnedMap();
}

/////////////////////////////////////////////////////////////////////////////

TEST_F(QuantumIteratorTest, FullExampleTest) {
  LLVMContext C;
  std::unique_ptr<Module> M = parseIR(C, *&Code);

  runInitialPasses(*M);

  for (Function &F : *M) {
    if (F.getName() == "_Z15FullExampleTestv") {
      // Program...
      // 1) move all H gates to the left
      //  a) cancel for HH
      //  b) convert rots
      //  c) change CX to CZ
      // 2)push RZs, to the left
      // 3)convert CZ CX CZ -> CZ CZ
      // 4)push RY to left
      /*initial circuit:
       *  -*----------------------------
       *   |
       *  -X--Rz--H--*---------*--------
       *             |         |
       *  --------H--X--Rx--*--X-Ry--H--
       *                    |
       *  ------------------X--H--------
       *
       *
       *  end result
       *
       *  -----*------------------------
       *       |
       *  -H---*--Rx---*---------------
       *               |
       *  -Rz--X--Rny--)---------------
       *       |       |
       *  -----*-------*---------------
       *
       *
       *
       *
       *  use alot of:
       *
       *    if ( qit.gotoFirstGateInstance(<gateID>) ) do {
       *             .
       *             .
       *             .
       *      <do something>
       *             .
       *             .
       *             .
       *    } while( qit.gotoNextGateInstance(<gateID>) );
       *
       *
       */

      QIter q_mov(F);
      QIter q_cur(F);

      // 1) move Hs left
      if (q_cur.gotoFirstGateInstance(kH))
        do {

          std::vector<QbitRef> qbit_cur = q_cur.getQubitOperands();
          bool stuck = false, rmv = false, end = false;
          q_mov = q_cur;

          // Move backward and test if we can either move or cancel q_cur with
          // q_move
          while (!stuck && !end) {

            if (!q_mov.isBegin()) {
              --q_mov;

              switch (q_mov.getIdentifier()) {

              // check for an H gate on same qubit
              case kH:
                if (q_mov.isSupportedOnQubit(qbit_cur[0]) == 1) {
                  q_mov.removeGate();
                  stuck = true;
                  rmv = true;
                }
                break;

              // check for Rotation gates
              case kRX:
                // X->Z
                if (q_mov.isSupportedOnQubit(qbit_cur[0]) == 1) {
                  q_mov.changeGate(kRZ);
                } else if (q_mov.isSupportedOnQubit(qbit_cur[0]) == -1)
                  stuck = true;
                break;

              case kRY:
                // invert the sign of angle
                if (q_mov.isSupportedOnQubit(qbit_cur[0]) == 1) {
                  std::vector<ParaRef> ang_refs = q_mov.getParametricOperands();
                  if (ang_refs[0].isResolved()) {
                    double angle = ang_refs[0].getValueResolved().value();
                    angle *= -1;
                    ang_refs[0].setValueResolved(angle);
                  } else { // in general, we add a  mult by -1 instruction in
                           // IR,
                    Value *neg_ang = dyn_cast<Value>(BinaryOperator::CreateNeg(
                        ang_refs[0].getValue(), "neg"));
                    ang_refs[0].setValue(neg_ang);
                  }
                  q_mov.setParametricOperands(ang_refs);
                } else if (q_mov.isSupportedOnQubit(qbit_cur[0]) == -1)
                  stuck = true;
                break;

              case kRZ:
                // Z->X
                if (q_mov.isSupportedOnQubit(qbit_cur[0]) == 1) {
                  q_mov.changeGate(kRX);
                } else if (q_mov.isSupportedOnQubit(qbit_cur[0]) == -1)
                  stuck = true;
                break;

              case kCX:
                // check if H is on target, in which case we change CX->CZ
                if (qbit_cur[0].isEqual(q_mov.getQubitOperand(1)) == 1) {
                  q_mov.changeGate(kCZ);
                } else
                  stuck = !(q_cur.commutesWith(q_mov) == 1);
                break;

              case kCZ:
                // check if H is on one of qubits and change CZ -> CX
                std::vector<QbitRef> cz_qbit = q_mov.getQubitOperands();
                if (qbit_cur[0].isEqual(cz_qbit[0]) == 1) {
                  cz_qbit[0] = cz_qbit[1];
                  cz_qbit[1] = qbit_cur[0];
                  q_mov.changeGate(kCX);
                  q_mov.setQubitOperands(cz_qbit);
                } else if (qbit_cur[0].isEqual(cz_qbit[1]) == 1) {
                  q_mov.changeGate(kCX);
                  q_mov.setQubitOperands(cz_qbit);
                } else
                  stuck = !(q_cur.commutesWith(q_mov) == 1);
                break;

                // default:
                //  stuck = !(q_cur.commutesWith(q_mov) == 1);
              }
            } else
              end = true;
          }

          if (!end)
            ++q_mov;
          if (q_mov != q_cur) {
            ASSERT_TRUE(q_cur.removeGate());
            if (!q_cur.isBegin())
              --q_cur;
            if (!rmv)
              ASSERT_TRUE(q_mov.insertGate(kH, qbit_cur));
          }

        } while (q_cur.gotoNextGateInstance(kH));

      // 2) Move RX, RZ to
      // left//////////////////////////////////////////////////////

      if (q_cur.gotoFirstGateInstance(kRX))
        do {

          std::vector<QbitRef> qbit_cur = q_cur.getQubitOperands();
          std::vector<ParaRef> ang_refs = q_cur.getParametricOperands();

          bool stuck = false, rmv = false, end = false;
          q_mov = q_cur;

          // Move backward and test if we can either move or cancel q_cur with
          // q_move
          while (!stuck && !end) {

            if (!q_mov.isBegin()) {
              --q_mov;

              if (q_mov.getIdentifier() == kRX) {
                if (q_mov.isSupportedOnQubit(qbit_cur[0]) == 1) {
                  std::vector<ParaRef> new_ang_refs =
                      q_mov.getParametricOperands();

                  if (new_ang_refs[0].isResolved() &&
                      ang_refs[0].isResolved()) {
                    double angle = new_ang_refs[0].getValueResolved().value();
                    angle += ang_refs[0].getValueResolved().value();
                    new_ang_refs[0].setValueResolved(angle);
                  } else { // in general, we include an add instruction,
                    Value *angle_val = dyn_cast<Value>(BinaryOperator::Create(
                        Instruction::FAdd, new_ang_refs[0].getValue(),
                        ang_refs[0].getValue(), "add"));
                    new_ang_refs[0].setValue(angle_val);
                  }

                  q_mov.setParametricOperands(ang_refs);
                  rmv = true;
                }

              } else
                stuck = !(q_cur.commutesWith(q_mov) == 1);

            } else
              end = true;
          }

          if (!end)
            ++q_mov;
          if (q_mov != q_cur) {
            ASSERT_TRUE(q_cur.removeGate());
            if (!q_cur.isBegin())
              --q_cur;
            if (!rmv)
              ASSERT_TRUE(q_mov.insertGate(kRX, qbit_cur, ang_refs));
          }

        } while (q_cur.gotoNextGateInstance(kRX));

      /////RZ

      if (q_cur.gotoFirstGateInstance(kRZ))
        do {

          std::vector<QbitRef> qbit_cur = q_cur.getQubitOperands();
          std::vector<ParaRef> ang_refs = q_cur.getParametricOperands();

          bool stuck = false, rmv = false, end = false;
          q_mov = q_cur;

          // Move backward and test if we can either move or cancel q_cur with
          // q_move
          while (!stuck && !end) {

            if (!q_mov.isBegin()) {
              --q_mov;

              if (q_mov.getIdentifier() == kRZ) {
                if (q_mov.isSupportedOnQubit(qbit_cur[0]) == 1) {
                  std::vector<ParaRef> new_ang_refs =
                      q_mov.getParametricOperands();

                  if (new_ang_refs[0].isResolved() &&
                      ang_refs[0].isResolved()) {
                    double angle = new_ang_refs[0].getValueResolved().value();
                    angle += ang_refs[0].getValueResolved().value();
                    new_ang_refs[0].setValueResolved(angle);
                  } else { // in general, we include an add instruction,
                    Value *angle_val = dyn_cast<Value>(BinaryOperator::Create(
                        Instruction::FAdd, new_ang_refs[0].getValue(),
                        ang_refs[0].getValue(), "add"));
                    new_ang_refs[0].setValue(angle_val);
                  }

                  q_mov.setParametricOperands(ang_refs);
                  rmv = true;
                }

              } else
                stuck = !(q_cur.commutesWith(q_mov) == 1);

            } else
              end = true;
          }

          if (!end)
            ++q_mov;
          if (q_mov != q_cur) {
            ASSERT_TRUE(q_cur.removeGate());
            if (!q_cur.isBegin())
              --q_cur;
            if (!rmv)
              ASSERT_TRUE(q_mov.insertGate(kRZ, qbit_cur, ang_refs));
          }

        } while (q_cur.gotoNextGateInstance(kRZ));

      // 3) look for CZ CX CZ -> CX CZ pattern
      if (q_cur.gotoFirstGateInstance(kCZ))
        do {

          std::vector<QbitRef> qbit_cur = q_cur.getQubitOperands();
          std::vector<QbitRef> qbit_temp;
          q_mov = q_cur;

          bool found = false, overlap0 = false, overlap1 = false;
          ;

          // look forward to find next CZ on same qubits
          while (!found && !q_mov.isEnd()) {
            if (q_mov.gotoNextGateInstance(kCZ)) {
              qbit_temp = q_mov.getQubitOperands();
              overlap0 = ((qbit_cur[0].isEqual(qbit_temp[0]) == 1) &&
                          (qbit_cur[1].isEqual(qbit_temp[1]) == 1));
              overlap1 = ((qbit_cur[1].isEqual(qbit_temp[0]) == 1) &&
                          (qbit_cur[0].isEqual(qbit_temp[1]) == 1));
              found = overlap0 || overlap1;
            }
          }

          // Now see if there is a CX between them with exactly on shared qubit
          QIter q_btw(F);
          q_btw = q_mov;

          if (overlap1) {
            qbit_cur[0] = qbit_temp[1];
            qbit_cur[1] = qbit_temp[0];
          }

          if (found) {
            found = false;
            while ((q_btw.commutesWith(q_mov) == 1) && (q_btw != q_cur)) {
              --q_btw;
              if (q_btw.getIdentifier() == kCX) {
                qbit_temp = q_btw.getQubitOperands();
                overlap1 = ((qbit_cur[0].isEqual(qbit_temp[0]) == 0) &&
                            (qbit_cur[1].isEqual(qbit_temp[1]) == 1));
                overlap0 = ((qbit_cur[1].isEqual(qbit_temp[0]) == 0) &&
                            (qbit_cur[0].isEqual(qbit_temp[1]) == 1));
                found = overlap0 || overlap1;
              }
            }
          }

          // if found, make sure no gates between
          QIter q_com(F);
          q_com = q_cur;

          if (found) {
            while (q_com != q_btw) {
              found = found && (q_cur.commutesWith(q_com) == 1);
              ++q_com;
            }

            // if we found the pattern, we remove q_cur and change qbit
            // arguments for q_mov

            if (found) {
              ASSERT_TRUE(q_cur.removeGate());
              if (overlap0)
                qbit_cur[0] = qbit_temp[0];
              else if (overlap1)
                qbit_cur[1] = qbit_temp[0];
              ASSERT_TRUE(q_mov.setQubitOperands(qbit_cur));
            }
          }
        } while (q_cur.gotoNextGateInstance(kCZ));

      // 4) Move RY to
      // left//////////////////////////////////////////////////////

      if (q_cur.gotoFirstGateInstance(kRY))
        do {

          std::vector<QbitRef> qbit_cur = q_cur.getQubitOperands();
          std::vector<ParaRef> ang_refs = q_cur.getParametricOperands();

          bool stuck = false, rmv = false, end = false;
          q_mov = q_cur;

          // Move backward and test if we can either move or cancel q_cur with
          // q_move
          while (!stuck && !end) {

            if (!q_mov.isBegin()) {
              --q_mov;

              if (q_mov.getIdentifier() == kRY) {
                if (q_mov.isSupportedOnQubit(qbit_cur[0]) == 1) {
                  std::vector<ParaRef> new_ang_refs =
                      q_mov.getParametricOperands();

                  if (new_ang_refs[0].isResolved() &&
                      ang_refs[0].isResolved()) {
                    double angle = new_ang_refs[0].getValueResolved().value();
                    angle += ang_refs[0].getValueResolved().value();
                    new_ang_refs[0].setValueResolved(angle);
                  } else { // in general, we include an add instruction,
                    Value *angle_val = dyn_cast<Value>(BinaryOperator::Create(
                        Instruction::FAdd, new_ang_refs[0].getValue(),
                        ang_refs[0].getValue(), "add"));
                    new_ang_refs[0].setValue(angle_val);
                  }

                  q_mov.setParametricOperands(ang_refs);
                  rmv = true;
                }

              } else
                stuck = !(q_cur.commutesWith(q_mov) == 1);

            } else
              end = true;
          }

          if (!end)
            ++q_mov;
          if (q_mov != q_cur) {
            ASSERT_TRUE(q_cur.removeGate());
            if (!q_cur.isBegin())
              --q_cur;
            if (!rmv)
              ASSERT_TRUE(q_mov.insertGate(kRY, qbit_cur, ang_refs));
          }

        } while (q_cur.gotoNextGateInstance(kRY));

      // verify the result//////////////////////////////////////////
      inst_iterator iter(F);
      QbitRef q0(iter);
      QbitRef q1 = q0;
      q1.setIndexResolved(1);
      QbitRef q2 = q0;
      q2.setIndexResolved(2);
      QbitRef q3 = q0;
      q3.setIndexResolved(3);

      Type *FTy = Type::getDoubleTy(F.getContext());
      ParaRef angle0(1.1234, FTy), angle1(0.35, FTy), angle2(-0.2, FTy);

      QIter q_chx(F);

      // RZ(q2, angle1)
      EXPECT_EQ(q_chx.getIdentifier(), kRZ);
      EXPECT_EQ(q_chx.getQubitOperand(0).isEqual(q2), 1);
      EXPECT_EQ(q_chx.getParametricOperand(0).isEqual(angle1), 1);
      ++q_chx;

      // H(q3)
      EXPECT_EQ(q_chx.getIdentifier(), kH);
      EXPECT_EQ(q_chx.getQubitOperand(0).isEqual(q3), 1);
      ++q_chx;

      // H(q1)
      EXPECT_EQ(q_chx.getIdentifier(), kH);
      EXPECT_EQ(q_chx.getQubitOperand(0).isEqual(q1), 1);
      ++q_chx;

      // CZ(q0,q1)
      EXPECT_EQ(q_chx.getIdentifier(), kCZ);
      EXPECT_EQ(q_chx.getQubitOperand(0).isEqual(q0), 1);
      EXPECT_EQ(q_chx.getQubitOperand(1).isEqual(q1), 1);
      ++q_chx;

      // RX(q1, angle0)
      EXPECT_EQ(q_chx.getIdentifier(), kRX);
      EXPECT_EQ(q_chx.getQubitOperand(0).isEqual(q1), 1);
      EXPECT_EQ(q_chx.getParametricOperand(0).isEqual(angle0), 1);
      ++q_chx;

      // CX(q3,q2)
      EXPECT_EQ(q_chx.getIdentifier(), kCX);
      EXPECT_EQ(q_chx.getQubitOperand(0).isEqual(q3), 1);
      EXPECT_EQ(q_chx.getQubitOperand(1).isEqual(q2), 1);
      ++q_chx;

      // RY(q1, angle2)
      EXPECT_EQ(q_chx.getIdentifier(), kRY);
      EXPECT_EQ(q_chx.getQubitOperand(0).isEqual(q2), 1);
      EXPECT_EQ(q_chx.getParametricOperand(0).isEqual(angle2), 1);
      ++q_chx;

      // CZ(q1,q3)
      EXPECT_EQ(q_chx.getIdentifier(), kCZ);
      EXPECT_EQ(q_chx.getQubitOperand(0).isEqual(q1), 1);
      EXPECT_EQ(q_chx.getQubitOperand(1).isEqual(q3), 1);
      ++q_chx;
    }
  };

  runPostPasses(*M);
  ParaRef::clearValueOwnedMap();
  QbitRef::clearIndexOwnedMap();
  /////////////////////////////////////////////////////////////////////////////
}

TEST_F(QuantumBBIteratorTest, FullExampleTest) {
  LLVMContext C;
  std::unique_ptr<Module> M = parseIR(C, *&Code);

  runInitialPasses(*M);

  for (Function &F : *M) {
    if (F.getName() == "_Z15FullExampleTestv") {
      // Program...
      // 1) move all H gates to the left
      //  a) cancel for HH
      //  b) convert rots
      //  c) change CX to CZ
      // 2)push RZs, to the left
      // 3)convert CZ CX CZ -> CZ CZ
      // 4)push RY to left
      /*initial circuit:
       *  -*----------------------------
       *   |
       *  -X--Rz--H--*---------*--------
       *             |         |
       *  --------H--X--Rx--*--X-Ry--H--
       *                    |
       *  ------------------X--H--------
       *
       *
       *  end result
       *
       *  -----*------------------------
       *       |
       *  -H---*--Rx---*---------------
       *               |
       *  -Rz--X--Rny--)---------------
       *       |       |
       *  -----*-------*---------------
       *
       *
       *
       *
       *  use alot of:
       *
       *    if ( qit.gotoFirstGateInstance(<gateID>) ) do {
       *             .
       *             .
       *             .
       *      <do something>
       *             .
       *             .
       *             .
       *    } while( qit.gotoNextGateInstance(<gateID>) );
       *
       *
       */

      QBBIter q_mov(F.getEntryBlock());
      QBBIter q_cur(F.getEntryBlock());

      // 1) move Hs left
      if (q_cur.gotoFirstGateInstance(kH))
        do {

          std::vector<QbitRef> qbit_cur = q_cur.getQubitOperands();
          bool stuck = false, rmv = false, end = false;
          q_mov = q_cur;

          // Move backward and test if we can either move or cancel q_cur with
          // q_move
          while (!stuck && !end) {

            if (!q_mov.isBegin()) {
              --q_mov;

              switch (q_mov.getIdentifier()) {

              // check for an H gate on same qubit
              case kH:
                if (q_mov.isSupportedOnQubit(qbit_cur[0]) == 1) {
                  q_mov.removeGate();
                  stuck = true;
                  rmv = true;
                }
                break;

              // check for Rotation gates
              case kRX:
                // X->Z
                if (q_mov.isSupportedOnQubit(qbit_cur[0]) == 1) {
                  q_mov.changeGate(kRZ);
                } else if (q_mov.isSupportedOnQubit(qbit_cur[0]) == -1)
                  stuck = true;
                break;

              case kRY:
                // invert the sign of angle
                if (q_mov.isSupportedOnQubit(qbit_cur[0]) == 1) {
                  std::vector<ParaRef> ang_refs = q_mov.getParametricOperands();
                  if (ang_refs[0].isResolved()) {
                    double angle = ang_refs[0].getValueResolved().value();
                    angle *= -1;
                    ang_refs[0].setValueResolved(angle);
                  } else { // in general, we add a  mult by -1 instruction in
                           // IR,
                    Value *neg_ang = dyn_cast<Value>(BinaryOperator::CreateNeg(
                        ang_refs[0].getValue(), "neg"));
                    ang_refs[0].setValue(neg_ang);
                  }
                  q_mov.setParametricOperands(ang_refs);
                } else if (q_mov.isSupportedOnQubit(qbit_cur[0]) == -1)
                  stuck = true;
                break;

              case kRZ:
                // Z->X
                if (q_mov.isSupportedOnQubit(qbit_cur[0]) == 1) {
                  q_mov.changeGate(kRX);
                } else if (q_mov.isSupportedOnQubit(qbit_cur[0]) == -1)
                  stuck = true;
                break;

              case kCX:
                // check if H is on target, in which case we change CX->CZ
                if (qbit_cur[0].isEqual(q_mov.getQubitOperand(1)) == 1) {
                  q_mov.changeGate(kCZ);
                } else
                  stuck = !(q_cur.commutesWith(q_mov) == 1);
                break;

              case kCZ:
                // check if H is on one of qubits and change CZ -> CX
                std::vector<QbitRef> cz_qbit = q_mov.getQubitOperands();
                if (qbit_cur[0].isEqual(cz_qbit[0]) == 1) {
                  cz_qbit[0] = cz_qbit[1];
                  cz_qbit[1] = qbit_cur[0];
                  q_mov.changeGate(kCX);
                  q_mov.setQubitOperands(cz_qbit);
                } else if (qbit_cur[0].isEqual(cz_qbit[1]) == 1) {
                  q_mov.changeGate(kCX);
                  q_mov.setQubitOperands(cz_qbit);
                } else
                  stuck = !(q_cur.commutesWith(q_mov) == 1);
                break;

                // default:
                //  stuck = !(q_cur.commutesWith(q_mov) == 1);
              }
            } else
              end = true;
          }

          if (!end)
            ++q_mov;
          if (q_mov != q_cur) {
            ASSERT_TRUE(q_cur.removeGate());
            if (!q_cur.isBegin())
              --q_cur;
            if (!rmv)
              ASSERT_TRUE(q_mov.insertGate(kH, qbit_cur));
          }

        } while (q_cur.gotoNextGateInstance(kH));

      // 2) Move RX, RZ to
      // left//////////////////////////////////////////////////////

      if (q_cur.gotoFirstGateInstance(kRX))
        do {

          std::vector<QbitRef> qbit_cur = q_cur.getQubitOperands();
          std::vector<ParaRef> ang_refs = q_cur.getParametricOperands();

          bool stuck = false, rmv = false, end = false;
          q_mov = q_cur;

          // Move backward and test if we can either move or cancel q_cur with
          // q_move
          while (!stuck && !end) {

            if (!q_mov.isBegin()) {
              --q_mov;

              if (q_mov.getIdentifier() == kRX) {
                if (q_mov.isSupportedOnQubit(qbit_cur[0]) == 1) {
                  std::vector<ParaRef> new_ang_refs =
                      q_mov.getParametricOperands();

                  if (new_ang_refs[0].isResolved() &&
                      ang_refs[0].isResolved()) {
                    double angle = new_ang_refs[0].getValueResolved().value();
                    angle += ang_refs[0].getValueResolved().value();
                    new_ang_refs[0].setValueResolved(angle);
                  } else { // in general, we include an add instruction,
                    Value *angle_val = dyn_cast<Value>(BinaryOperator::Create(
                        Instruction::FAdd, new_ang_refs[0].getValue(),
                        ang_refs[0].getValue(), "add"));
                    new_ang_refs[0].setValue(angle_val);
                  }

                  q_mov.setParametricOperands(ang_refs);
                  rmv = true;
                }

              } else
                stuck = !(q_cur.commutesWith(q_mov) == 1);

            } else
              end = true;
          }

          if (!end)
            ++q_mov;
          if (q_mov != q_cur) {
            ASSERT_TRUE(q_cur.removeGate());
            if (!q_cur.isBegin())
              --q_cur;
            if (!rmv)
              ASSERT_TRUE(q_mov.insertGate(kRX, qbit_cur, ang_refs));
          }

        } while (q_cur.gotoNextGateInstance(kRX));

      /////RZ

      if (q_cur.gotoFirstGateInstance(kRZ))
        do {

          std::vector<QbitRef> qbit_cur = q_cur.getQubitOperands();
          std::vector<ParaRef> ang_refs = q_cur.getParametricOperands();

          bool stuck = false, rmv = false, end = false;
          q_mov = q_cur;

          // Move backward and test if we can either move or cancel q_cur with
          // q_move
          while (!stuck && !end) {

            if (!q_mov.isBegin()) {
              --q_mov;

              if (q_mov.getIdentifier() == kRZ) {
                if (q_mov.isSupportedOnQubit(qbit_cur[0]) == 1) {
                  std::vector<ParaRef> new_ang_refs =
                      q_mov.getParametricOperands();

                  if (new_ang_refs[0].isResolved() &&
                      ang_refs[0].isResolved()) {
                    double angle = new_ang_refs[0].getValueResolved().value();
                    angle += ang_refs[0].getValueResolved().value();
                    new_ang_refs[0].setValueResolved(angle);
                  } else { // in general, we include an add instruction,
                    Value *angle_val = dyn_cast<Value>(BinaryOperator::Create(
                        Instruction::FAdd, new_ang_refs[0].getValue(),
                        ang_refs[0].getValue(), "add"));
                    new_ang_refs[0].setValue(angle_val);
                  }

                  q_mov.setParametricOperands(ang_refs);
                  rmv = true;
                }

              } else
                stuck = !(q_cur.commutesWith(q_mov) == 1);

            } else
              end = true;
          }

          if (!end)
            ++q_mov;
          if (q_mov != q_cur) {
            ASSERT_TRUE(q_cur.removeGate());
            if (!q_cur.isBegin())
              --q_cur;
            if (!rmv)
              ASSERT_TRUE(q_mov.insertGate(kRZ, qbit_cur, ang_refs));
          }

        } while (q_cur.gotoNextGateInstance(kRZ));

      // 3) look for CZ CX CZ -> CX CZ pattern
      if (q_cur.gotoFirstGateInstance(kCZ))
        do {

          std::vector<QbitRef> qbit_cur = q_cur.getQubitOperands();
          std::vector<QbitRef> qbit_temp;
          q_mov = q_cur;

          bool found = false, overlap0 = false, overlap1 = false;
          ;

          // look forward to find next CZ on same qubits
          while (!found && !q_mov.isEnd()) {
            if (q_mov.gotoNextGateInstance(kCZ)) {
              qbit_temp = q_mov.getQubitOperands();
              overlap0 = ((qbit_cur[0].isEqual(qbit_temp[0]) == 1) &&
                          (qbit_cur[1].isEqual(qbit_temp[1]) == 1));
              overlap1 = ((qbit_cur[1].isEqual(qbit_temp[0]) == 1) &&
                          (qbit_cur[0].isEqual(qbit_temp[1]) == 1));
              found = overlap0 || overlap1;
            }
          }

          // Now see if there is a CX between them with exactly on shared qubit
          QBBIter q_btw(F.getEntryBlock());
          q_btw = q_mov;

          if (overlap1) {
            qbit_cur[0] = qbit_temp[1];
            qbit_cur[1] = qbit_temp[0];
          }

          if (found) {
            found = false;
            while ((q_btw.commutesWith(q_mov) == 1) && (q_btw != q_cur)) {
              --q_btw;
              if (q_btw.getIdentifier() == kCX) {
                qbit_temp = q_btw.getQubitOperands();
                overlap1 = ((qbit_cur[0].isEqual(qbit_temp[0]) == 0) &&
                            (qbit_cur[1].isEqual(qbit_temp[1]) == 1));
                overlap0 = ((qbit_cur[1].isEqual(qbit_temp[0]) == 0) &&
                            (qbit_cur[0].isEqual(qbit_temp[1]) == 1));
                found = overlap0 || overlap1;
              }
            }
          }

          // if found, make sure no gates between
          QBBIter q_com(F.getEntryBlock());
          q_com = q_cur;

          if (found) {
            while (q_com != q_btw) {
              found = found && (q_cur.commutesWith(q_com) == 1);
              ++q_com;
            }

            // if we found the pattern, we remove q_cur and change qbit
            // arguments for q_mov

            if (found) {
              ASSERT_TRUE(q_cur.removeGate());
              if (overlap0)
                qbit_cur[0] = qbit_temp[0];
              else if (overlap1)
                qbit_cur[1] = qbit_temp[0];
              ASSERT_TRUE(q_mov.setQubitOperands(qbit_cur));
            }
          }
        } while (q_cur.gotoNextGateInstance(kCZ));

      // 4) Move RY to
      // left//////////////////////////////////////////////////////

      if (q_cur.gotoFirstGateInstance(kRY))
        do {

          std::vector<QbitRef> qbit_cur = q_cur.getQubitOperands();
          std::vector<ParaRef> ang_refs = q_cur.getParametricOperands();

          bool stuck = false, rmv = false, end = false;
          q_mov = q_cur;

          // Move backward and test if we can either move or cancel q_cur with
          // q_move
          while (!stuck && !end) {

            if (!q_mov.isBegin()) {
              --q_mov;

              if (q_mov.getIdentifier() == kRY) {
                if (q_mov.isSupportedOnQubit(qbit_cur[0]) == 1) {
                  std::vector<ParaRef> new_ang_refs =
                      q_mov.getParametricOperands();

                  if (new_ang_refs[0].isResolved() &&
                      ang_refs[0].isResolved()) {
                    double angle = new_ang_refs[0].getValueResolved().value();
                    angle += ang_refs[0].getValueResolved().value();
                    new_ang_refs[0].setValueResolved(angle);
                  } else { // in general, we include an add instruction,
                    Value *angle_val = dyn_cast<Value>(BinaryOperator::Create(
                        Instruction::FAdd, new_ang_refs[0].getValue(),
                        ang_refs[0].getValue(), "add"));
                    new_ang_refs[0].setValue(angle_val);
                  }

                  q_mov.setParametricOperands(ang_refs);
                  rmv = true;
                }

              } else
                stuck = !(q_cur.commutesWith(q_mov) == 1);

            } else
              end = true;
          }

          if (!end)
            ++q_mov;
          if (q_mov != q_cur) {
            ASSERT_TRUE(q_cur.removeGate());
            if (!q_cur.isBegin())
              --q_cur;
            if (!rmv)
              ASSERT_TRUE(q_mov.insertGate(kRY, qbit_cur, ang_refs));
          }

        } while (q_cur.gotoNextGateInstance(kRY));

      // verify the result//////////////////////////////////////////
      inst_iterator iter(F);
      QbitRef q0(iter);
      QbitRef q1 = q0;
      q1.setIndexResolved(1);
      QbitRef q2 = q0;
      q2.setIndexResolved(2);
      QbitRef q3 = q0;
      q3.setIndexResolved(3);

      Type *FTy = Type::getDoubleTy(F.getContext());
      ParaRef angle0(1.1234, FTy), angle1(0.35, FTy), angle2(-0.2, FTy);

      QBBIter q_chx(F.getEntryBlock());

      // RZ(q2, angle1)
      EXPECT_EQ(q_chx.getIdentifier(), kRZ);
      EXPECT_EQ(q_chx.getQubitOperand(0).isEqual(q2), 1);
      EXPECT_EQ(q_chx.getParametricOperand(0).isEqual(angle1), 1);
      ++q_chx;

      // H(q3)
      EXPECT_EQ(q_chx.getIdentifier(), kH);
      EXPECT_EQ(q_chx.getQubitOperand(0).isEqual(q3), 1);
      ++q_chx;

      // H(q1)
      EXPECT_EQ(q_chx.getIdentifier(), kH);
      EXPECT_EQ(q_chx.getQubitOperand(0).isEqual(q1), 1);
      ++q_chx;

      // CZ(q0,q1)
      EXPECT_EQ(q_chx.getIdentifier(), kCZ);
      EXPECT_EQ(q_chx.getQubitOperand(0).isEqual(q0), 1);
      EXPECT_EQ(q_chx.getQubitOperand(1).isEqual(q1), 1);
      ++q_chx;

      // RX(q1, angle0)
      EXPECT_EQ(q_chx.getIdentifier(), kRX);
      EXPECT_EQ(q_chx.getQubitOperand(0).isEqual(q1), 1);
      EXPECT_EQ(q_chx.getParametricOperand(0).isEqual(angle0), 1);
      ++q_chx;

      // CX(q3,q2)
      EXPECT_EQ(q_chx.getIdentifier(), kCX);
      EXPECT_EQ(q_chx.getQubitOperand(0).isEqual(q3), 1);
      EXPECT_EQ(q_chx.getQubitOperand(1).isEqual(q2), 1);
      ++q_chx;

      // RY(q1, angle2)
      EXPECT_EQ(q_chx.getIdentifier(), kRY);
      EXPECT_EQ(q_chx.getQubitOperand(0).isEqual(q2), 1);
      EXPECT_EQ(q_chx.getParametricOperand(0).isEqual(angle2), 1);
      ++q_chx;

      // CZ(q1,q3)
      EXPECT_EQ(q_chx.getIdentifier(), kCZ);
      EXPECT_EQ(q_chx.getQubitOperand(0).isEqual(q1), 1);
      EXPECT_EQ(q_chx.getQubitOperand(1).isEqual(q3), 1);
      ++q_chx;
    }
  };

  runPostPasses(*M);
  ParaRef::clearValueOwnedMap();
  QbitRef::clearIndexOwnedMap();
  /////////////////////////////////////////////////////////////////////////////
}

TEST_F(QuantumIteratorTest, GlobalQbitRefIRManipulationTest) {
  LLVMContext C;
  std::unique_ptr<Module> M = parseIR(C, *&Code);

  runInitialPasses(*M);

  // construct on global allocation

  std::vector<QbitRef> q_glob(2);
  unsigned cnt = 0;
  for (auto cnst_iter = M->global_begin(); cnst_iter != M->global_end();
       ++cnst_iter) {

    QbitRef temp(cnst_iter);

    if (!temp.isNull()) {
      q_glob[cnt] = temp;
      cnt++;
    }
  };

  for (Function &F : *M) {
    if (F.getName() == "_Z31GlobalQbitRefIRManipulationTesti") {

      QIter qit(F);
      /*
       source code:

      void GlobalQbitRefIRManipulationTEST_F(int idx){

        qbit q1_loc;
        qbit q2_loc;

        CNOTGate(q1_loc, q2_loc);
        RXGate(q1_loc, 0.2);
        HGate(q1_glob);
        HGate(q2_glob[idx]);

      }
      */

      // replace local qubit arguments
      //
      std::vector<QbitRef> args1(2);
      args1[0] = q_glob[1];
      args1[1] = q_glob[1];
      args1[1].setIndexResolved(1);
      ASSERT_TRUE(qit.setQubitOperands(args1));

      std::vector<QbitRef> args2(1);
      args2[0] = q_glob[0];
      ++qit;
      ASSERT_TRUE(qit.setQubitOperands(args2));

      ++qit;

      // replace H gates with RYs
      std::vector<ParaRef> paras(1);
      Type *Ty = Type::getDoubleTy(F.getContext());
      paras[0].setValueResolved(1.7, Ty);

      ASSERT_TRUE(qit.removeGate());
      ASSERT_TRUE(qit.insertGate(kRY, args2, paras));
      ++qit;

      std::vector<QbitRef> args3 = qit.getQubitOperands();

      ASSERT_TRUE(qit.removeGate());
      ASSERT_TRUE(qit.insertGate(kRY, args3, paras));

      // Check results
      qit.gotoBegin();

      ASSERT_EQ(qit.getIdentifier(), kCX);
      ASSERT_EQ(qit.getQubitOperand(0).isEqual(args1[0]), 1);
      ASSERT_EQ(qit.getQubitOperand(1).isEqual(args1[1]), 1);
      ++qit;

      ASSERT_EQ(qit.getIdentifier(), kRX);
      ASSERT_EQ(qit.getQubitOperand(0).isEqual(args2[0]), 1);
      ++qit;

      ASSERT_EQ(qit.getIdentifier(), kRY);
      ASSERT_EQ(qit.getQubitOperand(0).isEqual(args2[0]), 1);
      ASSERT_EQ(qit.getParametricOperand(0).isEqual(paras[0]), 1);
      ++qit;

      ASSERT_EQ(qit.getIdentifier(), kRY);
      EXPECT_EQ(qit.getQubitOperand(0).isEqual(args3[0]), 1);
      ASSERT_EQ(qit.getParametricOperand(0).isEqual(paras[0]), 1);
      ++qit;
    }
  };

  runPostPasses(*M);
  ParaRef::clearValueOwnedMap();
  QbitRef::clearIndexOwnedMap();
}

TEST_F(QuantumBBIteratorTest, GlobalQbitRefIRManipulationTest) {
  LLVMContext C;
  std::unique_ptr<Module> M = parseIR(C, *&Code);

  runInitialPasses(*M);
  ParaRef::setModule(*M);

  // construct on global allocation

  std::vector<QbitRef> q_glob(2);
  unsigned cnt = 0;
  for (auto cnst_iter = M->global_begin(); cnst_iter != M->global_end();
       ++cnst_iter) {

    QbitRef temp(cnst_iter);

    if (!temp.isNull()) {
      q_glob[cnt] = temp;
      cnt++;
    }
  };

  for (Function &F : *M) {
    if (F.getName() == "_Z31GlobalQbitRefIRManipulationTesti") {

      QBBIter qit(F.getEntryBlock());
      /*
       source code:

      void GlobalQbitRefIRManipulationTEST_F(int idx){

        qbit q1_loc;
        qbit q2_loc;

        CNOTGate(q1_loc, q2_loc);
        RXGate(q1_loc, 0.2);
        HGate(q1_glob);
        HGate(q2_glob[idx]);

      }
      */

      // replace local qubit arguments
      //
      std::vector<QbitRef> args1(2);
      args1[0] = q_glob[1];
      args1[1] = q_glob[1];
      args1[1].setIndexResolved(1);
      ASSERT_TRUE(qit.setQubitOperands(args1));

      std::vector<QbitRef> args2(1);
      args2[0] = q_glob[0];
      ++qit;
      ASSERT_TRUE(qit.setQubitOperands(args2));

      ++qit;

      // replace H gates with RYs
      std::vector<ParaRef> paras(1);
      Type *Ty = Type::getDoubleTy(F.getContext());
      paras[0].setValueResolved(1.7, Ty);

      ASSERT_TRUE(qit.removeGate());
      ASSERT_TRUE(qit.insertGate(kRY, args2, paras));
      ++qit;

      std::vector<QbitRef> args3 = qit.getQubitOperands();

      ASSERT_TRUE(qit.removeGate());
      ASSERT_TRUE(qit.insertGate(kRY, args3, paras));

      // Check results
      qit.gotoBegin();

      ASSERT_EQ(qit.getIdentifier(), kCX);
      ASSERT_EQ(qit.getQubitOperand(0).isEqual(args1[0]), 1);
      ASSERT_EQ(qit.getQubitOperand(1).isEqual(args1[1]), 1);
      ++qit;

      ASSERT_EQ(qit.getIdentifier(), kRX);
      ASSERT_EQ(qit.getQubitOperand(0).isEqual(args2[0]), 1);
      ++qit;

      ASSERT_EQ(qit.getIdentifier(), kRY);
      ASSERT_EQ(qit.getQubitOperand(0).isEqual(args2[0]), 1);
      ASSERT_EQ(qit.getParametricOperand(0).isEqual(paras[0]), 1);
      ++qit;

      ASSERT_EQ(qit.getIdentifier(), kRY);
      EXPECT_EQ(qit.getQubitOperand(0).isEqual(args3[0]), 1);
      ASSERT_EQ(qit.getParametricOperand(0).isEqual(paras[0]), 1);
      for (unsigned i = 0; i < args3.size(); i++) {
        Instruction *I = dyn_cast<Instruction>(args3[i].getIndex());
        if (!I)
          continue;
        for (Value *Op : I->operands()) {
          if (Instruction *IO = dyn_cast<Instruction>(Op)) {
            ASSERT_EQ(IO->comesBefore(I), 1);
          }
        }
        ASSERT_EQ(I->comesBefore(&(*qit)), 1);
      }

      for (unsigned i = 0; i < paras.size(); i++) {
        Instruction *I = dyn_cast<Instruction>(paras[i].getValue());
        if (!I)
          continue;
        for (Value *Op : I->operands()) {
          if (Instruction *IO = dyn_cast<Instruction>(Op)) {
            ASSERT_EQ(IO->comesBefore(I), 1);
          }
        }
        ASSERT_EQ(I->comesBefore(&(*qit)), 1);
      }
      ++qit;
    }
  };

  runPostPasses(*M);
  ParaRef::clearValueOwnedMap();
  QbitRef::clearIndexOwnedMap();
}
