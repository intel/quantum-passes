//===- QuantumIteratorUnresolvedParameterInsertTest.cpp ---------*- C++ -*-===//
//
//
// Copyright (C) 2022 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//
//===----------------------------------------------------------------------===//
/// \file "QuantumIteratorUnresolvedParameterInsertTest.cpp"
///
/// This file provides a basic unit test of the functionality of the QIter
/// class for the insertion of unresolved parameters.
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

#include "QuantumPasses/QuantumAnalysis/QuantumIteratorUnresolvedParameters.hpp"

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

const double TOL = 0.000000000001;

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

TEST_F(QuantumIteratorTest, InsertEndUnresolvedParameterTest) {
  LLVMContext C;
  std::unique_ptr<Module> M = parseIR(C, *&Code);

  // run if you are using annotation getters or
  // QIter////////////////////////////

  runInitialPasses(*M);

  ///////////////////////////////////////////////////////////////////////////////

  for (Function &F : *M) {
    if (F.getName() == "_Z8QBB__0__v.stub") {

      QIter qit(F);

      std::vector<ParaRef> paras;
      std::vector<QbitRef> qbs = qit.getQubitOperands();

      // get ParaRef for each global array entry
      paras = qit.getParametricOperands();
      ParaRef Para0 = paras[0];
      ++qit;
      paras = qit.getParametricOperands();
      ParaRef Para1 = paras[0];
      ++qit;
      paras = qit.getParametricOperands();
      ParaRef Para2 = paras[0];
      ++qit;
      paras = qit.getParametricOperands();
      ParaRef Para3 = paras[0];
      ++qit;
      paras = qit.getParametricOperands();
      ParaRef Para4 = paras[0];
      ++qit;

      ParaRef Const1(0.1);
      ParaRef Const2(0.2);

      // create different arthmetic
      std::vector<ParaRef> arth;

      arth.push_back(Para0 + Para1);
      arth.push_back(Para0 + Const1);
      arth.push_back(Const1 + Para0);
      arth.push_back(Const1 + Const2);

      arth.push_back(Para0 - Para1);
      arth.push_back(Para0 - Const1);
      arth.push_back(Const1 - Para0);
      arth.push_back(Const1 - Const2);

      arth.push_back(Para0 * Para1);
      arth.push_back(Para0 * Const1);
      arth.push_back(Const1 * Para0);
      arth.push_back(Const1 * Const2);

      arth.push_back(Para0 / Para1);
      arth.push_back(Para0 / Const1);
      arth.push_back(Const1 / Para0);
      arth.push_back(Const1 / Const2);

      arth.push_back(-Para0);
      arth.push_back(-Const1);

      // insert at end
      for (int i = 0; i < 18; i++) {
        paras[0] = arth[i];
        EXPECT_TRUE(qit.insertGate(kRZ, qbs, paras));
        ++qit;
      };

      qit.gotoBegin();
      qit.translateIterator(5);

      /////////////////////////////////////////////////////////////////////

      paras = qit.getParametricOperands();
      EXPECT_EQ(paras[0], arth[0]);
      if (BinaryOperator *BO = dyn_cast<BinaryOperator>(paras[0].getValue())) {
        EXPECT_EQ(Instruction::FAdd, BO->getOpcode());
        auto gep1 = dyn_cast<LoadInst>(Para0.getValue())->getPointerOperand();
        auto gep2 = dyn_cast<LoadInst>(BO->getOperand(0))->getPointerOperand();
        EXPECT_EQ(gep1, gep2);
        gep1 = dyn_cast<LoadInst>(Para1.getValue())->getPointerOperand();
        gep2 = dyn_cast<LoadInst>(BO->getOperand(1))->getPointerOperand();
        EXPECT_EQ(gep1, gep2);
      } else {
        errs() << "No Binary Operator: arth[0].\n";
        EXPECT_TRUE(false);
      }
      ++qit;

      paras = qit.getParametricOperands();
      EXPECT_EQ(paras[0], arth[1]);
      if (BinaryOperator *BO = dyn_cast<BinaryOperator>(paras[0].getValue())) {
        EXPECT_EQ(Instruction::FAdd, BO->getOpcode());
        auto gep1 = dyn_cast<LoadInst>(Para0.getValue())->getPointerOperand();
        auto gep2 = dyn_cast<LoadInst>(BO->getOperand(0))->getPointerOperand();
        EXPECT_EQ(gep1, gep2);
        EXPECT_EQ(Const1.getValue(), BO->getOperand(1));
      } else {
        errs() << "No Binary Operator: arth[1].\n";
        EXPECT_TRUE(false);
      }
      ++qit;

      paras = qit.getParametricOperands();
      EXPECT_EQ(paras[0], arth[2]);
      if (BinaryOperator *BO = dyn_cast<BinaryOperator>(paras[0].getValue())) {
        EXPECT_EQ(Instruction::FAdd, BO->getOpcode());
        EXPECT_EQ(Const1.getValue(), BO->getOperand(0));
        auto gep1 = dyn_cast<LoadInst>(Para0.getValue())->getPointerOperand();
        auto gep2 = dyn_cast<LoadInst>(BO->getOperand(1))->getPointerOperand();
        EXPECT_EQ(gep1, gep2);
      } else {
        errs() << "No Binary Operator: arth[2].\n";
        EXPECT_TRUE(false);
      }
      ++qit;

      paras = qit.getParametricOperands();
      EXPECT_EQ(paras[0], arth[3]);
      EXPECT_NEAR(paras[0].getValueResolved().value(), 0.3, TOL);
      ++qit;

      ///////////////////////////////////////////////////////////////////

      paras = qit.getParametricOperands();
      EXPECT_EQ(paras[0], arth[4]);
      if (BinaryOperator *BO = dyn_cast<BinaryOperator>(paras[0].getValue())) {
        EXPECT_EQ(Instruction::FSub, BO->getOpcode());
        auto gep1 = dyn_cast<LoadInst>(Para0.getValue())->getPointerOperand();
        auto gep2 = dyn_cast<LoadInst>(BO->getOperand(0))->getPointerOperand();
        EXPECT_EQ(gep1, gep2);
        gep1 = dyn_cast<LoadInst>(Para1.getValue())->getPointerOperand();
        gep2 = dyn_cast<LoadInst>(BO->getOperand(1))->getPointerOperand();
        EXPECT_EQ(gep1, gep2);
      } else {
        errs() << "No Binary Operator: arth[4].\n";
        EXPECT_TRUE(false);
      }
      ++qit;

      paras = qit.getParametricOperands();
      EXPECT_EQ(paras[0], arth[5]);
      if (BinaryOperator *BO = dyn_cast<BinaryOperator>(paras[0].getValue())) {
        EXPECT_EQ(Instruction::FSub, BO->getOpcode());
        auto gep1 = dyn_cast<LoadInst>(Para0.getValue())->getPointerOperand();
        auto gep2 = dyn_cast<LoadInst>(BO->getOperand(0))->getPointerOperand();
        EXPECT_EQ(gep1, gep2);
        EXPECT_EQ(Const1.getValue(), BO->getOperand(1));
      } else {
        errs() << "No Binary Operator: arth[5].\n";
        EXPECT_TRUE(false);
      }
      ++qit;

      paras = qit.getParametricOperands();
      EXPECT_EQ(paras[0], arth[6]);
      if (BinaryOperator *BO = dyn_cast<BinaryOperator>(paras[0].getValue())) {
        EXPECT_EQ(Instruction::FSub, BO->getOpcode());
        EXPECT_EQ(Const1.getValue(), BO->getOperand(0));
        auto gep1 = dyn_cast<LoadInst>(Para0.getValue())->getPointerOperand();
        auto gep2 = dyn_cast<LoadInst>(BO->getOperand(1))->getPointerOperand();
        EXPECT_EQ(gep1, gep2);
      } else {
        errs() << "No Binary Operator: arth[6].\n";
        EXPECT_TRUE(false);
      }
      ++qit;

      paras = qit.getParametricOperands();
      EXPECT_EQ(paras[0], arth[7]);
      EXPECT_NEAR(paras[0].getValueResolved().value(), -0.1, TOL);
      ++qit;

      /////////////////////////////////////////////////////////////////////

      paras = qit.getParametricOperands();
      EXPECT_EQ(paras[0], arth[8]);
      if (BinaryOperator *BO = dyn_cast<BinaryOperator>(paras[0].getValue())) {
        EXPECT_EQ(Instruction::FMul, BO->getOpcode());
        auto gep1 = dyn_cast<LoadInst>(Para0.getValue())->getPointerOperand();
        auto gep2 = dyn_cast<LoadInst>(BO->getOperand(0))->getPointerOperand();
        EXPECT_EQ(gep1, gep2);
        gep1 = dyn_cast<LoadInst>(Para1.getValue())->getPointerOperand();
        gep2 = dyn_cast<LoadInst>(BO->getOperand(1))->getPointerOperand();
        EXPECT_EQ(gep1, gep2);
      } else {
        errs() << "No Binary Operator: arth[8].\n";
        EXPECT_TRUE(false);
      }
      ++qit;

      paras = qit.getParametricOperands();
      EXPECT_EQ(paras[0], arth[9]);
      if (BinaryOperator *BO = dyn_cast<BinaryOperator>(paras[0].getValue())) {
        EXPECT_EQ(Instruction::FMul, BO->getOpcode());
        auto gep1 = dyn_cast<LoadInst>(Para0.getValue())->getPointerOperand();
        auto gep2 = dyn_cast<LoadInst>(BO->getOperand(0))->getPointerOperand();
        EXPECT_EQ(gep1, gep2);
        EXPECT_EQ(Const1.getValue(), BO->getOperand(1));
      } else {
        errs() << "No Binary Operator: arth[9].\n";
        EXPECT_TRUE(false);
      }
      ++qit;

      paras = qit.getParametricOperands();
      EXPECT_EQ(paras[0], arth[10]);
      if (BinaryOperator *BO = dyn_cast<BinaryOperator>(paras[0].getValue())) {
        EXPECT_EQ(Instruction::FMul, BO->getOpcode());
        EXPECT_EQ(Const1.getValue(), BO->getOperand(0));
        auto gep1 = dyn_cast<LoadInst>(Para0.getValue())->getPointerOperand();
        auto gep2 = dyn_cast<LoadInst>(BO->getOperand(1))->getPointerOperand();
        EXPECT_EQ(gep1, gep2);
      } else {
        errs() << "No Binary Operator: arth[10].\n";
        EXPECT_TRUE(false);
      }
      ++qit;

      paras = qit.getParametricOperands();
      EXPECT_EQ(paras[0], arth[11]);
      EXPECT_NEAR(paras[0].getValueResolved().value(), 0.02, TOL);
      ++qit;

      /////////////////////////////////////////////////////////////////////

      paras = qit.getParametricOperands();
      EXPECT_EQ(paras[0], arth[12]);
      if (BinaryOperator *BO = dyn_cast<BinaryOperator>(paras[0].getValue())) {
        EXPECT_EQ(Instruction::FDiv, BO->getOpcode());
        auto gep1 = dyn_cast<LoadInst>(Para0.getValue())->getPointerOperand();
        auto gep2 = dyn_cast<LoadInst>(BO->getOperand(0))->getPointerOperand();
        EXPECT_EQ(gep1, gep2);
        gep1 = dyn_cast<LoadInst>(Para1.getValue())->getPointerOperand();
        gep2 = dyn_cast<LoadInst>(BO->getOperand(1))->getPointerOperand();
        EXPECT_EQ(gep1, gep2);
      } else {
        errs() << "No Binary Operator: arth[12].\n";
        EXPECT_TRUE(false);
      }
      ++qit;

      paras = qit.getParametricOperands();
      EXPECT_EQ(paras[0], arth[13]);
      if (BinaryOperator *BO = dyn_cast<BinaryOperator>(paras[0].getValue())) {
        EXPECT_EQ(Instruction::FDiv, BO->getOpcode());
        auto gep1 = dyn_cast<LoadInst>(Para0.getValue())->getPointerOperand();
        auto gep2 = dyn_cast<LoadInst>(BO->getOperand(0))->getPointerOperand();
        EXPECT_EQ(gep1, gep2);
        EXPECT_EQ(Const1.getValue(), BO->getOperand(1));
      } else {
        errs() << "No Binary Operator: arth[13].\n";
        EXPECT_TRUE(false);
      }
      ++qit;

      paras = qit.getParametricOperands();
      EXPECT_EQ(paras[0], arth[14]);
      if (BinaryOperator *BO = dyn_cast<BinaryOperator>(paras[0].getValue())) {
        EXPECT_EQ(Instruction::FDiv, BO->getOpcode());
        EXPECT_EQ(Const1.getValue(), BO->getOperand(0));
        auto gep1 = dyn_cast<LoadInst>(Para0.getValue())->getPointerOperand();
        auto gep2 = dyn_cast<LoadInst>(BO->getOperand(1))->getPointerOperand();
        EXPECT_EQ(gep1, gep2);
      } else {
        errs() << "No Binary Operator: arth[14].\n";
        EXPECT_TRUE(false);
      }
      ++qit;

      paras = qit.getParametricOperands();
      EXPECT_EQ(paras[0], arth[15]);
      EXPECT_NEAR(paras[0].getValueResolved().value(), 0.5, TOL);
      ++qit;

      paras = qit.getParametricOperands();
      EXPECT_EQ(paras[0], arth[16]);
      ParaRef m1(-1.);
      if (UnaryOperator *UO = dyn_cast<UnaryOperator>(paras[0].getValue())) {
        EXPECT_EQ(Instruction::FNeg, UO->getOpcode());
        auto gep1 = dyn_cast<LoadInst>(Para0.getValue())->getPointerOperand();
        auto gep2 = dyn_cast<LoadInst>(UO->getOperand(0))->getPointerOperand();
        EXPECT_EQ(gep1, gep2);
      } else {
        errs() << "No Unary Operator: arth[16].\n";
        EXPECT_TRUE(false);
      }
      ++qit;

      paras = qit.getParametricOperands();
      EXPECT_EQ(paras[0], arth[17]);
      EXPECT_NEAR(paras[0].getValueResolved().value(), -0.1, TOL);
      ++qit;

      ParaRef::deleteAllOwnedParaRefs();
      // EXPECT_TRUE(false);
    }
  };

  runPostPasses(*M);

  /////////////////////////////////////////////////////////////////////////////
}

