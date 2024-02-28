//===- QuantumMapProgToPhysQubits.cpp - Dynamical qubit placement----===//
//
// Copyright (C) 2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//
//===----------------------------------------------------------------===//
//
// This file implements the class needed to describe the placement of
// program qubits (from the C++ input files to compile) to physical qubits
// (those described by the platform-config file). The placement may vary during
// the schedule and includes virtual and free qubits:
// - virtual qubit: program qubits not yet associated to a physical one
// - free qubit: physical qubit not associated to any program one
//
//===----------------------------------------------------------------===//

#include "IntelQuantumPasses/QuantumUtils/QuantumMapProgToPhysQubits.hpp"

#include <cassert>
#include <optional>

using namespace llvm;

//////////////////////////////////////////////////////////////////////

bool operator==(const scheduler::MapProgToPhysQubits &lhs,
                const scheduler::MapProgToPhysQubits &rhs) {
  return lhs.IsEqual(rhs);
}

bool operator!=(const scheduler::MapProgToPhysQubits &lhs,
                const scheduler::MapProgToPhysQubits &rhs) {
  return !(lhs.IsEqual(rhs));
}

namespace scheduler {

//////////////////////////////////////////////////////////////////////
// MapProgToPhysQubits methods
//////////////////////////////////////////////////////////////////////

unsigned MapProgToPhysQubits::get_virtual_value_() { return UINT_MAX; }
unsigned MapProgToPhysQubits::get_excluded_value_() { return UINT_MAX - 1; }
unsigned MapProgToPhysQubits::get_free_value_() { return UINT_MAX - 2; }

////////////////////////////////////////////////////////////////////////////////

// Utility to compare std maps.
bool UtilityKeyCompare(std::map<aqcc::QbitRef, unsigned> const &lhs,
                       std::map<aqcc::QbitRef, unsigned> const &rhs) {

  auto pred = [](std::pair<const aqcc::QbitRef, unsigned> a,
                 std::pair<const aqcc::QbitRef, unsigned> b) {
    return a.first == b.first;
  };

  return lhs.size() == rhs.size() &&
         std::equal(lhs.begin(), lhs.end(), rhs.begin(), pred);
}

bool MapProgToPhysQubits::IsEqual(const MapProgToPhysQubits &obj) const {

  bool is_equal = true;
  // Equality of std maps.
  is_equal = UtilityKeyCompare(this->getFromProgQbitRefToIdMap(),
                               obj.getFromProgQbitRefToIdMap());
  // Equality of std vectors.
  is_equal = is_equal && (this->prog_to_phys_map_ == obj.progToPhysMap());
  is_equal = is_equal && (this->phys_to_prog_map_ == obj.physToProgMap());
  return is_equal;
}

//////////////////////////////////////////////////////////////////////

MapProgToPhysQubits::MapProgToPhysQubits(MapProgToPhysQubits &obj) {

  from_prog_qbitref_to_range_ = obj.fromProgQbitRefToRange();
  from_id_to_prog_qbitref_ = obj.fromIdToProgQbitRef();
  prog_to_phys_map_ = obj.progToPhysMap();
  phys_to_prog_map_ = obj.physToProgMap();
}

//////////////////////////////////////////////////////////////////////

bool MapProgToPhysQubits::addProgQubitRegister(const aqcc::QbitRef &qref) {

  aqcc::QbitRef reg(qref);
  reg.setIndexResolved(0);
  // Return true if the qubit register was already present.
  if (from_prog_qbitref_to_range_.find(reg) !=
      from_prog_qbitref_to_range_.end()) {
    return true;
  }

  // check for resolved register size
  std::optional<unsigned> size = reg.getRegisterSize();
  if (!size.has_value()) {
    return false;
  }

  // Otherwise update map and return true.
  unsigned start = prog_to_phys_map_.size();
  unsigned end = start + size.value();

  from_prog_qbitref_to_range_[reg] = std::make_pair(start, end);
  prog_to_phys_map_.resize(end, get_excluded_value_());
  for (int i = 0, stop = end - start; i != stop; i++) {
    reg.setIndexResolved(i);
    from_id_to_prog_qbitref_.push_back(reg);
  }
  return true;
}

bool MapProgToPhysQubits::switchProgQubitRegisters(const aqcc::QbitRef &from,
                                                   const aqcc::QbitRef &to) {

  aqcc::QbitRef fm_cpy(from);
  fm_cpy.setIndexResolved(0);
  auto fm_temp = fm_cpy.getRegisterSize();
  if (!fm_temp.has_value())
    return false;
  unsigned fm_sz = fm_temp.value();

  aqcc::QbitRef to_cpy(to);
  to_cpy.setIndexResolved(0);
  auto to_temp = to_cpy.getRegisterSize();
  if (!to_temp.has_value())
    return false;
  unsigned to_sz = fm_temp.value();

  if (fm_sz != to_sz)
    return false;

  auto iter = from_prog_qbitref_to_range_.find(fm_cpy);
  if (iter == from_prog_qbitref_to_range_.end())
    return false;

  from_prog_qbitref_to_range_[to_cpy] = iter->second;

  for (unsigned i = (iter->second).first, end = (iter->second).second; i != end;
       i++) {
    to_cpy.setIndexResolved(i);
    from_id_to_prog_qbitref_[i] = to_cpy;
  }

  from_prog_qbitref_to_range_.erase(iter);
  return true;
}

//////////////////////////////////////////////////////////////////////

bool MapProgToPhysQubits::addProgQubit(const aqcc::QbitRef &qref) {

  addProgQubitRegister(qref);

  unsigned idx = getAbsoluteIdFromProgQbitRef(qref);
  if (idx == get_excluded_value_())
    return false;

  if (prog_to_phys_map_[idx] != get_excluded_value_()) {
    // QbitRef was already present in the map
    return false;
  }

  prog_to_phys_map_[idx] = get_virtual_value_();

  return true;
}

//////////////////////////////////////////////////////////////////////

void MapProgToPhysQubits::setNumPhysQubits(unsigned num_phys_qubits) {
  if (num_phys_qubits > phys_to_prog_map_.size())
    phys_to_prog_map_.resize(num_phys_qubits, get_free_value_());
}

//////////////////////////////////////////////////////////////////////

bool MapProgToPhysQubits::setTrivialProgToPhysMap() {

  unsigned num_virtual = getNumVirtualProgQubits();
  unsigned num_free = getNumFreePhysQubits();

  // Verify that there are at least as many physical qubits free
  // as program qubits to be mapped.
  if (num_free < num_virtual)
    return false;

  if (num_free == 0) // nothing to map
    return true;

  // get first free qid value
  unsigned qid = 0;
  unsigned prog_id = 0;

  while (num_virtual > 0) {
    // get next free value
    while (phys_to_prog_map_[qid] != get_free_value_())
      qid++;

    // get next virtual value
    while (prog_to_phys_map_[prog_id] != get_virtual_value_())
      prog_id++;

    prog_to_phys_map_[prog_id] = qid;
    phys_to_prog_map_[qid] = prog_id;
    num_virtual--;
  }

  return true;
}

//////////////////////////////////////////////////////////////////////

bool MapProgToPhysQubits::setTrivialProgToPhysMap(signed q_idx,
                                                  signed start_idx,
                                                  signed end_idx) {

  // Verify that there are at least as many physical qubits than program qubits.
  if (prog_to_phys_map_.size() > phys_to_prog_map_.size())
    return false;

  // sanity check
  if (end_idx < start_idx)
    return false;

  // check for out of bound condition
  if (end_idx > phys_to_prog_map_.size())
    return false;

  unsigned q_prog = q_idx;
  for (unsigned idx = start_idx; idx < end_idx; ++idx) {
    if (prog_to_phys_map_[idx] == get_virtual_value_()) {
      prog_to_phys_map_[idx] = q_prog;
      phys_to_prog_map_[q_prog] = idx;
      q_prog++;
    }
  }
  return true;
}

//////////////////////////////////////////////////////////////////////

bool MapProgToPhysQubits::setCustomProgToPhysMap(
    std::unordered_map<unsigned, unsigned> &custom_map) {
  // Verify that there are at least as many physical qubits
  // than program qubits.
  if (prog_to_phys_map_.size() > phys_to_prog_map_.size())
    return false;

  for (auto &pair : custom_map) {
    if (prog_to_phys_map_[pair.first] == get_virtual_value_()) {
      prog_to_phys_map_[pair.first] = pair.second;
      phys_to_prog_map_[pair.second] = pair.first;
    }
  }

  return true;
}

//////////////////////////////////////////////////////////////////////

bool MapProgToPhysQubits::setCustomProgToPhysMap(
    std::vector<unsigned> &phys_subset) {

  // get elements of phys_subset which are free
  std::vector<unsigned> to_map;
  for (unsigned qid : phys_subset) {
    if (qid < phys_to_prog_map_.size())
      if (phys_to_prog_map_[qid] == get_free_value_())
        to_map.push_back(qid);
  }

  unsigned prog_id = 0;
  for (unsigned qid : to_map) {

    // get next virtual value
    while (prog_id < prog_to_phys_map_.size()) {
      if (prog_to_phys_map_[prog_id] == get_virtual_value_())
        break;
      prog_id++;
    }

    if (prog_id == prog_to_phys_map_.size())
      break;

    prog_to_phys_map_[prog_id] = qid;
    phys_to_prog_map_[qid] = prog_id;
  }

  return true;
}

bool MapProgToPhysQubits::setCustomProgToPhysMap(
    std::set<unsigned> &phys_subset) {

  // get elements of phys_subset which are free
  std::vector<unsigned> to_map;
  for (unsigned qid : phys_subset) {
    if (qid < phys_to_prog_map_.size())
      if (phys_to_prog_map_[qid] == get_free_value_())
        to_map.push_back(qid);
  }

  unsigned prog_id = 0;
  for (unsigned qid : to_map) {

    // get next virtual value
    while (prog_id < prog_to_phys_map_.size()) {
      if (prog_to_phys_map_[prog_id] == get_virtual_value_())
        break;
      prog_id++;
    }

    if (prog_id == prog_to_phys_map_.size())
      break;

    prog_to_phys_map_[prog_id] = qid;
    phys_to_prog_map_[qid] = prog_id;
  }

  return true;
}

//////////////////////////////////////////////////////////////////////

bool MapProgToPhysQubits::setCustomProgToPhysMap(
    std::vector<unsigned> &phys_subset, int start_idx, int end_idx) {

  // sanity check
  if (start_idx > end_idx)
    return false;

  // Check for mismatch between the number of prog qubits to be assigned a
  // mapping and the number of mapping provided
  if (phys_subset.size() != (end_idx - start_idx))
    return false;

  int pos = 0;
  for (unsigned idx = start_idx; idx < end_idx; ++idx) {
    if (phys_subset[pos] != get_virtual_value_()) {
      prog_to_phys_map_[idx] = phys_subset[pos];
      phys_to_prog_map_[phys_subset[pos]] = idx;
      pos++;
      if (pos >= phys_subset.size())
        return true;
    }
  }
  return true;
}

//////////////////////////////////////////////////////////////////////

unsigned MapProgToPhysQubits::getAbsoluteIdFromProgQbitRef(
    const aqcc::QbitRef &qref) const {

  aqcc::QbitRef qref_loc(qref);
  auto pair = getRangeFromProgQbitRef(qref_loc);
  if (pair.first == 0 && pair.second == 0)
    return get_excluded_value_();
  assert(qref_loc.isResolved());
  unsigned idx = qref_loc.getIndexResolved().value();
  if (idx < (pair.second - pair.first)) {
    return pair.first + idx;
  }
  return get_excluded_value_();
}

unsigned
MapProgToPhysQubits::getIdFromProgQbitRef(const aqcc::QbitRef &qref) const {
  unsigned ab_id = getAbsoluteIdFromProgQbitRef(qref);
  if (ab_id != get_excluded_value_()) {
    if (prog_to_phys_map_[ab_id] == get_excluded_value_())
      return get_excluded_value_();
  }
  return ab_id;
}

//////////////////////////////////////////////////////////////////////

std::pair<unsigned, unsigned>
MapProgToPhysQubits::getRangeFromProgQbitRef(const aqcc::QbitRef &qref) const {

  aqcc::QbitRef reg(qref);
  reg.setIndexResolved(0);
  auto iter = from_prog_qbitref_to_range_.find(reg);

  if (iter == from_prog_qbitref_to_range_.end()) {
    std::pair<unsigned, unsigned> not_present(0, 0);
    return not_present;
  } else
    return (*iter).second;
}

//////////////////////////////////////////////////////////////////////

aqcc::QbitRef MapProgToPhysQubits::getQRef(unsigned i) {
  if (i < from_id_to_prog_qbitref_.size())
    return from_id_to_prog_qbitref_[i];
  return aqcc::QbitRef();
}

//////////////////////////////////////////////////////////////////////

std::map<aqcc::QbitRef, unsigned>
MapProgToPhysQubits::getFromProgQbitRefToIdMap() const {

  std::map<aqcc::QbitRef, unsigned> out;
  for (auto &pair : from_prog_qbitref_to_range_) {
    aqcc::QbitRef temp(pair.first);
    for (int i = 0, range = pair.second.second - pair.second.first; i != range;
         i++) {
      temp.setIndexResolved(i);
      out[temp] = i + pair.second.first;
    }
  }
  return out;
}

std::map<aqcc::QbitRef, unsigned>
MapProgToPhysQubits::getFromProgQbitRefToPhysMap() const {

  std::map<aqcc::QbitRef, unsigned> out;

  for (size_t i = 0; i < from_id_to_prog_qbitref_.size(); i++) {
    unsigned qid = prog_to_phys_map_[i];
    if (qid != get_excluded_value_())
      out[from_id_to_prog_qbitref_[i]] = qid;
  }

  return out;
}

//////////////////////////////////////////////////////////////////////

template <class int_type>
std::vector<int_type>
MapProgToPhysQubits::getQbitRegisterToPhysMap(const aqcc::QbitRef &qref) {

  std::vector<int_type> out;

  auto pair = getRangeFromProgQbitRef(qref);
  for (unsigned int i = pair.first; i < pair.second; i++) {
    out.push_back((int_type)getProgToPhys(i));
  }

  return out;
}

template std::vector<unsigned>
MapProgToPhysQubits::getQbitRegisterToPhysMap<unsigned>(const aqcc::QbitRef &);
template std::vector<uint16_t>
MapProgToPhysQubits::getQbitRegisterToPhysMap<uint16_t>(const aqcc::QbitRef &);
template std::vector<uint64_t>
MapProgToPhysQubits::getQbitRegisterToPhysMap<uint64_t>(const aqcc::QbitRef &);

template std::vector<unsigned>
MapProgToPhysQubits::getQbitRegisterToPhysMap<unsigned>(const iterator &);
template std::vector<uint16_t>
MapProgToPhysQubits::getQbitRegisterToPhysMap<uint16_t>(const iterator &);
template std::vector<uint64_t>
MapProgToPhysQubits::getQbitRegisterToPhysMap<uint64_t>(const iterator &);

std::set<unsigned> MapProgToPhysQubits::getMappedPhysQubits() {
  std::set<unsigned> out;
  for (unsigned i = 0; i < phys_to_prog_map_.size(); i++) {
    if (phys_to_prog_map_[i] != get_free_value_())
      out.insert(i);
  }
  return out;
}

//////////////////////////////////////////////////////////////////////

bool MapProgToPhysQubits::updateMapBySwappingPhysQubits(unsigned qubit_0,
                                                        unsigned qubit_1) {
  assert(qubit_0 != qubit_1);
  assert(qubit_0 < phys_to_prog_map_.size() &&
         qubit_1 < phys_to_prog_map_.size());
  // Update prog_to_phys_map_
  unsigned temp = phys_to_prog_map_[qubit_0];
  if (temp != get_free_value_())
    prog_to_phys_map_[temp] = qubit_1;
  temp = phys_to_prog_map_[qubit_1];
  if (temp != get_free_value_())
    prog_to_phys_map_[temp] = qubit_0;
  // Update phys_to_prog_map_
  phys_to_prog_map_[qubit_1] = phys_to_prog_map_[qubit_0];
  phys_to_prog_map_[qubit_0] = temp;
  return true;
}

//////////////////////////////////////////////////////////////////////

bool MapProgToPhysQubits::isConsistent() const {

  // Verify that there are at least as many physical qubits
  // than program qubits.
  unsigned num_prog_qubits = prog_to_phys_map_.size();
  unsigned num_phys_qubits = phys_to_prog_map_.size();

  unsigned q_prog, q_phys;
  // Whenever prog is not virtual, check that:
  // if prog-->phys then phys-->prog
  for (q_prog = 0; q_prog < num_prog_qubits; ++q_prog) {
    q_phys = prog_to_phys_map_[q_prog];
    if (q_phys == get_virtual_value_() || q_phys == get_excluded_value_())
      continue;
    else if (q_phys >= num_phys_qubits)
      return false;

    if (phys_to_prog_map_[q_phys] != q_prog) {
      return false;
    }
  }
  // Whenever phys is not free, check that:
  // if phys->prog then prog-->phys
  for (q_phys = 0; q_phys < num_phys_qubits; ++q_phys) {
    q_prog = phys_to_prog_map_[q_phys];
    if (q_prog == MapProgToPhysQubits::get_free_value_())
      continue;
    else if (q_prog >= num_prog_qubits)
      return false;

    if (prog_to_phys_map_[q_prog] != q_phys) {
      return false;
    }
  }
  return true;
}

//////////////////////////////////////////////////////////////////////

void MapProgToPhysQubits::removeProgQubits(
    const std::pair<unsigned, unsigned> &range) {
  for (unsigned i = range.first, stop = range.second; i != stop; i++)
    removeProgQubit(i);
}

void MapProgToPhysQubits::removeProgQubit(unsigned prog_id) {
  if (prog_id >= prog_to_phys_map_.size())
    return;
  unsigned qid = prog_to_phys_map_[prog_id];
  if (qid != get_excluded_value_()) {
    prog_to_phys_map_[prog_id] = get_excluded_value_();
    if (qid != get_virtual_value_())
      phys_to_prog_map_[qid] = get_free_value_();
  }
}

void MapProgToPhysQubits::removeAllProgQubits() {

  from_prog_qbitref_to_range_.clear();
  from_id_to_prog_qbitref_.clear();
  prog_to_phys_map_.clear();
  phys_to_prog_map_.assign(phys_to_prog_map_.size(), get_free_value_());
}

//////////////////////////////////////////////////////////////////////

void MapProgToPhysQubits::clearMapping() {
  prog_to_phys_map_.assign(prog_to_phys_map_.size(), get_virtual_value_());
  phys_to_prog_map_.assign(phys_to_prog_map_.size(), get_free_value_());
}

//////////////////////////////////////////////////////////////////////

bool MapProgToPhysQubits::join(const MapProgToPhysQubits &other) {

  bool out = false;

  // make sure they have the same physical qubit number
  setNumPhysQubits(other.getNumPhysQubits());

  // iterate through all qubit registers in other and try to add
  // the register to this
  for (auto range_it = other.begin(), end = other.end(); range_it != end;
       ++range_it) {

    unsigned other_pos = (range_it->second).first;
    unsigned pos_end = (range_it->second).second;

    if (addProgQubitRegister(range_it->first)) { // is already in this
      // compare physical mapping to determine if we return true
      unsigned pos = from_prog_qbitref_to_range_[range_it->first].first;
      while (other_pos != pos_end) {
        unsigned qid = prog_to_phys_map_[pos];
        unsigned other_qid = other.prog_to_phys_map_[other_pos];
        if (qid != get_excluded_value_()) {
          if (other_qid != get_excluded_value_())
            out = out ||
                  other.prog_to_phys_map_[other_pos] != prog_to_phys_map_[pos];
        } else {
          if (other_qid != get_excluded_value_()) {
            prog_to_phys_map_[pos] = other_qid;
            if (other_qid != get_virtual_value_())
              phys_to_prog_map_[other_qid] = pos;
          }
        }
        pos++;
        other_pos++;
      }
    } else { // not in this
      // set the qubit mapping of this to other
      unsigned pos = from_prog_qbitref_to_range_[range_it->first].first;
      while (other_pos != pos_end) {
        unsigned other_qid = other.prog_to_phys_map_[other_pos];
        if (other_qid != get_excluded_value_()) {
          prog_to_phys_map_[pos] = other_qid;
          if (other_qid != get_virtual_value_())
            phys_to_prog_map_[other_qid] = pos;
        }
        pos++;
        other_pos++;
      }
    }
  }
  return out;
}

bool MapProgToPhysQubits::intersect(const MapProgToPhysQubits &other) {

  bool out = false;

  // make sure they have the same physical qubit number
  setNumPhysQubits(other.getNumPhysQubits());

  auto this_it = begin();
  auto this_end = end();
  auto other_it = other.begin();
  auto other_end = other.end();

  while (this_it != this_end) {
    if (other_it == other_end) {
      // set all prog qubits for this_it to excluded
      removeProgQubits(this_it->second);
      ++this_it;
    } else if ((this_it->first) < (other_it->first)) {
      // set all prog qubits for this_it to excluded
      removeProgQubits(this_it->second);
      ++this_it;
    } else if ((other_it->first) < (this_it->first)) {
      // nothing to do but increment other_it
      ++other_it;
    } else {
      aqcc::QbitRef qubit = other_it->first;
      unsigned other_pos = (other_it->second).first;
      unsigned pos_end = (other_it->second).second;
      unsigned pos = (this_it->second).first;

      while (other_pos != pos_end) {
        if (isAProgQubit(qubit) && other.isAProgQubit(qubit)) { // is in both
          // compare physical mapping to determine if we return true
          out = out ||
                other.prog_to_phys_map_[other_pos] != prog_to_phys_map_[pos];
        } else
          removeProgQubit(pos);
        other_pos++;
        pos++;
        qubit.shiftIndexByResolved(1);
      }
      ++this_it;
      ++other_it;
    }
  }
  return out;
}

//////////////////////////////////////////////////////////////////////
// << overload

raw_ostream &operator<<(raw_ostream &output,
                        MapProgToPhysQubits &map_prog_to_phys_qubits) {

  unsigned num_prog_qubits = map_prog_to_phys_qubits.getNumProgQubits();
  for (unsigned q_prog = 0; q_prog < num_prog_qubits; ++q_prog) {
    aqcc::QbitRef qref_prog =
        map_prog_to_phys_qubits.fromIdToProgQbitRef()[q_prog];
    unsigned q_phys = map_prog_to_phys_qubits.progToPhysMap()[q_prog];
    if (q_phys == MapProgToPhysQubits::get_virtual_value_())
      output << qref_prog << " is virtual";
    else if (q_phys == MapProgToPhysQubits::get_virtual_value_())
      output << qref_prog << " is unmapped";
    else
      output << qref_prog << " --> phys qubit " << q_phys;
    output << "\n";
  }
  return output;
}

std::string printWithIdx(MapProgToPhysQubits &map_prog_to_phys_qubits) {
  std::string out;
  unsigned num_prog_total = map_prog_to_phys_qubits.prog_to_phys_map_.size();
  for (unsigned q_prog = 0; q_prog < num_prog_total; ++q_prog) {
    aqcc::QbitRef qref_prog =
        map_prog_to_phys_qubits.fromIdToProgQbitRef()[q_prog];
    if (map_prog_to_phys_qubits.prog_to_phys_map_[q_prog] !=
        MapProgToPhysQubits::get_excluded_value_())
      out += getPrettyName(qref_prog) + " --> " + std::to_string(q_prog) + "\n";
  }
  return out;
}

//////////////////////////////////////////////////////////////////////

} // end of namespace scheduler
