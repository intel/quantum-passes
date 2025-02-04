//===- Scheduler.hpp - Parallel schedule of quantum gates -----------------===//
//
// Copyright (C) 2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//
//===----------------------------------------------------------------------===//
/// \file "Scheduler.h"
//
// This file declares a transformation pass which schedules the program's
// gates with respect to the connectivity of the target platform. The
// information on connectivity, set of available gates and their duration, is
// provided via the platform-json file.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_QUANTUM_SCHEDULER_H
#define LLVM_QUANTUM_SCHEDULER_H

#include "IntelQuantumPasses/QuantumAnalysis/QuantumModule.h"
#include "IntelQuantumPasses/QuantumUtils/QuantumGeneralUtils.h"
#include "IntelQuantumPasses/QuantumUtils/QuantumMapProgToPhysQubits.hpp"

#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/PassManager.h"
#include "llvm/InitializePasses.h"
#include "llvm/Pass.h"

using namespace llvm;
using namespace aqcc;

//////////////////////////////////////////////////////////////////////

///
/// Greedy scheduler using shortest path routing
///
/// Features:
///
/// - this is a Transform pass inside LLVM and can therefore be
///   concatenated with other schedulers.
///
/// - the output is expressed in LLVM IR and this facilitates
///   targeting multiple backends via translator-to-backend passes.
///
/// - the scheduler uses the gate duration to exploit parallelization
///   and reduce resource contention. However the output is in
///   LLVM IR and represents a sequential list of instructions.
///
/// - the sequential circuit can be scheduled at runtime with an
///   as-early-as-possible policy. The goal is being able to recover
///   most of the parallelized schedule.
///
/// - measurements are treated like other quantum operations and
///   can happen mid-circuit in a QBB. There are two type of
///   measurement return methods (matched and queued) depending
///   on how the measurement outcomes are stored. For matched
///   measurements, a single outcome is associated ("matched") to
///   a specific physical qubit. For queued measurement, outcomes
///   are returned according to the order of the measurement
///   instructions (their "queue") irrespective of the physical
///   qubit involved (which might be the same).
///
/// - when matched measurements are used, there is an extra
///   contraint: only one matched measurement can be scheduled
///   per physical qubit in a QBB.
///
/// - the initial placement (of program to physical qubits) is provided
///   by the Quantum Module separately for every QBB. The placement
///   expected at the end of the QBB is also provided by the QM.
///   Usually the final placement correspond to the initial one, thus
///   enabling the 'retrace' method for the backward routing.
///
/// - for 'release' QBB, one can avoid the backward routing and the
///   scheduler updates the final placement in the Quantum Module
///   to reflect the actual one at the end of the QBB.
///
/// - to access the details of the quantum architecture, we use class
///   hardware::Machine. In such object, the qubit connectivity is
///   provided as a direct graph with vertex and edge properties
///   (representing the available 1- and 2-qubit gates respectively
///   and their duration).
///

/// This class is intended to contain the functionality of the scheduler pass
/// so that both the new (SchedulerPass) and old (SchedulerLegacyPass) pass
/// managers can be supported.
class QuantumScheduler {
private:
  std::string forward_scheduling_method_ = "none";
  std::string backward_scheduling_method_ = "retrace";
  std::set<std::string> NativeGateNames;
  bool serialize_ = false;
  /// Flag marking the fact that we are handling
  /// canonical gates, rather than the spin native
  /// gates.
  bool CanonicalSchedule = false;

public:
  std::vector<unsigned> hamil_path_for_sort_;

  // Efficient representation of the quantum hardware.
  hardware::Machine *machine_;

  unsigned num_prog_qubits_ = 0;
  unsigned num_phys_qubits_ = 0;

  // QUBIT-INDEX MAPS:
  scheduler::MapProgToPhysQubits qubit_map_; // Dynamical map
  scheduler::MapProgToPhysQubits
      initial_qubit_map_; // Realized at the beginning
                          // of every quantum kernel.
  // We express the table as a vec-of-vec structure:
  // - the outer index represents the physical qubit
  // - the inner index represents a time slot (integer multiple of a time unit)
  // Assumptions:
  // - physical qubits can be uniquely identified by their index
  // - if there is a qubit with index k, then all qubits 0, 1,..., k-1 exists
  //   too
  std::vector<std::vector<std::pair<int, unsigned long>>> table_;
  const int is_idle_ = INT_MAX;
  const int is_still_busy_ = INT_MAX - 1;
  int num_gates_ = 0;

