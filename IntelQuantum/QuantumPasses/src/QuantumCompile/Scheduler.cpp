//===- Scheduler.cpp - Parallel schedule of quantum gates -----------===//
//
// Copyright (C) 2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//
//===----------------------------------------------------------------===//
//
// This file implements a transformation pass which schedules the program's
// gates with respect to the connectivity of the target platform. The
// information on connectivity, set of available gates and their duration, is
// provided via the config-json file and accessible via an object of class
// hardware::Machine.
//
//===----------------------------------------------------------------===//

#include "IntelQuantumPasses/QuantumCompile/Scheduler.h"
#include "IntelQuantumPasses/QuantumAnalysis/QuantumGateIdentifiers.h"
#include "IntelQuantumPasses/QuantumAnalysis/QuantumIterator.h"
#include "IntelQuantumPasses/QuantumAnalysis/QuantumRefs.h"
#include "IntelQuantumPasses/QuantumAnalysis/QuantumSpinNativeToJson.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/raw_ostream.h"

// Files from the gitsubmodule config-json
#include <machine_config.hpp>
#include <platform_configuration.hpp>
#include <utilities.hpp>

// Files from the gitsubmodule swapnet
#include <swap_network.hpp>

#include <cassert>
#include <climits> // for INT_MAX
#include <cmath>   // for ceil()
#include <cstdlib> // for EXIT_SUCCESS and EXIT_FAILURE
#include <fstream>
#include <iomanip>
#include <list>
#include <string>
#include <vector>

#define DEBUG_TYPE "quantum_scheduler"

using namespace llvm;
using namespace aqcc;

//////////////////////////////////////////////////////////////////////

const std::string kNameRoutingSwap = "kSwapalp";
const int kIdRoutingSwapIntrinsic = GateIdentifier::kSpinSwapalp;
const std::string kNameRoutingSwapIntrinsic =
    getPlatformNameFromGateIdentifier(kIdRoutingSwapIntrinsic);

// TODO: When adding the routing-swap gate, one needs gate_id and
// parameters. One option is to impose that the routing swap gate does not
// require any parameter. Currently an empty vector of ParRef is passed.
// Check if this triggers errors with 'SpinSwapalp'.

static cl::opt<std::string>
    ForwardSchedulingMethod("forward-scheduling",
                            cl::desc("Forward-scheduling method"),
                            cl::init("none"));

static cl::opt<std::string>
    BackwardSchedulingMethod("backward-scheduling",
                             cl::desc("Backward-scheduling method"),
                             cl::init("retrace"));

//////////////////////////////////////////////////////////////////////

struct SchedulerLegacyPass : public ModulePass {

public:
  static char ID; // Pass identification, replacement for typeid
  QuantumModule &QM = QuantumCompilerLinkageLegacyPass::QM;
  QuantumScheduler QS;

  //////////////////////////////////////////////////////////////////////

  /// Default constructor.
  SchedulerLegacyPass() : ModulePass(ID) {}
  /// Destructor.
  ~SchedulerLegacyPass(){};

  /// Actions taken when pass is invoked.
  bool runOnModule(Module &M) override {
    QS.readCommandLineOptions(QM);
    QS.runImpl(M, QM);

    return false;
  }

  //////////////////////////////////////////////////////////////////////

  // If certain analysis passes are needed before this transfom pass,
  // list them here. NOTE: only analysis passes.
  //
  // INFO: Pass ResolveArrayIndices should not have been run.
  // TODO: Any way to confirm this?
  void getAnalysisUsage(AnalysisUsage &AU) const override {
    AU.setPreservesAll();
    AU.addRequired<QuantumSpinNativeToJsonWrapperPass>();
  }

}; // end of struct SchedulerLegacyPass

//////////////////////////////////////////////////////////////////////
// Definition of QuantumScheduler methods
//////////////////////////////////////////////////////////////////////

void QuantumScheduler::initialization(QuantumModule &QM) {
  // The platform and machine object are obtained from the QuantumModule.
  machine_ = &QM.machine;
  num_phys_qubits_ = QM.getNumPlatformQubits();
}

//////////////////////////////////////////////////////////////////////

void QuantumScheduler::readCommandLineOptions(QuantumModule &QM) {
  // Determine the scheduling methods.
  std::string ErrMsg;
  if (!QM.has_platform) {
    // In absence of a platform json file, use default methods assuming
    // all-to-all connectivity and no serialization.
    forward_scheduling_method_ = "none";
    backward_scheduling_method_ = "retrace";
    serialize_ = false;
    ErrMsg = "No platform configuration detected. "
             "Assuming an all-to-all connectivity for the qubits. \n"
             "Default forward method: none\n"
             "Default backward method: greedy\n";
    displayWarning(ErrMsg);
  } else {
    this->setForwardSchedulingMethodAndSerializeOption(ForwardSchedulingMethod);
    this->setBackwardSchedulingMethod(BackwardSchedulingMethod);
  }
}

//////////////////////////////////////////////////////////////////////

