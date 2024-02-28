//===- QuantumInitPlacement.h -----------------------------------------===//
//
// Copyright (C) 2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//
//===----------------------------------------------------------------------===//
/// \file "QuantumInitPlacement.h"
///
/// This file defines the analysis pass which finds an initial placement (of
/// program to physical qubits) for a workload and modifies the global qubit map
/// of QuantumModule. Based on GreedyScheduler.hpp and
/// QuantumMapProgToPhysQubits.hpp
//===----------------------------------------------------------------------===//

#ifndef LLVM_QUANTUM_INIT_PLACEMENT_H
#define LLVM_QUANTUM_INIT_PLACEMENT_H

#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/InstIterator.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Intrinsics.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/PassManager.h"
#include "llvm/IRReader/IRReader.h"
#include "llvm/InitializePasses.h"
#include "llvm/Pass.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/JSON.h" // JSON package
#include "llvm/Support/SourceMgr.h"
#include "llvm/Support/raw_ostream.h"

#include "IntelQuantumPasses/QuantumAnalysis/QuantumCompilerLinkage.h"
#include "IntelQuantumPasses/QuantumAnalysis/QuantumGateIdentifiers.h"
#include "IntelQuantumPasses/QuantumAnalysis/QuantumIterator.h"
#include "IntelQuantumPasses/QuantumAnalysis/QuantumRefs.h"
#include "IntelQuantumPasses/QuantumAnalysis/QuantumSpinNativeToJson.h"
#include "IntelQuantumPasses/QuantumUtils/QuantumGeneralUtils.h"
#include "IntelQuantumPasses/QuantumUtils/QuantumMapProgToPhysQubits.hpp"
#include "IntelQuantumPasses/QuantumUtils/QuantumPlacementUtils.h"

#include <directed_graph.hpp>
#include <machine_config.hpp>
#include <platform_configuration.hpp>

#include <cassert>
#include <cstdlib> // for EXIT_SUCCESS and EXIT_FAILURE
#include <fstream>
#include <list>
#include <map>
#include <stdexcept>
#include <string>
#include <unordered_set>

namespace llvm {

////////////////////////////////////////////////////////////////////////////////
namespace aqcc {
class QuantumInitPlacement {
public:
  // // Initial values for program and platform qubits
  // unsigned num_prog_qubits_ = 0;
  // unsigned num_phys_qubits_ = 0;

  // QUBIT-INDEX MAPS:
  scheduler::MapProgToPhysQubits qubit_map_; // Dynamic map
  // scheduler::MapProgToPhysQubits reference_qubit_map_; // Realized at the
  // beginning and end of every quantum kernel.

  static std::string placement_method_;

  // Set by the user if "custom" placement method chosen.
  // std::vector<unsigned> custom_subset_;

  /// Minimal constructor.
  QuantumInitPlacement() {}
  /// Destructor.
  ~QuantumInitPlacement(){};

  /// Actions taken when pass is invoked.
  bool run(Module &M, QuantumModule &QM);

  void setPlacementMethod(std::string method) { placement_method_ = method; }

  ////////////////////////////////////////////////////////////////////////////////

  using CustomMap = std::vector<std::pair<QbitRef, std::vector<unsigned>>>;

  // Checks if the custom placement specified by the user is valid
  void validateCustomSubset(CustomMap &custom_subset_, int num_phys_qubits_);

  /// Populates custom_subset_ with the user-defined qubit mapping ("custom"
  /// placement method) and validates it using validateCustomSubset method.
  /// The function iterates through the QuantumModule globals and captures qbit
  /// datatype initializations. The user can specify their desired placement by
  /// initializing qbit variables. Example: qbit qreg[5] = {0,5,3,1,6};

  /// This initialization will map the qubits as follows (as long as the
  /// platform has the specified physical qubits):
  ///  Program qubit: 0 --> Physical qubit: 0
  ///  Program qubit: 1 --> Physical qubit: 5
  ///  Program qubit: 2 --> Physical qubit: 3
  ///  Program qubit: 3 --> Physical qubit: 1
  ///  Program qubit: 4 --> Physical qubit: 6

  int getCustomSubset(QuantumModule &QM, CustomMap &custom_subset_);

  int setCustomSetToPhyMap(QuantumModule &QM);

  int setTrivialProgToPhysMap(QuantumModule &QM);

  /// Identify all program qubits.
  /// Class 'MapProgToPhysQubits' internally builds a map<QbitRef, unsigned>
  /// that associate a unique id (unsigend) to every QbitRef. When all QbitRef
  /// can be resolved, this map spans all program qubits, even from multiple
  /// variables. The method now iterates through QuantumModule globals' to
  /// identify all program qubits.
  // int identifyAllDistinctProgramQubits();

  /// Initial placement of program-to-physical qubits.
  ///
  /// Methods for qubit placement:
  /// "trivial": Placement in which prog_qubit with id = j is associated with
  ///   the j-th phys_qubit.
  /// "dense": Placement to a subset of the platform's physical qubits with
  ///   higher connectivity. "custom": User-defined placement.
  int placeProgToPhysQubits(QuantumModule &QM);

  int setDenseProgToPhysMap(QuantumModule &QM);
  int setLocalProgToPhysMap(QuantumModule &QM);
};

class QuantumInitPlacementPass
    : public PassInfoMixin<QuantumInitPlacementPass> {
public:
  PreservedAnalyses run(Module &M, ModuleAnalysisManager &MAM);
};

} // namespace aqcc

////////////////////////////////////////////////////////////////////////////////

// helper functions for quicksort implementation
int partition(std::vector<unsigned> &v, int start, int end);

void quicksort(std::vector<unsigned> &v, int start, int end);

} // namespace llvm

#endif // for header guard LLVM_QUANTUM_INIT_PLACEMENT_H