TEST_F(QuantumIteratorTest, InsertBeginUnresolvedParameterTest) {
  LLVMContext C;
  std::unique_ptr<Module> M = parseIR(C, *&Code);

  // run if you are using annotation getters or
  // QIter////////////////////////////

  runInitialPasses(*M);

  ///////////////////////////////////////////////////////////////////////////////

  for (Function &F : *M) {
    if (F.getName() == "_Z8QBB__0__v.stub") {

      QIter qit(F);

      std::vector<ParaRef> paras;
      std::vector<QbitRef> qbs = qit.getQubitOperands();

      // get ParaRef for each global array entry
      paras = qit.getParametricOperands();
      ParaRef Para0 = paras[0];
      ++qit;
      paras = qit.getParametricOperands();
      ParaRef Para1 = paras[0];
      ++qit;
      paras = qit.getParametricOperands();
      ParaRef Para2 = paras[0];
      ++qit;
      paras = qit.getParametricOperands();
      ParaRef Para3 = paras[0];
      ++qit;
      paras = qit.getParametricOperands();
      ParaRef Para4 = paras[0];
      ++qit;

      ParaRef Const1(0.1);
      ParaRef Const2(0.2);

      // create different arthmetic
      std::vector<ParaRef> arth;

      arth.push_back(Para0 + Para1);
      arth.push_back(Para0 + Const1);
      arth.push_back(Const1 + Para0);
      arth.push_back(Const1 + Const2);

      arth.push_back(Para0 - Para1);
      arth.push_back(Para0 - Const1);
      arth.push_back(Const1 - Para0);
      arth.push_back(Const1 - Const2);

      arth.push_back(Para0 * Para1);
      arth.push_back(Para0 * Const1);
      arth.push_back(Const1 * Para0);
      arth.push_back(Const1 * Const2);

      arth.push_back(Para0 / Para1);
      arth.push_back(Para0 / Const1);
      arth.push_back(Const1 / Para0);
      arth.push_back(Const1 / Const2);

      arth.push_back(-Para0);
      arth.push_back(-Const1);

      // insert at Begining
      qit.gotoBegin();
      // reverse to preserve order
      std::reverse(arth.begin(), arth.end());

      for (int i = 0; i < 18; i++) {
        paras[0] = arth[i];
        EXPECT_TRUE(qit.insertGate(kRZ, qbs, paras));
      };

      qit.gotoBegin();
      // switch order back
      std::reverse(arth.begin(), arth.end());

      /////////////////////////////////////////////////////////////////////

      paras = qit.getParametricOperands();
      EXPECT_EQ(paras[0], arth[0]);
      if (BinaryOperator *BO = dyn_cast<BinaryOperator>(paras[0].getValue())) {
        EXPECT_EQ(Instruction::FAdd, BO->getOpcode());
        auto gep1 = dyn_cast<LoadInst>(Para0.getValue())->getPointerOperand();
        auto gep2 = dyn_cast<LoadInst>(BO->getOperand(0))->getPointerOperand();
        EXPECT_EQ(gep1, gep2);
        gep1 = dyn_cast<LoadInst>(Para1.getValue())->getPointerOperand();
        gep2 = dyn_cast<LoadInst>(BO->getOperand(1))->getPointerOperand();
        EXPECT_EQ(gep1, gep2);
      } else {
        errs() << "No Binary Operator: arth[0].\n";
        EXPECT_TRUE(false);
      }
      ++qit;

      paras = qit.getParametricOperands();
      EXPECT_EQ(paras[0], arth[1]);
      if (BinaryOperator *BO = dyn_cast<BinaryOperator>(paras[0].getValue())) {
        EXPECT_EQ(Instruction::FAdd, BO->getOpcode());
        auto gep1 = dyn_cast<LoadInst>(Para0.getValue())->getPointerOperand();
        auto gep2 = dyn_cast<LoadInst>(BO->getOperand(0))->getPointerOperand();
        EXPECT_EQ(gep1, gep2);
        EXPECT_EQ(Const1.getValue(), BO->getOperand(1));
      } else {
        errs() << "No Binary Operator: arth[1].\n";
        EXPECT_TRUE(false);
      }
      ++qit;

      paras = qit.getParametricOperands();
      EXPECT_EQ(paras[0], arth[2]);
      if (BinaryOperator *BO = dyn_cast<BinaryOperator>(paras[0].getValue())) {
        EXPECT_EQ(Instruction::FAdd, BO->getOpcode());
        EXPECT_EQ(Const1.getValue(), BO->getOperand(0));
        auto gep1 = dyn_cast<LoadInst>(Para0.getValue())->getPointerOperand();
        auto gep2 = dyn_cast<LoadInst>(BO->getOperand(1))->getPointerOperand();
        EXPECT_EQ(gep1, gep2);
      } else {
        errs() << "No Binary Operator: arth[2].\n";
        EXPECT_TRUE(false);
      }
      ++qit;

      paras = qit.getParametricOperands();
      EXPECT_EQ(paras[0], arth[3]);
      EXPECT_NEAR(paras[0].getValueResolved().value(), 0.3, TOL);
      ++qit;

      ///////////////////////////////////////////////////////////////////

      paras = qit.getParametricOperands();
      EXPECT_EQ(paras[0], arth[4]);
      if (BinaryOperator *BO = dyn_cast<BinaryOperator>(paras[0].getValue())) {
        EXPECT_EQ(Instruction::FSub, BO->getOpcode());
        auto gep1 = dyn_cast<LoadInst>(Para0.getValue())->getPointerOperand();
        auto gep2 = dyn_cast<LoadInst>(BO->getOperand(0))->getPointerOperand();
        EXPECT_EQ(gep1, gep2);
        gep1 = dyn_cast<LoadInst>(Para1.getValue())->getPointerOperand();
        gep2 = dyn_cast<LoadInst>(BO->getOperand(1))->getPointerOperand();
        EXPECT_EQ(gep1, gep2);
      } else {
        errs() << "No Binary Operator: arth[4].\n";
        EXPECT_TRUE(false);
      }
      ++qit;

      paras = qit.getParametricOperands();
      EXPECT_EQ(paras[0], arth[5]);
      if (BinaryOperator *BO = dyn_cast<BinaryOperator>(paras[0].getValue())) {
        EXPECT_EQ(Instruction::FSub, BO->getOpcode());
        auto gep1 = dyn_cast<LoadInst>(Para0.getValue())->getPointerOperand();
        auto gep2 = dyn_cast<LoadInst>(BO->getOperand(0))->getPointerOperand();
        EXPECT_EQ(gep1, gep2);
        EXPECT_EQ(Const1.getValue(), BO->getOperand(1));
      } else {
        errs() << "No Binary Operator: arth[5].\n";
        EXPECT_TRUE(false);
      }
      ++qit;

      paras = qit.getParametricOperands();
      EXPECT_EQ(paras[0], arth[6]);
      if (BinaryOperator *BO = dyn_cast<BinaryOperator>(paras[0].getValue())) {
        EXPECT_EQ(Instruction::FSub, BO->getOpcode());
        EXPECT_EQ(Const1.getValue(), BO->getOperand(0));
        auto gep1 = dyn_cast<LoadInst>(Para0.getValue())->getPointerOperand();
        auto gep2 = dyn_cast<LoadInst>(BO->getOperand(1))->getPointerOperand();
        EXPECT_EQ(gep1, gep2);
      } else {
        errs() << "No Binary Operator: arth[6].\n";
        EXPECT_TRUE(false);
      }
      ++qit;

      paras = qit.getParametricOperands();
      EXPECT_EQ(paras[0], arth[7]);
      EXPECT_NEAR(paras[0].getValueResolved().value(), -0.1, TOL);
      ++qit;

      /////////////////////////////////////////////////////////////////////

      paras = qit.getParametricOperands();
      EXPECT_EQ(paras[0], arth[8]);
      if (BinaryOperator *BO = dyn_cast<BinaryOperator>(paras[0].getValue())) {
        EXPECT_EQ(Instruction::FMul, BO->getOpcode());
        auto gep1 = dyn_cast<LoadInst>(Para0.getValue())->getPointerOperand();
        auto gep2 = dyn_cast<LoadInst>(BO->getOperand(0))->getPointerOperand();
        EXPECT_EQ(gep1, gep2);
        gep1 = dyn_cast<LoadInst>(Para1.getValue())->getPointerOperand();
        gep2 = dyn_cast<LoadInst>(BO->getOperand(1))->getPointerOperand();
        EXPECT_EQ(gep1, gep2);
      } else {
        errs() << "No Binary Operator: arth[8].\n";
        EXPECT_TRUE(false);
      }
      ++qit;

      paras = qit.getParametricOperands();
      EXPECT_EQ(paras[0], arth[9]);
      if (BinaryOperator *BO = dyn_cast<BinaryOperator>(paras[0].getValue())) {
        EXPECT_EQ(Instruction::FMul, BO->getOpcode());
        auto gep1 = dyn_cast<LoadInst>(Para0.getValue())->getPointerOperand();
        auto gep2 = dyn_cast<LoadInst>(BO->getOperand(0))->getPointerOperand();
        EXPECT_EQ(gep1, gep2);
        EXPECT_EQ(Const1.getValue(), BO->getOperand(1));
      } else {
        errs() << "No Binary Operator: arth[9].\n";
        EXPECT_TRUE(false);
      }
      ++qit;

      paras = qit.getParametricOperands();
      EXPECT_EQ(paras[0], arth[10]);
      if (BinaryOperator *BO = dyn_cast<BinaryOperator>(paras[0].getValue())) {
        EXPECT_EQ(Instruction::FMul, BO->getOpcode());
        EXPECT_EQ(Const1.getValue(), BO->getOperand(0));
        auto gep1 = dyn_cast<LoadInst>(Para0.getValue())->getPointerOperand();
        auto gep2 = dyn_cast<LoadInst>(BO->getOperand(1))->getPointerOperand();
        EXPECT_EQ(gep1, gep2);
      } else {
        errs() << "No Binary Operator: arth[10].\n";
        EXPECT_TRUE(false);
      }
      ++qit;

      paras = qit.getParametricOperands();
      EXPECT_EQ(paras[0], arth[11]);
      EXPECT_NEAR(paras[0].getValueResolved().value(), 0.02, TOL);
      ++qit;

      /////////////////////////////////////////////////////////////////////

      paras = qit.getParametricOperands();
      EXPECT_EQ(paras[0], arth[12]);
      if (BinaryOperator *BO = dyn_cast<BinaryOperator>(paras[0].getValue())) {
        EXPECT_EQ(Instruction::FDiv, BO->getOpcode());
        auto gep1 = dyn_cast<LoadInst>(Para0.getValue())->getPointerOperand();
        auto gep2 = dyn_cast<LoadInst>(BO->getOperand(0))->getPointerOperand();
        EXPECT_EQ(gep1, gep2);
        gep1 = dyn_cast<LoadInst>(Para1.getValue())->getPointerOperand();
        gep2 = dyn_cast<LoadInst>(BO->getOperand(1))->getPointerOperand();
        EXPECT_EQ(gep1, gep2);
      } else {
        errs() << "No Binary Operator: arth[12].\n";
        EXPECT_TRUE(false);
      }
      ++qit;

      paras = qit.getParametricOperands();
      EXPECT_EQ(paras[0], arth[13]);
      if (BinaryOperator *BO = dyn_cast<BinaryOperator>(paras[0].getValue())) {
        EXPECT_EQ(Instruction::FDiv, BO->getOpcode());
        auto gep1 = dyn_cast<LoadInst>(Para0.getValue())->getPointerOperand();
        auto gep2 = dyn_cast<LoadInst>(BO->getOperand(0))->getPointerOperand();
        EXPECT_EQ(gep1, gep2);
        EXPECT_EQ(Const1.getValue(), BO->getOperand(1));
      } else {
        errs() << "No Binary Operator: arth[13].\n";
        EXPECT_TRUE(false);
      }
      ++qit;

      paras = qit.getParametricOperands();
      EXPECT_EQ(paras[0], arth[14]);
      if (BinaryOperator *BO = dyn_cast<BinaryOperator>(paras[0].getValue())) {
        EXPECT_EQ(Instruction::FDiv, BO->getOpcode());
        EXPECT_EQ(Const1.getValue(), BO->getOperand(0));
        auto gep1 = dyn_cast<LoadInst>(Para0.getValue())->getPointerOperand();
        auto gep2 = dyn_cast<LoadInst>(BO->getOperand(1))->getPointerOperand();
        EXPECT_EQ(gep1, gep2);
      } else {
        errs() << "No Binary Operator: arth[14].\n";
        EXPECT_TRUE(false);
      }
      ++qit;

      paras = qit.getParametricOperands();
      EXPECT_EQ(paras[0], arth[15]);
      EXPECT_NEAR(paras[0].getValueResolved().value(), 0.5, TOL);
      ++qit;

      paras = qit.getParametricOperands();
      EXPECT_EQ(paras[0], arth[16]);
      ParaRef m1(-1.);
      if (UnaryOperator *UO = dyn_cast<UnaryOperator>(paras[0].getValue())) {
        EXPECT_EQ(Instruction::FNeg, UO->getOpcode());
        auto gep1 = dyn_cast<LoadInst>(Para0.getValue())->getPointerOperand();
        auto gep2 = dyn_cast<LoadInst>(UO->getOperand(0))->getPointerOperand();
        EXPECT_EQ(gep1, gep2);
      } else {
        errs() << "No Unary Operator: arth[16].\n";
        EXPECT_TRUE(false);
      }
      ++qit;

      paras = qit.getParametricOperands();
      EXPECT_EQ(paras[0], arth[17]);
      EXPECT_NEAR(paras[0].getValueResolved().value(), -0.1, TOL);
      ++qit;

      ParaRef::deleteAllOwnedParaRefs();
      // ASSERT_TRUE(false);
    }
  };

  runPostPasses(*M);

  /////////////////////////////////////////////////////////////////////////////
}

