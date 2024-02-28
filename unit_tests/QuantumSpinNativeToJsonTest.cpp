//===- QuantumAnnotationsToJsonTest.cpp -------------------------*- C++ -*-===//
//
//
// Copyright (C) 2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//
//===----------------------------------------------------------------------===//
/// \file "QuantumSpinNativeToJsonTest.cpp"
///
/// This file provides a basic unit test to verify the functionality of the
/// QuantumAnnotationToJsonPass which should build a StringRef -> json::Object
/// map representing the quantum annotation data. This data should be
/// retrievable in the desired data type.
///
//===----------------------------------------------------------------------===//

#include "IntelQuantumPasses/QuantumAnalysis/QuantumSpinNativeToJson.h"
#include "IntelQuantumPasses/QuantumAnalysis/QuantumAnnotationGetters.h"
#include "IntelQuantumPasses/QuantumAnalysis/QuantumCompilerLinkage.h"
#include "IntelQuantumPasses/QuantumAnalysis/QuantumGateIdentifiers.h"

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

#include "include/QuantumPasses/QuantumAnalysis/QuantumSpinNativeToJsonTest.hpp"

using namespace llvm;

static std::unique_ptr<Module> parseIR(LLVMContext &C, const char *IR) {
  SMDiagnostic Err;
  std::unique_ptr<Module> Mod = parseAssemblyString(IR, Err, C);
  if (!Mod)
    Err.print("QuantumSpinNativeToJsonTest", errs());
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
const unsigned GATE_TYPES = 41;

// forward function declaration
// TODO: Not all annotations are autofilled by pass

void TestGate(StringRef IRGateName, std::vector<double> expected_real,
              StringRef expected_real_str, std::vector<double> expected_im,
              StringRef expected_im_str, StringRef expected_matrix_order,
              bool expected_is_hermitian, bool expected_is_unitary,
              bool expected_is_mutable, bool expected_is_user_pulse,
              std::vector<int> expected_qubit_list,
              std::vector<int> expected_parametric_list,
              std::vector<int> expected_control_qubit_list,
              std::vector<int> expected_local_basis_list,
              int expected_identifier);

////////////////////////////////////////////////////////////////////////////////////////////

TEST(QuantumSpinNativeToJsonTest, BasicBuild) {
  LLVMContext C;

  std::unique_ptr<Module> M = parseIR(C, *&Code);

  QuantumAnnotationsToJson *QATJ = new QuantumAnnotationsToJson();
  QuantumSpinNativeToJson *SNTJ = new QuantumSpinNativeToJson();

  QuantumModule QM;
  bool worked = QATJ->doInitialization(*M, QM);
  worked = SNTJ->addSpinNativeFunctions(*M, QM);

  // bool worked = P->doInitialization(*M);

  std::map<StringRef, json::Object> *data_ptr = QATJ->getQGateMetadata(&(*M));

  ASSERT_EQ(data_ptr->size(), GATE_TYPES);

  for (std::map<StringRef, json::Object>::iterator it = data_ptr->begin();
       it != data_ptr->end(); ++it) {
    errs() << "Gate Key : " << it->first << "\n";
  };

  runPostPasses(*M);

  errs() << "Check rotz gate...\n";

  TestGate( // rotz
      "llvm.quantum.rotz", {}, "NativeRZ", {}, "NativeRZ", "rm", false, true,
      true, false, {0}, {1, 2}, {}, {1}, kSpinRotZ);

  errs() << "Check rotxy gate...\n";

  TestGate( // rotxy
      "llvm.quantum.rotxy", {}, "NativeRXY", {}, "NativeRXY", "rm", false, true,
      true, false, {0}, {1, 2, 3}, {}, {0}, kSpinRotXY);

  errs() << "Check cphase gate...\n";

  TestGate( // rotcphase
      "llvm.quantum.cphase", {}, "NativeCPhase", {}, "NativeCPhase", "rm",
      false, true, true, false, {0, 1}, {2, 3}, {0}, {1, 1}, kSpinCPhase);

  errs() << "Check swapalp gate...\n";

  TestGate( // swapalp
      "llvm.quantum.swapalp", {}, "NativeSwapalp", {}, "NativeSwapalp", "rm",
      false, true, true, false, {0, 1}, {2, 3}, {}, {0, 0}, kSpinSwapalp);

  errs() << "Check measz gate...\n";

  TestGate( // measz
      "llvm.quantum.measz", {}, "NativeMeasZ", {}, "NativeMeasZ", "rm", true,
      false, true, false, {0}, {1, 2, 3}, {}, {1}, kSpinMeasZ);

  errs() << "Check prepz gate...\n";

  TestGate( // prepz
      "llvm.quantum.prepz", {}, "NativePrepZ", {}, "NativePrepZ", "rm", false,
      false, true, false, {0}, {1}, {}, {1}, kSpinPrepZ);

  // IMM versions

  errs() << "Check rotz_IMM gate...\n";

  TestGate( // rotz_IMM
      "llvm.quantum.rotz.imm", {}, "NativeRZ_IMM", {}, "NativeRZ_IMM", "rm",
      false, true, true, false, {0}, {1, 2}, {}, {1}, kSpinRotZ_IMM);

  errs() << "Check rotxy_IMM gate...\n";

  TestGate( // rotxy_IMM
      "llvm.quantum.rotxy.imm", {}, "NativeRXY_IMM", {}, "NativeRXY_IMM", "rm",
      false, true, true, false, {0}, {1, 2, 3}, {}, {0}, kSpinRotXY_IMM);

  errs() << "Check cphase_IMM gate...\n";

  TestGate( // rotcphase_IMM
      "llvm.quantum.cphase.imm", {}, "NativeCPhase_IMM", {}, "NativeCPhase_IMM",
      "rm", false, true, true, false, {0, 1}, {2, 3}, {0}, {1, 1},
      kSpinCPhase_IMM);

  errs() << "Check swapalp_IMM gate...\n";

  TestGate( // swapalp_IMM
      "llvm.quantum.swapalp.imm", {}, "NativeSwapalp_IMM", {},
      "NativeSwapalp_IMM", "rm", false, true, true, false, {0, 1}, {2, 3}, {},
      {0, 0}, kSpinSwapalp_IMM);

  errs() << "Check measz_IMM gate...\n";

  TestGate( // measz_IMM
      "llvm.quantum.measz.imm", {}, "NativeMeasZ_IMM", {}, "NativeMeasZ_IMM",
      "rm", true, false, true, false, {0}, {1}, {}, {1}, kSpinMeasZ_IMM);

  errs() << "Check prepz_IMM gate...\n";

  TestGate( // prepz_IMM
      "llvm.quantum.prepz.imm", {}, "NativePrepZ_IMM", {}, "NativePrepZ_IMM",
      "rm", false, false, true, false, {0}, {1}, {}, {1}, kSpinPrepZ_IMM);

  // IMM_IDX versions

  errs() << "Check rotz_IMM_IDX gate...\n";

  TestGate( // rotz_IMM_IDX
      "llvm.quantum.rotz.imm.index", {}, "NativeRZ_IMM_IDX", {},
      "NativeRZ_IMM_IDX", "rm", false, true, true, false, {0}, {1, 2}, {}, {1},
      kSpinRotZ_IMM_IDX);

  errs() << "Check rotxy_IMM_IDX gate...\n";

  TestGate( // rotxy_IMM_IDX
      "llvm.quantum.rotxy.imm.index", {}, "NativeRXY_IMM_IDX", {},
      "NativeRXY_IMM_IDX", "rm", false, true, true, false, {0}, {1, 2, 3}, {},
      {0}, kSpinRotXY_IMM_IDX);

  errs() << "Check cphase_IMM_IDX gate...\n";

  TestGate( // rotcphase_IMM_IDX
      "llvm.quantum.cphase.imm.index", {}, "NativeCPhase_IMM_IDX", {},
      "NativeCPhase_IMM_IDX", "rm", false, true, true, false, {0, 1}, {2, 3},
      {0}, {1, 1}, kSpinCPhase_IMM_IDX);

  errs() << "Check swapalp_IMM_IDX gate...\n";

  TestGate( // swapalp_IMM_IDX
      "llvm.quantum.swapalp.imm.index", {}, "NativeSwapalp_IMM_IDX", {},
      "NativeSwapalp_IMM_IDX", "rm", false, true, true, false, {0, 1}, {2, 3},
      {}, {0, 0}, kSpinSwapalp_IMM_IDX);

  errs() << "Check measz_IMM_IDX gate...\n";

  TestGate( // measz_IMM_IDX
      "llvm.quantum.measz.imm.index", {}, "NativeMeasZ_IMM_IDX", {},
      "NativeMeasZ_IMM_IDX", "rm", true, false, true, false, {0}, {1, 2}, {},
      {1}, kSpinMeasZ_IMM_IDX);
}

////////////////////////////////////////////////////////////////////////////////////
