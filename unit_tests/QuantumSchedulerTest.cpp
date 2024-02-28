//===- QuantumQbitRefTest.cpp -------------------------*- C++ -*-===//
//
//
// Copyright (C) 2023 Intel Corporation
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
#include "IntelQuantumPasses/QuantumAnalysis/QuantumInitPlacement.h"
#include "IntelQuantumPasses/QuantumCompile/Scheduler.h"
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
#include "llvm/Support/JSON.h" //JSON PACKAGE
#include "llvm/Support/SourceMgr.h"
#include "llvm/Support/raw_ostream.h"

#include "gtest/gtest-spi.h"
#include "gtest/gtest.h"

#include <map>
#include <vector>

// Minimum working .ll file
#include "include/QuantumPasses/QuantumCompile/QuantumSchedulerTest.hpp"

// Code representing a single QBB with a few CNOT gates that cannot be scheduled
// without SWAPs on a linear connectivity. QBB is "hold" so initial and final
// qubit map should correspond to the desired ones from the QuantumModule.
// --> CodeHold

// Code representing a single QBB with a few CNOT gates that cannot be scheduled
// without SWAPs on a linear connectivity. QBB is "release" so the final qubit
// map should be updated in the QuantumModule.
// --> CodeRelease

// Code representing a single QBB with a several CNOT gates on 10 qubits.
// QBB is "hold".
// --> CodeCnots

using namespace llvm;
using namespace aqcc;

static std::unique_ptr<Module> parseIR(LLVMContext &C, const char *IR) {
  SMDiagnostic Err;
  std::unique_ptr<Module> Mod = parseAssemblyString(IR, Err, C);
  if (!Mod)
    Err.print("QuantumSchedulerTest", errs());
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

  QuantumSpinNativeToJson SNTJ = QuantumSpinNativeToJson();
  SNTJ.addSpinNativeFunctions(M, QM);
}

///////////////////////////////////////////////////////////////////////////////////

// Test of QuantumScheduler.setupForSpecificBackwardSchedulingMethods()
TEST(QuantumSchedulerTest, setupForSpecificBackwardSchedulingMethodsTest) {
  LLVMContext C;
  std::unique_ptr<Module> M = parseIR(C, *&CodeHold);
  QuantumModule QM;
  QuantumScheduler QS;

  // The function should recognize that the connectivity is a 12x1 array.
  QM.sdk_json_filename =
      std::string(CONFIGURATION_DIR) + "/intel-quantum-sdk_12q_linear.json";
  runInitialPasses(*M, QM);
  QS.setBackwardSchedulingMethod("grid");
  testing::internal::CaptureStderr();
  QS.setupForSpecificBackwardSchedulingMethods(QM);
  std::string output = testing::internal::GetCapturedStderr();
  EXPECT_EQ(QS.getBackwardSchedulingMethod(), "grid_12x1");
  EXPECT_EQ(
      output,
      "WARNING: Quantum SDK - Scheduler says: Scheduler has num_phys_qubits_=0 "
      "different from the one (12) in the QuantumModule. Update the former.\n");

  // The function should recognize that the connectivity is a 5x5 array.
  QM.sdk_json_filename =
      std::string(CONFIGURATION_DIR) + "/square_25q_sdk.json";
  runInitialPasses(*M, QM);
  EXPECT_EQ(QM.getNumPlatformQubits(), 25);
  // However the num_phys_qubits_ is not updateed in the QS.
  // This is done in QS.setupForSpecificBackwardSchedulingMethods() for "grid".
  QS.setBackwardSchedulingMethod("grid");
  EXPECT_EQ(QS.num_phys_qubits_, 12);
  testing::internal::CaptureStderr();
  QS.setupForSpecificBackwardSchedulingMethods(QM);
  output = testing::internal::GetCapturedStderr();
  EXPECT_EQ(output, "WARNING: Quantum SDK - Scheduler says: Scheduler has "
                    "num_phys_qubits_=12 different from the one (25) in the "
                    "QuantumModule. Update the former.\n");
  EXPECT_EQ(QS.num_phys_qubits_, 25);
  EXPECT_EQ(QS.getBackwardSchedulingMethod(), "grid_5x5");
}

///////////////////////////////////////////////////////////////////////////////////