void QuantumScheduler::runImpl(Module &M, QuantumModule &QM) {
  int status;
  std::string ErrMsg;

  this->initialization(QM);

  // Situation from previous passes:
  // - unique id of program qubits has been determined
  // - program qubits have been placed onto physical qubits
  this->confirmExistenceOfQubitMapsInQuantumModule(QM);

  // When needed, one can print information from the platform/machine as
  // follows:
  // 1) platform.printInstanceID(); // TODO: not enabled in class
  // 2) PlatformConfiguration
  LLVM_DEBUG(QM.MachinePrint());

  // Core implementation:
  // - greedy approach to create the scheduled table (forward routing)
  // - backward routing to reach the expected final qubit placement

  // Setup for certain backward scheduling methods, like searching for
  // an Hamiltonian path.
  // This is done here to avoid repeating it for every QBB.
  this->setupForSpecificBackwardSchedulingMethods(QM);

  // Looping on the quantum basic blocks (QBB).
  // INFO: The loop already exclude some functions like those defining
  //       quantum types and spin native gates.
  for (auto &QK : QM) {

    LLVM_DEBUG(dbgs() << "Quantum kernel name:  " << QK.getFunction()->getName()
                      << "\n");

    for (auto &QBB : QK) {

      // Set intial qubit map (assumed realized at the beginning
      // of the QBB) as indicated by the proper entry of:
      //   QM.qubit_maps
      // We already checked that the map exists.
      initial_qubit_map_ = *(QM.qubit_maps[&QBB]);
      qubit_map_ = initial_qubit_map_;

      // Create table with scheduled gates and routing operations.
      // There are three options that influence the working of the scheduler:
      // 1. if the QBB releases the qubits, it is not necessary to return
      //    to the expected final map
      // 2. if the measurement can be queued, there is no constraint
      //    of a single measurement per physical qubit and no necessity
      //    of moving the measurement at the end of the QBB
      // 3. if measurements are matched and no operation follows on the
      //    measured program qubit, we postponed them at the end of the
      //    QBB, even after the re-routing back to the final placement
      //    if applicable).
      if (forward_scheduling_method_ == "greedy") {

        // --------- Call to the actual forward scheduling --------
        status = populateTableFromQBB(QBB, QM);
        // --------------------------------------------------------
        if (status != EXIT_SUCCESS) {
          ErrMsg = "Cannot populate table for QBB " + (QBB.getName()).str();
          displayErrorAndExit("Scheduler", ErrMsg);
        }

        // The methods currently available are:
        // - "retrace"
        // - "bubble-sort"
        // - "oddeven-sort"
        // - "grid_AAxBB"  <-- AA = num rows, BB = num cols
        if (isReleaseQBB(QBB) == false) {

          // The "retrace" method leads to a final qubit placement
          // equal to the initial one. While this is the expected behavior
          // as of Dec 2023, this constraint may be relaxed in the
          // future. In preparation, we add a check that the initial
          // and final maps provided by the Quantum Module coincide.
          if (backward_scheduling_method_ == "retrace" &&
              !(initial_qubit_map_.IsEqual(*(QM.final_qubit_maps[&QBB])))) {
            ErrMsg = "retrace cannot lead to a final placement different "
                     "from that at the beginning of QBB " +
                     DemangleQuantumKernelName(QK.getFunction()->getName());
            displayErrorAndExit("Scheduler", ErrMsg);
          }

          // --------- Call to the actual backward scheduling --------
          status = extendTableWithRoutingBackToExpectedFinalQubitMap(
              QBB, QM, backward_scheduling_method_);
          // --------------------------------------------------------
          // The above function returns FAILURE if qubit_map_ does not
          // correspond to the expected final qubit placement.
          if (status != EXIT_SUCCESS) {
            ErrMsg = "Cannot add routing to desired final placement "
                     "in QBB " +
                     (QBB.getName()).str();
            displayErrorAndExit("Scheduler", ErrMsg);
          }
        } else {
          // For hold QBB, the final qubit placement is the expected one.
          // For release QBB, here we update the final qubit placement
          // saved in the QuantumModule.
          std::shared_ptr<scheduler::MapProgToPhysQubits> temp_map_ptr(
              new scheduler::MapProgToPhysQubits(qubit_map_));
          QM.final_qubit_maps[&QBB] = temp_map_ptr;
        }
      }

      // For debugging, it may be helpful to print the table
      // associated to the QBBs:
      LLVM_DEBUG(this->printTable());

      // Update IR to reflect the scheduled table and clean up
      // intermediate variables. If method is "none", we do nothing
      // because there was no scheduling.
      if (forward_scheduling_method_ != "none") {

        status = reorderGatesOfQuantumBasicBlock(QBB);
        if (status != EXIT_SUCCESS) {
          ErrMsg = "Cannot reorder the quantum operations of QBB" +
                   (QK.getFunction()->getName()).str();
          displayErrorAndExit("Scheduler", ErrMsg);
        }

        // Clean variables for routing swaps.
        LLVM_DEBUG(dbgs() << "~~~~ Added " << routing_source_.size()
                          << " swap gates to this function for routing.\n");
        routing_source_.clear();
        routing_target_.clear();

        // Print qubit placement at the end of this function.
        LLVM_DEBUG(
            dbgs() << "~~~~ Qubit placement at the end of the function:\n"
                   << qubit_map_);

        // TODO: validation not available in open-source repository.
        int verify = 1;
        LLVM_DEBUG(errs() << "Verifier is " << verify << ".\n");

        if (verify == 0)
          displayErrorAndExit(
              "Scheduler", "verification failed for a quantum basic block in " +
                               (QBB.getParent()->getName()).str());
        else if (verify == -1)
          errs()
              << "WARNING: Quantum SDK - [Scheduler] a quantum basic block in "
                 "'" +
                     DemangleQuantumKernelName(QBB.getParent()->getName())
              << "' could not be confirmed as equivalent to the original "
                 "sequence of gates.\n"
              << "This may be due to the use of dynamic parameters which could "
                 "not be confirmed as equal; see debug info for details.\n";

        if (serialize_)
          setSerializedSlicing(QBB);
        else
          setSingleSliceBeginAndEnd(QBB);
      } else if (serialize_)
        setSerializedSlicing(QBB);
    }
  }
} // runOnModule

