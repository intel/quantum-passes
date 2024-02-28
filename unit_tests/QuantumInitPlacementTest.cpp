//===-  QuantumInitPlacementTest.cpp -----------------------------*- C++
//-*-===//
//
//
//===----------------------------------------------------------------------===//
//
// Copyright (C) 2022 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//
//===----------------------------------------------------------------------===//
/// \file " QuantumInitPlacementTest.cpp"
///
/// This file implements unit test for the  QuantumInitPlacement Analysis Pass
///
//===----------------------------------------------------------------------===//

#include "IntelQuantumPasses/QuantumAnalysis/QuantumInitPlacement.h"
#include "IntelQuantumPasses/QuantumAnalysis/QuantumAnnotationsToJson.h"
#include "IntelQuantumPasses/QuantumAnalysis/QuantumCompilerLinkage.h"

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

#include "gtest/gtest-spi.h"
#include "gtest/gtest.h"

#include <map>

#include "QuantumPasses/QuantumCompile/QuantumInitPlacementTest.hpp"

using namespace llvm;
using namespace aqcc;

static std::unique_ptr<Module> parseIR(LLVMContext &C, const char *IR) {
  SMDiagnostic Err;
  std::unique_ptr<Module> Mod = parseAssemblyString(IR, Err, C);
  if (!Mod)
    Err.print("QuantumInitPlacementTest", errs());
  return Mod;
}

static void runInitialPasses(Module &M, QuantumModule &QM) {
  QuantumCompilerLinkage QCL;
  QCL.doInitialization(M, QM);
  QCL.locateOrDeclareQRTFunctions(M, QM);
  QM.setSure();
  QCL.checkForConditioning(M, QM);
  QCL.checkForSeparation(M, QM);
  QCL.findMaxQSDAUsage(M, QM);
  QuantumAnnotationsToJson QAJ;
  QAJ.doInitialization(M, QM);
}

///////////////////////////////////////////////////////////////////////////////////

TEST(QuantumInitPlacementTest, BasicBuild) {

  /* .ll generated from GHZ_5.cpp
          qbit q[5];
          cbit c[7];

          quantum_kernel void GHZ_5() {
            H(q[0]);
            CNOT(q[0], q[1]);
            CNOT(q[1], q[2]);
            CNOT(q[2], q[3]);
            CNOT(q[3], q[4]);
          }

          int main(){
            GHZ_5();
          }
  */

  LLVMContext C;
  std::unique_ptr<Module> M = parseIR(C, *&Code);

  QuantumModule QM;
  // manually set json filename
  QM.sdk_json_filename =
      std::string(CONFIGURATION_DIR) + "/intel-quantum-sdk_12q_linear.json";
  runInitialPasses(*M, QM);

  QuantumInitPlacement QIP;
  QIP.setPlacementMethod("dense");

  QIP.run(*M, QM);

  std::vector<unsigned> expected_assignment = {1, 2, 3, 4, 5};
  unsigned expected_num_prog_qubits = 5;
  for (auto &QK : QM) {
    for (auto &QBB : QK) {
      unsigned phys_qubits = QM.qubit_maps[&QBB]->getNumPhysQubits();
      unsigned prog_qubits = QM.qubit_maps[&QBB]->getNumProgQubits();
      EXPECT_EQ(phys_qubits, 12);
      EXPECT_EQ(prog_qubits, expected_num_prog_qubits);
      for (int i = 0; i < prog_qubits; i++) {
        std::cout << "\tProgram qubit: " << i << " --> Physical qubit: "
                  << QM.qubit_maps[&QBB]->getProgToPhys(i) << "\n";
        EXPECT_EQ(QM.qubit_maps[&QBB]->getProgToPhys(i),
                  expected_assignment[i]);
      }
    }
  }
}

// TEST(QuantumInitPlacementTest, TrivialOn7QAllToAll){

//   LLVMContext C;
//   std::unique_ptr<Module> M = parseIR(C, *&Code);
//   legacy::PassManager PM;

//   auto &QM = QuantumCompilerLinkagePass::QM;

//   //manually set json filename
//   QM.sdk_json_filename =
//   "../../llvm/unittests/Analysis/QuantumPlatformTestFiles/intel-quantum-sdk_7q.json";
//   QM.initializeData(*M);

//   QuantumInitPlacementPass *QIP = new QuantumInitPlacementPass();
//   QIP->setPlacementMethod("trivial");

//   PM.add(new QuantumCompilerLinkageWrapperPass());
//   PM.add(new QuantumAnnotationsToJsonPass());
//   PM.add(QIP);

//   PM.run(*M);

//   unsigned phys_qubits = QIP->num_phys_qubits_;
//   unsigned prog_qubits = QIP->num_prog_qubits_;

//   EXPECT_EQ(phys_qubits, 7);
//   EXPECT_EQ(prog_qubits, 5);

