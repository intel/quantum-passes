//===- QuantumLocalSearchTest.cpp -------------------------------*- C++ -*-===//
//
//
// Copyright (C) 2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//
//===----------------------------------------------------------------------===//
/// \file "QuantumIteratorTest.cpp"
///
/// This file provides a basic unit test of the functionality of the Local
/// Search method.
///
//===----------------------------------------------------------------------===//

//////////////////////////////////////////////////////////////////////
// Test fixture class: LocalSearchTest
//////////////////////////////////////////////////////////////////////
/*
class LocalSearchTest : public ::testing::Test
{

protected:

    QuantumGateIdentifiersTest()
    { }

    // just after the 'constructor'
    void SetUp() override
    {
        //code for setup here;
    }

  // just before the 'destructor'
  //  void TearDown() override {}

    int status_;
};
*/
//////////////////////////////////////////////////////////////////////
// Test macros:

#include "IntelQuantumPasses/QuantumAnalysis/QuantumInitPlacement.h"

#include "IntelQuantumPasses/QuantumAnalysis/QuantumCompilerLinkage.h"
#include "IntelQuantumPasses/QuantumAnalysis/QuantumGateIdentifiers.h"
#include "IntelQuantumPasses/QuantumAnalysis/QuantumIterator.h"
#include "IntelQuantumPasses/QuantumAnalysis/QuantumRefs.h"
#include "llvm/Analysis/CGSCCPassManager.h"
#include "llvm/Analysis/LoopAnalysisManager.h"
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
#include "llvm/IRReader/IRReader.h"
#include "llvm/InitializePasses.h"
#include "llvm/Pass.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/SourceMgr.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Transforms/Scalar.h"
#include "llvm/Transforms/Utils/Local.h"

#include "gtest/gtest-spi.h"
#include "gtest/gtest.h"

#include <map>

#include "QuantumPasses/QuantumAnalysis/QuantumLocalSearchTest.hpp"

class QuantumInitPlacementPassCostMock
    : public PassInfoMixin<QuantumInitPlacementPassCostMock> {
public:
  PreservedAnalyses run(Module &M, ModuleAnalysisManager &MAM);
};

// Mocks out Pass for testing Differential Cost
PreservedAnalyses
QuantumInitPlacementPassCostMock::run(Module &M, ModuleAnalysisManager &MAM) {
  QuantumModuleProxy &QMP = MAM.getResult<QuantumCompilerLinkageAnalysis>(M);
  QuantumInitPlacement QIP;
  aqcc::initializeGlobalQubitMap(*(QMP.QM));
  aqcc::initializeQBBQubitMap(*(QMP.QM));
  std::vector<unsigned> dense_placement;
  std::set<unsigned> dense_placement_set;
  std::vector<unsigned> local_subset;
  unsigned total_global_qbits = 25;
  unsigned total_prog_qbits = 25;

  findBestDenseSet(*(QMP.QM), total_prog_qbits, dense_placement_set);
  std::transform(dense_placement_set.begin(), dense_placement_set.end(),
                 std::back_inserter(dense_placement),
                 [](auto &val) { return val; });
  // Should have found 25 phys qubits to use
  EXPECT_EQ(dense_placement.size(), 25);
  ApplicationGraph application_graph;
  LocalSearch local_search;
  local_search.setNumIterations(10000);
  local_search.setNumResets(1);
  std::unordered_map<unsigned, unsigned> fixed_set;
  std::unordered_map<unsigned, double> heuristic_differential;
  application_graph.constructApplicationGraph(*(QMP.QM));
  local_search.precomputeSwapHeuristic(dense_placement, fixed_set, *(QMP.QM));

  std::unordered_map<unsigned, unsigned> current_placement = fixed_set;
  for (size_t i = 0; i < 25; ++i) {
    current_placement[i] = dense_placement[i];
  }
  local_search.initializePhysToProgramMap(current_placement);
  unsigned num_possible_swaps = local_search.connected_pairs.size();
  // Should have 40 edges
  EXPECT_EQ(num_possible_swaps, 40);
  std::vector<unsigned> swaps_to_check(num_possible_swaps);
  std::iota(swaps_to_check.begin(), swaps_to_check.end(), 0);
  for (auto &swap_index : swaps_to_check) {
    std::pair<unsigned, unsigned> swap =
        local_search.connected_pairs[swap_index];
    double cost_before_swap =
        local_search.valueHeuristic(current_placement, application_graph);
    local_search.swapQubits(current_placement, swap.first, swap.second);
    double cost_after_swap =
        local_search.valueHeuristic(current_placement, application_graph);
    // Undo swap
    local_search.swapQubits(current_placement, swap.first, swap.second);
    double cost_differential = local_search.valueHeuristicDifferential(
        current_placement, swap.first, swap.second, application_graph);
    heuristic_differential[swap_index] = cost_differential;
    // Testing that the cost differential gives the same as the difference of
    // the cost before and after swapping. This tests that computing the
    // differential is correct
    EXPECT_EQ(cost_differential, cost_after_swap - cost_before_swap);
  }

  // Pick random swap index
  std::pair<unsigned, unsigned> swap = local_search.connected_pairs[10];
  local_search.updateSwapChecks(current_placement, swaps_to_check, swap.first,
                                swap.second, heuristic_differential, *(QMP.QM),
                                application_graph);
  // This number should be somewhat "small"
  EXPECT_LT(swaps_to_check.size(), 30);
  for (unsigned swap_index = 0; swap_index < num_possible_swaps; ++swap_index) {
    std::pair<unsigned, unsigned> considered_swap =
        local_search.connected_pairs[swap_index];
    // We know how to calculate this one, so we can skip
    if (considered_swap.first == swap.first &&
        considered_swap.second == swap.second)
      continue;
    if (std::find(swaps_to_check.begin(), swaps_to_check.end(), swap_index) ==
        swaps_to_check.end()) {
      double cost_before_swap = local_search.valueHeuristicDifferential(
          current_placement, considered_swap.first, considered_swap.second,
          application_graph);
      local_search.swapQubits(current_placement, swap.first, swap.second);
      double cost_after_swap = local_search.valueHeuristicDifferential(
          current_placement, considered_swap.first, considered_swap.second,
          application_graph);
      local_search.swapQubits(current_placement, swap.first, swap.second);
      // Important check here -- if the swap to be considered is not in
      // swaps_to_check, then the value heuristic differential must be the same
      // as it was before
      EXPECT_EQ(cost_before_swap, cost_after_swap);
    }
  }

  return PreservedAnalyses::all();
}

