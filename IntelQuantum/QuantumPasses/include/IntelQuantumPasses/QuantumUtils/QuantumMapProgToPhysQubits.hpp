//===- QuantumMapProgToPhysQubits.hpp - Dynamical qubit placement ---===//
//
// Copyright (C) 2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//
//===----------------------------------------------------------------===//
//
// This file declares the class needed to describe the placement of
// program qubits (from the C++ input files to compile) to physical qubits
// (those described by the platform-config file). The placement may vary during
// the schedule and includes virtual and free qubits:
// - virtual qubit: program qubits not yet associated to a physical one
// - free qubit: physical qubit not associated to any program one
//
//===----------------------------------------------------------------===//

#ifndef LLVM_QUANTUM_MAPPROGTOPHYSQUBITS_H
#define LLVM_QUANTUM_MAPPROGTOPHYSQUBITS_H

#include <climits> // for INT_MAX
#include <iomanip>
#include <map>
#include <set>
#include <unordered_map>
#include <vector>

#include "IntelQuantumPasses/QuantumAnalysis/QuantumGateIdentifiers.h"
#include "IntelQuantumPasses/QuantumAnalysis/QuantumRefs.h"

using namespace llvm;

namespace scheduler {

class MapProgToPhysQubits;
raw_ostream &operator<<(raw_ostream &, MapProgToPhysQubits &);
bool operator==(const MapProgToPhysQubits &, const MapProgToPhysQubits &);
bool operator!=(const MapProgToPhysQubits &, const MapProgToPhysQubits &);

//////////////////////////////////////////////////////////////////////

class MapProgToPhysQubits {

private:
  /// QUBIT-INDEX MAPS:
  /// Program qubits are identified via their QbitRef from the LLVM QIR.
  /// To facilitate the qubit-map, we associate a unique id (unsigned integer)
  /// to every program qubit.
  // A unique id is also associated to every physical qubit, starting from 0
  /// and ending with num_phys_qubits_-1 (the number of physical qubits
  /// needs to be provided).

  std::map<aqcc::QbitRef, std::pair<unsigned, unsigned>>
      from_prog_qbitref_to_range_;
  std::vector<aqcc::QbitRef> from_id_to_prog_qbitref_;
  std::vector<unsigned> prog_to_phys_map_;
  std::vector<unsigned> phys_to_prog_map_;

  // INFO: Care must be taken to have the maps always compatible.

  //// Conventional values:
  /// In the program-to-physical map, it may happen that a program qubit
  /// is not yet associated to any physical qubit. Such qubit is said to
  /// be 'virtual':
  static unsigned get_virtual_value_();

  /// In the program-to-physical map, it may happen that a program qubit
  /// has been added as part of a prog array but was never meant to be
  /// assigned to a physical qubit. Such qubit is said to be 'excluded':
  static unsigned get_excluded_value_();
  /// The excluded value is returned when an unknown program qubit is
  /// queried in the program-to-physical map.

  /// In the physical-to-program map, it may happen that a physical qubit
  /// has no associated program qubit (hardware can be larger than
  /// necessary). Such qubit is considered 'free':
  static unsigned get_free_value_();

public:
  static unsigned GetFreeValue() { return get_free_value_(); }
  static unsigned GetExcludedValue() { return get_excluded_value_(); }
  static unsigned GetVirtualValue() { return get_virtual_value_(); }

  //////////////////////////////////////////////////////////////////////

  class iterator;

  /// Default constructor.
  MapProgToPhysQubits() {}
  /// Copy constructor.
  MapProgToPhysQubits(MapProgToPhysQubits &obj);

  /// Default destructor.
  ~MapProgToPhysQubits() {}

  /// Comparator.
  friend bool operator==(const MapProgToPhysQubits &lhs,
                         const MapProgToPhysQubits &rhs);
  friend bool operator!=(const MapProgToPhysQubits &lhs,
                         const MapProgToPhysQubits &rhs);
  bool IsEqual(const MapProgToPhysQubits &obj) const;

  /// Get number of program / physical qubits.
  unsigned getNumProgQubits() const {
    unsigned prog_qubits = 0;
    for (int i = 0; i < prog_to_phys_map_.size(); i++) {
      if (prog_to_phys_map_[i] != get_excluded_value_())
        prog_qubits++;
    }
    return prog_qubits;
  }
  unsigned getNumVirtualProgQubits() {
    unsigned virtual_qubits = 0;
    for (int i = 0; i < prog_to_phys_map_.size(); i++) {
      if (prog_to_phys_map_[i] == get_virtual_value_())
        virtual_qubits++;
    }
    return virtual_qubits;
  }
  unsigned getNumFreePhysQubits() {
    unsigned free_qubits = 0;
    for (int i = 0; i < phys_to_prog_map_.size(); i++) {
      if (phys_to_prog_map_[i] == get_free_value_())
        free_qubits++;
    }
    return free_qubits;
  }
  unsigned getNumPhysQubits() const { return phys_to_prog_map_.size(); }
  unsigned getNumMappedQubits() const {
    unsigned mapped_qubits = 0;
    for (int i = 0; i < prog_to_phys_map_.size(); i++) {
      if (prog_to_phys_map_[i] != get_virtual_value_() &&
          prog_to_phys_map_[i] != get_excluded_value_())
        mapped_qubits++;
    }
    return mapped_qubits;
  }