//////////////////////////////////////////////////////////////////////

void QuantumScheduler::confirmExistenceOfQubitMapsInQuantumModule(
    QuantumModule &QM) {
  std::string ErrMsg;
  if (QM.global_qubit_map) {
    LLVM_DEBUG(dbgs() << "QM.global_qubit_map:\n"
                      << *QM.global_qubit_map << "\n");
  } else {
    ErrMsg = "Cannot find the global qubit map in the Quantum Module";
    displayErrorAndExit("Scheduler", ErrMsg);
  }

  // To find out how many global (program) qubits are used, one has to
  // look at the QM.global_qubit_map. However the maps stored in
  // QM.qubit_maps_ may differ due to the addition of local (program)
  // qubits. In this case, the difference should only affect the local
  // qubits. Initial and final maps do not need to coincide (apart in
  // the case of "retracing" as backward routing method).

  for (auto &QK : QM) {
    for (auto &QBB : QK) {
      if (!QM.qubit_maps[&QBB]) {
        ErrMsg = "Cannot find the initial qubit map for QK " +
                 DemangleQuantumKernelName(QK.getFunction()->getName());
        displayErrorAndExit("Scheduler", ErrMsg);
      }
      // Single-QBB maps may include local qubits.
      qubit_map_ = *(QM.qubit_maps[&QBB]);
      LLVM_DEBUG(dbgs() << QBB.getName() << ":\n" << qubit_map_ << "\n");
    }
  }
}

//////////////////////////////////////////////////////////////////////

void QuantumScheduler::setForwardSchedulingMethodAndSerializeOption(
    std::string method) {
  size_t greedy_idx = method.find("greedy");
  size_t none_idx = method.find("none");

  if (greedy_idx != std::string::npos) {
    forward_scheduling_method_ = "greedy";
    serialize_ = method.find("serial", greedy_idx) != std::string::npos;
  } else if (none_idx != std::string::npos) {
    forward_scheduling_method_ = "none";
    serialize_ = method.find("serial", none_idx) != std::string::npos;
  } else {
    std::string ErrMsg =
        "Unknwon optimization method for the forward scheduling. "
        "Using default: " +
        forward_scheduling_method_;
    displayWarning(ErrMsg);
    serialize_ = method.find("serial") != std::string::npos;
  }
}

//////////////////////////////////////////////////////////////////////

void QuantumScheduler::setBackwardSchedulingMethod(std::string method) {
  // Confirm that backward routing method is known.
  if (method == "retrace" || method == "bubble-sort" ||
      method == "oddeven-sort" || method == "grid") {
    backward_scheduling_method_ = method;
  } else {
    backward_scheduling_method_ = "retrace";
    std::string ErrMsg =
        "Unknwon optimization method for the backward scheduling. "
        "Using default: " +
        backward_scheduling_method_;
    displayWarning(ErrMsg);
  }
}

//////////////////////////////////////////////////////////////////////

void QuantumScheduler::setupForSpecificBackwardSchedulingMethods(
    QuantumModule &QM) {
  std::string ErrMsg;
  int status;

  // Since methods bubble-sort and oddeven-sort rely on finding
  // an Hamiltonian path of the (physical) qubit connectivity graph,
  // if the heuristic fails to find such path we fall back to the default
  // method "retrace".
  if (backward_scheduling_method_ == "bubble-sort" ||
      backward_scheduling_method_ == "oddeven-sort") {
    // The second argument below is the starting point (recall that it may
    // be modified while looking for the Hamiltonian path).
    status = machine_->GetHamiltonianPath(hamil_path_for_sort_, 0);
    if (status != EXIT_SUCCESS) {
      ErrMsg = "Cannot find the Hamiltonian path for sorting. "
               "Revert to default method 'retrace' for the backward routing.";
      displayWarning(ErrMsg);
      backward_scheduling_method_ = "retrace";
    }
  }
  // The grid method relies on a specific connectivity: a 2D grid of qubits.
  // The method search for the most convenient 2D grid size (i.e. num of rows
  // and columns). This is a requirement verified once per compilation
  // (as opposed to once per quantum kernel). If the connectivity is not a grid,
  // we change the method to "retrace".
  if (backward_scheduling_method_ == "grid") {
    // Class Machine defined in the config-json repo has no method to identify
    // the larger 2d grid describing the qubit connectivity. However, it has
    // methods to verify if a certain 2d grid is contained in the qubit
    // connectivity. Numerical experiments suggest that the "grid" method works
    // better for many short rows as opposed to a few long rows (if square is
    // not possible).
    if (num_phys_qubits_ != QM.getNumPlatformQubits()) {
      ErrMsg =
          "Scheduler has num_phys_qubits_=" + std::to_string(num_phys_qubits_) +
          " different from the one (" +
          std::to_string(QM.getNumPlatformQubits()) +
          ") in the QuantumModule. Update the former.";
      displayWarning(ErrMsg);
      num_phys_qubits_ = QM.getNumPlatformQubits();
    }

    unsigned num_cols = std::sqrt(num_phys_qubits_) +
                        1; // +1 is necessary when nqubits is not a square
    unsigned num_rows;
    bool is_grid = false;
    for (; num_cols > 0; --num_cols) {
      if (num_phys_qubits_ % num_cols == 0) {
        num_rows = num_phys_qubits_ / num_cols;
      } else {
        continue;
      }
      is_grid =
          QM.has2DGridWithRowMajorOrder(num_rows, num_cols, kNameRoutingSwap);
      if (is_grid) {
        break;
      } else {
        // Exchange role of rows and columns.
        is_grid =
            QM.has2DGridWithRowMajorOrder(num_cols, num_rows, kNameRoutingSwap);
        if (is_grid) {
          unsigned temp = num_cols;
          num_cols = num_rows;
          num_rows = temp;
          break;
        }
      }
    }

    if (is_grid != true) {
      ErrMsg = "Cannot verify that the connectivity is a 2D grid. "
               "Revert to default method 'retrace' for the backward routing.";
      displayWarning(ErrMsg);
      backward_scheduling_method_ = "retrace";
    } else {
      backward_scheduling_method_ =
          "grid_" + std::to_string(num_rows) + "x" + std::to_string(num_cols);
      LLVM_DEBUG(dbgs() << "Backward scheduling method updated to:  "
                        << backward_scheduling_method_ << "\n");
    }
  }
};