static std::unique_ptr<Module> parseIR(LLVMContext &C, const char *IR) {
  SMDiagnostic Err;
  std::unique_ptr<Module> Mod = llvm::parseAssemblyString(IR, Err, C);
  if (!Mod)
    Err.print("QuantumInitPlacementTest", errs());
  return Mod;
}

TEST(LocalSearchTest, local_3x3_1) {

  LLVMContext C;
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
  MPM.addPass(QuantumInitPlacementPass());

  std::unique_ptr<Module> M = parseIR(C, *&local_3x3_grid_1);

  QuantumModuleProxy &QMP = MAM.getResult<QuantumCompilerLinkageAnalysis>(*M);

  // manually set json filename
  QMP.QM->sdk_json_filename =
      std::string(CONFIGURATION_DIR) + "/square_9q_sdk.json";
  QMP.QM->platform_filename =
      std::string(CONFIGURATION_DIR) + "/square_9q.json";
  QMP.QM->platform = std::make_unique<TPL::PlatformConfiguration>();
  QMP.QM->platform->loadPlatformConfig(QMP.QM->platform_filename, false);
  QMP.QM->has_platform = QMP.QM->platform->platformConfigLoaded();
  QMP.QM->machine.ClearAndResize();
  QMP.QM->machine.CreateConnectivityGraphWithQubitProperties(*QMP.QM->platform);
  QMP.QM->machine.DetermineAvailableGatesAndDurations(*QMP.QM->platform);
  QMP.QM->machine.ComputeSwapDistanceMatrix();
  QMP.QM->machine.ComputeDistanceMatrix();

  QuantumInitPlacement::placement_method_ = "local";

  MPM.run(*M, MAM);
  for (auto &QK : *(QMP.QM)) {
    for (auto &QBB : QK) {
      std::vector<std::pair<unsigned, unsigned>> expected_connected_pairs;
      expected_connected_pairs.push_back({0, 2});
      expected_connected_pairs.push_back({2, 4});
      expected_connected_pairs.push_back({0, 1});
      expected_connected_pairs.push_back({1, 3});
      expected_connected_pairs.push_back({4, 5});
      expected_connected_pairs.push_back({5, 6});

      for (const auto &pair : expected_connected_pairs) {
        EXPECT_EQ(QMP.QM->getDistanceBetweenQubits(
                      QMP.QM->qubit_maps[&QBB]->getProgToPhys(pair.first),
                      QMP.QM->qubit_maps[&QBB]->getProgToPhys(pair.second)),
                  1);
      }
    }
  }
}

