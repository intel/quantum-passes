//===- QuantumModule.h ---------------------------------------------====//
//
// Copyright (C) 2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//
//===----------------------------------------------------------------------===//
//
// This file defines a super-class which contains all of the meta information
// regarding a quantum compilation.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_QUANTUM_MODULE_H
#define LLVM_QUANTUM_MODULE_H

#include "llvm/ADT/SCCIterator.h"
#include "llvm/Analysis/CFG.h"
#include "llvm/IR/CFG.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/GlobalValue.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Operator.h"
#include "llvm/Pass.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/JSON.h" // JSON package
#include "llvm/Support/SourceMgr.h"
#include "llvm/Support/raw_ostream.h"

#include <string>
#include <vector>

#include "IntelQuantumPasses/QuantumAnalysis/QuantumGateIdentifiers.h"
#include "IntelQuantumPasses/QuantumAnalysis/QuantumRefs.h"
#include "IntelQuantumPasses/QuantumUtils/QuantumMapProgToPhysQubits.hpp"

// Files from the gitsubmodule config-json
#include <machine_config.hpp>
#include <platform_configuration.hpp>

namespace llvm {
namespace aqcc {

/// container for quantum kernels (and other compilation data)
class QuantumModule;
/// container for iterating over quantum basic blocks
class QuantumKernel;
/// container for iterating over all quantum gate definitions
class QuantumGates;

class QuantumModule {

public:
  /// The class contains two Modules (ptrs) one for the original module (base)
  /// and one for the quantum split-off. This class owns the pointer to the
  /// q_split and and sometimes owns the base pointer
  Module *base = nullptr;
  bool is_base_owned = false;
  Module *q_split = nullptr;
  LLVMContext C; // This is the Context only if base is owned by this class
  SMDiagnostic Err;

  /// the quantum gate metadata
  /// This also suffices as a list of the gates
  using GateDataMap = std::map<StringRef, json::Object>;
  GateDataMap q_gate_metadata;

  /// the list of quantum kernels. QuantumKernel object is defined below
  std::vector<QuantumKernel> q_kernel_list;
  /// list of the simple QBBs (in the base module)
  std::map<Function *, BasicBlock *> QBB_link_map;

  /// The references to the QRT library functions
  Function *QRT_initialize = nullptr;
  Function *QRT_is_initialized = nullptr;
  Function *QRT_measure_move = nullptr;
  Function *QRT_run_qbb = nullptr;
  Function *QRT_dismantle = nullptr;
  Function *QRT_exit = nullptr;

  /// QSDA related info
  Value *QSDA = nullptr;
  std::map<BasicBlock *, unsigned> QSDA_used_map;
  unsigned max_QSDA_used = 4; // aqcc::QSDA_RESERVED_POSITIONS; should match
                              // this value in "QuantumGeneralUtils.h"

  /// QSCbA related info
  std::map<BasicBlock *, unsigned> QSCbA_used_map;
  unsigned max_QSCbA_used = 0;
  bool is_meas_queued = false;

  /// Qubit to physical map-map
  using ProgToPhysMapMap =
      std::map<BasicBlock *, std::shared_ptr<scheduler::MapProgToPhysQubits>>;
  /// Global map (typically just for intialization of qbits):
  /// it considers only the global qubits shared across the quantum kernels.
  std::shared_ptr<scheduler::MapProgToPhysQubits> global_qubit_map;
  /// Initial qubit placement at the beginning of every single QBB:
  /// each map contains both global and local (those initialized inside the QBB)
  /// qubits.
  ProgToPhysMapMap qubit_maps;

  /// Final qubit placement at the end of every single QBB.
  /// NOTE: The final map is not forced to be the same as the initial map,
  ///       especially for those QBBs that release the qubits. Even for
  ///       hold QBB, the correspondence may be limited to the global qubits.
  ProgToPhysMapMap final_qubit_maps;

  /// Config related info
  std::string sdk_json_filename = "";
  std::string sdk_path = "";
  std::string platform_filename = ""; // must get from sdk_json

  std::unique_ptr<TPL::PlatformConfiguration> platform;
  bool has_platform = false;

  hardware::Machine machine;