//////////////////////////////////////////////////////////////////////

int QuantumScheduler::populateTableFromQBB(BasicBlock &QBB, QuantumModule &QM) {
  // Start with a properly sized table (one component per physical qubit).
  table_.assign(num_phys_qubits_, {});

  bool print_info = true; // Turn on printing of debug info.
  LLVMContext &C = QBB.getContext();
  QBBIter qit(QBB);
  int gate_index = 0;
  std::vector<unsigned> prog_qubits;
  std::vector<unsigned> phys_qubits;
  bool status;
  std::vector<unsigned> phys_qubits_already_measured;
  std::string ErrMsg;

  for (qit.gotoBegin(); !qit.isEnd(); ++qit) {
    std::vector<QbitRef> qubits = qit.getQubitOperands();
    std::vector<ParaRef> paras = qit.getParametricOperands();
    prog_qubits.clear();
    phys_qubits.clear();
    for (unsigned i = 0; i < qubits.size(); ++i) {
      // All QbitRef should be resolvable thanks to
      // a previous pass filling QuantumModule.global_qubit_map
      prog_qubits.push_back(qubit_map_.getIdFromProgQbitRef(qubits[i]));
      phys_qubits.push_back(qubit_map_.getProgToPhys(qubits[i]));
    }

    int gate_id = getBaseVersionForSpinNative(qit.getIdentifier());
    // INFO: Since the platform config file does not uses the gate
    //       identifiers, we use the gate name.
    int gate_duration;
    // INFO: Spin-native state preparation and measurement are
    //       part of the API of class Machine.
    //       Instead, generic SPAM operations are not.
    // INFO: Irrespective of the measurements being queued or
    //       matched, they will be scheduled in mid-circuit and
    //       therefore treated as regular operations without
    //       postponing them to the end of the QBB.
    if (gate_id == GateIdentifier::kINIT || gate_id == GateIdentifier::kMEAS) {
      gate_duration = 1; // After discretization
    } else {
      gate_duration = QM.getGateDurationInClockCycles(gate_id, phys_qubits);
    }

    // If gate_duration is negative, it is not available for the current
    // qubit placement.
    // - if 1q gate, look for gate available in another qubit
    // - if 2q gate, look for pairs of qubits for which gate is available
    // - if needed, add routing starting from both sides
    // - when recreating the LLVM IR, add routing gates
    //
    // A special behavior is also expected for kSpinMeasZ when
    // the measurements are 'matched'.
    bool is_a_matched_meas = false;
    is_a_matched_meas =
        (gate_id == GateIdentifier::kSpinMeasZ) && QM.isMeasurementMatched();
    if (gate_duration < 0 || is_a_matched_meas) {

      int time_cost = INT_MAX;
      int gate_duration_at_goal;
      int gate_duration_for_swap;
      int duration_routing;
      std::vector<unsigned> path;
      std::vector<unsigned> path_from_goal;

      // If 1q-gate, look for same gate on another physical qubit,
      // then add a one-way routing towards that location.
      if (phys_qubits.size() == 1) {
        if (print_info)
          LLVM_DEBUG(dbgs()
                     << "~~ 1-qubit gate on phys (" << phys_qubits[0] << ")\n");
        // Add the routing.
        unsigned q_start = phys_qubits.front();
        if (is_a_matched_meas) {
          status = QM.updateShortestPathToAvailable1QubitGate(
              gate_id, q_start, path, phys_qubits_already_measured);
          phys_qubits_already_measured.push_back(path.back());
        } else
          status = QM.updateShortestPathToAvailable1QubitGate(gate_id, q_start,
                                                              path);

        if (status != EXIT_SUCCESS) {
          std::string gate_name;
          std::string json_gate_name =
              getPlatformNameFromGateIdentifier(gate_id);
          if (json_gate_name == "kRotz")
            gate_name = "qurotz";
          else if (json_gate_name == "kCPhase")
            gate_name = "qucphase";
          else
            gate_name = json_gate_name;

          ErrMsg = "1-qubit gate " + gate_name + " on qubit " +
                   std::to_string(q_start) +
                   " is not available in the platform.\nPotential solutions "
                   "are to use the -O1 flag when compiling or to use an "
                   "alternative configuration file. Refer to documentation for "
                   "details.\n";
          displayErrorAndExit("Scheduler", ErrMsg);
        }
        for (unsigned j = 1; j < path.size(); ++j) {
          routing_source_.push_back(path[j - 1]);
          routing_target_.push_back(path[j]);
          // Fill the table
          gate_duration_for_swap = QM.getGateDurationInClockCycles(
              kNameRoutingSwap, {path[j - 1], path[j]});

          this->addGateToTable({path[j - 1], path[j]}, -routing_source_.size(),
                               gate_duration_for_swap);
          if (print_info)
            LLVM_DEBUG(dbgs() << "~~ [1q] rout. swap on phys (" << path[j - 1]
                              << ", " << path[j] << ")\n");
          // Update qubit map
          qubit_map_.updateMapBySwappingPhysQubits(path[j - 1], path[j]);
        }
        // Update fields of the current gate from the program.
        unsigned q_goal = path.back();
        gate_duration = QM.getGateDurationInClockCycles(gate_id, {q_goal});
        phys_qubits = {q_goal};
      }
      // If 2q-gate, use methods from class Machine to identify a pair
      // of physical qubits suitable to implement the gate.
      // The method returns two paths, one for the control qubit ('path')
      // and one for the target qubit ('path_from_goal').
      else if (phys_qubits.size() == 2) {
        if (print_info)
          LLVM_DEBUG(dbgs() << "~~ 2-qubit gate "
                            << getPlatformNameFromGateIdentifier(gate_id)
                            << " on phys (" << phys_qubits[0] << ", "
                            << phys_qubits[1] << ")\n");
        // Identify two physical qubits that can implement the 2q-gate.
        // Select it based on the SWAP-distance + gate duration (done by Machine
        // object).
        unsigned q_start = phys_qubits.front(); // control qubit
        unsigned q_goal = phys_qubits.back();   // target qubit
        status = QM.updateShortestPathToAvailable2QubitGate(
            gate_id, q_start, q_goal, path, path_from_goal);

        if (status != EXIT_SUCCESS) {
          ErrMsg = "2-qubit gate " +
                   getPlatformNameFromGateIdentifier(gate_id) + "(" +
                   std::to_string(q_start) + "," + std::to_string(q_goal) +
                   ") is not available in the platform";
          displayErrorAndExit("Scheduler", ErrMsg);
        }
        // Routing of the first qubit in the pair of the gate's arguments.
        for (unsigned j = 1; j < path.size(); ++j) {
          routing_source_.push_back(path[j - 1]);
          routing_target_.push_back(path[j]);
          // Fill the table
          gate_duration_for_swap = QM.getGateDurationInClockCycles(
              kNameRoutingSwap, {path[j - 1], path[j]});
          this->addGateToTable({path[j - 1], path[j]}, -routing_source_.size(),
                               gate_duration_for_swap);
          if (print_info)
            LLVM_DEBUG(dbgs() << "~~ [2q src] rout. swap on phys ("
                              << path[j - 1] << ", " << path[j] << ")\n");
          // Update qubit map
          qubit_map_.updateMapBySwappingPhysQubits(path[j - 1], path[j]);
        }
        // Routing of the second qubit in the pair of the gate's arguments.
        for (unsigned j = 1; j < path_from_goal.size(); ++j) {
          routing_source_.push_back(path_from_goal[j - 1]);
          routing_target_.push_back(path_from_goal[j]);
          // Fill the table
          gate_duration_for_swap = QM.getGateDurationInClockCycles(
              kNameRoutingSwap, {path_from_goal[j - 1], path_from_goal[j]});

          this->addGateToTable({path_from_goal[j - 1], path_from_goal[j]},
                               -routing_source_.size(), gate_duration_for_swap);
          if (print_info)
            LLVM_DEBUG(dbgs() << "~~ [2q trg] rout. swap on phys ("
                              << path_from_goal[j - 1] << ", "
                              << path_from_goal[j] << ")\n");
          // Update qubit map
          qubit_map_.updateMapBySwappingPhysQubits(path_from_goal[j - 1],
                                                   path_from_goal[j]);
        }
        // Update fields of the current gate from the program.
        phys_qubits = {path.back(), path_from_goal.back()};
        gate_duration = QM.getGateDurationInClockCycles(gate_id, phys_qubits);
      } else {
        dbgs() << "ERROR: gate involving 3 or more qubits!\n";
        return EXIT_FAILURE;
      }
    }

    // In table, each qubit is a associated with a vector. The current
    // vector's size indicates how many time slots are already used for
    // the specific qubit. If the gate is a multiqubit gate, we may
    // need to pad the 'less used' qubit with a few empty spots. An
    // empty spot is indicated by nullptr.

    // INFO: The line below works for measurements too. In particular,
    //       one does not need to explicitly update the cbit hosting
    //       or receiving the outcome.
    this->addGateToTable(phys_qubits, gate_index, gate_duration);

    // Change qubit operands of qit
    if (qubits.size() != phys_qubits.size()) {
      displayErrorAndExit("Scheduler",
                          "Inconsistent number of physical qubits");
    }
    for (unsigned i = 0; i < phys_qubits.size(); ++i) {
      QbitRef qr(phys_qubits[i], C);
      qubits[i] = qr;
    }
    qit.setQubitOperands(qubits);

    gate_index += 1;
  } // end of for loop on qit

  num_gates_ = gate_index; // It includes the measurements.
  return EXIT_SUCCESS;
} // populateTableFromQBB