TEST(LocalSearchTest, local_search_test) {
  LLVMContext C;
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
  MPM.addPass(QuantumInitPlacementPass());

  std::unique_ptr<Module> M = parseIR(C, *&local_3x3_grid_1);

  QuantumModuleProxy &QMP = MAM.getResult<QuantumCompilerLinkageAnalysis>(*M);
  // manually set json filename
  QMP.QM->sdk_json_filename =
      std::string(CONFIGURATION_DIR) + "/square_9q_sdk.json";
  QMP.QM->platform_filename =
      std::string(CONFIGURATION_DIR) + "/square_9q.json";
  QMP.QM->platform = std::make_unique<TPL::PlatformConfiguration>();
  QMP.QM->platform->loadPlatformConfig(QMP.QM->platform_filename, false);
  QMP.QM->has_platform = QMP.QM->platform->platformConfigLoaded();
  QMP.QM->machine.ClearAndResize();
  QMP.QM->machine.CreateConnectivityGraphWithQubitProperties(*QMP.QM->platform);
  QMP.QM->machine.DetermineAvailableGatesAndDurations(*QMP.QM->platform);
  QMP.QM->machine.ComputeSwapDistanceMatrix();
  QMP.QM->machine.ComputeDistanceMatrix();

  LocalSearch local_search;
  local_search.setNumIterations(10000);
  local_search.setNumResets(1);
  ApplicationGraph application_graph;
  /* Application graph:

  x - 3 - x
  |   |   |
  0 - 1 - 6
  |   |   |
  2 - 4 - 5

  */
  application_graph.addEdgeToApplicationGraph(2, 4);
  application_graph.addEdgeToApplicationGraph(4, 5);
  application_graph.addEdgeToApplicationGraph(4, 1);
  application_graph.addEdgeToApplicationGraph(1, 6);
  application_graph.addEdgeToApplicationGraph(0, 1);
  application_graph.addEdgeToApplicationGraph(2, 0);
  application_graph.addEdgeToApplicationGraph(5, 6);
  application_graph.addEdgeToApplicationGraph(1, 3);

  local_search.setNumIterations(1000);
  // Fix corners
  std::unordered_map<unsigned, unsigned> fixed_qubits = {{2, 6}, {5, 8}};
  std::vector<unsigned> program_qubits = {0, 1, 3, 4, 6};
  std::vector<unsigned> phys_qubits = {0, 1, 2, 3, 4, 5, 7};
  std::vector<unsigned> starting_qubits;
  auto placement =
      local_search.localSearch(program_qubits, phys_qubits, fixed_qubits,
                               starting_qubits, application_graph, *(QMP.QM));
  // Check to see that expected placement has occurred
  EXPECT_EQ(placement.size(), 7);
  EXPECT_EQ(placement[3], 1);
  EXPECT_EQ(placement[0], 3);
  EXPECT_EQ(placement[1], 4);
  EXPECT_EQ(placement[6], 5);
  EXPECT_EQ(placement[2], 6);
  EXPECT_EQ(placement[4], 7);
  EXPECT_EQ(placement[5], 8);

  // Flip fixed qubits
  fixed_qubits = {{2, 8}, {5, 6}};
  local_search.localSearchReset();
  placement =
      local_search.localSearch(program_qubits, phys_qubits, fixed_qubits,
                               starting_qubits, application_graph, *(QMP.QM));
  // Check to see that expected placement has occurred
  EXPECT_EQ(placement.size(), 7);
  EXPECT_EQ(placement[3], 1);
  EXPECT_EQ(placement[6], 3);
  EXPECT_EQ(placement[1], 4);
  EXPECT_EQ(placement[0], 5);
  EXPECT_EQ(placement[5], 6);
  EXPECT_EQ(placement[4], 7);
  EXPECT_EQ(placement[2], 8);
}

