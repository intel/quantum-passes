//===- QuantumQbitRefTest.cpp -------------------------*- C++ -*-===//
//
//
// Copyright (C) 2022 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//
//===----------------------------------------------------------------------===//
/// \file "QuantumQbitRefTest.cpp"
///
/// This file provides a basic unit test to verify the functionality of the
/// QbitRef class and its methods.
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

#include "QuantumPasses/QuantumAnalysis/QuantumQbitRefTest.hpp"

using namespace llvm;
using namespace aqcc;

static std::unique_ptr<Module> parseIR(LLVMContext &C, const char *IR) {
  SMDiagnostic Err;
  std::unique_ptr<Module> Mod = parseAssemblyString(IR, Err, C);
  if (!Mod)
    Err.print("QuantumQbitRefTest", errs());
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
TEST(QbitRefTest, <template>)
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

TEST(QbitRefTest, CtorSetterTest) {
  LLVMContext C;
  std::unique_ptr<Module> M = parseIR(C, *&Code);

  for (Function &F : *M) {

    if (F.getName() == "_Z14CtorSetterTestqPqj") {

      /*Source for this function

       void CtorSetterTest( qbit q0, qbit q1[2], unsigned idx_in) {

         qbit q3;
         qbit q4[2];

       }
       */

      // construct on different qubit types
      inst_iterator iter = inst_begin(F);
      QbitRef qref0(&*iter);
      ++iter;
      QbitRef qref1(&*iter);
      ++iter;
      ++iter;
      QbitRef qref2(&*iter);
      ++iter;
      QbitRef qref3(&*iter);

      // for arguments
      Value *arg0 = dyn_cast<Value>(F.getArg(0));
      Value *arg1 = dyn_cast<Value>(F.getArg(1));
      Value *arg2 = dyn_cast<Value>(F.getArg(2));

      // check register is the arguments for qref0, qref1
      ASSERT_EQ(arg0, qref0.getRegister());
      ASSERT_EQ(arg1, qref1.getRegister());

      // check index is const 0 for all
      ASSERT_EQ(0, qref0.getIndexResolved().value());
      ASSERT_EQ(0, qref1.getIndexResolved().value());
      ASSERT_EQ(0, qref2.getIndexResolved().value());
      ASSERT_EQ(0, qref3.getIndexResolved().value());

      // set resolved index of qref1, qref3, qref5
      qref1.setIndexResolved(1);
      qref3.setIndexResolved(1);

      // check index is const 1 for qref1 and qref3
      ASSERT_EQ(1, qref1.getIndexResolved().value());
      ASSERT_EQ(1, qref3.getIndexResolved().value());

      // set unresolved index to idx argument
      qref1.setIndex(arg2);
      qref3.setIndex(arg2);
      QbitRef qref4(arg1, arg2);

      // check unresolved idx argument
      ASSERT_EQ(arg2, qref1.getIndex());
      ASSERT_EQ(arg2, qref3.getIndex());
      ASSERT_EQ(arg2, qref4.getIndex());
    }
  };
}

////////////////////////////////////////////////////////////////////////////////

TEST(QbitRefTest, ResolutionTest) {
  LLVMContext C;
  std::unique_ptr<Module> M = parseIR(C, *&Code);

  runInitialPasses(*M);

  for (Function &F : *M) {
    if (F.getName() == "_Z14ResolutionTestqPqj") {

      /* Source for this file:

        void ResolutionTest( qbit q0, qbit q1[2], unsigned idx_in) {

          qbit q3;
          qbit q4[2];

          int idx = 0;

          //directly resolved
          HGate(q0);
          HGate(q1[0]);
          HGate(q1[1]);
          HGate(q3);
          HGate(q4[0]);
          HGate(q4[1]);

          //resolved through const
          HGate(q1[idx]);
          HGate(q4[idx]);

          //resolved through foldable const
          idx = idx+1;
          HGate(q1[idx]);
          HGate(q4[idx]);

          //unresolvable through argument
          HGate(q1[idx_in]);
          HGate(q4[idx_in]);

        }
        */

      QIter qit(F);

      // directly resolved
      QbitRef qbit0 = qit.getQubitOperand(0);
      ++qit;
      QbitRef qbit10 = qit.getQubitOperand(0);
      ++qit;
      QbitRef qbit11 = qit.getQubitOperand(0);
      ++qit;
      QbitRef qbit2 = qit.getQubitOperand(0);
      ++qit;
      QbitRef qbit30 = qit.getQubitOperand(0);
      ++qit;
      QbitRef qbit31 = qit.getQubitOperand(0);

      ASSERT_TRUE(qbit0.isResolved());
      ASSERT_TRUE(qbit10.isResolved());
      ASSERT_TRUE(qbit11.isResolved());
      ASSERT_TRUE(qbit2.isResolved());
      ASSERT_TRUE(qbit30.isResolved());
      ASSERT_TRUE(qbit31.isResolved());

      // resolved through constant
      ++qit;
      qbit10 = qit.getQubitOperand(0);
      ++qit;
      qbit30 = qit.getQubitOperand(0);

      ASSERT_FALSE(qbit10.isResolved());
      ASSERT_FALSE(qbit30.isResolved());

      qbit10.updateToSimplified();
      qbit30.updateToSimplified();

      errs() << qbit10 << "\n";
      errs() << qbit30 << "\n";

      // For now, we do not yet support this
      // ASSERT_TRUE(qbit10.isResolved());
      // ASSERT_TRUE(qbit30.isResolved());

      // Unresolved through foldable constant
      ++qit;
      qbit10 = qit.getQubitOperand(0);
      ++qit;
      qbit30 = qit.getQubitOperand(0);

      qbit10.updateToSimplified();
      qbit30.updateToSimplified();

      errs() << qbit10 << "\n";
      errs() << qbit30 << "\n";

      // For now, we do not yet support this
      // ASSERT_TRUE(qbit10.isResolved());
      // ASSERT_TRUE(qbit30.isResolved());

      // Unresolved through argument
      ++qit;
      qbit10 = qit.getQubitOperand(0);
      ++qit;
      qbit30 = qit.getQubitOperand(0);

      errs() << qbit10 << "\n";
      errs() << qbit30 << "\n";

      qbit10.updateToSimplified();
      qbit30.updateToSimplified();

      ASSERT_FALSE(qbit10.isResolved());
      ASSERT_FALSE(qbit30.isResolved());
    }
  };
}

TEST(QbitRefTest, EqualityTest) {
  LLVMContext C;
  std::unique_ptr<Module> M = parseIR(C, *&Code);

  runInitialPasses(*M);

  for (Function &F : *M) {
    if (F.getName() == "_Z12EqualityTestqPqjPj") {

      /* Source for this file:
          void EqualityTest( qbit q0, qbit q1[2], unsigned idx_in, unsigned
         idx_in1[2] ){

            qbit q3;
            qbit q4[2];

            //direct equality and inequality
            HGate(q0);
            HGate(q0);
            HGate(q1[0]);
            HGate(q1[0]);
            HGate(q1[1]);
            HGate(q1[1]);
            HGate(q3);
            HGate(q3);
            HGate(q4[0]);
            HGate(q4[0]);
            HGate(q4[1]);
            HGate(q4[1]);

            //Equality/ through unresolved argument
            HGate(q1[idx_in]);
            HGate(q1[idx_in]);
            HGate(q4[idx_in]);
            HGate(q4[idx_in]);
            HGate(q1[idx_in1[0]]);
            HGate(q1[idx_in1[0]]);
            HGate(q4[idx_in1[1]]);
            HGate(q4[idx_in1[1]]);

            //Inequality through unresolved argument
            HGate(q1[idx_in]);
            idx_in++;
            HGate(q1[idx_in]);
            HGate(q4[idx_in]);
            idx_in++;
            HGate(q4[idx_in]);
            HGate(q1[idx_in1[0]]);
            idx_in1[0]++;
            HGate(q1[idx_in1[0]]);
            HGate(q4[idx_in1[1]]);
            idx_in1[1]++;
            HGate(q4[idx_in1[1]]);


            //Unresolvable equality through sum of arguments
            HGate(q1[idx_in + 1]);
            HGate(q1[idx_in + 1]);
            HGate(q4[idx_in + 1]);
            HGate(q4[idx_in + 1]);
            HGate(q1[idx_in1[0] + 1]);
            HGate(q1[idx_in1[0] + 1]);
            HGate(q4[idx_in1[1] + 1]);
            HGate(q4[idx_in1[1] + 1]);
            HGate(q1[idx_in + idx_in1[0]]);
            HGate(q1[idx_in + idx_in1[0]]);
            HGate(q4[idx_in + idx_in1[1]]);
            HGate(q4[idx_in + idx_in1[1]]);

          }
          */

      QIter gate_end1(F);
      QIter gate_end2(F);
      gate_end1.translateIterator(12);
      gate_end2.translateIterator(11);

      QIter gate1(F);
      QIter gate2(F);
      QbitRef qbit1, qbit2;

      // direct equality/inequality
      while ((gate1 != gate_end1)) {

        qbit1 = gate1.getQubitOperand(0);
        gate2 = gate1;
        ++gate2;
        qbit2 = gate2.getQubitOperand(0);

        EXPECT_EQ(qbit1.isEqual(qbit2), 1);

        while (gate2 != gate_end2) {
          ++gate2;
          qbit2 = gate2.getQubitOperand(0);
          EXPECT_EQ(qbit1.isEqual(qbit2), 0);
        }
        gate1.translateIterator(2);
      }

      ////////////////////////////////////////////////
      gate_end2 = gate_end1;
      gate1 = gate_end1;
      gate2 = gate_end1;
      gate_end1.translateIterator(8);
      gate_end2.translateIterator(7);

      // Equality/inequality through unresolved argument
      while ((gate1 != gate_end1)) {

        qbit1 = gate1.getQubitOperand(0);
        gate2 = gate1;
        ++gate2;
        qbit2 = gate2.getQubitOperand(0);
        EXPECT_EQ(qbit1.isEqual(qbit2), 1);

        while (gate2 != gate_end2) {
          ++gate2;
          qbit2 = gate2.getQubitOperand(0);
          if (qbit1.getRegister() == qbit2.getRegister())
            EXPECT_EQ(qbit1.isEqual(qbit2), -1);
          else
            EXPECT_EQ(qbit1.isEqual(qbit2), 0);
        }
        gate1.translateIterator(2);
      }

      gate_end2 = gate_end1;
      gate1 = gate_end1;
      gate_end1.translateIterator(8);
      gate_end2.translateIterator(7);

      // Inequality through unresolved addition to argument
      while ((gate1 != gate_end1)) {

        qbit1 = gate1.getQubitOperand(0);
        gate2 = gate1;
        ++gate2;
        qbit2 = gate2.getQubitOperand(0);
        EXPECT_EQ(qbit1.isEqual(qbit2), -1);

        while (gate2 != gate_end2) {
          ++gate2;
          qbit2 = gate2.getQubitOperand(0);
          if (qbit1.getRegister() == qbit2.getRegister())
            EXPECT_EQ(qbit1.isEqual(qbit2), -1);
          else
            EXPECT_EQ(qbit1.isEqual(qbit2), 0);
        }
        gate1.translateIterator(2);
      }

      gate_end2 = gate_end1;
      gate1 = gate_end1;
      gate_end1.translateIterator(12);
      gate_end2.translateIterator(11);

      // Inequality through unresolved addition to argument
      while ((gate1 != gate_end1)) {

        qbit1 = gate1.getQubitOperand(0);
        gate2 = gate1;
        ++gate2;
        qbit2 = gate2.getQubitOperand(0);
        EXPECT_EQ(qbit1.isEqual(qbit2), 1);

        while (gate2 != gate_end2) {
          ++gate2;
          qbit2 = gate2.getQubitOperand(0);
          if (qbit1.getRegister() == qbit2.getRegister())
            EXPECT_EQ(qbit1.isEqual(qbit2), -1);
          else
            EXPECT_EQ(qbit1.isEqual(qbit2), 0);
        }
        gate1.translateIterator(2);
      }
    }
  };
}

///////////////////////////////////////////////////////////////////////////////////////
//

TEST(QbitRefTest, GlobalTest) {
  LLVMContext C;
  std::unique_ptr<Module> M = parseIR(C, *&Code);

  runInitialPasses(*M);

  // construct on global allocation

  std::vector<QbitRef> global_qregs(2);
  unsigned cnt = 0;
  for (auto cnst_iter = M->global_begin(); cnst_iter != M->global_end();
       ++cnst_iter) {

    QbitRef temp(cnst_iter);

    if (!temp.isNull()) {
      global_qregs[cnt] = temp;
      cnt++;
    }
  };

  for (Function &F : *M) {
    if (F.getName() == "_Z10GlobalTestii") {

      /*
       *    source code:
       *	void GlobalTest(int idx1, int idx2) {

                HGate(q_glob1);
                HGate(q_glob1);
                HGate(q_glob2[0]);
                HGate(q_glob2[0]);
                HGate(q_glob2[1]);
                HGate(q_glob2[1]);
                HGate(q_glob2[idx1]);
                HGate(q_glob2[idx1]);
                HGate(q_glob2[idx1 +1]);
                HGate(q_glob2[idx1 +1]);
                HGate(q_glob2[idx1 +idx2]);
                HGate(q_glob2[idx1 +idx2]);

              }
      */

      QIter qit1(F);
      QIter qit2(F);
      QIter qit_switch(F);
      qit_switch.translateIterator(6);
      QIter qit_stp(F);
      qit_stp.gotoEnd();
      --qit_stp;
      --qit_stp;

      // basic equality
      for (qit1.gotoBegin(); qit1 != qit_switch; qit1.translateIterator(2)) {

        qit2 = qit1;
        ++qit2;
        std::vector<QbitRef> qbit1 = qit1.getQubitOperands();
        std::vector<QbitRef> qbit2 = qit2.getQubitOperands();

        ASSERT_EQ(qbit1[0].isEqual(qbit2[0]), 1);
        ++qit2;

        for (qit2; qit2 != qit_switch; ++qit2) {
          qbit2 = qit2.getQubitOperands();
          ASSERT_EQ(qbit1[0].isEqual(qbit2[0]), 0);
        };

        for (qit2; !qit2.isEnd(); ++qit2) {
          qbit2 = qit2.getQubitOperands();
          if (qit1.isBegin())
            ASSERT_EQ(qbit1[0].isEqual(qbit2[0]), 0);
          else
            ASSERT_EQ(qbit1[0].isEqual(qbit2[0]), -1);
        };
      };

      for (qit1; !qit1.isEnd(); qit1.translateIterator(2)) {

        qit2 = qit1;
        ++qit2;
        std::vector<QbitRef> qbit1 = qit1.getQubitOperands();
        std::vector<QbitRef> qbit2 = qit2.getQubitOperands();

        ASSERT_EQ(qbit1[0].isEqual(qbit2[0]), 1);
        ++qit2;

        for (qit2; !qit2.isEnd(); ++qit2) {
          qbit2 = qit2.getQubitOperands();
          ASSERT_EQ(qbit1[0].isEqual(qbit2[0]), -1);
        };
      };
    }
  };
}

TEST(QbitRefTest, PushBackFailureTest) {

  // FIXED!!
  // This test expresses a compile-time error which prevents vectors of QbitRefs
  // from using push_back
  // However, std::list and emplace_back works fine

  LLVMContext C;
  std::unique_ptr<Module> M = parseIR(C, *&Code);

  runInitialPasses(*M);

  for (Function &F : *M) {
    if (F.getName() == "_Z14ResolutionTestqPqj") {

      QIter qit(F);

      std::list<QbitRef> test_l;
      std::vector<QbitRef> test_v;
      QbitRef qubit = qit.getQubitOperand(0);
      test_l.emplace_back(qubit);
      test_v.push_back(qubit);
    }
  };
}

TEST(QbitRefTest, IndexShiftTest) {
  LLVMContext C;
  std::unique_ptr<Module> M = parseIR(C, *&Code);

  runInitialPasses(*M);

  // construct on global allocation

  std::vector<QbitRef> global_qregs(2);
  unsigned cnt = 0;
  for (auto cnst_iter = M->global_begin(); cnst_iter != M->global_end();
       ++cnst_iter) {

    QbitRef temp(cnst_iter);

    if (!temp.isNull()) {
      global_qregs[cnt] = temp;
      cnt++;
    }
  };

  for (Function &F : *M) {
    if (F.getName() == "_Z10GlobalTestii") {

      /*
       *    source code:
       *	void GlobalTest(int idx1, int idx2) {

                HGate(q_glob1);
                HGate(q_glob1);
                HGate(q_glob2[0]);
                HGate(q_glob2[0]);
                HGate(q_glob2[1]);
                HGate(q_glob2[1]);
                HGate(q_glob2[idx1]);
                HGate(q_glob2[idx1]);
                HGate(q_glob2[idx1 +1]);
                HGate(q_glob2[idx1 +1]);
                HGate(q_glob2[idx1 +idx2]);
                HGate(q_glob2[idx1 +idx2]);

              }
      */

      QIter qit(F);
      Value *idx1 = dyn_cast<Value>(F.getArg(0));
      Value *idx2 = dyn_cast<Value>(F.getArg(1));

      // get a constant index QbitRef
      qit.translateIterator(2);
      QbitRef const_idx = qit.getQubitOperand(0);

      // get a unresolved index
      qit.translateIterator(4);
      QbitRef unresolved_idx = qit.getQubitOperand(0);

      // shift constant by a constant
      QbitRef const_by_const = const_idx;
      const_by_const.shiftIndexByResolved(1);
      EXPECT_EQ(const_by_const.isEqual(global_qregs[1]), 1);

      // shift constant by an unresolved
      QbitRef const_by_unresolved = const_idx;
      const_by_unresolved.shiftIndexBy(unresolved_idx.getIndex());
      qit.translateIterator(2);
      QbitRef idx1p1 = qit.getQubitOperand(0);
      EXPECT_EQ(const_by_unresolved.isEqual(idx1p1), 1);

      // shift unresolved by constant
      QbitRef unresolved_by_const = unresolved_idx;
      unresolved_by_const.shiftIndexByResolved(1);
      EXPECT_EQ(const_by_unresolved.isEqual(idx1p1), 1);

      // shift unresolved by unresolved
      QbitRef unresolved_by_unresolved = unresolved_idx;
      unresolved_by_unresolved.shiftIndexBy(idx2);
      qit.translateIterator(2);
      QbitRef idx1pidx2 = qit.getQubitOperand(0);
      EXPECT_EQ(const_by_unresolved.isEqual(idx1pidx2), 1);

      // We will also test that the generated add instruction do not seg fault
      ParaRef::clearUnownedInValueOwnedMap();
    }
  };
}
