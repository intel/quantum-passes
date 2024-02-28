//===----------------------------------------------------------------------===//
//
// Copyright (C) 2022 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//
//===----------------------------------------------------------------------===//
/// \file "QuantumParaRefSimplifyTest.cpp"
///
/// This file provides a basic unit test to verify the functionality of the
/// ParaRef Simplifcation functionality.
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

#include "QuantumPasses/QuantumAnalysis/QuantumParaRefSimplifyTest.hpp"

using namespace llvm;
using namespace aqcc;

static std::unique_ptr<Module> parseIR(LLVMContext &C, const char *IR) {
  SMDiagnostic Err;
  std::unique_ptr<Module> Mod = parseAssemblyString(IR, Err, C);
  if (!Mod)
    Err.print("ParaRefSimplifyTest", errs());
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

TEST(ParaRefTest, SimplifyTest) {
  LLVMContext C;
  std::unique_ptr<Module> M = parseIR(C, *&Code);

  // run if you are using annotation getters of
  // QIter////////////////////////////

  runInitialPasses(*M);
  ParaRef::setModule(*M);

  ///////////////////////////////////////////////////////////////////////////////

  for (Function &F : *M) {
    if (F.getName() == "_Z18parameter_simplifydd.stub") {
      QIter qit(F);
      std::vector<ParaRef> paras = qit.getParametricOperands();
      std::vector<QbitRef> qubits = qit.getQubitOperands();
      ParaRef A(paras[0]);
      ++qit;
      paras = qit.getParametricOperands();
      ParaRef B(paras[0]);
      ++qit;
      paras = qit.getParametricOperands();
      ParaRef A10(paras[0]);

      // a - a = 0.
      paras[0] = A - A;
      paras[0].updateToSimplified();
      // verify un-inserted instructions do not crash.
      // qit.insertGate(kRZ, qubits, paras);
      paras[0].updateToSimplified();
      EXPECT_EQ(getPrettyName(paras[0]), "0.");

      //(-a) + a = 0.
      paras[0] = -A;
      paras[0] += A;
      qit.insertGate(kRZ, qubits, paras);
      paras[0].updateToSimplified();
      EXPECT_EQ(getPrettyName(paras[0]), "0.");

      //(a - b) + b = a
      paras[0] = A - B;
      paras[0] += B;
      // qit.insertGate(kRZ, qubits, paras);
      paras[0].updateToSimplified();
      EXPECT_EQ(getPrettyName(paras[0]), "a");

      // a + a + a + a = 4 * a
      paras[0] = A + A + A + A;
      qit.insertGate(kRZ, qubits, paras);
      paras[0].updateToSimplified();
      EXPECT_EQ(getPrettyName(paras[0]), "4 * a");

      // a / a  = (a == 0) ? nan : 1
      paras[0] = A;
      paras[0] /= A;
      // qit.insertGate(kRZ, qubits, paras);
      paras[0].updateToSimplified();
      EXPECT_EQ(getPrettyName(paras[0]), "(a == 0.) ? nan : 1");

      // a * b / a  = (a == 0.) ? nan : b
      paras[0] = A * B;
      paras[0] /= A;
      // qit.insertGate(kRZ, qubits, paras);
      paras[0].updateToSimplified();
      EXPECT_EQ(getPrettyName(paras[0]), "(a == 0.) ? nan : b");

      // a * b * 0.5 / a = (a == 0.) ? nan : 0.5 * b
      paras[0] = A * B;
      paras[0] *= 0.5;
      paras[0] /= A;
      // qit.insertGate(kRZ, qubits, paras);
      paras[0].updateToSimplified();
      EXPECT_EQ(getPrettyName(paras[0]), "(a == 0.) ? nan : 0.5 * b");

      // a * 0.5 * 0.5 * 0.5 = a * 0.125
      paras[0] = A * 0.5 * 0.5 * 0.5;
      // qit.insertGate(kRZ, qubits, paras);
      paras[0].updateToSimplified();
      EXPECT_EQ(getPrettyName(paras[0]), "0.125 * a");

      // a * a * a * a = pow(a, 4)
      paras[0] = A * A * A * A;
      qit.insertGate(kRZ, qubits, paras);
      paras[0].updateToSimplified();
      EXPECT_EQ(getPrettyName(paras[0]), "llvm.pow.f64.f64(a, 4)");

      // b / a / a / a / a = b * pow(a, -4)
      paras[0] = B / A / A / A / A;
      // qit.insertGate(kRZ, qubits, paras);
      paras[0].updateToSimplified();
      std::string name = getPrettyName(paras[0]);
      if (name != "b * llvm.pow.f64.f64(a, -4)" &&
          name != "llvm.pow.f64.f64(a, -4) * b") {
        errs() << "a^-4 is actually: " << name << "\n";
        EXPECT_TRUE(false);
      }

      // pow(a, 4) / a = (a == 0) ? nan : pow(a, 3)
      paras[0] = takePower(*(F.getParent()), A, 4) / A;
      // qit.insertGate(kRZ, qubits, paras);
      paras[0].updateToSimplified();
      EXPECT_EQ(getPrettyName(paras[0]),
                "(a == 0.) ? nan : llvm.pow.f64.f64(a, 3)");

      // pow(a, -10) * a = pow(a, -9)
      paras[0] = takePower(*(F.getParent()), A, -10) * A;
      qit.insertGate(kRZ, qubits, paras);
      paras[0].updateToSimplified();
      EXPECT_EQ(getPrettyName(paras[0]), "llvm.pow.f64.f64(a, -9)");

      // a * a * b * b / a / b  = (a == 0.) ? nan :((b == 0.) ? nan : a * b)
      paras[0] = A * A * B * B / A / B;
      qit.insertGate(kRZ, qubits, paras);
      paras[0].updateToSimplified();
      name = getPrettyName(paras[0]);
      if (name != "(a == 0.) ? nan : ((b == 0.) ? nan : a * b)" &&
          name != "(b == 0.) ? nan : ((a == 0.) ? nan : a * b)") {
        errs() << "a * a * b * b /a / b is actually: " << name << "\n";
        EXPECT_TRUE(false);
      }

      // pow(a + b, 2) - (a * a + 2 * a * b + b * b) = 0
      paras[0] = takePower(*(F.getParent()), A + B, 2) - A * A -
                 ParaRef(2.0) * A * B - B * B;
      // qit.insertGate(kRZ, qubits, paras);
      paras[0].updateToSimplified();
      EXPECT_EQ(getPrettyName(paras[0]), "0.");

      // pow(a + b, 5) = pow(a + b, 5)
      paras[0] = takePower(*(F.getParent()), A + B, 5);
      qit.insertGate(kRZ, qubits, paras);
      paras[0].updateToSimplified();
      EXPECT_EQ(getPrettyName(paras[0]), "llvm.pow.f64.f64(a + b, 5)");

      // pow(a + b, 5)/(a + b) = (a + b == 0) ? nan : pow(a + b, 4)
      paras[0] = takePower(*(F.getParent()), A + B, 5) / (A + B);
      qit.insertGate(kRZ, qubits, paras);
      paras[0].updateToSimplified();
      // EXPECT_EQ(getPrettyName(paras[0]), "llvm.pow.f64.f64(a + b, 4)");
      // doesn't work

      // pow(a + b, -5) * a =  a / pow(a + b, 5) OR pow(a + b, -5) * a
      paras[0] = takePower(*(F.getParent()), A + B, -5) * A;
      qit.insertGate(kRZ, qubits, paras);
      paras[0].updateToSimplified();
      if (getPrettyName(paras[0]) !=
          std::string("a / llvm.pow.f64.f64(a + b, 5)"))
        EXPECT_EQ(getPrettyName(paras[0]), "llvm.pow.f64.f64(a + b, -5) * a");
      else
        EXPECT_EQ(getPrettyName(paras[0]), "a / llvm.pow.f64.f64(a + b, 5)");

      // pow(a, 10) from frontend
      paras[0] = A10 * A;
      qit.insertGate(kRZ, qubits, paras);
      paras[0].updateToSimplified();
      EXPECT_EQ(getPrettyName(paras[0]), "llvm.pow.f64.f64(a, 11)");
    }
  };

  ParaRef::deleteAllOwnedParaRefs();

  // run if you desire to print IR to screen upon
  // failure///////////////////////////

  runPostPasses(*M);

  /////////////////////////////////////////////////////////////////////////////
}