//////////////////////////////////////////////////////////////////////

int QuantumScheduler::extendTableWithRoutingBackToExpectedFinalQubitMap(
    BasicBlock &QBB, QuantumModule &QM, std::string method) {
  bool print_info = true; // Turn on printing of debug info.
  int gate_duration_for_swap;
  int status;
  std::string ErrMsg;

  // The retrace approach applies all SWAPs adding while scheduling
  // in reverse order. This is inefficient since there is no need to
  // reproduce all the intermediate placements obtained executing the QBB.
  if (method == "retrace") {
    // Instead of using the routing_source_ and routing_target_ directly,
    // we work with a copy to simplify it by removing consecutive SWAPs
    // on the same pair of physical qubits.
    std::vector<unsigned> temp_routing_source = routing_source_;
    std::vector<unsigned> temp_routing_target = routing_target_;
    unsigned phys_q_source, phys_q_target;
    bool was_swap_removed = true;
    bool is_a_match = false;
    while (was_swap_removed) {
      was_swap_removed = false;
      for (int i = temp_routing_source.size() - 1; i >= 1; i--) {
        phys_q_source = temp_routing_source[i];
        phys_q_target = temp_routing_target[i];
        for (int j = i - 1; j >= 0; j--) {
          is_a_match = (temp_routing_source[j] == phys_q_source &&
                        temp_routing_target[j] == phys_q_target) ||
                       (temp_routing_source[j] == phys_q_target &&
                        temp_routing_target[j] == phys_q_source);
          if (is_a_match) {
            // Found match!
            was_swap_removed = true;
            temp_routing_source.erase(temp_routing_source.begin() + i);
            temp_routing_target.erase(temp_routing_target.begin() + i);
            temp_routing_source.erase(temp_routing_source.begin() + j);
            temp_routing_target.erase(temp_routing_target.begin() + j);
            i -= 1;
            break;
          }
          if (temp_routing_source[j] == phys_q_source ||
              temp_routing_source[j] == phys_q_target ||
              temp_routing_target[j] == phys_q_source ||
              temp_routing_target[j] == phys_q_target) {
            break;
          }
        }
      }
    }

    unsigned num_swaps_to_add = temp_routing_source.size();
    for (int index_swap = num_swaps_to_add; index_swap > 0; index_swap--) {
      phys_q_source = temp_routing_source[index_swap - 1];
      phys_q_target = temp_routing_target[index_swap - 1];
      // Add retraced swap.
      routing_source_.push_back(phys_q_source);
      routing_target_.push_back(phys_q_target);
      // Fill the table.
      gate_duration_for_swap = QM.getGateDurationInClockCycles(
          kNameRoutingSwap, {phys_q_source, phys_q_target});
      this->addGateToTable({phys_q_source, phys_q_target},
                           -routing_source_.size(), gate_duration_for_swap);
      if (print_info)
        LLVM_DEBUG(dbgs() << "~~ [retrace] rout. swap on phys ("
                          << phys_q_source << ", " << phys_q_target << ")\n");
      // Update qubit map.
      qubit_map_.updateMapBySwappingPhysQubits(phys_q_source, phys_q_target);
    }
  }
  // The appraches based on 1d sorting algorithms (here "bubble-sort" and
  // "oddeven-sort") work well for linear connectivities.
  // Their implementation does not assume a trivial order of the physical
  // qubits along the line.
  else if (method == "bubble-sort" || method == "oddeven-sort") {
    // Use the previously identified Hamiltonian path in the connectivity graph.
    std::vector<unsigned> path = hamil_path_for_sort_;
    if (hamil_path_for_sort_.size() != num_phys_qubits_) {
      ErrMsg = "Inconsistency with the length of an Hamiltonian path "
               "of the connectivity graph for physical qubits.";
      displayErrorAndExit("Scheduler", ErrMsg);
    }

    std::vector<unsigned>::iterator it, it_now;
    // Remove from the linear order those physical qubits that are "free"
    // for both current and initial placement.
    it = path.end();
    while (it != path.begin()) {
      --it;
      if (initial_qubit_map_.isPhysicalQubitFree(*it) == true &&
          qubit_map_.isPhysicalQubitFree(*it) == true) {
        if (it == path.begin() || it == path.end() - 1) {
          it = path.erase(it);
        } else if (machine_->IsGateAvailable(kNameRoutingSwap,
                                             {*(it - 1), *(it + 1)})) {
          it = path.erase(it);
        }
      }
    }
    num_prog_qubits_ = qubit_map_.getNumProgQubits();
    if (path.size() < num_prog_qubits_) {
      ErrMsg = "path does not pass throgh all program qubits";
      displayErrorAndExit("Scheduler", ErrMsg);
    }

    // From the current qubit_map_ back to the expected final qubit map.
    unsigned phys_q_source, phys_q_target;
    // Use swapnet to obtain the list of SWAP gates in terms of the physical
    // qubits.
    swapnet::SwapNetwork sn;
    sn.initial_sequence = qubit_map_.getPhysToProg(path);
    sn.final_sequence = QM.final_qubit_maps[&QBB]->getPhysToProg(path);
    sn.position_labels = path;
    sn.SetWildcardValue(scheduler::MapProgToPhysQubits::GetFreeValue());
    if (method == "bubble-sort")
      sn.Sort("bubble");
    else if (method == "oddeven-sort")
      sn.Sort("oddeven");
    std::vector<std::pair<unsigned, unsigned>> list_of_swaps =
        sn.GetSwapsByLabels();

    // Add the swap gates to the IR while applying them to the qubit map.
    for (auto &&s : list_of_swaps) {
      phys_q_source = s.first;
      phys_q_target = s.second;
      // Add retraced swap.
      routing_source_.push_back(phys_q_source);
      routing_target_.push_back(phys_q_target);
      // Fill the table.
      gate_duration_for_swap = QM.getGateDurationInClockCycles(
          kNameRoutingSwap, {phys_q_source, phys_q_target});
      this->addGateToTable({phys_q_source, phys_q_target},
                           -routing_source_.size(), gate_duration_for_swap);
      if (print_info)
        LLVM_DEBUG(dbgs() << "~~ [sort] rout. swap on phys (" << phys_q_source
                          << ", " << phys_q_target << ")\n");
      // Update qubit map.
      qubit_map_.updateMapBySwappingPhysQubits(phys_q_source, phys_q_target);
    }
  }
  // For a 2D grid with 12 rows and 4 columns, the method has the form
  // "grid_12x4"
  else if (method.substr(0, 5) == "grid_") {
    // Use swapnet to obtain the list of SWAP gates in terms of the physical
    // qubits.
    swapnet::SwapNetwork sn;
    sn.initial_sequence = qubit_map_.physToProgMap();
    sn.final_sequence = QM.final_qubit_maps[&QBB]->physToProgMap();
    sn.SetWildcardValue(scheduler::MapProgToPhysQubits::GetFreeValue());
    sn.Sort(method);
    std::vector<std::pair<unsigned, unsigned>> list_of_swaps = sn.GetSwaps();

    // From the current qubit_map_ back to the expected final qubit map.
    unsigned phys_q_source, phys_q_target;
    // Add the swap gates to the IR while applying them to the qubit map.
    for (auto &&s : list_of_swaps) {
      phys_q_source = s.first;
      phys_q_target = s.second;
      // Add retraced swap.
      routing_source_.push_back(phys_q_source);
      routing_target_.push_back(phys_q_target);
      // Fill the table.
      gate_duration_for_swap = QM.getGateDurationInClockCycles(
          kNameRoutingSwap, {phys_q_source, phys_q_target});
      this->addGateToTable({phys_q_source, phys_q_target},
                           -routing_source_.size(), gate_duration_for_swap);
      if (print_info)
        LLVM_DEBUG(dbgs() << "~~ [sort] rout. swap on phys (" << phys_q_source
                          << ", " << phys_q_target << ")\n");
      // Update qubit map.
      qubit_map_.updateMapBySwappingPhysQubits(phys_q_source, phys_q_target);
    }
////////////////////// FIXME TBD (kept in preparation of debugging)
#if 0
LLVM_DEBUG(
dbgs() << "path (w.r.t. phys qubits):\n";
for (auto q: path)
  dbgs() << q << "\t";
dbgs() << "\ninitial q map (w.r.t. path):\n";
for (auto q: sn.initial_sequence)
  dbgs() << q << "\t";
dbgs() << "\nfinial q map (w.r.t. path):\n";
for (auto q: sn.final_sequence)
  dbgs() << q << "\t";
dbgs() << "\nswaps (w.r.t. phys qubits):\n";
for (auto s: list_of_swaps)
  dbgs() << "(" << s.first << "," << s.second << ")\t";
dbgs() << "\n";
)
#endif
    //////////////////////
  }

  if (qubit_map_.IsEqual(*(QM.final_qubit_maps[&QBB])) == true) {
    return EXIT_SUCCESS;
  } else {
    LLVM_DEBUG(dbgs() << "Qubit map reached after the backward routing is "
                         "different from the expected one.\n");
    return EXIT_FAILURE;
  }
} // extendTableWithRoutingBackToExpectedFinalQubitMap