// Holistic test of behavior for hold QBB.
TEST(QuantumSchedulerTest, HoldQBB) {
  /* .ll generated from CNOTs_beyond_linear_connectivity.cpp
          qbit q[5];
            H(q[0]);
            CNOT(q[0], q[1]);
            CNOT(q[1], q[2]);
            CNOT(q[3], q[0]);
            CNOT(q[1], q[4]);
            CNOT(q[4], q[2]);
            CNOT(q[2], q[3]);
            // release_quantum_state(); // Uncomment to release the quantum
     state.
          }

          int main(){
            circuit();
          }
  */

  LLVMContext C;
  std::unique_ptr<Module> M = parseIR(C, *&CodeHold);

  QuantumModule QM;
  // manually set json filename
  QM.sdk_json_filename =
      std::string(CONFIGURATION_DIR) + "/intel-quantum-sdk_12q_linear.json";
  runInitialPasses(*M, QM);

  QuantumInitPlacement QIP;
  QIP.setPlacementMethod("trivial");
  QIP.run(*M, QM);

  QuantumScheduler QS;
  QS.initialization(QM);
  // QS.readCommandLineOptions() is avoided in favor of setting the
  // routing methods directly.
  QS.setForwardSchedulingMethodAndSerializeOption("greedy");
  QS.setBackwardSchedulingMethod("bubble-sort");
  EXPECT_EQ(QS.getForwardSchedulingMethod(), "greedy");
  EXPECT_EQ(QS.getBackwardSchedulingMethod(), "bubble-sort");
  QS.runImpl(*M, QM);

  std::vector<unsigned> expected_assignment = {0, 1, 2, 3, 4};
  unsigned expected_num_prog_qubits = 5;
  for (auto &QK : QM) {
    for (auto &QBB : QK) {
      unsigned phys_qubits = QM.qubit_maps[&QBB]->getNumPhysQubits();
      unsigned prog_qubits = QM.qubit_maps[&QBB]->getNumProgQubits();
      EXPECT_EQ(phys_qubits, 12);
      EXPECT_EQ(prog_qubits, expected_num_prog_qubits);
      for (int i = 0; i < prog_qubits; i++) {
        EXPECT_EQ(QM.qubit_maps[&QBB]->getProgToPhys(i),
                  expected_assignment[i]);
        // Confirm that final map of the QBB is the same as the initial one.
        EXPECT_EQ(QM.final_qubit_maps[&QBB]->getProgToPhys(i),
                  expected_assignment[i]);
      }
    }
  }
}

///////////////////////////////////////////////////////////////////////////////////

// Holistic test of behavior for release QBB.
TEST(QuantumSchedulerTest, ReleaseQBB) {

  /* .ll generated from CNOTs_beyond_linear_connectivity.cpp
     as above, but with "release_quantum_state();" at the end of the circuit.
  */
  const char *array_of_args[] = {"--forward-scheduling=greedy"};
  cl::ParseCommandLineOptions(1, array_of_args);

  LLVMContext C;
  std::unique_ptr<Module> M = parseIR(C, *&CodeRelease);

  QuantumModule QM;
  // manually set json filename
  QM.sdk_json_filename =
      std::string(CONFIGURATION_DIR) + "/intel-quantum-sdk_12q_linear.json";
  runInitialPasses(*M, QM);

  QuantumInitPlacement QIP;
  QIP.setPlacementMethod("trivial");
  QIP.run(*M, QM);

  QuantumScheduler QS;
  QS.initialization(QM);
  // QS.readCommandLineOptions() is avoided in favor of setting the
  // routing methods directly.
  QS.setForwardSchedulingMethodAndSerializeOption("greedy");
  QS.setBackwardSchedulingMethod("bubble-sort");
  EXPECT_EQ(QS.getForwardSchedulingMethod(), "greedy");
  EXPECT_EQ(QS.getBackwardSchedulingMethod(), "bubble-sort");
  QS.runImpl(*M, QM);
  EXPECT_EQ(QS.getBackwardSchedulingMethod(), "bubble-sort");
  EXPECT_EQ(QS.getForwardSchedulingMethod(), "greedy");

  std::vector<unsigned> expected_assignment = {0, 1, 2, 3, 4};
  std::vector<unsigned> expected_final_assignment = {0, 1, 3, 4,
                                                     2}; // ProgToPhys
  // The final assignment was found by direct inspection of the circuit after
  // scheduling. In practice, this utest confirm that the scheduler's behavior
  // is unchanged by future updates.
  unsigned expected_num_prog_qubits = 5;
  for (auto &QK : QM) {
    for (auto &QBB : QK) {
      unsigned phys_qubits = QM.qubit_maps[&QBB]->getNumPhysQubits();
      unsigned prog_qubits = QM.qubit_maps[&QBB]->getNumProgQubits();
      EXPECT_EQ(phys_qubits, 12);
      EXPECT_EQ(prog_qubits, expected_num_prog_qubits);
      for (int i = 0; i < prog_qubits; i++) {
        EXPECT_EQ(QM.qubit_maps[&QBB]->getProgToPhys(i),
                  expected_assignment[i]);
        // Confirm that final map of the QBB is not the same as the initial one.
        if (false)
          std::cout << "\tProgram qubit: " << i << "\n"
                    << " --> Initial physical qubit: "
                    << QM.qubit_maps[&QBB]->getProgToPhys(i) << "\n"
                    << " --> Final physical qubit: "
                    << QM.final_qubit_maps[&QBB]->getProgToPhys(i) << "\n";
        EXPECT_EQ(QM.final_qubit_maps[&QBB]->getProgToPhys(i),
                  expected_final_assignment[i]);
      }
    }
  }
}