//   std::cout << "Mapping method: " << QIP->placement_method_ << "\n";
//   for(int i = 0; i < QIP->num_prog_qubits_; i++) {
//     std::cout <<  "\tProgram qubit: " << i << " --> Physical qubit: " <<
//     QIP->reference_qubit_map_.getProgToPhys(i)
//               << "\n";
//     // Since we are using the trivial method
//     // Expect program qubit to be mapped to the cooresponding physical qubit
//     // e.g., program q0 -> physical q0
//     EXPECT_EQ(QIP->reference_qubit_map_.getProgToPhys(i), i);
//     }
// }

// TEST(QuantumInitPlacementTest, TooFewPlatformQubits){

//   LLVMContext C;
//   std::unique_ptr<Module> M = parseIR(C, *&Code);
//   legacy::PassManager PM;

//   auto &QM = QuantumCompilerLinkagePass::QM;

//   //manually set json filename
//   QM.sdk_json_filename =
//   "../../llvm/unittests/Analysis/QuantumPlatformTestFiles/intel-quantum-sdk_2q.json";
//   QM.initializeData(*M);

//   QuantumInitPlacementPass *QIP = new QuantumInitPlacementPass();
//   QIP->setPlacementMethod("trivial");

//   PM.add(new QuantumCompilerLinkageWrapperPass());
//   PM.add(new QuantumAnnotationsToJsonPass());
//   PM.add(QIP);

//   // Config json has too few physical qubits to map
//   // expect program to terminate
//   EXPECT_DEATH(PM.run(*M), "ERROR: Quantum SDK - QuantumInitPlacementPass
//   says: Unable to map program qubits to physical qubits.\nLLVM ERROR: Cannot
//   process further. Exiting...");

// }

TEST(QuantumInitPlacementTest, TrivialOn12QLinear) {

  LLVMContext C;
  std::unique_ptr<Module> M = parseIR(C, *&Code);

  QuantumModule QM;
  // manually set json filename
  QM.sdk_json_filename =
      std::string(CONFIGURATION_DIR) + "/intel-quantum-sdk_12q_linear.json";
  runInitialPasses(*M, QM);

  QuantumInitPlacement QIP;
  QIP.setPlacementMethod("trivial");

  QIP.run(*M, QM);

  unsigned expected_num_prog_qubits = 5;

  for (auto &QK : QM) {
    for (auto &QBB : QK) {
      unsigned phys_qubits = QM.qubit_maps[&QBB]->getNumPhysQubits();
      unsigned prog_qubits = QM.qubit_maps[&QBB]->getNumProgQubits();
      EXPECT_EQ(phys_qubits, 12);
      EXPECT_EQ(prog_qubits, expected_num_prog_qubits);
      for (int i = 0; i < prog_qubits; i++) {
        std::cout << "\tProgram qubit: " << i << " --> Physical qubit: "
                  << QM.qubit_maps[&QBB]->getProgToPhys(i) << "\n";
        EXPECT_EQ(QM.qubit_maps[&QBB]->getProgToPhys(i), i);
      }
    }
  }
}

TEST(QuantumInitPlacementTest, DenseOn12QLinear) {

  LLVMContext C;
  std::unique_ptr<Module> M = parseIR(C, *&Code);

  QuantumModule QM;
  // manually set json filename
  QM.sdk_json_filename =
      std::string(CONFIGURATION_DIR) + "/intel-quantum-sdk_12q_linear.json";
  runInitialPasses(*M, QM);

  QuantumInitPlacement QIP;
  QIP.setPlacementMethod("dense");

  QIP.run(*M, QM);

  unsigned expected_num_prog_qubits = 5;
  std::vector<unsigned> expected_assignment = {1, 2, 3, 4,
                                               5}; //{3, 4, 2, 5, 1};

  for (auto &QK : QM) {
    for (auto &QBB : QK) {
      unsigned phys_qubits = QM.qubit_maps[&QBB]->getNumPhysQubits();
      unsigned prog_qubits = QM.qubit_maps[&QBB]->getNumProgQubits();
      EXPECT_EQ(phys_qubits, 12);
      EXPECT_EQ(prog_qubits, expected_num_prog_qubits);
      for (int i = 0; i < prog_qubits; i++) {
        std::cout << "\tProgram qubit: " << i << " --> Physical qubit: "
                  << QM.qubit_maps[&QBB]->getProgToPhys(i) << "\n";
        EXPECT_EQ(QM.qubit_maps[&QBB]->getProgToPhys(i),
                  expected_assignment[i]);
        EXPECT_EQ(QM.qubit_maps[&QBB]->getPhysToProg(expected_assignment[i]),
                  i);
      }
    }
  }
}