TEST_F(QuantumIteratorTest, InsertEndChainOfUnresolvedParameterTest) {
  LLVMContext C;
  std::unique_ptr<Module> M = parseIR(C, *&Code);

  // run if you are using annotation getters or
  // QIter////////////////////////////

  runInitialPasses(*M);

  ///////////////////////////////////////////////////////////////////////////////

  for (Function &F : *M) {
    if (F.getName() == "_Z8QBB__0__v.stub") {

      QIter qit(F);

      std::vector<ParaRef> paras;
      std::vector<QbitRef> qbs = qit.getQubitOperands();

      // get ParaRef for each global array entry
      paras = qit.getParametricOperands();
      ParaRef Para0 = paras[0];
      ++qit;
      paras = qit.getParametricOperands();
      ParaRef Para1 = paras[0];
      ++qit;
      paras = qit.getParametricOperands();
      ParaRef Para2 = paras[0];
      ++qit;
      paras = qit.getParametricOperands();
      ParaRef Para3 = paras[0];
      ++qit;
      paras = qit.getParametricOperands();
      ParaRef Para4 = paras[0];
      ++qit;

      ParaRef Const1(0.1);
      ParaRef Const2(0.2);

      // para0 + para1 + para3
      std::vector<ParaRef> chain;
      chain.push_back(Para0 + Para1 + Para2);
      EXPECT_FALSE(chain.back().isNull());

      // para0 - para1 * para2
      chain.push_back(Para0 - Para1 * Para2);
      EXPECT_FALSE(chain.back().isNull());

      // para1 / Const1 * para2
      chain.push_back(Para1 / Const1 * Para2);
      EXPECT_FALSE(chain.back().isNull());

      // (para0 - para1 + para2 + para3 * para4) / Const2;
      chain.push_back((Para0 - Para1 + Para2 + Para3 * Para4) / Const2);
      EXPECT_FALSE(chain.back().isNull());

      for (int i = 0; i < 4; i++) {
        paras[0] = chain[i];
        EXPECT_TRUE(qit.insertGate(kRZ, qbs, paras));
        ++qit;
      };

      qit.gotoBegin();
      qit.translateIterator(5);

      //////////////////////////////////////////////////////////////////////////////

      paras = qit.getParametricOperands();
      EXPECT_EQ(paras[0], chain[0]);
      if (BinaryOperator *BO = dyn_cast<BinaryOperator>(paras[0].getValue())) {
        EXPECT_EQ(Instruction::FAdd, BO->getOpcode());
        auto gep1 = dyn_cast<LoadInst>(Para2.getValue())->getPointerOperand();
        auto gep2 = dyn_cast<LoadInst>(BO->getOperand(1))->getPointerOperand();
        EXPECT_EQ(gep1, gep2);
        if (BinaryOperator *BO1 = dyn_cast<BinaryOperator>(BO->getOperand(0))) {
          EXPECT_EQ(Instruction::FAdd, BO1->getOpcode());
          gep1 = dyn_cast<LoadInst>(Para0.getValue())->getPointerOperand();
          gep2 = dyn_cast<LoadInst>(BO1->getOperand(0))->getPointerOperand();
          EXPECT_EQ(gep1, gep2);
          gep1 = dyn_cast<LoadInst>(Para1.getValue())->getPointerOperand();
          gep2 = dyn_cast<LoadInst>(BO1->getOperand(1))->getPointerOperand();
          EXPECT_EQ(gep1, gep2);
        }

      } else {
        errs() << "No Binary Operator: chain[0].\n";
        EXPECT_TRUE(false);
      }
      ++qit;

      paras = qit.getParametricOperands();
      EXPECT_EQ(paras[0], chain[1]);
      if (BinaryOperator *BO = dyn_cast<BinaryOperator>(paras[0].getValue())) {
        EXPECT_EQ(Instruction::FSub, BO->getOpcode());
        auto gep1 = dyn_cast<LoadInst>(Para0.getValue())->getPointerOperand();
        auto gep2 = dyn_cast<LoadInst>(BO->getOperand(0))->getPointerOperand();
        EXPECT_EQ(gep1, gep2);
        if (BinaryOperator *BO1 = dyn_cast<BinaryOperator>(BO->getOperand(1))) {
          EXPECT_EQ(Instruction::FMul, BO1->getOpcode());
          gep1 = dyn_cast<LoadInst>(Para1.getValue())->getPointerOperand();
          gep2 = dyn_cast<LoadInst>(BO1->getOperand(0))->getPointerOperand();
          EXPECT_EQ(gep1, gep2);
          gep1 = dyn_cast<LoadInst>(Para2.getValue())->getPointerOperand();
          gep2 = dyn_cast<LoadInst>(BO1->getOperand(1))->getPointerOperand();
          EXPECT_EQ(gep1, gep2);
        }

      } else {
        errs() << "No Binary Operator: chain[1].\n";
        EXPECT_TRUE(false);
      }
      ++qit;

      paras = qit.getParametricOperands();
      EXPECT_EQ(paras[0], chain[2]);
      if (BinaryOperator *BO = dyn_cast<BinaryOperator>(paras[0].getValue())) {
        EXPECT_EQ(Instruction::FMul, BO->getOpcode());
        auto gep1 = dyn_cast<LoadInst>(Para2.getValue())->getPointerOperand();
        auto gep2 = dyn_cast<LoadInst>(BO->getOperand(1))->getPointerOperand();
        EXPECT_EQ(gep1, gep2);
        if (BinaryOperator *BO1 = dyn_cast<BinaryOperator>(BO->getOperand(0))) {
          EXPECT_EQ(Instruction::FDiv, BO1->getOpcode());
          gep1 = dyn_cast<LoadInst>(Para1.getValue())->getPointerOperand();
          gep2 = dyn_cast<LoadInst>(BO1->getOperand(0))->getPointerOperand();
          EXPECT_EQ(gep1, gep2);
          ParaRef temp(BO1->getOperand(1));
          EXPECT_NEAR(temp.getValueResolved().value(), 0.1, TOL);
        }

      } else {
        errs() << "No Binary Operator: chain[2].\n";
        EXPECT_TRUE(false);
      }
      ++qit;

      paras = qit.getParametricOperands();
      EXPECT_EQ(paras[0], chain[3]);
      if (BinaryOperator *BO = dyn_cast<BinaryOperator>(paras[0].getValue())) {
        EXPECT_EQ(Instruction::FDiv, BO->getOpcode());
        ParaRef temp(BO->getOperand(1));
        EXPECT_NEAR(temp.getValueResolved().value(), 0.2, TOL);
        if (BinaryOperator *BO1 = dyn_cast<BinaryOperator>(BO->getOperand(0))) {
          EXPECT_EQ(Instruction::FAdd, BO1->getOpcode());
          if (BinaryOperator *BO2 =
                  dyn_cast<BinaryOperator>(BO1->getOperand(0))) {
            EXPECT_EQ(Instruction::FAdd, BO2->getOpcode());
            auto gep1 =
                dyn_cast<LoadInst>(Para2.getValue())->getPointerOperand();
            auto gep2 =
                dyn_cast<LoadInst>(BO2->getOperand(1))->getPointerOperand();
            EXPECT_EQ(gep1, gep2);
            if (BinaryOperator *BO3 =
                    dyn_cast<BinaryOperator>(BO2->getOperand(0))) {
              EXPECT_EQ(Instruction::FSub, BO3->getOpcode());
              gep1 = dyn_cast<LoadInst>(Para0.getValue())->getPointerOperand();
              gep2 =
                  dyn_cast<LoadInst>(BO3->getOperand(0))->getPointerOperand();
              EXPECT_EQ(gep1, gep2);
              gep1 = dyn_cast<LoadInst>(Para1.getValue())->getPointerOperand();
              gep2 =
                  dyn_cast<LoadInst>(BO3->getOperand(1))->getPointerOperand();
              EXPECT_EQ(gep1, gep2);
            }
          }
          if (BinaryOperator *BO2 =
                  dyn_cast<BinaryOperator>(BO1->getOperand(1))) {
            EXPECT_EQ(Instruction::FMul, BO2->getOpcode());
            auto gep1 =
                dyn_cast<LoadInst>(Para3.getValue())->getPointerOperand();
            auto gep2 =
                dyn_cast<LoadInst>(BO2->getOperand(0))->getPointerOperand();
            EXPECT_EQ(gep1, gep2);
            gep1 = dyn_cast<LoadInst>(Para4.getValue())->getPointerOperand();
            gep2 = dyn_cast<LoadInst>(BO2->getOperand(1))->getPointerOperand();
            EXPECT_EQ(gep1, gep2);
          }
        }
      } else {
        errs() << "No Binary Operator: chain[3].\n";
        EXPECT_TRUE(false);
      }
      ++qit;

      ParaRef::deleteAllOwnedParaRefs();
      // EXPECT_TRUE(false);
    }
  };

  runPostPasses(*M);

  /////////////////////////////////////////////////////////////////////////////
}