//////////////////////////////////////////////////////////////////////

/// Convert the table into a sequence of gates.
std::vector<int> QuantumScheduler::convertTableToSequence() const {
  std::vector<bool> was_program_gate_already_added(num_gates_, false);
  std::vector<bool> was_routing_swap_already_added(routing_source_.size(),
                                                   false);
  std::vector<int> sequence;

  int g_index;
  int time = 0;
  int counter = 1;
  while (counter > 0) {
    counter = 0;
    for (int iq = 0; iq < table_.size(); ++iq) {
      // Verify that schedule for the qubit is longer than 'time'
      if (table_[iq].size() > time)
        g_index = table_[iq][time];
      else
        continue;
      ++counter;
      // Neglect idle and still-in-use qubits.
      if (g_index == is_idle_ || g_index == is_still_busy_)
        continue;
      // Program gate, verify that gate has not yet been added to the sequence.
      else if (g_index >= 0 &&
               was_program_gate_already_added[g_index] == false) {
        sequence.push_back(g_index);
        was_program_gate_already_added[g_index] = true;
      }
      // Routing swap, verify that gate has not yet been added to the sequence.
      else if (g_index < 0 &&
               was_routing_swap_already_added[-g_index - 1] == false) {
        sequence.push_back(g_index);
        was_routing_swap_already_added[-g_index - 1] = true;
      }
    }
    ++time;
  }

  if (sequence.size() != num_gates_ + routing_source_.size()) {
    LLVM_DEBUG(dbgs() << "Debug info:\n"
                      << "num gates = " << num_gates_
                      << " , routing swaps = " << routing_source_.size()
                      << " , length of sequence = " << sequence.size() << "\n"
                      << "The gates added to the sequence are (recall this is "
                         "the g_index and not the gate code):\n");
    for (int &j : sequence)
      LLVM_DEBUG(dbgs() << j << "\n");

    displayErrorAndExit("Scheduler",
                        "Number of scheduled gates does not equal to the "
                        "number of unscheduled gates plus inserted SWAP gates");
  }

  return sequence;
} // convertTableToSequence