  /// Index to determine the position of the compilation according to the
  /// following enum:
  enum QCompPosition {
    kUnsure = -1,
    kBegin = 0,         // beginning
    kValid = 1,         // quantum kernels are valid
    kConditioned = 2,   // quantum kernels are conditioned
    kNative = 3,        // quantum BBs are lowered to non-IMM native gates
    kMapped = 4,        // quantum BBs have been mapped and scheduled
    kAnglesInRange = 5, // resolved angles are in their native-gate range
    kQSDASpilt = 6,     // QSDA is created and gate parameters are stored in it
    kClearOfParams =
        7, // classical QSDA parameter manipulation is removed from QBBs
    kMeasMoved = 8, // QRT measurement move instructions have been inserted
    kAllIMM = 9,    // All native gate parameters are immediates
    kSimpleQBBSplit = 10,
    kReadyForModSplit = 11, // the base module is ready to be split
    kModSplit =
        12, // the module has been split in the the classical and quantum parts
    kFinalized = 13 // The classical module is fully finalized and ready for
                    // classical compilation
  };

  QCompPosition pos = kUnsure;

  /// Does the module contain FLEQ?
  bool has_fleq = false;

  /// The following intializes some of the data based on what can be obtain
  /// immediately from the base module. This is used primarily by Ctors
  void initializeData(Module &M);

public:
  /// default constructor
  QuantumModule() = default;

  /// Constructor from a prexisting model
  /// Assumption is that something else owns this module
  QuantumModule(Module &M, const std::string &json_filename = "",
                bool is_ext_owned = true)
      : base(&M), sdk_json_filename(json_filename),
        is_base_owned(!is_ext_owned) {
    initializeData(M);
  }

  /// Constructor from file
  QuantumModule(const std::string &filename,
                const std::string &json_filename = "");

  /// Dtor
  ~QuantumModule() {

    if (is_base_owned)
      delete base;
    else
      base = nullptr;
    delete q_split;
  }

  /// Get the classical module.
  Module *getBaseModule() { return base; }
  /// If circuit has gone through the separation and splitting
  /// get the quatnum instructions.
  Module *getQuantumSplitModule() { return q_split; }

  QCompPosition getCompilationPosition() { return pos; }
  bool isSure() { return pos >= kBegin; }
  bool isValid() { return pos >= kValid; }
  bool isConditioned() { return pos >= kConditioned; }
  bool isNative() { return pos >= kNative; }
  bool isMapped() { return pos >= kMapped; }
  bool isQSDASplit() { return pos >= kQSDASpilt; }
  bool isClearOfParams() { return pos >= kClearOfParams; }
  bool isAnglesInRange() { return pos >= kAnglesInRange; }
  bool isAllIMM() { return pos >= kAllIMM; }
  bool isMeasMoved() { return pos >= kMeasMoved; }
  bool isSimpleQBBSplit() { return pos >= kSimpleQBBSplit; }
  bool isReadyForModuleSplit() { return pos >= kReadyForModSplit; }
  bool isModSplit() { return pos >= kModSplit; }
  bool isFinalized() { return pos >= kFinalized; }

  /// Get an error string, typically for parsing Platform
  std::string &getErrorString() { return err_str_; };

  void setSure() { pos = kBegin; }
  void setValid() { pos = kValid; }
  void setConditioned() { pos = kConditioned; }
  void setNative() { pos = kNative; }
  void setMapped() { pos = kMapped; }
  void setQSDASplit() { pos = kQSDASpilt; }
  void setClearOfParams() { pos = kClearOfParams; }
  void setAnglesInRange() { pos = kAnglesInRange; }
  void setAllIMM() { pos = kAllIMM; }
  void setMeasMoved() { pos = kMeasMoved; }
  void setSimpleQBBSplit() { pos = kSimpleQBBSplit; }
  void setReadyForModuleSplit() { pos = kReadyForModSplit; }
  void setModSplit() { pos = kModSplit; }
  void setFinalized() { pos = kFinalized; }

  /// The backend may have two methods for handling measurement
  /// return:
  ///     -Queued: an index is assigned to each measurement
  ///      in the QBB on a first-come-first-serve basis. This
  ///      index is passed through the QuISA istruction to
  ///     the control processor as well as passed to the runtime
  ///      call to coordinate the return
  ///      NOTE: any qubit can be measured any number of times in this
  ///      mode.
  ///
  ///    -Matched: like "queued" an index is passed both through the
  ///      QuISA and to the runtime, but in this case the index is matched
  ///      to the qubit ID (making the index redundant in the QuISA)
  ///      NOTE: this mode constrains measurements to be at most once
  ///      per qubit per QBB.

