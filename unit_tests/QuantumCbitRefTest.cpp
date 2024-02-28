//===- QuantumCbitRefTest.cpp -----------------------------------*- C++ -*-===//
//
//===----------------------------------------------------------------------===//
//
// Copyright (C) 2022 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//
//===----------------------------------------------------------------------===//
/// \file "QuantumCbitRefTest.cpp"
///
/// This file provides a basic unit test to verify the functionality of the
/// ParaRef class and its methods.
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

#include "QuantumPasses/QuantumAnalysis/QuantumCbitRefTest.hpp"

using namespace llvm;
using namespace aqcc;

static std::unique_ptr<Module> parseIR(LLVMContext &C, const char *IR) {
  SMDiagnostic Err;
  std::unique_ptr<Module> Mod = parseAssemblyString(IR, Err, C);
  if (!Mod)
    Err.print("CbitRefTest", errs());
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

const double TOL = 0.000000000001;

////////////////////////////////////////////////////////////////////////////////////////////
/*
TEST(CbitRefTest, <template>)
{
  LLVMContext C;
  std::unique_ptr<Module> M = parseIR(C, *&Code);

//run if you are using annotation getters of QIter////////////////////////////

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

TEST(CbitRefTest, ParametricAccessTest) {
  LLVMContext C;
  std::unique_ptr<Module> M = parseIR(C, *&Code);

  runInitialPasses(*M);

  for (Function &F : *M) {

    if (F.getName() == "_Z19ParameterAccessTestv") {

      /*Source for this function

        void ParameterAccessTest(){
        RX(q[0], 0.1);
        MeasX(q[1], c[0]);
        MeasY(q[2], c[1]);
        MeasZ(q[3], c[2]);
      }
      */

      QIter qit(F);

      // RX
      std::vector<ParaRef> paras = qit.getParametricOperands();
      ASSERT_FALSE(paras[0].isCbitRef());
      ASSERT_TRUE(paras[0].isFloatingPoint());
      ++qit;

      // MeasX
      paras = qit.getParametricOperands();
      ASSERT_TRUE(paras[0].isCbitRef());
      ASSERT_FALSE(paras[0].isFloatingPoint());
      ++qit;

      // MeasY
      paras = qit.getParametricOperands();
      ASSERT_TRUE(paras[0].isCbitRef());
      ASSERT_FALSE(paras[0].isFloatingPoint());
      ++qit;

      // MeasZ
      paras = qit.getParametricOperands();
      ASSERT_TRUE(paras[0].isCbitRef());
      ASSERT_FALSE(paras[0].isFloatingPoint());
      ++qit;
    }
  };
}

////////////////////////////////////////////////////////////////////////////////

TEST(CbitRefTest, CbitEqualTest) {
  LLVMContext C;
  std::unique_ptr<Module> M = parseIR(C, *&Code);

  runInitialPasses(*M);

  for (Function &F : *M) {

    if (F.getName() == "_Z13CbitEqualTestv") {

      /*Source for this function

       void CbitEqualTest(){

       MeasZ(q[0], c[0]);
       MeasZ(q[1], c[0]);

       int i = 1;

       MeasZ(q[2], c[i]);
       MeasZ(q[3], c[i]);
       MeasZ(q[4], c[i+1]);
       MeasZ(q[0], c[i+1]);

     }
     */

      QIter qit1(F);
      std::vector<ParaRef> para1 = qit1.getParametricOperands();

      QIter qit2(F);
      ++qit2;
      std::vector<ParaRef> para2 = qit2.getParametricOperands();
      EXPECT_EQ(para1[0].isEqual(para2[0]), 1);

      ++qit2;
      para2 = qit2.getParametricOperands();
      EXPECT_EQ(para1[0].isEqual(para2[0]), -1);

      ++qit2;
      para2 = qit2.getParametricOperands();
      EXPECT_EQ(para1[0].isEqual(para2[0]), -1);

      ++qit2;
      para2 = qit2.getParametricOperands();
      EXPECT_EQ(para1[0].isEqual(para2[0]), -1);

      ++qit2;
      para2 = qit2.getParametricOperands();
      EXPECT_EQ(para1[0].isEqual(para2[0]), -1);

      ++qit1;
      ++qit1;
      qit2 = qit1;
      ++qit2;
      para2 = qit2.getParametricOperands();
      EXPECT_EQ(para1[0].isEqual(para2[0]), -1);

      ++qit2;
      para2 = qit2.getParametricOperands();
      EXPECT_EQ(para1[0].isEqual(para2[0]), -1);

      ++qit2;
      para2 = qit2.getParametricOperands();
      EXPECT_EQ(para1[0].isEqual(para2[0]), -1);

      ++qit1;
      ++qit1;
      qit2 = qit1;
      ++qit2;
      para2 = qit2.getParametricOperands();
      EXPECT_EQ(para1[0].isEqual(para2[0]), -1);
    }
  };
}
///////////////////////////////////////////////////////////////////////////////////////
//
TEST(CbitRefTest, SwitchGateTest) {
  LLVMContext C;
  std::unique_ptr<Module> M = parseIR(C, *&Code);

  runInitialPasses(*M);

  for (Function &F : *M) {

    if (F.getName() == "_Z14SwitchGateTestv") {

      /*Source for this function
      void SwitchGateTest(){

       int i =1;
       MeasX(q[0], c[0]);
       MeasX(q[1], c[i]);

      }
      */

      QIter qit(F);
      EXPECT_TRUE(qit.changeGate(kMEASY));
      ++qit;

      EXPECT_TRUE(qit.changeGate(kMEASZ));

      qit.gotoBegin();
      EXPECT_EQ(qit.getIdentifier(), kMEASY);
      ++qit;
      EXPECT_EQ(qit.getIdentifier(), kMEASZ);
    }
  };

  legacy::PassManager PMprint;
  PMprint.add(llvm::createPrintModulePass(errs()));
  PMprint.run(*M);
}

TEST(CbitRefTest, InsertMeasTest) {
  LLVMContext C;
  std::unique_ptr<Module> M = parseIR(C, *&Code);

  runInitialPasses(*M);

  for (Function &F : *M) {

    if (F.getName() == "_Z14InsertMeasTestv") {

      /*Source for this function
      void InsertMeasTest(){

         H(q[0]);

      }
      */
      std::vector<ParaRef> cbitref;
      for (auto cnst_iter = M->global_begin(); cnst_iter != M->global_end();
           ++cnst_iter) {

        if (cnst_iter->getName() == "test")
          cbitref.push_back(ParaRef(dyn_cast<Value>(&*cnst_iter)));
      };

      QIter qit(F);
      std::vector<QbitRef> qbts = qit.getQubitOperands();

      // insert before
      EXPECT_TRUE(qit.insertGate(kMEASZ, qbts, {cbitref}));

      ++qit;
      ++qit;
      // insert after
      EXPECT_TRUE(qit.insertGate(kMEASX, qbts, cbitref));

      qit.gotoBegin();

      EXPECT_EQ(qit.getIdentifier(), kMEASZ);
      ++qit;
      EXPECT_EQ(qit.getIdentifier(), kH);
      ++qit;
      EXPECT_EQ(qit.getIdentifier(), kMEASX);
    }
  };

  runPostPasses(*M);
}