//////////////////////////////////////////////////////////////////////

int QuantumScheduler::reorderGatesOfQuantumBasicBlock(BasicBlock &QBB) {
  std::vector<int> g_sequence = convertTableToSequence();
  std::vector<QbitRef> qubits;
  std::vector<ParaRef> paras;
  int gate_id;

  // Get current end for place of insertion
  LLVMContext &C = QBB.getContext();
  QBBIter qit_end(QBB);
  qit_end.gotoEnd();

  int ig_old = 0;
  QBBIter qit_target(QBB);
  for (const int &ig : g_sequence) {
    // Program gate
    if (ig >= 0) {
      qit_target.translateIterator(ig - ig_old);
      qubits = qit_target.getQubitOperands();
      paras = qit_target.getParametricOperands();
      // set last operand to slice_mid
      paras.back().setValueResolved(SLICE_MID);
      gate_id = qit_target.getIdentifier();
      ig_old = ig;
    }
    // Routing swap
    else {
      QbitRef qr0(routing_source_[-ig - 1], C);
      QbitRef qr1(routing_target_[-ig - 1], C);

      // Take qubits from one of the CP gates (after native spin conversion
      // pass).
      qubits = std::vector<QbitRef>({qr0, qr1});
      // paras = GetParasRoutingSwapIntrinsic(QBB);
      std::vector<ParaRef> params(2);
      params[0].setValueResolved(FP_PI, Type::getDoubleTy(QBB.getContext()));
      params[1] = ParaRef(SLICE_MID, Type::getInt32Ty(QBB.getContext()));
      paras = params;
      gate_id = kIdRoutingSwapIntrinsic;
    }
    // Insert at the end
    if (qit_end.insertGate(gate_id, qubits, paras))
      ++qit_end; // Iterate past inserted gate to be at the end (insert
                 // inserts before current gate)
    else {
      dbgs() << "ERROR: Quantum SDK - problem in inserting scheduled gates "
                "with index "
             << ig << "\n";
      return EXIT_FAILURE;
    }
  }

  // Remove old gates
  QBBIter qit_rm_gate(QBB);
  QBBIter qit_first_new_gate(QBB);
  qit_first_new_gate.translateIterator(num_gates_);
  while (qit_rm_gate != qit_first_new_gate)
    qit_rm_gate.removeGate(); // remove puts QIter at the next gate in the list

  return EXIT_SUCCESS;
} // reorderGatesOfQuantumBasicBlock