  inline bool isMeasurementQueued() { return is_meas_queued; }
  inline bool isMeasurementMatched() { return !is_meas_queued; }

  /// we will use the QM as a interface for getting Platform info
  /// The following are wrappers for functions in the Machine class
  /// default is -1 if not supported by platform
  int getGateDurationInClockCycles(int, const std::vector<unsigned> &) const;

  int getGateDurationInClockCycles(std::string gate_name,
                                   std::vector<unsigned> qids) const;

  inline unsigned getNumPlatformQubits() { return machine.GetNumQubits(); }
  inline unsigned getNumGlobalProgramQubits() {
    return global_qubit_map->getNumProgQubits();
  }

  /// Checks if a gate is supported on a given list of qids
  inline bool isGateSupported(int id, std::vector<unsigned> &qids) const {
    return machine.GetDuration(getPlatformNameFromGateIdentifier(id), qids) > 0;
  }

  /// checks if a gate exists in the platform configuration
  bool isGateInPlatformConfig(int id) const {
    return machine.IsGateAvailable(getPlatformNameFromGateIdentifier(id));
  }

  bool isGateAvailable(std::string gate_name,
                       std::vector<unsigned> qubits) const {
    return machine.IsGateAvailable(gate_name, qubits);
  }

  bool isGateInPlatformConfig(std::string gate_name) const {
    return machine.IsGateAvailable(gate_name);
  }

  double getDuration(std::string gate_name,
                     std::vector<unsigned int> qubits) const {
    return machine.GetDuration(gate_name, qubits);
  }

  int getHamiltonianPath(std::vector<unsigned> &path, unsigned start) const {
    return machine.GetHamiltonianPath(path, start);
  }

  bool has2DGridWithRowMajorOrder(unsigned num_rows, unsigned num_columns,
                                  std::string gate_name) const {
    return machine.Has2dGridWithRowMajorOrder(num_rows, num_columns, gate_name);
  }

  double getUniqueClockCycleTime() const {
    return machine.GetUniqueClockCycleTime();
  }

  int updateShortestPathToAvailable1QubitGate(
      int id, unsigned q_start, std::vector<unsigned> &qids,
      std::vector<unsigned> excluded = {}) {
    return machine.ShortestPathToAvailable1QubitGate(
        getPlatformNameFromGateIdentifier(id), q_start, qids, excluded);
  }

  int updateShortestPathToAvailable2QubitGate(
      int id, unsigned q_start, unsigned q_goal,
      std::vector<unsigned> &path_from_start,
      std::vector<unsigned> &path_from_goal) {
    return machine.ShortestPathsToAvailable2QubitGate(
        getPlatformNameFromGateIdentifier(id), q_start, q_goal, path_from_start,
        path_from_goal);
  }

  void MachinePrint() { machine.Print(); }

  int findPathWithAStarSearch(unsigned start, unsigned goal,
                              std::vector<unsigned> &path) const {
    return machine.AStarSearch(start, goal, path);
  }

  std::vector<unsigned> &getVecIndices() { return machine.VecIndices(); }

  std::vector<unsigned> &getVecTargets() { return machine.VecTargets(); }

  unsigned getNumVertices() const { return machine.GetNumVertices(); }

  void getVecEdgesConnectedToVertex(const unsigned vertex,
                                    std::vector<unsigned> &connected_edges) {
    machine.VecEdgesConnectedToVertex(vertex, connected_edges);
  }

  /// This is the graph distance between qubits
  unsigned getDistanceBetweenQubits(unsigned q0, unsigned q1) {
    return machine.GetDistanceBetweenQubits(q0, q1);
  }

  void getQubitsConnectedToQubit(const unsigned qubit,
                                 std::vector<unsigned> &connected_qubits) {
    machine.VecQubitsConnectedToQubit(qubit, connected_qubits);
  }

  /// iterators into quantum parts of the base model
  /// basic iterator is over quantum kernels with separate
  /// interators for gates

  using iterator = std::vector<QuantumKernel>::iterator;
  iterator begin() { return q_kernel_list.begin(); }
  iterator end() { return q_kernel_list.end(); }

  /// return a QuantumGates object constructed on this QModule
  QuantumGates gates();

  void removeGate(GateDataMap::iterator);

private:
  std::string err_str_ = "";

  void getPlatformConfigFilename();
  void buildPlatformFromConfigFilename();