TEST(LocalSearchTest, local_3x3_2) {

  LLVMContext C;
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
  MPM.addPass(QuantumInitPlacementPass());

  std::unique_ptr<Module> M = parseIR(C, *&local_3x3_grid_2);

  QuantumModuleProxy &QMP = MAM.getResult<QuantumCompilerLinkageAnalysis>(*M);

  // manually set json filename
  QMP.QM->sdk_json_filename =
      std::string(CONFIGURATION_DIR) + "/square_9q_sdk.json";
  QMP.QM->platform_filename =
      std::string(CONFIGURATION_DIR) + "/square_9q.json";
  QMP.QM->platform = std::make_unique<TPL::PlatformConfiguration>();
  QMP.QM->platform->loadPlatformConfig(QMP.QM->platform_filename, false);
  QMP.QM->has_platform = QMP.QM->platform->platformConfigLoaded();
  QMP.QM->machine.ClearAndResize();
  QMP.QM->machine.CreateConnectivityGraphWithQubitProperties(*QMP.QM->platform);
  QMP.QM->machine.DetermineAvailableGatesAndDurations(*QMP.QM->platform);
  QMP.QM->machine.ComputeSwapDistanceMatrix();
  QMP.QM->machine.ComputeDistanceMatrix();

  QuantumInitPlacement::placement_method_ = "local";

  MPM.run(*M, MAM);
  for (auto &QK : *(QMP.QM)) {
    for (auto &QBB : QK) {
      std::vector<std::pair<unsigned, unsigned>> expected_connected_pairs;
      expected_connected_pairs.push_back({0, 1});
      expected_connected_pairs.push_back({1, 2});
      expected_connected_pairs.push_back({2, 6});
      expected_connected_pairs.push_back({2, 3});
      expected_connected_pairs.push_back({4, 1});
      expected_connected_pairs.push_back({5, 4});

      double distance = 0;

      for (const auto &pair : expected_connected_pairs) {
        distance += QMP.QM->getDistanceBetweenQubits(
            QMP.QM->qubit_maps[&QBB]->getProgToPhys(pair.first),
            QMP.QM->qubit_maps[&QBB]->getProgToPhys(pair.second));
      }

      // Depending on which physical qubits are used, may have a single pair
      // that is not connected
      EXPECT_LE(distance, 7);
    }
  }
}

TEST(LocalSearchTest, square_grid) {
  LLVMContext C;
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
  MPM.addPass(QuantumInitPlacementPass());

  std::unique_ptr<Module> M = parseIR(C, *&tfd_example);

  QuantumModuleProxy &QMP = MAM.getResult<QuantumCompilerLinkageAnalysis>(*M);

  // manually set json filename
  QMP.QM->sdk_json_filename =
      std::string(CONFIGURATION_DIR) + "/square_9q_sdk.json";
  QMP.QM->platform_filename =
      std::string(CONFIGURATION_DIR) + "/square_9q.json";
  QMP.QM->platform = std::make_unique<TPL::PlatformConfiguration>();
  QMP.QM->platform->loadPlatformConfig(QMP.QM->platform_filename, false);
  QMP.QM->has_platform = QMP.QM->platform->platformConfigLoaded();
  QMP.QM->machine.ClearAndResize();
  QMP.QM->machine.CreateConnectivityGraphWithQubitProperties(*QMP.QM->platform);
  QMP.QM->machine.DetermineAvailableGatesAndDurations(*QMP.QM->platform);
  QMP.QM->machine.ComputeSwapDistanceMatrix();
  QMP.QM->machine.ComputeDistanceMatrix();

  QuantumInitPlacement::placement_method_ = "local";

  MPM.run(*M, MAM);
  for (auto &QK : *(QMP.QM)) {
    for (auto &QBB : QK) {
      std::vector<std::pair<unsigned, unsigned>> expected_connected_pairs;
      expected_connected_pairs.push_back({0, 1});
      expected_connected_pairs.push_back({1, 3});
      expected_connected_pairs.push_back({0, 2});
      expected_connected_pairs.push_back({2, 3});

      for (const auto &pair : expected_connected_pairs) {
        EXPECT_EQ(QMP.QM->getDistanceBetweenQubits(
                      QMP.QM->qubit_maps[&QBB]->getProgToPhys(pair.first),
                      QMP.QM->qubit_maps[&QBB]->getProgToPhys(pair.second)),
                  1);
      }
    }
  }
}