///////////////////////////////////////////////////////////////////////////////////

// Comparison with by-hand scheduling.
TEST(QuantumSchedulerTest, simpleCnotSchedule) {

  /* .ll generated from random_CNOTs_10q.cpp
          qbit q[10];
          cbit c[1];

          quantum_kernel void circuit() {
            H(q[0]);
            CNOT(q[0], q[1]);
            CNOT(q[1], q[2]);
            H(q[3]);
            CNOT(q[3], q[1]);
            CNOT(q[3], q[6]);
            H(q[5]);
            CNOT(q[5], q[8]);
            CNOT(q[1], q[5]);
            H(q[6]);
            CNOT(q[6], q[2]);
            CNOT(q[6], q[9]);
            H(q[9]);
            CNOT(q[9], q[0]);
            CNOT(q[8], q[9]);
            CNOT(q[4], q[7]);
            CNOT(q[5], q[3]);
            H(q[2]);
            CNOT(q[2], q[3]);
            CNOT(q[1], q[6]);
            H(q[1]);
            CNOT(q[1], q[0]);
            CNOT(q[9], q[7]);
            CNOT(q[4], q[3]);
            CNOT(q[5], q[7]);
            H(q[4]);
            CNOT(q[4], q[7]);
            CNOT(q[1], q[0]);
            H(q[4]);
            CNOT(q[3], q[2]);
            CNOT(q[5], q[4]);
          }

          int main(){
            circuit();
          }
  */

  LLVMContext C;
  std::unique_ptr<Module> M = parseIR(C, *&CodeCnots);

  QuantumModule QM;
  // manually set json filename
  QM.sdk_json_filename =
      std::string(CONFIGURATION_DIR) + "/intel-quantum-sdk_12q_linear.json";
  runInitialPasses(*M, QM);

  QuantumInitPlacement QIP;
  QIP.setPlacementMethod("trivial");
  QIP.run(*M, QM);

  QuantumScheduler QS;
  QS.initialization(QM);
  QS.setForwardSchedulingMethodAndSerializeOption("greedy");
  QS.setBackwardSchedulingMethod("bubble-sort");
  QS.setupForSpecificBackwardSchedulingMethods(QM);
  EXPECT_EQ(QS.getBackwardSchedulingMethod(), "bubble-sort");

  std::vector<unsigned> initial_placement = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  std::vector<unsigned> expected_forward_placement = {0, 1, 3, 4, 7, 6,
                                                      2, 8, 5, 9}; // ProgToPhys
  // The final assignment was found by direct inspection of the circuit after
  // scheduling. In practice, this utest confirm that the scheduler's behavior
  // is unchanged by future updates.
  for (auto &QK : QM) {
    for (auto &QBB : QK) {
      QS.initial_qubit_map_ = *(QM.qubit_maps[&QBB]);
      QS.qubit_map_ = QS.initial_qubit_map_;
      int status = QS.populateTableFromQBB(QBB, QM);
      EXPECT_EQ(status, EXIT_SUCCESS);
      unsigned phys_qubits = QM.qubit_maps[&QBB]->getNumPhysQubits();
      unsigned prog_qubits = QM.qubit_maps[&QBB]->getNumProgQubits();
      EXPECT_EQ(prog_qubits, 10);
      for (int i = 0; i < prog_qubits; i++) {
        EXPECT_EQ(QM.qubit_maps[&QBB]->getProgToPhys(i), initial_placement[i]);
        // Confirm that final map of the QBB is the same as the initial one.
        EXPECT_EQ(QS.qubit_map_.getProgToPhys(i),
                  expected_forward_placement[i]);
      }
      EXPECT_EQ(QS.routing_source_.size(),
                50); // 50 SWAPs for the forward routing
      EXPECT_EQ(QS.routing_target_.size(), 50);
      //
      status = QS.extendTableWithRoutingBackToExpectedFinalQubitMap(
          QBB, QM, QS.getBackwardSchedulingMethod());
      EXPECT_EQ(status, EXIT_SUCCESS);
      for (int i = 0; i < prog_qubits; i++) {
        // Confirm that final map of the QBB is the same as the initial one.
        EXPECT_EQ(QS.qubit_map_.getProgToPhys(i), initial_placement[i]);
        EXPECT_EQ(QM.final_qubit_maps[&QBB]->getProgToPhys(i),
                  initial_placement[i]);
      }
      EXPECT_EQ(QS.routing_source_.size(),
                58); // 50 + 8 SWAPs for the backward routing
      EXPECT_EQ(QS.routing_target_.size(), 58);
    }
  }
}