TEST_F(QuantumIteratorTest, InsertBeginChainOfUnresolvedParameterTest) {
  LLVMContext C;
  std::unique_ptr<Module> M = parseIR(C, *&Code);

  // run if you are using annotation getters or
  // QIter////////////////////////////

  runInitialPasses(*M);

  ///////////////////////////////////////////////////////////////////////////////

  for (Function &F : *M) {
    if (F.getName() == "_Z8QBB__0__v.stub") {

      QIter qit(F);

      std::vector<ParaRef> paras;
      std::vector<QbitRef> qbs = qit.getQubitOperands();

      // get ParaRef for each global array entry
      paras = qit.getParametricOperands();
      ParaRef Para0 = paras[0];
      ++qit;
      paras = qit.getParametricOperands();
      ParaRef Para1 = paras[0];
      ++qit;
      paras = qit.getParametricOperands();
      ParaRef Para2 = paras[0];
      ++qit;
      paras = qit.getParametricOperands();
      ParaRef Para3 = paras[0];
      ++qit;
      paras = qit.getParametricOperands();
      ParaRef Para4 = paras[0];
      ++qit;

      ParaRef Const1(0.1);
      ParaRef Const2(0.2);

      // para0 + para1 + para3
      std::vector<ParaRef> chain;
      chain.push_back(Para0 + Para1 + Para2);
      EXPECT_FALSE(chain.back().isNull());

      // para0 - para1 * para2
      chain.push_back(Para0 - Para1 * Para2);
      EXPECT_FALSE(chain.back().isNull());

      // para1 / Const1 * para2
      chain.push_back(Para1 / Const1 * Para2);
      EXPECT_FALSE(chain.back().isNull());

      // (para0 - para1 + para2 + para3 * para4) / Const2;
      chain.push_back((Para0 - Para1 + Para2 + Para3 * Para4) / Const2);
      EXPECT_FALSE(chain.back().isNull());

      // insert at Begining
      qit.gotoBegin();
      // reverse to preserve order
      std::reverse(chain.begin(), chain.end());

      for (int i = 0; i < 4; i++) {
        paras[0] = chain[i];
        EXPECT_TRUE(qit.insertGate(kRZ, qbs, paras));
      };

      qit.gotoBegin();
      // put in original order
      std::reverse(chain.begin(), chain.end());

      //////////////////////////////////////////////////////////////////////////////

      paras = qit.getParametricOperands();
      EXPECT_EQ(paras[0], chain[0]);
      if (BinaryOperator *BO = dyn_cast<BinaryOperator>(paras[0].getValue())) {
        EXPECT_EQ(Instruction::FAdd, BO->getOpcode());
        auto gep1 = dyn_cast<LoadInst>(Para2.getValue())->getPointerOperand();
        auto gep2 = dyn_cast<LoadInst>(BO->getOperand(1))->getPointerOperand();
        EXPECT_EQ(gep1, gep2);
        if (BinaryOperator *BO1 = dyn_cast<BinaryOperator>(BO->getOperand(0))) {
          EXPECT_EQ(Instruction::FAdd, BO1->getOpcode());
          gep1 = dyn_cast<LoadInst>(Para0.getValue())->getPointerOperand();
          gep2 = dyn_cast<LoadInst>(BO1->getOperand(0))->getPointerOperand();
          EXPECT_EQ(gep1, gep2);
          gep1 = dyn_cast<LoadInst>(Para1.getValue())->getPointerOperand();
          gep2 = dyn_cast<LoadInst>(BO1->getOperand(1))->getPointerOperand();
          EXPECT_EQ(gep1, gep2);
        }

      } else {
        errs() << "No Binary Operator: chain[0].\n";
        EXPECT_TRUE(false);
      }
      ++qit;

      paras = qit.getParametricOperands();
      EXPECT_EQ(paras[0], chain[1]);
      if (BinaryOperator *BO = dyn_cast<BinaryOperator>(paras[0].getValue())) {
        EXPECT_EQ(Instruction::FSub, BO->getOpcode());
        auto gep1 = dyn_cast<LoadInst>(Para0.getValue())->getPointerOperand();
        auto gep2 = dyn_cast<LoadInst>(BO->getOperand(0))->getPointerOperand();
        EXPECT_EQ(gep1, gep2);
        if (BinaryOperator *BO1 = dyn_cast<BinaryOperator>(BO->getOperand(1))) {
          EXPECT_EQ(Instruction::FMul, BO1->getOpcode());
          gep1 = dyn_cast<LoadInst>(Para1.getValue())->getPointerOperand();
          gep2 = dyn_cast<LoadInst>(BO1->getOperand(0))->getPointerOperand();
          EXPECT_EQ(gep1, gep2);
          gep1 = dyn_cast<LoadInst>(Para2.getValue())->getPointerOperand();
          gep2 = dyn_cast<LoadInst>(BO1->getOperand(1))->getPointerOperand();
          EXPECT_EQ(gep1, gep2);
        }

      } else {
        errs() << "No Binary Operator: chain[1].\n";
        EXPECT_TRUE(false);
      }
      ++qit;

      paras = qit.getParametricOperands();
      EXPECT_EQ(paras[0], chain[2]);
      if (BinaryOperator *BO = dyn_cast<BinaryOperator>(paras[0].getValue())) {
        EXPECT_EQ(Instruction::FMul, BO->getOpcode());
        auto gep1 = dyn_cast<LoadInst>(Para2.getValue())->getPointerOperand();
        auto gep2 = dyn_cast<LoadInst>(BO->getOperand(1))->getPointerOperand();
        EXPECT_EQ(gep1, gep2);
        if (BinaryOperator *BO1 = dyn_cast<BinaryOperator>(BO->getOperand(0))) {
          EXPECT_EQ(Instruction::FDiv, BO1->getOpcode());
          gep1 = dyn_cast<LoadInst>(Para1.getValue())->getPointerOperand();
          gep2 = dyn_cast<LoadInst>(BO1->getOperand(0))->getPointerOperand();
          EXPECT_EQ(gep1, gep2);
          ParaRef temp(BO1->getOperand(1));
          EXPECT_NEAR(temp.getValueResolved().value(), 0.1, TOL);
        }

      } else {
        errs() << "No Binary Operator: chain[2].\n";
        EXPECT_TRUE(false);
      }
      ++qit;

      paras = qit.getParametricOperands();
      EXPECT_EQ(paras[0], chain[3]);
      if (BinaryOperator *BO = dyn_cast<BinaryOperator>(paras[0].getValue())) {
        EXPECT_EQ(Instruction::FDiv, BO->getOpcode());
        ParaRef temp(BO->getOperand(1));
        EXPECT_NEAR(temp.getValueResolved().value(), 0.2, TOL);
        if (BinaryOperator *BO1 = dyn_cast<BinaryOperator>(BO->getOperand(0))) {
          EXPECT_EQ(Instruction::FAdd, BO1->getOpcode());
          if (BinaryOperator *BO2 =
                  dyn_cast<BinaryOperator>(BO1->getOperand(0))) {
            EXPECT_EQ(Instruction::FAdd, BO2->getOpcode());
            auto gep1 =
                dyn_cast<LoadInst>(Para2.getValue())->getPointerOperand();
            auto gep2 =
                dyn_cast<LoadInst>(BO2->getOperand(1))->getPointerOperand();
            EXPECT_EQ(gep1, gep2);
            if (BinaryOperator *BO3 =
                    dyn_cast<BinaryOperator>(BO2->getOperand(0))) {
              EXPECT_EQ(Instruction::FSub, BO3->getOpcode());
              gep1 = dyn_cast<LoadInst>(Para0.getValue())->getPointerOperand();
              gep2 =
                  dyn_cast<LoadInst>(BO3->getOperand(0))->getPointerOperand();
              EXPECT_EQ(gep1, gep2);
              gep1 = dyn_cast<LoadInst>(Para1.getValue())->getPointerOperand();
              gep2 =
                  dyn_cast<LoadInst>(BO3->getOperand(1))->getPointerOperand();
              EXPECT_EQ(gep1, gep2);
            }
          }
          if (BinaryOperator *BO2 =
                  dyn_cast<BinaryOperator>(BO1->getOperand(1))) {
            EXPECT_EQ(Instruction::FMul, BO2->getOpcode());
            auto gep1 =
                dyn_cast<LoadInst>(Para3.getValue())->getPointerOperand();
            auto gep2 =
                dyn_cast<LoadInst>(BO2->getOperand(0))->getPointerOperand();
            EXPECT_EQ(gep1, gep2);
            gep1 = dyn_cast<LoadInst>(Para4.getValue())->getPointerOperand();
            gep2 = dyn_cast<LoadInst>(BO2->getOperand(1))->getPointerOperand();
            EXPECT_EQ(gep1, gep2);
          }
        }
      } else {
        errs() << "No Binary Operator: chain[3].\n";
        EXPECT_TRUE(false);
      }
      ++qit;

      ParaRef::deleteAllOwnedParaRefs();
      // EXPECT_TRUE(false);
    }
  };

  runPostPasses(*M);

  /////////////////////////////////////////////////////////////////////////////
}