  // List of routing operations.
  // Indicated by a negative gate index "-j", they represent swaps between
  // qubits routing_source_[j-1]  and routinig_target_[j-1].
  std::vector<unsigned> routing_source_, routing_target_;

  //////////////////////////////////////////////////////////////////////

  // Initialize a few variables from the QuantumModule.
  void initialization(QuantumModule &QM);

  /// Store the command-line-options for ease of access in unit tests.
  void readCommandLineOptions(QuantumModule &QM);

  /// Shared implementation for use by old and new pass managers
  void runImpl(Module &M, QuantumModule &QM);

  //////////////////////////////////////////////////////////////////////
private:
  void displayWarning(std::string ErrMsg) {
    errs() << "WARNING: Quantum SDK - Scheduler says: " << ErrMsg << "\n";
  }

  //////////////////////////////////////////////////////////////////////
public:
  /// Confirm the existence of global, initial, and final qubit maps.
  void confirmExistenceOfQubitMapsInQuantumModule(QuantumModule &QM);

  /// Set the specific forward scheduling method.
  ///
  /// Also look for keyword 'serial' in the method's name.
  void setForwardSchedulingMethodAndSerializeOption(std::string method);

  /// Get the forward scheduling method.
  std::string getForwardSchedulingMethod() {
    return forward_scheduling_method_;
  }

  /// Setup for specific backward scheduling methods.
  ///
  /// - "bubble-sort" and "oddeven-sort" rely on finding
  //    an Hamiltonian path of the (physical) qubit connectivity graph.
  //    If the heuristic fails to find such path, fall back to the
  //    default method "retrace".
  /// - "grid" relies on a 2D grid connectivity. We search for the most
  //    convenient 2D grid size. If the connectivity is not a grid,
  //    fall back to the default method "retrace".
  void setupForSpecificBackwardSchedulingMethods(QuantumModule &QM);
  void setBackwardSchedulingMethod(std::string method);

  void setCanonicalScheduling(bool SetVal);

  /// Get the backward scheduling method.
  std::string getBackwardSchedulingMethod() {
    return backward_scheduling_method_;
  }

  /// Read quantum gates and populate table.
  ///
  /// When called, it assumes that qubit_map_ reflects the qubit placement
  /// expected at the beginning of the QBB.
  int populateTableFromQBB(BasicBlock &QBB, QuantumModule &QM);

  /// Add to table the routing SWAPs to return every prog qubit to its reference
  /// physical qubit. This function is invoked only if the QBB does not release
  /// the qubits.
  ///
  /// The routing operations result in the change:
  ///    qubit_map_   -->   QM.final_qubit_maps[&QBB]
  /// If method is "retrace", we enforce that:
  ///    QM.qubit_map_[&QBB]  ==  QM.final_qubit_maps[&QBB]
  int extendTableWithRoutingBackToExpectedFinalQubitMap(
      BasicBlock &QBB, QuantumModule &QM, std::string = "retrace");

  //////////////////////////////////////////////////////////////////////

  /// Reorder according to table.
  ///
  /// This is done in two steps:
  /// 1) scheduled gates are added after the unscheduled circuit
  /// 2) the first part (i.e. the unscheduled cirucit) is removed,
  ///    in this way leaving only the scheduled circuit
  //
  // TODO: Is there a better way to do this that does not feel like
  //       duplicating the circuit and then pruning it?
  //       For example, one could move everything associated with a QIter to
  //       either before or after in the gate sequence. Would this move 'loads'
  //       too?
  int reorderGatesOfQuantumBasicBlock(BasicBlock &QBB);

  //////////////////////////////////////////////////////////////////////

  /// Add a single (1- or 2-qubit) gate to the table.
  void addGateToTable(std::vector<unsigned> phys_qubits, int gate_index,
                      int gate_duration);

  /// Convert the table into a sequence of gates.
  std::vector<int> convertTableToSequence() const;

  // /// Print table to screen.
  void printTable() const;

  //////////////////////////////////////////////////////////////////////
};

//////////////////////////////////////////////////////////////////////

class SchedulerPass : public PassInfoMixin<SchedulerPass> {
public:
  PreservedAnalyses run(Module &M, ModuleAnalysisManager &MAM);
};

#endif