//===- QuantumAnnotationsToJsonTest.cpp -------------------------*- C++ -*-===//
//
//===----------------------------------------------------------------------===//
//
// Copyright (C) 2022 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//
//===----------------------------------------------------------------------===//
/// \file "QuantumAnnotationsToJsonTest.cpp"
///
/// This file provides a basic unit test to verify the functionality of the
/// QuantumAnnotationToJsonPass which should build a StringRef -> json::Object
/// map representing the quantum annotation data. This data should be
/// retrievable in the desired data type.
///
//===----------------------------------------------------------------------===//

#include "IntelQuantumPasses/QuantumAnalysis/QuantumAnnotationsToJson.h"
#include "IntelQuantumPasses/QuantumAnalysis/QuantumAnnotationGetters.h"
#include "IntelQuantumPasses/QuantumAnalysis/QuantumGateIdentifiers.h"

#include "llvm/AsmParser/Parser.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/InstIterator.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/PassManager.h"
#include "llvm/IRReader/IRReader.h"
#include "llvm/InitializePasses.h"
#include "llvm/Pass.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/JSON.h" //JSON PACKAGE
#include "llvm/Support/SourceMgr.h"
#include "llvm/Support/raw_ostream.h"

#include "gtest/gtest.h"

#include <iostream>
#include <map>
#include <optional>

#include "QuantumPasses/QuantumAnalysis/QuantumAnnotationsToJsonTest.hpp"

using namespace llvm;

static std::unique_ptr<Module> parseIR(LLVMContext &C, const char *IR) {
  SMDiagnostic Err;
  std::unique_ptr<Module> Mod = parseAssemblyString(IR, Err, C);
  if (!Mod)
    Err.print("QuantumAnnotationsToJsonTest", errs());
  return Mod;
}

const double TOL = 0.000000000001;
const unsigned GATE_TYPES = 24;

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