  friend class QuantumKernel;
  friend class QuantumGates;
};

struct QuantumModuleProxy {
public:
  QuantumModule *QM;

  QuantumModuleProxy(QuantumModule *ExternalQM) { QM = ExternalQM; }
};

/// Cheap wrapper for quantum kernel as a function, mostly for use of iterating
/// over QBBs
struct QuantumKernel {

private:
  Function *qk_ = nullptr;
  Function::iterator first_;
  bool is_a_qk_ = false;

public:
  QuantumKernel() = delete;
  QuantumKernel(Function &F, bool check = false);
  ~QuantumKernel() = default;

  operator Function &() const { return *qk_; }

  Function *getFunction() const {
    if (is_a_qk_)
      return qk_;
    return nullptr;
  }

  struct iterator {

  private:
    Function::iterator it_;
    Function *F_;

  public:
    iterator() = delete;
    iterator(const Function::iterator &it, Function *F) : it_(it), F_(F) {}
    ~iterator() = default;

    BasicBlock &operator*() { return *it_; }

    iterator operator++();
    iterator operator--();

    friend bool operator!=(const iterator &a, const iterator &b) {
      return a.it_ != b.it_;
    }

    friend bool operator==(const iterator &a, const iterator &b) {
      return a.it_ == b.it_;
    }
  };

  iterator begin() { return iterator(first_, qk_); }
  iterator end() { return iterator(qk_->end(), qk_); }
  iterator begin() const { return iterator(first_, qk_); }
  iterator end() const { return iterator(qk_->end(), qk_); }

  /// The following is functionality to extract the strongly-connected
  /// component(SCC) for QBBs inside the quantum kernel in a topological
  /// sorted order. The SCCs are defined according to the overall
  /// control flow, but the following only contains QBBs.
  class scc_range {
  private:
    std::vector<std::pair<std::vector<BasicBlock *>, bool>> qbb_sccs_;

  public:
    /// we use the base LLVM SCC iterator to construct qbb_sccs_
    /// but it orders the SCCs in reverse topological order
    /// so instead of reversing the order of the vector, we just use
    /// the reverse iterator as the primary iterator
    using iterator = std::vector<
        std::pair<std::vector<BasicBlock *>, bool>>::reverse_iterator;

    scc_range() = delete;
    scc_range(Function *F);
    iterator begin() { return qbb_sccs_.rbegin(); }
    iterator end() { return qbb_sccs_.rend(); }
  };

  scc_range qbb_sccs() { return scc_range(qk_); }

  friend bool operator<(const QuantumKernel &a, const QuantumKernel &b) {
    return a.qk_ < b.qk_;
  }
};

/// Cheap struct for iterating over all gates, both canonical and custom
struct QuantumGates {

private:
  QuantumModule::GateDataMap *q_gate_md_ptr;
  Module *M_ptr;

public:
  QuantumGates() = delete;
  QuantumGates(Module &M, std::map<StringRef, json::Object> *gate_ptr)
      : M_ptr(&M), q_gate_md_ptr(gate_ptr) {}
  QuantumGates(QuantumModule &QM)
      : M_ptr(QM.base), q_gate_md_ptr(&QM.q_gate_metadata){};
  ~QuantumGates() = default;

  struct iterator {
  private:
    QuantumModule::GateDataMap::iterator it_;
    Module *it_M_ptr_;

  public:
    iterator() = delete;
    iterator(const QuantumModule::GateDataMap::iterator it, Module *M)
        : it_(it), it_M_ptr_(M) {}
    ~iterator() = default;

    Function &operator*() { return *(it_M_ptr_->getFunction(it_->first)); }
    operator QuantumModule::GateDataMap::iterator() { return it_; }
    iterator operator++() {
      ++it_;
      return *this;
    }
    iterator operator--() {
      --it_;
      return *this;
    }
    Function *operator->() { return it_M_ptr_->getFunction(it_->first); }
    friend bool operator!=(const iterator &a, const iterator &b) {
      return a.it_ != b.it_;
    }
    friend bool operator==(const iterator &a, const iterator &b) {
      return a.it_ == b.it_;
    }
  };

  iterator begin() { return iterator(q_gate_md_ptr->begin(), M_ptr); }
  iterator end() { return iterator(q_gate_md_ptr->end(), M_ptr); }
};

} // namespace aqcc
} // namespace llvm

#endif
