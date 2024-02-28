//////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//
//////////////////////////////////////////////////////////////////////

#ifndef QUANTUM_GATE_IDENTIFIERS_TEST_HPP
#define QUANTUM_GATE_IDENTIFIERS_TEST_HPP

#include "IntelQuantumPasses/QuantumAnalysis/QuantumGateIdentifiers.h"

//////////////////////////////////////////////////////////////////////
// Test fixture class: QuantumGateIdentifiersTest
//////////////////////////////////////////////////////////////////////

class QuantumGateIdentifiersTest : public ::testing::Test {

protected:
  QuantumGateIdentifiersTest() {}

  // just after the 'constructor'
  void SetUp() override {
    // code for setup here;
  }

  // just before the 'destructor'
  //  void TearDown() override {}

  int status_;
};

//////////////////////////////////////////////////////////////////////
// Test macros:

TEST_F(QuantumGateIdentifiersTest, Mock) {
  std::cout << "Add relevant unit tests\n";
  status_ = EXIT_SUCCESS;
  EXPECT_EQ(status_, EXIT_SUCCESS);
}

//////////////////////////////////////////////////////////////////////

TEST_F(QuantumGateIdentifiersTest, GateIdentifier) {
  // Check the identifiers of a few gates.
  EXPECT_EQ(llvm::GateIdentifier::kIDLE, 0);
  EXPECT_TRUE(llvm::GateIdentifier::kPREPZ > 0);
  EXPECT_TRUE(llvm::GateIdentifier::kMEASZ > 0);

  // Check the identifiers of inverse gates.
  EXPECT_EQ(llvm::GateIdentifier::kH,
            llvm::getInverseSQGateID(llvm::GateIdentifier::kH));
  EXPECT_EQ(llvm::GateIdentifier::kZ,
            llvm::getInverseSQGateID(llvm::GateIdentifier::kZ));
  EXPECT_EQ(llvm::GateIdentifier::kS,
            llvm::getInverseSQGateID(llvm::GateIdentifier::kZS));
}

//////////////////////////////////////////////////////////////////////

#endif // header guards QUANTUM_GATE_IDENTIFIERS_TEST_HPP