  /// Add program qubit using QbitRef.
  bool addProgQubit(const aqcc::QbitRef &qref);
  bool addProgQubitRegister(const aqcc::QbitRef &qref);
  bool switchProgQubitRegisters(const aqcc::QbitRef &from,
                                const aqcc::QbitRef &to);

  // remove program qubits
  void removeProgQubits(const std::pair<unsigned, unsigned> &range);
  void removeProgQubit(unsigned);

  /// Set number of physical qubits.
  void setNumPhysQubits(unsigned num_phys_qubits);

  /// Set maps.
  bool setTrivialProgToPhysMap();
  bool setTrivialProgToPhysMap(signed q_idx, signed start_idx, signed end_idx);
  bool setCustomProgToPhysMap(std::vector<unsigned> &phys_subset);
  bool
  setCustomProgToPhysMap(std::unordered_map<unsigned, unsigned> &custom_map);
  // same as above but does so in no particular order
  bool setCustomProgToPhysMap(std::set<unsigned> &phys_subset);
  bool setCustomProgToPhysMap(std::vector<unsigned> &phys_subset, int start_idx,
                              int end_idx);

  /// Return mapped id values.
  ///
  /// If the program QbitRef is unknown, return excluded_value_.
  unsigned getProgToPhys(const aqcc::QbitRef &qref) {
    unsigned id = getIdFromProgQbitRef(qref);
    if (id == get_excluded_value_())
      return get_excluded_value_();
    return prog_to_phys_map_[id];
  }

  inline bool isAProgQubit(const aqcc::QbitRef &qref) const {
    if (qref.isQID())
      return false;
    return getIdFromProgQbitRef(qref) != get_excluded_value_();
  }

  // Return physical qubit id from its program qubit id.
  unsigned getProgToPhys(unsigned prog_qubit) {
    return prog_to_phys_map_[prog_qubit];
  }
  std::vector<unsigned> getProgToPhys(std::vector<unsigned> &prog_qubits) {
    std::vector<unsigned> phys_qubits;
    for (auto prog_qubit : prog_qubits)
      phys_qubits.push_back(prog_to_phys_map_[prog_qubit]);
    return phys_qubits;
  }

  // Return program qubit id from its physical qubit id.
  unsigned getPhysToProg(unsigned phys_qubit) {
    return phys_to_prog_map_[phys_qubit];
  }
  std::vector<unsigned> getPhysToProg(std::vector<unsigned> &phys_qubits) {
    std::vector<unsigned> prog_qubits;
    for (auto phys_qubit : phys_qubits)
      prog_qubits.push_back(phys_to_prog_map_[phys_qubit]);
    return prog_qubits;
  }

  inline bool isPhysicalQubitFree(unsigned phys_qubit) {
    if (phys_qubit >= phys_to_prog_map_.size())
      return true;
    return phys_to_prog_map_[phys_qubit] == get_free_value_();
  }

  /// Return id of a program qubit from its QbitRef key.
  ///
  /// If the QbitRef was never added, it returns 'exluded_value_'.
  unsigned getIdFromProgQbitRef(const aqcc::QbitRef &qref) const;

  /// Returns an id even if the passed qubit is excluded
  /// Only returns excluded_value if the QbitRef is not found anywhere
  unsigned getAbsoluteIdFromProgQbitRef(const aqcc::QbitRef &qref) const;
  std::map<aqcc::QbitRef, unsigned> getFromProgQbitRefToIdMap() const;
  std::map<aqcc::QbitRef, unsigned> getFromProgQbitRefToPhysMap() const;

  /// Return range of ids of a program qubit register from its QbitRef key.
  ///
  /// If the register was never added, it returns pair(0, 0).
  std::pair<unsigned, unsigned>
  getRangeFromProgQbitRef(const aqcc::QbitRef &qref) const;

  unsigned operator[](const aqcc::QbitRef &qref) { return getProgToPhys(qref); }

  aqcc::QbitRef getQRef(unsigned i);

  /// Return full map, used when copying the object.
  std::map<aqcc::QbitRef, std::pair<unsigned, unsigned>>
  fromProgQbitRefToRange() const {
    return from_prog_qbitref_to_range_;
  }
  std::vector<aqcc::QbitRef> fromIdToProgQbitRef() const {
    return from_id_to_prog_qbitref_;
  }
  std::vector<unsigned> progToPhysMap() const { return prog_to_phys_map_; }
  std::vector<unsigned> physToProgMap() const { return phys_to_prog_map_; }

