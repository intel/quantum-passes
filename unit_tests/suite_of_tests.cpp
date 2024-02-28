//////////////////////////////////////////////////////////////////////////////
/// UNIT TESTS: using the 'googletest' framework.
///
/// 'googletest' framework can be downloaded from:
/// https://github.com/google/googletest
///
/// The test suite is divided into fixtures.
/// Each fixture is a collection of tests that uses a similar setup.
/// For each test, the setup is recreated separately so a fixture aggregate
/// them for convenience and not as dependent tests.
//////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//
//////////////////////////////////////////////////////////////////////////////

#include <cstdlib> // EXIT_SUCCESS , EXIT_FAILURE
#include <string>
#include <utility>
#include <vector>

// googletest
#include "gtest/gtest.h"

// test the intermediate representations:
#include "include/QuantumPasses/QuantumAnalysis/QuantumGateIdentifiers_test.hpp"

///////////////////////////////////////////////////////////////////////////////
// main

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

//////////////////////////////////////////////////////////////////////////////