TEST(LocalSearchTest, local_5x5_grid) {

  LLVMContext C;
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
  MPM.addPass(QuantumInitPlacementPass());

  std::unique_ptr<Module> M = parseIR(C, *&local_5x5_grid);

  QuantumModuleProxy &QMP = MAM.getResult<QuantumCompilerLinkageAnalysis>(*M);

  // manually set json filename
  QMP.QM->sdk_json_filename =
      std::string(CONFIGURATION_DIR) + "/square_25q_sdk.json";
  QMP.QM->platform_filename =
      std::string(CONFIGURATION_DIR) + "/square_25q.json";
  QMP.QM->platform = std::make_unique<TPL::PlatformConfiguration>();
  QMP.QM->platform->loadPlatformConfig(QMP.QM->platform_filename, false);
  QMP.QM->has_platform = QMP.QM->platform->platformConfigLoaded();
  QMP.QM->machine.ClearAndResize();
  QMP.QM->machine.CreateConnectivityGraphWithQubitProperties(*QMP.QM->platform);
  QMP.QM->machine.DetermineAvailableGatesAndDurations(*QMP.QM->platform);
  QMP.QM->machine.ComputeSwapDistanceMatrix();
  QMP.QM->machine.ComputeDistanceMatrix();

  QuantumInitPlacement::placement_method_ = "local";

  MPM.run(*M, MAM);
  for (auto &QK : *(QMP.QM)) {
    for (auto &QBB : QK) {
      std::vector<std::pair<unsigned, unsigned>> expected_connected_pairs;
      expected_connected_pairs.push_back({0, 1});
      expected_connected_pairs.push_back({0, 2});
      expected_connected_pairs.push_back({1, 16});
      expected_connected_pairs.push_back({1, 19});
      expected_connected_pairs.push_back({16, 17});
      expected_connected_pairs.push_back({16, 15});

      expected_connected_pairs.push_back({17, 18});
      expected_connected_pairs.push_back({17, 20});
      expected_connected_pairs.push_back({18, 21});
      expected_connected_pairs.push_back({2, 19});
      expected_connected_pairs.push_back({2, 12});
      expected_connected_pairs.push_back({19, 15});

      expected_connected_pairs.push_back({19, 13});
      expected_connected_pairs.push_back({15, 20});
      expected_connected_pairs.push_back({15, 14});
      expected_connected_pairs.push_back({20, 21});
      expected_connected_pairs.push_back({20, 22});
      expected_connected_pairs.push_back({21, 6});

      expected_connected_pairs.push_back({12, 13});
      expected_connected_pairs.push_back({12, 7});
      expected_connected_pairs.push_back({13, 14});
      expected_connected_pairs.push_back({13, 24});
      expected_connected_pairs.push_back({14, 22});
      expected_connected_pairs.push_back({14, 8});

      expected_connected_pairs.push_back({22, 6});
      expected_connected_pairs.push_back({22, 23});
      expected_connected_pairs.push_back({6, 5});
      expected_connected_pairs.push_back({7, 24});
      expected_connected_pairs.push_back({7, 11});
      expected_connected_pairs.push_back({24, 8});

      expected_connected_pairs.push_back({24, 10});
      expected_connected_pairs.push_back({8, 23});
      expected_connected_pairs.push_back({8, 9});
      expected_connected_pairs.push_back({23, 5});
      expected_connected_pairs.push_back({23, 4});
      expected_connected_pairs.push_back({5, 3});

      expected_connected_pairs.push_back({11, 10});
      expected_connected_pairs.push_back({10, 9});
      expected_connected_pairs.push_back({9, 4});
      expected_connected_pairs.push_back({4, 3});

      for (const auto &pair : expected_connected_pairs) {
        EXPECT_EQ(QMP.QM->getDistanceBetweenQubits(
                      QMP.QM->qubit_maps[&QBB]->getProgToPhys(pair.first),
                      QMP.QM->qubit_maps[&QBB]->getProgToPhys(pair.second)),
                  1);
      }
    }
  }
}

TEST(LocalSearchTest, differential_cost) {
  LLVMContext C;
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
  MPM.addPass(QuantumInitPlacementPassCostMock());

  std::unique_ptr<Module> M = parseIR(C, *&local_5x5_grid);

  QuantumModuleProxy &QMP = MAM.getResult<QuantumCompilerLinkageAnalysis>(*M);

  // manually set json filename
  QMP.QM->sdk_json_filename =
      std::string(CONFIGURATION_DIR) + "/square_25q_sdk.json";
  QMP.QM->initializeData(*M);

  QuantumInitPlacement::placement_method_ = "local";
  MPM.run(*M, MAM);
}