  /// Return a vector of the mapping for the qubit register (array)
  /// represented by qref.
  ///
  /// Because LLVM uses the type of the vector to infer the creation
  /// of a ConstantDataArray, we template this function so that we can
  /// control the type.
  template <class int_type = unsigned>
  std::vector<int_type> getQbitRegisterToPhysMap(const aqcc::QbitRef &qref);

  template <class int_type = unsigned>
  std::vector<int_type> getQbitRegisterToPhysMap(const iterator &it) {
    return getQbitRegisterToPhysMap<int_type>(*it);
  }

  /// returns a set of physical qubits which have been mapped
  std::set<unsigned> getMappedPhysQubits();

  /// Update map by exchanging the information content of two physical qubits.
  bool updateMapBySwappingPhysQubits(unsigned qubit_0, unsigned qubit_1);
  bool updateMapBySwappingPhysQubits(std::vector<unsigned> qubits) {
    if (qubits.size() == 2)
      return updateMapBySwappingPhysQubits(qubits[0], qubits[1]);
    else
      return false;
  }

  /// Check compatibility of program-to-physical map and its inverse.
  bool isConsistent() const;

  /// Remove all program qubits.
  void removeAllProgQubits();

  /// Clear previous qubit mapping
  void clearMapping();

  /// Manipulations of overall Map

  /// Joins the passed map "other" with "this" map such that all
  /// program qubits in either map is now in "this".
  /// The mapping to physical qubits is also added such that if
  /// a qubit is in both maps, "this" mapping takes precedence
  /// over "other". returns true if there are such mapping
  /// conflicts and false otherwise.
  /// NOTE: the intermediate program qubit id is not preserved
  /// from  "other".
  bool join(const MapProgToPhysQubits &other);

  /// Intersects the passed map "other" with "this" map such that
  /// only program qubits in both maps are now in "this".
  /// The mapping to physical qubits is such that if a qubit is in
  /// both maps, "this" mapping takes precedence over "other".
  /// returns true if there are such mapping conflicts and false
  /// otherwise.
  /// NOTE: Qubits in "this" and not in "other" are not removed,
  /// but their intermediate program qubit id is set to excluded.
  /// NOTE: the intermediate program qubit id is not preserved
  /// from "other".
  bool intersect(const MapProgToPhysQubits &other);

  /// Iterator ranges over all qubit registers, not individual qubits.
  struct iterator {

  private:
    std::map<aqcc::QbitRef, std::pair<unsigned, unsigned>>::iterator it_;

    iterator(const std::map<aqcc::QbitRef,
                            std::pair<unsigned, unsigned>>::iterator &it)
        : it_(it) {}

  public:
    const aqcc::QbitRef &operator*() { return it_->first; }
    const aqcc::QbitRef &operator*() const { return it_->first; }
    auto operator->() const { return &*it_; }
    iterator operator++() {
      ++it_;
      return *this;
    }
    iterator operator--() {
      --it_;
      return *this;
    }

    friend bool operator==(const iterator &a, const iterator &b) {
      return a.it_ == b.it_;
    }
    friend bool operator!=(const iterator &a, const iterator &b) {
      return a.it_ != b.it_;
    }

    friend class MapProgToPhysQubits;
  };

  struct const_iterator {

  private:
    std::map<aqcc::QbitRef, std::pair<unsigned, unsigned>>::const_iterator it_;

    const_iterator(
        const std::map<aqcc::QbitRef,
                       std::pair<unsigned, unsigned>>::const_iterator &it)
        : it_(it) {}

  public:
    const aqcc::QbitRef &operator*() { return it_->first; }
    const aqcc::QbitRef &operator*() const { return it_->first; }
    auto operator->() const { return &*it_; }
    const_iterator operator++() {
      ++it_;
      return *this;
    }
    const_iterator operator--() {
      --it_;
      return *this;
    }

    friend bool operator==(const const_iterator &a, const const_iterator &b) {
      return a.it_ == b.it_;
    }
    friend bool operator!=(const const_iterator &a, const const_iterator &b) {
      return a.it_ != b.it_;
    }

    friend class MapProgToPhysQubits;
  };

  iterator begin() { return iterator(from_prog_qbitref_to_range_.begin()); }
  iterator end() { return iterator(from_prog_qbitref_to_range_.end()); }
  const_iterator begin() const {
    return const_iterator(from_prog_qbitref_to_range_.begin());
  }
  const_iterator end() const {
    return const_iterator(from_prog_qbitref_to_range_.end());
  }

  /// string stream printing overload.
  friend raw_ostream &operator<<(raw_ostream &, MapProgToPhysQubits &);
  friend std::string printWithIdx(MapProgToPhysQubits &);
};

//////////////////////////////////////////////////////////////////////

} // end of namespace scheduler

#endif
