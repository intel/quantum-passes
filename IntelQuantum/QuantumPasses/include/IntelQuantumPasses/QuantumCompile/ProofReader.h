//===- ProofReader.h ----------------------------------------------------====//
//
// Copyright (C) 2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//
//===----------------------------------------------------------------------===//
//
// This file defines a class which reads in a block of IR referenced by a QBB
// and returns a vector of vector of gates used in the QBB
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_QUANTUM_PROOFREADER_H
#define LLVM_QUANTUM_PROOFREADER_H

#include "IntelQuantumPasses/QuantumAnalysis/QuantumAnnotationsToJson.h"
#include "IntelQuantumPasses/QuantumAnalysis/QuantumGateIdentifiers.h"
#include "IntelQuantumPasses/QuantumAnalysis/QuantumIterator.h"
#include "IntelQuantumPasses/QuantumAnalysis/QuantumRefs.h"
#include "llvm/IRReader/IRReader.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/SourceMgr.h"
#include "llvm/Support/raw_ostream.h"

#include <unordered_map>

namespace proofreader {
struct GateToken {
  std::string name;
  std::string start;
  std::string finish;
  std::string parameter = "";
  // symbol_map = {"qubit_name_2": "H", "qubit_name_10": "Z"}
  std::map<std::string, std::string> symbol_map;

  // must invoke setStartFinishQubits() before using the start and finish qubits
  void setStartFinishQubits();
  std::string getStartQubit();
  std::string getFinishQubit();
};

class ProofReader {
public:
  ProofReader(BasicBlock &QBB);
  ProofReader() = delete;
  ~ProofReader() = default;

  // Returns a vector of vector of GateTokens
  // Each vector are ready to be printed in a column
  std::vector<std::vector<GateToken>> getListOfGates();

  /// Returns the number of qubit used in this QBB
  unsigned getNumQubits();

  /// Returns a vector of all qubits used in this circuit
  /// Each string starts with the quantum register name,
  /// followed by a space and the qubit's index
  /// e.g., {"qreg 0", "anc_reg 2"}
  std::vector<std::string> getListOfQubits();

  /// Set the source file name
  void setSourceFileName(const std::string &source_name);

  /// Set the QBB name
  void setQBBName(const std::string &qbb_name);

  /// Set the compilation position ID
  void
  setCompilationPosition(const QuantumModule::QCompPosition &compilation_pos);

  const std::string &getSourceFileName() const { return source_file_name; }

  const std::string &getQBBName() const { return qbb_demangled_name; }

  const QuantumModule::QCompPosition &getCompilationPositionID() const {
    return position;
  }

private:
  int num_qubit;
  std::vector<std::vector<GateToken>> gates;
  std::vector<std::string> qubits;
  std::string source_file_name;
  std::string qbb_demangled_name;
  QuantumModule::QCompPosition position;

  /// Reads in a QBB and processes the gates inside the QBB
  /// Puts the processed gates into the "gates" data structure
  void readInQBBData(BasicBlock &QBB);

  /// Extract the list of qubits used in the circuit
  /// and put it into the "qubits" vector
  void setQubitsList(std::unordered_map<std::string, unsigned> qubits_used);

  /// Sort the gates by a topological order
  void sortByTopologicalOrder(
      std::unordered_map<unsigned, unsigned> &parents,
      std::unordered_map<unsigned, std::vector<unsigned>> &children,
      std::vector<GateToken> &all_gates);

  /// sort the gates by starting qubit in ascending order
  /// starting qubit is defined as the qubit used in the gate and
  /// the qubit with the smallest alphanumeric name
  /// e.g:
  /// gate1 is H on qreg_2, gate2 is CNOT on qreg0 and qreg3
  /// after sorting, we have {gate2, gate1}
  /// Example of the algorithm
  /// gates_sorted:
  /// finish q index 4: {[0,0], [1,4]}
  ///
  /// 1. Trying to insert [3, 7]
  ///    Look for the first finish q index that's smaller than 3,
  ///    since there isn't a smaller one, meaning that we must take up a new
  ///    column
  /// to print [3,7] insert a new entry into gates_sorted with gates_sorted[7] =
  ///{[3, 7]} gates_sorted = { 4: {[0,0], [1,4]} 7: {[3, 7]}
  ///    }
  ///
  /// 2. Trying to insert [5, 6]
  /// Look for first finish time that's smaller than 5
  /// Found 4, we get an iter pointing at 7
  ///    if 7 is begin(), insert a new entry (meaning we cannot print in the
  ///    same
  /// column) else, get the iter pointing to the one before 7
  ///        -> append the interval to the one before 7, update the finish time
  ///        of
  /// the current row
  ///        -> we can print in the same column
  ///    gates_sorted = {
  ///    6: {[0,0], [1,4], [5, 6]}
  ///    7: {[3, 7]}
  ///    }

  void sortByQubitsInterval(std::vector<GateToken> &list_of_gates);

}; // end of ProofReader class

} // namespace proofreader

#endif // end of LLVM_QUANTUM_PROOFREADER_H