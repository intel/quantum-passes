//===- QuantumAnnotationGetters.h -----------------------------------------===//
//
// Copyright (C) 2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//
//===----------------------------------------------------------------------===//
/// \file "QuantumAnnotationGetters.h"
///
/// This file provides some basic functions for easy retrieval of quantum
// annotations data.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_QUANTUM_ANNOTATION_GETTERS_H
#define LLVM_QUANTUM_ANNOTATION_GETTERS_H

#include "IntelQuantumPasses/QuantumAnalysis/QuantumAnnotationsToJson.h"
#include "IntelQuantumPasses/QuantumUtils/QuantumGeneralUtils.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Module.h"
#include "llvm/Support/JSON.h" // JSON package

#include <map>

namespace llvm {

////////////////////////////////////////////////////////////////////////////////
// set of getter functions for easily accessing metadata through the identifier
// Most functions are overloaded to use both the indentifier and the ptr to the
// desired gate data Both are useful in certian cases

/// Gets map iterator to gate data for identifier int
std::map<std::string, json::Object>::iterator getGateDataPtr(int identifier);

/// returns identifier for data pointer
int getGateIdentifier(const std::map<std::string, json::Object>::iterator iter);

/// retruns where identifier represents a gate
bool isAGate(int identifier);

/// Get the llvm::Function pointer corresponding to identifier
Function *getGateFunction(int identifier, Module *M);

/// get the function name corresponding to identifier
StringRef getGateName(int identifier);

// TODO: add once matrix form is fully resolveed
// (matrix) GetGateMatrixReal(int identifier);
// (matrix) Get GateMatrixImag(int identifier);

/// Is gate at map iterator Hermitian?
bool isGateHermitian(std::map<std::string, json::Object>::iterator iter);

/// Is that gate for identifier Hermitian?
bool isGateHermitian(int identifier);

/// Is gate at map iterator Unitary?
bool isGateUnitary(std::map<std::string, json::Object>::iterator iter);

/// Is that gate for identifier Unitary?
bool isGateUnitary(int identifier);

/// Is gate at map iterator Mutable?
bool isGateMutable(std::map<std::string, json::Object>::iterator iter);

/// Is  gate for identifier Mutable?
bool isGateMutable(int identifier);

/// Is gate at map iterator a user defined pulse sequence?
bool isGateUserPulse(std::map<std::string, json::Object>::iterator iter);

/// Is gate for identifier a user defined pulse sequence?
bool isGateUserPulse(int identifier);

/// For map iterator, populate vector<unsigned> with list corresponding to
/// StringRef key and return true, or return false if entry was not found
bool getGateList(std::map<std::string, json::Object>::iterator iter,
                 std::string entry, std::vector<unsigned> &list);

/// For identifier, populate vector<unsigned> with list corresponding to
/// StringRef key and return true, or return false if entry was not found
bool getGateList(int identifier, std::string entry,
                 std::vector<unsigned> &list);

/// For map iterator, populate vector<int> with list corresponding to
/// StringRef key and return true, or return false if entry was not found
bool getGateList(std::map<std::string, json::Object>::iterator iter,
                 std::string entry, std::vector<int> &list);

/// For identifier, populate vector<int> with list corresponding to
/// StringRef key and return true, or return false if entry was not found
bool getGateList(int identifier, std::string entry, std::vector<int> &list);

/// For map iterator, return number of elements in qubit_list
unsigned
getGateNumQubitOperands(std::map<std::string, json::Object>::iterator iter);

/// For identifier, return number of elements in qubit_list
unsigned getGateNumQubitOperands(int identifier);

/// For map iterator, return number of elements in parametric_list
unsigned getGateNumParametricOperands(
    std::map<std::string, json::Object>::iterator iter);

/// For identifier, return number of elements in parametric_list
unsigned getGateNumParametricOperands(int identifier);

} // namespace llvm

#endif // for header guard LLVM_QUANTUM_ANNOTATION_GETTERS_H