////////////////////////////////////////////////////////////////////////////////

void QuantumScheduler::addGateToTable(std::vector<unsigned> phys_qubits,
                                      int gate_index, int gate_duration) {
  // Determine the time at which the gate starts.
  unsigned start_time = 0;
  for (const unsigned &phys_q : phys_qubits) {
    unsigned occupied_time = table_[phys_q].size();
    if (occupied_time > start_time)
      start_time = occupied_time;
  }

  // Extend schedule for 'less occupied' qubits and add gate.
  for (const unsigned &phys_q : phys_qubits) {
    table_[phys_q].resize(start_time, is_idle_);
    table_[phys_q].push_back(gate_index);
    table_[phys_q].resize(start_time + gate_duration, is_still_busy_);
  }
} // addGateToTable

//////////////////////////////////////////////////////////////////////

void QuantumScheduler::printTable() const {
  dbgs() << "table (row by row, meaning qubit by qubit):\n";
  for (int index_q = 0; index_q < table_.size(); index_q++) {
    dbgs() << "q" << index_q << ":\t";
    for (auto &gate_index : table_[index_q]) {
      std::stringstream dbgstr;
      if (gate_index == is_idle_)
        dbgstr << ".  ";
      else if (gate_index == is_still_busy_)
        dbgstr << "=  ";
      else
        dbgstr << std::left << std::setw(3) << gate_index;
      dbgs() << dbgstr.str();
    }
    dbgs() << "\n";
  }
}

////////////////////////////////////////////////////////////////////////////////

char SchedulerLegacyPass::ID = 0;

INITIALIZE_PASS(SchedulerLegacyPass, "q-scheduler", "SchedulerLegacyPass",
                false, false)

PreservedAnalyses SchedulerPass::run(Module &M, ModuleAnalysisManager &MAM) {
  QuantumScheduler QS;
  QuantumModuleProxy QMP = MAM.getResult<QuantumCompilerLinkageAnalysis>(M);

  QS.readCommandLineOptions(*QMP.QM);
  QS.runImpl(M, *QMP.QM);

  return PreservedAnalyses::all();
}

#undef DEBUG_TYPE
