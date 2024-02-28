//===- QuantumParaRef.cpp -------------------------*- C++ -*-===//
//
//===----------------------------------------------------------------------===//
//
// Copyright (C) 2022 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//
//===----------------------------------------------------------------------===//
/// \file "QuantumParaRefTest.cpp"
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

#include "QuantumPasses/QuantumAnalysis/QuantumParaRefTest.hpp"

using namespace llvm;
using namespace aqcc;

static std::unique_ptr<Module> parseIR(LLVMContext &C, const char *IR) {
  SMDiagnostic Err;
  std::unique_ptr<Module> Mod = parseAssemblyString(IR, Err, C);
  if (!Mod)
    Err.print("QuantumParaRefTest", errs());
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
TEST(ParaRefTest, <template>)
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

TEST(ParaRefTest, CtorSetterTest) {
  LLVMContext C;
  std::unique_ptr<Module> M = parseIR(C, *&Code);

  runInitialPasses(*M);
  ParaRef::setModule(*M);

  for (Function &F : *M) {

    if (F.getName() == "_Z14CtorSetterTestiPidPd") {

      /*Source for this function

        void CtorSetterTest( int para1, int para2[2], double para3, double
        para4[2]) {

         int para5=0;
         int para6[] ={0,1};
         double para7 =.01;
         double para8[] = {.01,.02};

         qbit q;

          RXGate(q, para1);
          RXGate(q, para2[0]);
          RXGate(q, para2[1]);
          RXGate(q, para3);
          RXGate(q, para4[0]);
          RXGate(q, para4[1]);
          RXGate(q, para5);
          RXGate(q, para6[0]);
          RXGate(q, para6[1]);
          RXGate(q, para7);
          RXGate(q, para8[0]);
          RXGate(q, para8[1]);

        }
      */

      // construct on constant value
      // this requires a type

      Type *FTy = Type::getDoubleTy(F.getContext());
      Type *ITy = Type::getInt32Ty(F.getContext());

      ParaRef para0(0.1, FTy);
      ParaRef para1(1, ITy);

      EXPECT_NEAR(0.1, para0.getValueResolved().value(), TOL);
      EXPECT_NEAR(1, para1.getValueResolved().value(), TOL);

      inst_iterator iter = inst_begin(F);
      QIter qit(F);
      iter = qit.getGateInstIterator();

      --iter;
      ParaRef para2(iter);
      EXPECT_EQ(dyn_cast<Value>(&*iter), para2.getValue());
      iter = ++qit;

      --iter;
      ParaRef para3(iter);
      EXPECT_EQ(dyn_cast<Value>(&*iter), para3.getValue());
      iter = ++qit;

      --iter;
      ParaRef para4(iter);
      EXPECT_EQ(dyn_cast<Value>(&*iter), para4.getValue());
      iter = ++qit;

      --iter;
      ParaRef para5(iter);
      EXPECT_EQ(dyn_cast<Value>(&*iter), para5.getValue());
      iter = ++qit;

      --iter;
      ParaRef para6(iter);
      EXPECT_EQ(dyn_cast<Value>(&*iter), para6.getValue());
      iter = ++qit;

      --iter;
      ParaRef para7(iter);
      EXPECT_EQ(dyn_cast<Value>(&*iter), para7.getValue());
      iter = ++qit;

      --iter;
      ParaRef para8(iter);
      EXPECT_EQ(dyn_cast<Value>(&*iter), para8.getValue());
      iter = ++qit;

      --iter;
      ParaRef para9(iter);
      EXPECT_EQ(dyn_cast<Value>(&*iter), para9.getValue());
      iter = ++qit;

      --iter;
      ParaRef para10(iter);
      EXPECT_EQ(dyn_cast<Value>(&*iter), para10.getValue());
      iter = ++qit;

      --iter;
      ParaRef para11(iter);
      EXPECT_EQ(dyn_cast<Value>(&*iter), para11.getValue());
      iter = ++qit;

      --iter;
      ParaRef para12(iter);
      EXPECT_EQ(dyn_cast<Value>(&*iter), para12.getValue());
      iter = ++qit;

      --iter;
      ParaRef para13(iter);
      EXPECT_EQ(dyn_cast<Value>(&*iter), para13.getValue());
    }
  };
}

////////////////////////////////////////////////////////////////////////////////

TEST(ParaRefTest, ResolutionTest) {
  LLVMContext C;
  std::unique_ptr<Module> M = parseIR(C, *&Code);

  runInitialPasses(*M);
  ParaRef::setModule(*M);

  for (Function &F : *M) {
    if (F.getName() == "_Z14ResolutionTestiPidPd") {

      /* Source for this file:

        void ResolutionTest( int para1, int para2[2], double para3, double
        para4[2]) {

         int para5=0;
         int para6[] ={0,1};
         double para7 =.01;
         double para8[] = {.01,.02};

          qbit q;

          //directly resolved
          RXGate(q, 0);
          RXGate(q, .01);

          //resolved through const
          RXGate(q, para5);
          RXGate(q, para6[0]);
          RXGate(q, para6[1]);
          RXGate(q, para7);
          RXGate(q, para8[0]);
          RXGate(q, para8[1]);

          //resolved through foldable const
          para5 += 1;
          para6[0] +=1;
          para6[1] +=1;
          para7 += 1;
          para8[0] += 1;
          para8[1] += 1;

          RXGate(q, para5);
          RXGate(q, para6[0]);
          RXGate(q, para6[1]);
          RXGate(q, para7);
          RXGate(q, para8[0]);
          RXGate(q, para8[1]);

          //unresolvable through argument
          RXGate(q, para1);
          RXGate(q, para2[0]);
          RXGate(q, para2[1]);
          RXGate(q, para3);
          RXGate(q, para4[0]);
          RXGate(q, para4[1]);

        }
        */

      QIter qit(F);

      // directly resolved
      ParaRef para0 = qit.getParametricOperand(0);
      ++qit;
      ParaRef para1 = qit.getParametricOperand(0);

      EXPECT_TRUE(para0.isResolved());
      EXPECT_TRUE(para1.isResolved());

      // resolved through constant
      ++qit;
      ParaRef para2 = qit.getParametricOperand(0);
      ++qit;
      ParaRef para3 = qit.getParametricOperand(0);
      ++qit;
      ParaRef para4 = qit.getParametricOperand(0);
      ++qit;
      ParaRef para5 = qit.getParametricOperand(0);
      ++qit;
      ParaRef para6 = qit.getParametricOperand(0);
      ++qit;
      ParaRef para7 = qit.getParametricOperand(0);

      EXPECT_FALSE(para2.isResolved());
      EXPECT_FALSE(para3.isResolved());
      EXPECT_FALSE(para4.isResolved());
      EXPECT_FALSE(para5.isResolved());
      EXPECT_FALSE(para6.isResolved());
      EXPECT_FALSE(para7.isResolved());

      para2.updateToSimplified();
      para3.updateToSimplified();
      para4.updateToSimplified();
      para5.updateToSimplified();
      para6.updateToSimplified();
      para7.updateToSimplified();

      // For now, we do not yet support this
      // EXPECT_TRUE(para2.isResolved());
      // EXPECT_TRUE(para3.isResolved());
      // EXPECT_TRUE(para4.isResolved());
      // EXPECT_TRUE(para5.isResolved());
      // EXPECT_TRUE(para6.isResolved());
      // EXPECT_TRUE(para7.isResolved());

      // resolved through foldable constant
      ++qit;
      ParaRef para8 = qit.getParametricOperand(0);
      ++qit;
      ParaRef para9 = qit.getParametricOperand(0);
      ++qit;
      ParaRef para10 = qit.getParametricOperand(0);
      ++qit;
      ParaRef para11 = qit.getParametricOperand(0);
      ++qit;
      ParaRef para12 = qit.getParametricOperand(0);
      ++qit;
      ParaRef para13 = qit.getParametricOperand(0);

      EXPECT_FALSE(para8.isResolved());
      EXPECT_FALSE(para9.isResolved());
      EXPECT_FALSE(para10.isResolved());
      EXPECT_FALSE(para11.isResolved());
      EXPECT_FALSE(para12.isResolved());
      EXPECT_FALSE(para13.isResolved());

      para8.updateToSimplified();
      para9.updateToSimplified();
      para10.updateToSimplified();
      para11.updateToSimplified();
      para12.updateToSimplified();
      para13.updateToSimplified();

      // For now, we do not yet support this
      // EXPECT_TRUE(para8.isResolved());
      // EXPECT_TRUE(para9.isResolved());
      // EXPECT_TRUE(para10.isResolved());
      // EXPECT_TRUE(para11.isResolved());
      // EXPECT_TRUE(para12.isResolved());
      // EXPECT_TRUE(para13.isResolved());

      // Unresolved through argument
      ++qit;
      ParaRef para14 = qit.getParametricOperand(0);
      ++qit;
      ParaRef para15 = qit.getParametricOperand(0);
      ++qit;
      ParaRef para16 = qit.getParametricOperand(0);
      ++qit;
      ParaRef para17 = qit.getParametricOperand(0);
      ++qit;
      ParaRef para18 = qit.getParametricOperand(0);
      ++qit;
      ParaRef para19 = qit.getParametricOperand(0);

      EXPECT_FALSE(para14.isResolved());
      EXPECT_FALSE(para15.isResolved());
      EXPECT_FALSE(para16.isResolved());
      EXPECT_FALSE(para17.isResolved());
      EXPECT_FALSE(para18.isResolved());
      EXPECT_FALSE(para19.isResolved());

      para14.updateToSimplified();
      para15.updateToSimplified();
      para16.updateToSimplified();
      para17.updateToSimplified();
      para18.updateToSimplified();
      para19.updateToSimplified();

      // Should not be able to resolve
      EXPECT_FALSE(para14.isResolved());
      EXPECT_FALSE(para15.isResolved());
      EXPECT_FALSE(para16.isResolved());
      EXPECT_FALSE(para17.isResolved());
      EXPECT_FALSE(para18.isResolved());
      EXPECT_FALSE(para19.isResolved());
    }
  };
}

TEST(ParaRefTest, EqualityTest) {
  LLVMContext C;
  std::unique_ptr<Module> M = parseIR(C, *&Code);

  runInitialPasses(*M);
  ParaRef::setModule(*M);

  for (Function &F : *M) {
    if (F.getName() == "_Z12EqualityTestiPidPd") {

      /* Source for this file:
          void EqualityTest( int para1, int para2[2], double para3, double
         para4[2]) {

           int para5=0;
           int para6[] ={0,1};
           double para7 =.01;
           double para8[] = {.01,.02};

            qbit q;

            //direct equality and inequality
            RXGate(q, 1);
            RXGate(q, 1);
            RXGate(q, 0.1);
            RXGate(q, 0.1);

            //Equality/ through unresolved argument
            RXGate(q, para1);
            RXGate(q, para1);
            RXGate(q, para2[0]);
            RXGate(q, para2[0]);
            RXGate(q, para2[1]);
            RXGate(q, para2[1]);
            RXGate(q, para3);
            RXGate(q, para3);
            RXGate(q, para4[0]);
            RXGate(q, para4[0]);
            RXGate(q, para4[1]);
            RXGate(q, para4[1]);

            //Inequality through unresolved argument
            RXGate(q, para1);
            para1 +=1;
            RXGate(q, para1);
            RXGate(q, para2[0]);
            para2[0] +=1;
            RXGate(q, para2[0]);
            RXGate(q, para2[1]);
            para2[1] +=1;
            RXGate(q, para2[1]);
            RXGate(q, para3);
            para3 +=1;
            RXGate(q, para3);
            RXGate(q, para4[0]);
            para4[0] +=1;
            RXGate(q, para4[0]);
            RXGate(q, para4[1]);
            para4[1] +=1;
            RXGate(q, para4[1]);


            //Unresolvable equality through sum of arguments
            RXGate(q, para1+1);
            RXGate(q, para1+1);
            RXGate(q, para2[0]+1);
            RXGate(q, para2[0]+1);
            RXGate(q, para2[1]+1);
            RXGate(q, para2[1]+1);
            RXGate(q, para3+1);
            RXGate(q, para3+1);
            RXGate(q, para4[0]+1);
            RXGate(q, para4[0]+1);
            RXGate(q, para4[1]+1);
            RXGate(q, para4[1]+1);
            RXGate(q, para1+para2[0]);
            RXGate(q, para3+para4[0]);

          }
          */

      QIter gate_end1(F);
      QIter gate_end2(F);
      gate_end1.translateIterator(4);
      gate_end2.translateIterator(3);

      QIter gate1(F);
      QIter gate2(F);
      ParaRef para1, para2;

      // direct equality/inequality
      while ((gate1 != gate_end1)) {

        para1 = gate1.getParametricOperand(0);
        gate2 = gate1;
        ++gate2;
        para2 = gate2.getParametricOperand(0);

        EXPECT_EQ(para1.isEqual(para2), 1);

        while (gate2 != gate_end2) {
          ++gate2;
          para2 = gate2.getParametricOperand(0);
          EXPECT_EQ(para1.isEqual(para2), 0);
        }
        gate1.translateIterator(2);
      }

      ////////////////////////////////////////////////
      gate_end2 = gate_end1;
      gate1 = gate_end1;
      gate2 = gate_end1;
      gate_end1.translateIterator(12);
      gate_end2.translateIterator(11);

      // Equality/inequality through unresolved argument
      while ((gate1 != gate_end1)) {

        para1 = gate1.getParametricOperand(0);
        gate2 = gate1;
        ++gate2;
        para2 = gate2.getParametricOperand(0);
        EXPECT_EQ(para1.isEqual(para2), 1);

        while (gate2 != gate_end2) {
          ++gate2;
          para2 = gate2.getParametricOperand(0);
          EXPECT_EQ(para1.isEqual(para2), -1);
        }
        gate1.translateIterator(2);
      }

      gate_end2 = gate_end1;
      gate1 = gate_end1;
      gate_end1.translateIterator(12);
      gate_end2.translateIterator(11);

      // Inequality through unresolved addition to argument
      while ((gate1 != gate_end1)) {

        para1 = gate1.getParametricOperand(0);
        gate2 = gate1;
        ++gate2;
        para2 = gate2.getParametricOperand(0);
        EXPECT_EQ(para1.isEqual(para2), -1);

        while (gate2 != gate_end2) {
          ++gate2;
          para2 = gate2.getParametricOperand(0);
          EXPECT_EQ(para1.isEqual(para2), -1);
        }
        gate1.translateIterator(2);
      }

      gate_end2 = gate_end1;
      gate1 = gate_end1;
      gate_end1.translateIterator(16);
      gate_end2.translateIterator(15);

      // Inequality through unresolved addition to argument
      while ((gate1 != gate_end1)) {

        para1 = gate1.getParametricOperand(0);
        gate2 = gate1;
        ++gate2;
        para2 = gate2.getParametricOperand(0);
        if (para1.isEqual(para2) != 1)
          errs() << *para1.getValue() << ", " << *para2.getValue() << "\n";
        EXPECT_EQ(para1.isEqual(para2), 1);

        while (gate2 != gate_end2) {
          ++gate2;
          para2 = gate2.getParametricOperand(0);
          EXPECT_EQ(para1.isEqual(para2), -1);
        }
        gate1.translateIterator(2);
      }
    }
    ParaRef::deleteAllOwnedParaRefs();
  };
}

TEST(ParaRefTest, ArithmeticTest) {
  LLVMContext C;
  std::unique_ptr<Module> M = parseIR(C, *&Code);

  runInitialPasses(*M);
  ParaRef::setModule(*M);

  for (Function &F : *M) {
    if (F.getName() == "_Z14ArithmeticTestddd") {
      /* Source code:

      void ArithmeticTest(double para1, double para2, double cnst) {

       qbit q;

       RXGate(q, 1.2);
       RXGate(q, 1.8);
       RXGate(q, para1);
       RXGate(q, para2);

      }
      */

      QIter qit(F);
      ParaRef para1 = qit.getParametricOperand(0);
      ++qit;
      ParaRef para2 = qit.getParametricOperand(0);
      ++qit;
      ParaRef para3 = qit.getParametricOperand(0);
      ++qit;
      ParaRef para4 = qit.getParametricOperand(0);
      ParaRef para_check;

      // Two const ParaRefs
      para_check = -para1;
      EXPECT_NEAR(para_check.getValueResolved().value(), -1.2, TOL);

      para_check = para1 + para2;
      EXPECT_NEAR(para_check.getValueResolved().value(), 1.2 + 1.8, TOL);

      para_check = para1 - para2;
      EXPECT_NEAR(para_check.getValueResolved().value(), 1.2 - 1.8, TOL);

      para_check = para1 * para2;
      EXPECT_NEAR(para_check.getValueResolved().value(), 1.2 * 1.8, TOL);

      para_check = para1 / para2;
      EXPECT_NEAR(para_check.getValueResolved().value(), 1.2 / 1.8, TOL);

      // const ParaRef and resolved constant
      para_check = para1 + 1.8;
      EXPECT_NEAR(para_check.getValueResolved().value(), 1.2 + 1.8, TOL);

      para_check = para1 - 1.8;
      EXPECT_NEAR(para_check.getValueResolved().value(), 1.2 - 1.8, TOL);

      para_check = para1 * 1.8;
      EXPECT_NEAR(para_check.getValueResolved().value(), 1.2 * 1.8, TOL);

      para_check = para1 / 1.8;
      EXPECT_NEAR(para_check.getValueResolved().value(), 1.2 / 1.8, TOL);

      // ParaRef and constParRef
      para_check = -para3;
      if (UnaryOperator *UO = dyn_cast<UnaryOperator>(para_check.getValue())) {
        EXPECT_EQ(Instruction::FNeg, UO->getOpcode());
        EXPECT_EQ(para3.getValue(), UO->getOperand(0));
      } else {
        errs() << "No Unary Operator: -para3.\n";
        ASSERT_TRUE(false);
      }

      para_check = para1 + para3;
      if (BinaryOperator *BO =
              dyn_cast<BinaryOperator>(para_check.getValue())) {
        EXPECT_EQ(Instruction::FAdd, BO->getOpcode());
        EXPECT_EQ(para1.getValue(), BO->getOperand(0));
        EXPECT_EQ(para3.getValue(), BO->getOperand(1));
      } else {
        errs() << "No Binary Operator: para1 + para3.\n";
        ASSERT_TRUE(false);
      }

      para_check = para1 - para3;
      if (BinaryOperator *BO =
              dyn_cast<BinaryOperator>(para_check.getValue())) {
        EXPECT_EQ(Instruction::FSub, BO->getOpcode());
        EXPECT_EQ(para1.getValue(), BO->getOperand(0));
        EXPECT_EQ(para3.getValue(), BO->getOperand(1));
      } else {
        errs() << "No Binary Operator: para1 - para3.\n";
        ASSERT_TRUE(false);
      }

      para_check = para1 * para3;
      if (BinaryOperator *BO =
              dyn_cast<BinaryOperator>(para_check.getValue())) {
        EXPECT_EQ(Instruction::FMul, BO->getOpcode());
        EXPECT_EQ(para1.getValue(), BO->getOperand(0));
        EXPECT_EQ(para3.getValue(), BO->getOperand(1));
      } else {
        errs() << "No Binary Operator: para1 * para3.\n";
        ASSERT_TRUE(false);
      }

      para_check = para1 / para3;
      if (BinaryOperator *BO =
              dyn_cast<BinaryOperator>(para_check.getValue())) {
        EXPECT_EQ(Instruction::FDiv, BO->getOpcode());
        EXPECT_EQ(para1.getValue(), BO->getOperand(0));
        EXPECT_EQ(para3.getValue(), BO->getOperand(1));
      } else {
        errs() << "No Binary Operator: para1 / para3.\n";
        ASSERT_TRUE(false);
      }

      // ParaRef and ParaRef
      para_check = para3 + para4;
      if (BinaryOperator *BO =
              dyn_cast<BinaryOperator>(para_check.getValue())) {
        EXPECT_EQ(Instruction::FAdd, BO->getOpcode());
        EXPECT_EQ(para3.getValue(), BO->getOperand(0));
        EXPECT_EQ(para4.getValue(), BO->getOperand(1));
      } else {
        errs() << "No Binary Operator: para3 + para4.\n";
        ASSERT_TRUE(false);
      }

      para_check = para3 - para4;
      if (BinaryOperator *BO =
              dyn_cast<BinaryOperator>(para_check.getValue())) {
        EXPECT_EQ(Instruction::FSub, BO->getOpcode());
        EXPECT_EQ(para3.getValue(), BO->getOperand(0));
        EXPECT_EQ(para4.getValue(), BO->getOperand(1));
      } else {
        errs() << "No Binary Operator: para3 - para4.\n";
        ASSERT_TRUE(false);
      }

      para_check = para3 * para4;
      if (BinaryOperator *BO =
              dyn_cast<BinaryOperator>(para_check.getValue())) {
        EXPECT_EQ(Instruction::FMul, BO->getOpcode());
        EXPECT_EQ(para3.getValue(), BO->getOperand(0));
        EXPECT_EQ(para4.getValue(), BO->getOperand(1));
      } else {
        errs() << "No Binary Operator: para3 * para4.\n";
        ASSERT_TRUE(false);
      }

      para_check = para3 / para4;
      if (BinaryOperator *BO =
              dyn_cast<BinaryOperator>(para_check.getValue())) {
        EXPECT_EQ(Instruction::FDiv, BO->getOpcode());
        EXPECT_EQ(para3.getValue(), BO->getOperand(0));
        EXPECT_EQ(para4.getValue(), BO->getOperand(1));
      } else {
        errs() << "No Binary Operator: para3 / para4.\n";
        ASSERT_TRUE(false);
      }

      // Two const ParaRefs
      para1 += para2;
      EXPECT_NEAR(para1.getValueResolved().value(), 1.2 + 1.8, TOL);

      para1 -= para2;
      EXPECT_NEAR(para1.getValueResolved().value(), 1.2, TOL);

      para1 *= para2;
      EXPECT_NEAR(para1.getValueResolved().value(), 1.2 * 1.8, TOL);

      para1 /= para2;
      EXPECT_NEAR(para1.getValueResolved().value(), 1.2, TOL);

      // const ParaRef and resolved constant
      para1 += 1.8;
      EXPECT_NEAR(para1.getValueResolved().value(), 1.2 + 1.8, TOL);

      para1 -= 1.8;
      EXPECT_NEAR(para1.getValueResolved().value(), 1.2, TOL);

      para1 *= 1.8;
      EXPECT_NEAR(para1.getValueResolved().value(), 1.2 * 1.8, TOL);

      para1 /= 1.8;
      EXPECT_NEAR(para1.getValueResolved().value(), 1.2, TOL);

      // ParaRef and constParRef
      para_check = para1;
      para_check += para3;
      if (BinaryOperator *BO =
              dyn_cast<BinaryOperator>(para_check.getValue())) {
        EXPECT_EQ(Instruction::FAdd, BO->getOpcode());
        EXPECT_EQ(para1.getValue(), BO->getOperand(0));
        EXPECT_EQ(para3.getValue(), BO->getOperand(1));
      } else {
        errs() << "No Binary Operator: para1 + para3.\n";
        ASSERT_TRUE(false);
      }

      para_check = para1;
      para_check -= para3;
      if (BinaryOperator *BO =
              dyn_cast<BinaryOperator>(para_check.getValue())) {
        EXPECT_EQ(Instruction::FSub, BO->getOpcode());
        EXPECT_EQ(para1.getValue(), BO->getOperand(0));
        EXPECT_EQ(para3.getValue(), BO->getOperand(1));
      } else {
        errs() << "No Binary Operator: para1 - para3.\n";
        ASSERT_TRUE(false);
      }

      para_check = para1;
      para_check *= para3;
      if (BinaryOperator *BO =
              dyn_cast<BinaryOperator>(para_check.getValue())) {
        EXPECT_EQ(Instruction::FMul, BO->getOpcode());
        EXPECT_EQ(para1.getValue(), BO->getOperand(0));
        EXPECT_EQ(para3.getValue(), BO->getOperand(1));
      } else {
        errs() << "No Binary Operator: para1 * para3.\n";
        ASSERT_TRUE(false);
      }

      para_check = para1;
      para_check /= para3;
      if (BinaryOperator *BO =
              dyn_cast<BinaryOperator>(para_check.getValue())) {
        EXPECT_EQ(Instruction::FDiv, BO->getOpcode());
        EXPECT_EQ(para1.getValue(), BO->getOperand(0));
        EXPECT_EQ(para3.getValue(), BO->getOperand(1));
      } else {
        errs() << "No Binary Operator: para1 / para3.\n";
        ASSERT_TRUE(false);
      }

      // ParaRef and ParaRef
      para_check = para3;
      para_check += para4;
      if (BinaryOperator *BO =
              dyn_cast<BinaryOperator>(para_check.getValue())) {
        EXPECT_EQ(Instruction::FAdd, BO->getOpcode());
        EXPECT_EQ(para3.getValue(), BO->getOperand(0));
        EXPECT_EQ(para4.getValue(), BO->getOperand(1));
      } else {
        errs() << "No Binary Operator: para3 + para4.\n";
        ASSERT_TRUE(false);
      }

      para_check = para3;
      para_check -= para4;
      if (BinaryOperator *BO =
              dyn_cast<BinaryOperator>(para_check.getValue())) {
        EXPECT_EQ(Instruction::FSub, BO->getOpcode());
        EXPECT_EQ(para3.getValue(), BO->getOperand(0));
        EXPECT_EQ(para4.getValue(), BO->getOperand(1));
      } else {
        errs() << "No Binary Operator: para3 - para4.\n";
        ASSERT_TRUE(false);
      }

      para_check = para3;
      para_check *= para4;
      if (BinaryOperator *BO =
              dyn_cast<BinaryOperator>(para_check.getValue())) {
        EXPECT_EQ(Instruction::FMul, BO->getOpcode());
        EXPECT_EQ(para3.getValue(), BO->getOperand(0));
        EXPECT_EQ(para4.getValue(), BO->getOperand(1));
      } else {
        errs() << "No Binary Operator: para3 * para4.\n";
        ASSERT_TRUE(false);
      }

      para_check = para3;
      para_check /= para4;
      if (BinaryOperator *BO =
              dyn_cast<BinaryOperator>(para_check.getValue())) {
        EXPECT_EQ(Instruction::FDiv, BO->getOpcode());
        EXPECT_EQ(para3.getValue(), BO->getOperand(0));
        EXPECT_EQ(para4.getValue(), BO->getOperand(1));
      } else {
        errs() << "No Binary Operator: para3 / para4.\n";
        ASSERT_TRUE(false);
      }

      ParaRef::deleteAllOwnedParaRefs();
    }
  };

  runPostPasses(*M);
}

///////////////////////////////////////////////////////////////////////////////////////
//