TEST(QuantumAnnotationsToJsonTest, BasicBuild) {
  LLVMContext C;

  std::unique_ptr<Module> M = parseIR(C, *&Code);

  QuantumAnnotationsToJson *P = new QuantumAnnotationsToJson();

  QuantumModule QM;
  bool worked = P->doInitialization(*M, QM);

  std::map<std::string, json::Object> *data_ptr = P->getQGateMetadata(&(*M));

  ASSERT_EQ(data_ptr->size(), GATE_TYPES);

  for (std::map<std::string, json::Object>::iterator it = data_ptr->begin();
       it != data_ptr->end(); ++it) {
    errs() << "Gate Key : " << it->first << "\n";
  };

  errs() << "Check H gate...\n";

  TestGate( // HGate
      "_Z1HRt", {}, "1/sqrt(2)*[1, 1, 1, -1]", {0, 0, 0, 0}, "", "rm", true,
      true, true, false, {0}, {}, {}, {4}, kH);

  errs() << "Check X gate...\n";

  TestGate( // XGate
      "_Z1XRt", {0, 1, 1, 0}, "", {0, 0, 0, 0}, "", "rm", true, true, true,
      false, {0}, {}, {}, {1}, kX);

  errs() << "Check Y gate...\n";

  TestGate( // YGate
      "_Z1YRt", {0, 0, 0, 0}, "", {0, -1, 1, 0}, "", "rm", true, true, true,
      false, {0}, {}, {}, {2}, kY);

  errs() << "Check Z gate...\n";

  TestGate( // ZGate
      "_Z1ZRt", {1, 0, 0, -1}, "", {0, 0, 0, 0}, "", "rm", true, true, true,
      false, {0}, {}, {}, {3}, kZ);

  errs() << "Check S gate...\n";

  TestGate( // SGate
      "_Z1SRt", {1, 0, 0, 0}, "", {0, 0, 0, 1}, "", "rm", false, true, true,
      false, {0}, {}, {}, {3}, kS);

  errs() << "Check Sdag gate...\n";

  TestGate( // SdagGate
      "_Z4SdagRt", {1, 0, 0, 0}, "", {0, 0, 0, -1}, "", "rm", false, true, true,
      false, {0}, {}, {}, {3}, kSDAG);

  errs() << "Check T gate...\n";

  TestGate( // TGate
      "_Z1TRt", {}, "[1, 0, 0, cos(pi/4)]", {}, "[0, 0, 0, sin(pi/4)]", "rm",
      false, true, true, false, {0}, {}, {}, {3}, kT);

  errs() << "Check Tdag gate...\n";

  TestGate( // TdagGate
      "_Z4TdagRt", {}, "[1, 0, 0, cos(pi/4)]", {}, "[0, 0, 0, -sin(pi/4)]",
      "rm", false, true, true, false, {0}, {}, {}, {3}, kTDAG);

  errs() << "Check RX gate...\n";

  TestGate( // RXGate
      "_Z2RXRtd", {}, "[cos(angle/2), 0, 0, cos(angle/2)]", {},
      "[0, -sin(angle/2), sin(angle/2), 0]", "rm", false, true, true, false,
      {0}, {1}, {}, {1}, kRX);

  errs() << "Check RY gate...\n";

  TestGate( // RYGate
      "_Z2RYRtd", {},
      "[cos(angle/2), -sin(angle/2), sin(angle/2), cos(angle/2)]", {0, 0, 0, 0},
      "", "rm", false, true, true, false, {0}, {1}, {}, {2}, kRY);

  errs() << "Check RZ gate...\n";

  TestGate( // RZGate
      "_Z2RZRtd", {}, "[cos(angle/2), 0, 0, cos(angle/2)]", {},
      "[-sin(angle/2), 0, 0, sin(angle/2)]", "rm", false, true, true, false,
      {0}, {1}, {}, {3}, kRZ);

  errs() << "Check CNOT gate...\n";

  TestGate( // CNOTGate
      "_Z4CNOTRtS_", {1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0}, "",
      {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, "", "rm", true, true,
      true, false, {0, 1}, {}, {0}, {3, 1}, kCX);

  errs() << "Check CZ gate...\n";

  TestGate( // CZGate
      "_Z2CZRtS_", {1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, -1}, "",
      {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, "", "rm", true, true,
      true, false, {0, 1}, {}, {0, 1}, {3, 3}, kCZ);

  errs() << "Check SWAP gate...\n";

  TestGate( // SWAPGate
      "_Z4SWAPRtS_", {1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1}, "",
      {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, "", "rm", true, true,
      true, false, {0, 1}, {}, {}, {0, 0}, kSWAP);

  errs() << "Check Toffoli gate...\n";

  TestGate( // ToffoliGate
      "_Z7ToffoliRtS_S_",
      {1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0,
       0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
       0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0},
      "", {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
           0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
           0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
      "", "rm", true, true, true, false, {0, 1, 2}, {}, {0, 1}, {3, 3, 1},
      kTOFF);

  errs() << "Check PrepX gate...\n";

  TestGate( // PrepX
      "_Z5PrepXRt", {},
      "{qt(prob0) : [0.5, 0.5, 0.5, 0.5], qt(prob1) : [0.5, -0.5, 0.5, -0.5]}",
      {}, "{qt(prob0) : [0, 0, 0, 0], qt(prob1) : [0, 0, 0, 0]}", "rm", false,
      false, true, false, {0}, {}, {}, {1}, kPREPX);

  errs() << "Check PrepY gate...\n";

  TestGate( // PrepY
      "_Z5PrepYRt", {},
      "{qt(prob0) : [0.5, 0, 0, 0.5], qt(prob1) : [0.5, 0, 0, -0.5]}", {},
      "{qt(prob0) : [0, 0.5, -0.5, 0], qt(prob1) : [0, 0.5, 0.5, 0]}", "rm",
      false, false, true, false, {0}, {}, {}, {2}, kPREPY);

  errs() << "Check PrepZ gate...\n";

  TestGate( // PrepZ
      "_Z5PrepZRt", {}, "{qt(prob0) : [1, 0, 0, 0], qt(prob1) : [0, 1, 0, 0]}",
      {}, "{qt(prob0) : [0, 0, 0, 0], qt(prob1) : [0, 0, 0, 0]}", "rm", false,
      false, true, false, {0}, {}, {}, {3}, kPREPZ);

  errs() << "Check MeasX gate...\n";

  TestGate( // MeasX
      "_Z5MeasXRtRbb", {},
      "{qt(prob0) : [0.5, 0.5, 0.5, 0.5], qt(prob1) : [0.5, -0.5, -0.5, 0.5]}",
      {}, "{qt(prob0) : [0, 0, 0, 0], qt(prob1) : [0, 0, 0, 0]}", "rm", false,
      false, true, false, {0}, {1, 2}, {}, {1}, kMEASX);

  errs() << "Check MeasY gate...\n";

  TestGate( // MeasY
      "_Z5MeasYRtRbb", {},
      "{qt(prob0) : [0.5, 0, 0, 0.5], qt(prob1) : [0.5, 0, 0, 0.5]}", {},
      "{qt(prob0) : [0, 0.5, -0.5, 0], qt(prob1) : [0, -0.5, 0.5, 0]}", "rm",
      false, false, true, false, {0}, {1, 2}, {}, {2}, kMEASY);

  errs() << "Check MeasZ gate...\n";

  TestGate( // MeasZ
      "_Z5MeasZRtRbb", {},
      "{qt(prob0) : [1, 0, 0, 0], qt(prob1) : [0, 0, 0, 1]}", {},
      "{qt(prob0) : [0, 0, 0, 0], qt(prob1) : [0, 0, 0, 0]}", "rm", false,
      false, true, false, {0}, {1, 2}, {}, {3}, kMEASZ);

  errs() << "Check CPhase gate...\n";

  TestGate( // CPhase
      "_Z6CPhaseRtS_d", {},
      "[1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, cos(angle)]", {},
      "[0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -sin(angle)]", "rm", false,
      true, true, false, {0, 1}, {2}, {0, 1}, {3, 3}, kCPHASE);

  errs() << "Check RXY gate...\n";

  TestGate( // RXY
      "_Z3RXYRtdd", {},
      "[cos(theta/2), -sin(theta/2) sin(phi), sin(theta/2) sin(phi), "
      "cos(theta/2)]",
      {}, "[0, -sin(theta/2)cos(phi), sin(theta/2)cos(phi), 0]", "rm", false,
      true, true, false, {0}, {1, 2}, {}, {0}, kRXY);

  errs() << "Check SwapA gate...\n";

  TestGate( // SwapA
      "_Z5SwapARtS_d", {},
      "[1, 0, 0, 0, 0, (1+cos(angle))/2, (1-cos(angle))/2, 0, 0, "
      "(1-cos(angle))/2, (1+cos(angle))/2, 0, 0, 0, 0, 1]",
      {},
      "[1, 0, 0, 0, 0, sin(angle)/2, -sin(angle)/2, 0, 0, -sin(angle)/2, "
      "sin(angle)/2, 0, 0, 0, 0, 1]",
      "rm", false, true, true, false, {0, 1}, {2}, {}, {0, 0}, kSWAPA);
}

////////////////////////////////////////////////////////////////////////////////////
// function definition
void TestGate(StringRef IRGateName, std::vector<double> expected_real,
              StringRef expected_real_str, std::vector<double> expected_im,
              StringRef expected_im_str, StringRef expected_matrix_order,
              bool expected_is_hermitian, bool expected_is_unitary,
              bool expected_is_mutable, bool expected_is_user_pulse,
              std::vector<int> expected_qubit_list,
              std::vector<int> expected_parametric_list,
              std::vector<int> expected_control_qubit_list,
              std::vector<int> expected_local_basis_list,
              int expected_identifier) {

  std::map<std::string, json::Object> *data_ptr =
      QuantumAnnotationsToJson::qGateMetadata;

  unsigned cnt = 0;

  if (data_ptr == nullptr) {
    errs() << "No pointer to QGateMetadata.\n";
    ASSERT_TRUE(false);
  };

  std::map<std::string, json::Object>::iterator entry =
      data_ptr->find(IRGateName.str());

  if (entry == data_ptr->end()) {
    errs() << "Didn't find " << IRGateName << " in the QGateMetadata\n";
    ASSERT_TRUE(false);
  };

  // check the matrices
  // TODO: change this to matrix Gate Getters once they are implemented

  if (json::Object *O = &(entry->second)) {

    // Check real matrix
    if (json::Array *arr_real = O->getArray("matrix_real")) {

      for (auto it = arr_real->begin(); it != arr_real->end(); ++it) {

        ASSERT_TRUE(cnt < expected_real.size());
        if (std::optional<double> elem = it->getAsNumber()) {
          EXPECT_NEAR(expected_real[cnt], elem.value(), TOL);
        } else {
          errs() << "Element " << cnt << " of " << IRGateName
                 << " matrix_real is not a double: " << elem << "\n";
          EXPECT_TRUE(false);
        }
        cnt++;
      };

    } else if (std::optional<StringRef> arr_real =
                   O->getString("matrix_real")) {
      ASSERT_EQ(arr_real.value(), expected_real_str);
    } else {
      errs() << "Did not find matrix_real for " << IRGateName << "\n";
      EXPECT_TRUE(false);
    }

    cnt = 0;
    // Check Imaginary matrix
    if (json::Array *arr_imag = O->getArray("matrix_imag")) {

      for (auto it = arr_imag->begin(); it != arr_imag->end(); ++it) {

        ASSERT_TRUE(cnt < expected_im.size());
        if (std::optional<double> elem = it->getAsNumber()) {
          EXPECT_NEAR(expected_im[cnt], elem.value(), TOL);
        } else {
          errs() << "Element " << cnt << " of " << IRGateName
                 << " matrix_imag is not a double: " << elem << "\n";
          EXPECT_TRUE(false);
        }
        cnt++;
      };

    } else if (std::optional<StringRef> arr_imag =
                   O->getString("matrix_imag")) {
      ASSERT_EQ(arr_imag.value(), expected_im_str);
    } else {
      errs() << "Did not find matrix_imag for " << IRGateName << "\n";
      EXPECT_TRUE(false);
    }

    // Check matrix order
    if (std::optional<StringRef> mat_order = O->getString("matrix_order")) {

      EXPECT_EQ(mat_order.value(), expected_matrix_order);

    } else {
      errs() << "Did not find matrix_order for " << IRGateName << "\n";
      EXPECT_TRUE(false);
    }

  } // end get Object
  // from here on we will use Gate Getters

  // check boolean values

  bool test_bool;

  test_bool = isGateHermitian(entry);

  if (test_bool != expected_is_hermitian) {
    errs() << IRGateName << " is_hermitian = " << test_bool << ": expected "
           << expected_is_hermitian << "\n";
    EXPECT_TRUE(false);
  }

  test_bool = isGateUnitary(entry);

  if (test_bool != expected_is_unitary) {
    errs() << IRGateName << " is_unitary = " << test_bool << ": expected "
           << expected_is_unitary << "\n";
    EXPECT_TRUE(false);
  }

  test_bool = isGateMutable(entry);

  if (test_bool != expected_is_mutable) {
    errs() << IRGateName << " is_mutable = " << test_bool << ": expected "
           << expected_is_mutable << "\n";
    EXPECT_TRUE(false);
  }

  test_bool = isGateUserPulse(entry);

  if (test_bool != expected_is_user_pulse) {
    errs() << IRGateName << " is_user_pulse = " << test_bool << ": expected "
           << expected_is_user_pulse << "\n";
    EXPECT_TRUE(false);
  }

  // Check lists

  std::vector<int> test_list, expected_list;

  EXPECT_TRUE(getGateList(entry, "qubit_list", test_list));
  expected_list = expected_qubit_list;

  if (test_list != expected_list) {

    errs() << IRGateName << " qubit_list = {";

    for (int i = 0; i < test_list.size(); i++) {
      errs() << test_list[i] << ", ";
    };
    errs() << "} ";

    errs() << ": expected {";

    for (int i = 0; i < expected_list.size(); i++) {
      errs() << expected_list[i] << ", ";
    };
    errs() << "}\n";

    EXPECT_TRUE(false);
  }

  test_list.clear();
  expected_list.clear();

  EXPECT_TRUE(getGateList(entry, "parametric_list", test_list));
  expected_list = expected_parametric_list;

  if (test_list != expected_list) {

    errs() << IRGateName << " parametric_list = {";

    for (int i = 0; i < test_list.size(); i++) {
      errs() << test_list[i] << ", ";
    };
    errs() << "} ";

    errs() << ": expected {";

    for (int i = 0; i < expected_list.size(); i++) {
      errs() << expected_list[i] << ", ";
    };
    errs() << "}\n";

    EXPECT_TRUE(false);
  }

  test_list.clear();
  expected_list.clear();

  EXPECT_TRUE(getGateList(entry, "control_qubit_list", test_list));
  expected_list = expected_control_qubit_list;

  if (test_list != expected_list) {

    errs() << IRGateName << " control_qubit_list = {";

    for (int i = 0; i < test_list.size(); i++) {
      errs() << test_list[i] << ", ";
    };
    errs() << "} ";

    errs() << ": expected {";

    for (int i = 0; i < expected_list.size(); i++) {
      errs() << expected_list[i] << ", ";
    };
    errs() << "}\n";

    EXPECT_TRUE(false);
  }

  test_list.clear();
  expected_list.clear();

  EXPECT_TRUE(getGateList(entry, "local_basis_list", test_list));
  expected_list = expected_local_basis_list;

  if (test_list != expected_list) {

    errs() << IRGateName << " local_basis_list = {";

    for (int i = 0; i < test_list.size(); i++) {
      errs() << test_list[i] << ", ";
    };
    errs() << "} ";

    errs() << ": expected {";

    for (int i = 0; i < expected_list.size(); i++) {
      errs() << expected_list[i] << ", ";
    };
    errs() << "}\n";

    EXPECT_TRUE(false);
  }

  // Check identifier

  int test_int;

  test_int = getGateIdentifier(entry);

  if (test_int != expected_identifier) {
    errs() << IRGateName << " identifier = " << test_int << ": expected "
           << expected_identifier << "\n";
    EXPECT_TRUE(false);
  }
}