TEST(QuantumInitPlacementTest, DenseOverWritesCustomPlacement) {

  LLVMContext C;
  std::unique_ptr<Module> M = parseIR(C, *&Code);

  QuantumModule QM;
  // manually set json filename
  QM.sdk_json_filename =
      std::string(CONFIGURATION_DIR) + "/intel-quantum-sdk_12q_linear.json";
  runInitialPasses(*M, QM);

  QuantumInitPlacement QIP;
  QIP.setPlacementMethod("dense");

  QIP.run(*M, QM);

  unsigned expected_num_prog_qubits = 5;
  std::vector<unsigned> expected_assignment = {1, 2, 3, 4,
                                               5}; //{3, 4, 2, 5, 1};

  for (auto &QK : QM) {
    for (auto &QBB : QK) {
      unsigned phys_qubits = QM.qubit_maps[&QBB]->getNumPhysQubits();
      unsigned prog_qubits = QM.qubit_maps[&QBB]->getNumProgQubits();
      EXPECT_EQ(phys_qubits, 12);
      EXPECT_EQ(prog_qubits, expected_num_prog_qubits);
      for (int i = 0; i < prog_qubits; i++) {
        std::cout << "\tProgram qubit: " << i << " --> Physical qubit: "
                  << QM.qubit_maps[&QBB]->getProgToPhys(i) << "\n";
        EXPECT_EQ(QM.qubit_maps[&QBB]->getProgToPhys(i),
                  expected_assignment[i]);
        EXPECT_EQ(QM.qubit_maps[&QBB]->getPhysToProg(expected_assignment[i]),
                  i);
      }
    }
  }
}

TEST(QuantumInitPlacementTest, TrivialOn12QWithOnlyLocalQubits) {

  LLVMContext C;
  std::unique_ptr<Module> M = parseIR(C, *&Code3);

  QuantumModule QM;
  // manually set json filename
  QM.sdk_json_filename =
      std::string(CONFIGURATION_DIR) + "/intel-quantum-sdk_12q_linear.json";
  runInitialPasses(*M, QM);

  QuantumInitPlacement QIP;
  QIP.setPlacementMethod("dense");

  QIP.run(*M, QM);

  legacy::PassManager PM;

  std::vector<unsigned> expected_num_prog_qubits = {5, 5, 2};
  int idx = 0;

  for (auto &QK : QM) {
    for (auto &QBB : QK) {
      if (idx >= expected_num_prog_qubits.size())
        break;
      unsigned phys_qubits = QM.qubit_maps[&QBB]->getNumPhysQubits();
      unsigned prog_qubits = QM.qubit_maps[&QBB]->getNumProgQubits();
      EXPECT_EQ(phys_qubits, 12);
      EXPECT_EQ(prog_qubits, expected_num_prog_qubits[idx]);
      idx++;
    }
  }
}

TEST(QuantumInitPlacementTest, TrivialOn12QWithLocalandGlobalQubits) {

  LLVMContext C;
  std::unique_ptr<Module> M = parseIR(C, *&Code4);

  QuantumModule QM;
  // manually set json filename
  QM.sdk_json_filename =
      std::string(CONFIGURATION_DIR) + "/intel-quantum-sdk_12q_linear.json";
  runInitialPasses(*M, QM);

  QuantumInitPlacement QIP;
  QIP.setPlacementMethod("dense");

  QIP.run(*M, QM);

  std::vector<unsigned> expected_num_prog_qubits = {5};
  int idx = 0;

  for (auto &QK : QM) {
    for (auto &QBB : QK) {
      if (idx >= expected_num_prog_qubits.size())
        break;
      unsigned phys_qubits = QM.qubit_maps[&QBB]->getNumPhysQubits();
      unsigned prog_qubits = QM.qubit_maps[&QBB]->getNumProgQubits();
      EXPECT_EQ(phys_qubits, 12);
      EXPECT_EQ(prog_qubits, expected_num_prog_qubits[idx]);
      idx++;
    }
  }
}

TEST(QuantumInitPlacementTest, TrivialOn12QPassByReference) {

  LLVMContext C;
  std::unique_ptr<Module> M = parseIR(C, *&Code5);

  QuantumModule QM;
  // manually set json filename
  QM.sdk_json_filename =
      std::string(CONFIGURATION_DIR) + "/intel-quantum-sdk_12q_linear.json";
  runInitialPasses(*M, QM);

  QuantumInitPlacement QIP;
  QIP.setPlacementMethod("dense");

  QIP.run(*M, QM);

  std::vector<unsigned> expected_num_prog_qubits = {6, 6};
  int idx = 0;

  for (auto &QK : QM) {
    for (auto &QBB : QK) {
      if (idx >= expected_num_prog_qubits.size())
        break;
      unsigned phys_qubits = QM.qubit_maps[&QBB]->getNumPhysQubits();
      unsigned prog_qubits = QM.qubit_maps[&QBB]->getNumProgQubits();
      EXPECT_EQ(phys_qubits, 12);
      EXPECT_EQ(prog_qubits, expected_num_prog_qubits[idx]);
      idx++;
    }
  }
}
