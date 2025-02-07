//===- QuantumAnnotationGetters.cpp ---------------------------------------===//
//
// Copyright (C) 2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//
//===----------------------------------------------------------------------===//

#include "IntelQuantumPasses/QuantumAnalysis/QuantumAnnotationGetters.h"

#include <optional>

namespace llvm {

////////////////////////////////////////////////////////////////////////////////

std::map<std::string, json::Object>::iterator getGateDataPtr(int identifier) {

  std::map<std::string, json::Object> *qgatemetadata =
      &(QBBIter::getQuantumModule()->q_gate_metadata);
  std::map<std::string, json::Object>::iterator out = qgatemetadata->begin();

  bool found = false;

  while (!found && (out != qgatemetadata->end())) {

    if (json::Object *O = &(out->second)) {
      if (std::optional<long int> gate_id = O->getInteger("identifier")) {
        found = (gate_id.value() == identifier);
      }
    }
    if (!found)
      ++out;
  }

  return out;
}

////////////////////////////////////////////////////////////////////////////////
int getGateIdentifier(std::map<std::string, json::Object>::iterator iter) {
  if (json::Object *O = &(iter->second)) {
    if (std::optional<long int> gate_id = O->getInteger("identifier")) {
      return gate_id.value();
    }
  }
  return 0;
}

////////////////////////////////////////////////////////////////////////////////
bool isAGate(int identifier) {
  std::map<std::string, json::Object> *qgatemetadata =
      &(QBBIter::getQuantumModule()->q_gate_metadata);
  return (getGateDataPtr(identifier) != qgatemetadata->end());
}

////////////////////////////////////////////////////////////////////////////////

Function *getGateFunction(int identifier, Module *M) {
  StringRef gate_name = getGateDataPtr(identifier)->first;
  return M->getFunction(gate_name);
}

////////////////////////////////////////////////////////////////////////////////
StringRef getGateName(int identifier) {
  return getGateDataPtr(identifier)->first;
}

////////////////////////////////////////////////////////////////////////////////
bool isGateHermitian(std::map<std::string, json::Object>::iterator iter) {

  if (json::Object *O = &(iter->second)) {

    if (std::optional<bool> herm = O->getBoolean("is_hermitian")) {
      return herm.value();
    }
  }
  return false;
}

////////////////////////////////////////////////////////////////////////////////
bool isGateHermitian(int identifier) {
  return isGateHermitian(getGateDataPtr(identifier));
}

////////////////////////////////////////////////////////////////////////////////
bool isGateUnitary(std::map<std::string, json::Object>::iterator iter) {

  if (json::Object *O = &(iter->second)) {

    if (std::optional<bool> result = O->getBoolean("is_unitary")) {
      return result.value();
    }
  }
  return false;
}

////////////////////////////////////////////////////////////////////////////////
bool isGateUnitary(int identifier) {
  return isGateUnitary(getGateDataPtr(identifier));
}

////////////////////////////////////////////////////////////////////////////////
bool isGateMutable(std::map<std::string, json::Object>::iterator iter) {

  if (json::Object *O = &(iter->second)) {

    if (std::optional<bool> result = O->getBoolean("is_mutable")) {
      return result.value();
    }
  }
  return false;
}

/////////////////////////////////////////////////////////////////////////////////
bool isGateMutable(int identifier) {
  return isGateMutable(getGateDataPtr(identifier));
}

////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
bool isGateUserPulse(std::map<std::string, json::Object>::iterator iter) {

  if (json::Object *O = &(iter->second)) {

    if (std::optional<bool> result = O->getBoolean("is_user_pulse")) {
      return result.value();
    }
  }
  return false;
}

////////////////////////////////////////////////////////////////////////////////
bool isGateUserPulse(int identifier) {
  return isGateUserPulse(getGateDataPtr(identifier));
}

////////////////////////////////////////////////////////////////////////////////
bool getGateList(std::map<std::string, json::Object>::iterator iter,
                 std::string entry, std::vector<unsigned> &list) {

  std::optional<long int> val;

  if (json::Object *O = &(iter->second)) {

    if (json::Array *arr = O->getArray(entry)) {

      for (auto iter = arr->begin(); iter != arr->end(); ++iter) {

        val = iter->getAsInteger();
        list.push_back(val.value());
      };
    } else
      return false;
  } else
    return false;

  return true;
}

////////////////////////////////////////////////////////////////////////////////
bool getGateList(int identifier, std::string entry,
                 std::vector<unsigned> &list) {
  return getGateList(getGateDataPtr(identifier), entry, list);
}

////////////////////////////////////////////////////////////////////////////////
bool getGateList(std::map<std::string, json::Object>::iterator iter,
                 std::string entry, std::vector<int> &list) {

  std::optional<long int> val;

  if (json::Object *O = &(iter->second)) {

    if (json::Array *arr = O->getArray(entry)) {

      for (auto iter = arr->begin(); iter != arr->end(); ++iter) {

        val = iter->getAsInteger();
        list.push_back(val.value());
      };
    } else
      return false;
  } else
    return false;

  return true;
}

////////////////////////////////////////////////////////////////////////////////
//
bool getGateList(int identifier, std::string entry, std::vector<int> &list) {
  return getGateList(getGateDataPtr(identifier), entry, list);
}

////////////////////////////////////////////////////////////////////////////////

unsigned
getGateNumQubitOperands(std::map<std::string, json::Object>::iterator iter) {

  if (json::Object *O = &(iter->second)) {

    if (json::Array *arr = O->getArray("qubit_list")) {
      return arr->size();
    }
  }
  return 0;
}

////////////////////////////////////////////////////////////////////////////////
unsigned getGateNumQubitOperands(int identifier) {
  return getGateNumQubitOperands(getGateDataPtr(identifier));
}

////////////////////////////////////////////////////////////////////////////////
unsigned getGateNumParametricOperands(
    std::map<std::string, json::Object>::iterator iter) {

  if (json::Object *O = &(iter->second)) {

    if (json::Array *arr = O->getArray("parametric_list")) {
      return arr->size();
    }
  }
  return 0;
}

////////////////////////////////////////////////////////////////////////////////
unsigned getGateNumParametricOperands(int identifier) {
  return getGateNumParametricOperands(getGateDataPtr(identifier));
}

////////////////////////////////////////////////////////////////////////////////

} // namespace llvm
