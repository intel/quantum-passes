//===- ProofReader.cpp - Read in Quantum Circuits -------------------------===//
//
// Copyright (C) 2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//
//===----------------------------------------------------------------------===//
//
// This file implements a module pass to read quantum IR and return a data
// structure
//   holding information about each gate
//
//===----------------------------------------------------------------------===//

#include "IntelQuantumPasses/QuantumCompile/ProofReader.h"
#include "IntelQuantumPasses/QuantumAnalysis/QuantumAnnotationGetters.h"
#include "IntelQuantumPasses/QuantumAnalysis/QuantumGateIdentifiers.h"
#include "IntelQuantumPasses/QuantumAnalysis/QuantumIterator.h"
#include "IntelQuantumPasses/QuantumAnalysis/QuantumModule.h"
#include "IntelQuantumPasses/QuantumAnalysis/QuantumRefs.h"
#include "IntelQuantumPasses/QuantumAnalysis/QuantumSpinNativeToJson.h"
#include "IntelQuantumPasses/QuantumUtils/NaturalSort.hpp"
#include "IntelQuantumPasses/QuantumUtils/QuantumGeneralUtils.h"

#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/InstIterator.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Intrinsics.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/PassManager.h"
#include "llvm/IRReader/IRReader.h"
#include "llvm/InitializePasses.h"
#include "llvm/Pass.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/SourceMgr.h"
#include "llvm/Support/raw_ostream.h"

#include <deque>
#include <fstream>
#include <functional>
#include <iostream>
#include <iterator>
#include <map>
#include <string>
#include <unordered_map>

using namespace llvm;
using namespace aqcc;
using namespace natsort;
namespace proofreader {

void GateToken::setStartFinishQubits() {
  std::vector<std::string> qubits_in_gate;
  for (auto &qname_sym : symbol_map) {
    qubits_in_gate.push_back(qname_sym.first);
  }

  std::sort(qubits_in_gate.begin(), qubits_in_gate.end(), nat_comp);
  start = *qubits_in_gate.begin();
  finish = *(std::prev(qubits_in_gate.end()));
  return;
}

std::string GateToken::getStartQubit() { return start; }

std::string GateToken::getFinishQubit() { return finish; }

ProofReader::ProofReader(BasicBlock &QBB) {
  // Set the source file and qbb name
  std::string source_name_str =
      QBB.getParent()->getParent()->getSourceFileName();
  size_t last_dash_pos = source_name_str.find_last_of("/");
  std::string source_name_substr;
  if (last_dash_pos != std::string::npos)
    source_name_substr = source_name_str.substr(last_dash_pos + 1);
  else
    source_name_substr = source_name_str;

  std::string source_name =
      source_name_substr.substr(0, source_name_substr.find(".", 0));

  std::string qbb_name_with_brakets =
      DemangleQuantumKernelName(QBB.getParent()->getFunction().getName());

  this->source_file_name = source_name;
  this->qbb_demangled_name =
      qbb_name_with_brakets.substr(0, qbb_name_with_brakets.find("(", 0));

  // Read in and process data
  readInQBBData(QBB);
}

std::vector<std::vector<GateToken>> ProofReader::getListOfGates() {
  return this->gates;
}

unsigned ProofReader::getNumQubits() { return this->num_qubit; }

std::vector<std::string> ProofReader::getListOfQubits() { return this->qubits; }

void ProofReader::setSourceFileName(const std::string &source_name) {
  this->source_file_name = source_name.c_str();
}

void ProofReader::setQBBName(const std::string &qbb_name) {
  this->qbb_demangled_name = qbb_name.c_str();
}

void ProofReader::setCompilationPosition(
    const QuantumModule::QCompPosition &compilation_pos) {
  this->position = compilation_pos;
}

void ProofReader::readInQBBData(BasicBlock &QBB) {
  std::unordered_map<unsigned, unsigned>
      parents; // tracks how many parents each gate/node has
               // key is the index of the gate itself;
               // value is the number of parent nodes this gate has
  std::unordered_map<unsigned, std::vector<unsigned>>
      children; // gates that must come after this current gate
  std::unordered_map<std::string, unsigned>
      last_used_in_gate; // tracks the last gate this qubit is used in

  std::vector<GateToken> all_gates;
  unsigned index = 0;

  aqcc::QBBIter qi(QBB); // iterator to step through each QBB

  for (qi.gotoBegin(); !qi.isEnd(); ++qi) {
    parents[index] = 0;
    children[index] = {};

    GateToken gate;
    GateIdentifier gate_id = static_cast<GateIdentifier>(qi.getIdentifier());
    gate.name =
        GetNameFromGateIdentifier(getCanonicalVersionForSpinNative(gate_id));
    bool is_spin_gate =
        (gate_id >= kSpinBegin && gate_id <= kSpinEnd) ||
        (gate_id >= kSpinIMMBegin && gate_id <= kSpinIMMEnd) ||
        (gate_id >= kSpinIMM_IDXBegin && gate_id <= kSpinIMM_IDXEnd);

    // Construct a string representing the rotation angle if
    // the gate is a rotation
    std::vector<ParaRef> params = qi.getParametricOperands();
    // Remove slice id for native gates
    if (is_spin_gate)
      params.pop_back();

    if (gate_id == kMEASX || gate_id == kMEASY || gate_id == kMEASZ ||
        gate_id == kSpinMeasZ)
      params.pop_back();
    // If we have a SwapA with angle of pi, display the gate as a SWAP
    if (gate_id == kSpinSwapalp_IMM || gate_id == kSpinSwapalp) {
      assert(params.size() == 1);
      Value *angle_value = params[0].getValue();
      if (angle_value != nullptr) {
        ConstantFP *FP = dyn_cast<ConstantFP>(angle_value);
        if (FP != nullptr) {
          double angle = (FP->getValueAPF()).convertToDouble();
          if (angle == FP_PI) {
            gate.name = "SWAP";
            params.pop_back();
          }
        }
      }
    }

    // If we have a CPhase with angle of pi, display the gate as a CZ
    if (gate_id == kSpinCPhase_IMM || gate_id == kSpinCPhase) {
      assert(params.size() == 1);
      Value *angle_value = params[0].getValue();
      if (angle_value != nullptr) {
        ConstantFP *FP = dyn_cast<ConstantFP>(angle_value);
        if (FP != nullptr) {
          double angle = (FP->getValueAPF()).convertToDouble();
          if (angle == FP_PI) {
            gate.name = "CZ";
            params.pop_back();
          }
        }
      }
    }

    if (params.size() > 0) {
      gate.parameter += "(";
      for (auto &p : params) {
        if (gate_id >= kSpinIMM_IDXBegin && gate_id <= kSpinIMM_IDXEnd &&
            gate_id != kSpinMeasZ_IMM_IDX) {
          size_t index = std::stoi(getPrettyName(p));
          switch (index) {
          case 0:
            gate.parameter += "0";
            break;
          case 1:
            gate.parameter += std::to_string(FP_PIby2);
            break;
          case 2:
            gate.parameter += std::to_string(FP_PI);
            break;
          case 3:
            gate.parameter += std::to_string(FP_3PIby2);
            break;
          default:
            gate.parameter +=
                "quantum_shared_double_array[" + std::to_string(index) + "]";
            break;
          }
        } else {
          gate.parameter += getPrettyName(p);
        }
        gate.parameter += ", ";
      }
      // drop the last comma and space
      gate.parameter.resize(gate.parameter.size() - 2);
      gate.parameter += ")";
    }

    std::vector<aqcc::QbitRef> wires = qi.getQubitOperands();
    int length = wires.size();

    for (std::vector<aqcc::QbitRef>::iterator it = wires.begin();
         it != wires.end(); ++it) {

      aqcc::QbitRef gate_qubit = *it;

      // Resolve qubit index
      if (!gate_qubit.isResolved()) {
        if (!gate_qubit.updateToSimplified()) {
          std::cout << "Print Circuit Pass can't resolve qubit indices."
                    << std::endl;
          // return false;//change to break and print what is in circuit
          return;
        }
      }
      // try again in the positive because the inner-if may have succeeded
      if (gate_qubit.isResolved()) {
        // qubit_val is the index of the qubit inside the quantum register
        unsigned qubit_val = gate_qubit.getIndexResolved().value();
        // Attempt to use user-defined quantum register name
        std::string qubit_reg_name;
        if (gate_qubit.getRegister())
          qubit_reg_name = gate_qubit.getRegister()->getName();
        else
          // if getRegister() returns a null pointer
          // this means the qubits have been mapped and are not longer
          // associated with user defined qreg default to use "qid" as qubit
          // register name
          qubit_reg_name = "qid";

        std::string qubit_ref =
            qubit_reg_name + "_" + std::to_string(qubit_val);

        // Resolve the symbol used in the gate
        // single qubit gate
        if (length == 1)
          gate.symbol_map[qubit_ref] = gate.name;
        // Two qubit gates
        else if (length == 2) {
          if (it == wires.begin()) {
            if (gate.name == "SWAP" | gate.name == "SwapA")
              gate.symbol_map[qubit_ref] = gate.name;
            else
              gate.symbol_map[qubit_ref] = "o";
          } else {
            std::string target_sym;
            if (gate.name == "CNOT")
              target_sym = "X";
            else if (gate.name == "CZ")
              target_sym = "Z";
            else if (gate.name == "SWAP")
              target_sym = "SWAP";
            else if (gate.name == "SwapA")
              target_sym = "SwapA";
            else if (gate.name == "CPhase")
              target_sym = "Phase";
            else
              target_sym = "Unknown";

            gate.symbol_map[qubit_ref] = target_sym;
          }
        } else if (length == 3) {
          if (it != wires.end() - 1)
            gate.symbol_map[qubit_ref] = "o";
          else
            gate.symbol_map[qubit_ref] = "X";
        } else {
          std::cout << "Undefined gate. \n";
          return;
        }

        // Check to see we have seen this qubit before
        if (last_used_in_gate.count(qubit_ref) != 0) {
          parents[index]++;
          children[last_used_in_gate[qubit_ref]].push_back(index);
        } else {
          children[index] = {};
        }
        last_used_in_gate[qubit_ref] = index;
      } // end of resolving qubit ref
    }   // end of loop over each wire

    gate.setStartFinishQubits();
    all_gates.push_back(gate);
    index++;
  } // end for over qbb iter

  // sort the gates by topological order to print all executable gates
  // at the beginning of the circuit; greedily repeat until reaching the
  // end of the circuit
  sortByTopologicalOrder(parents, children, all_gates);

  // set the number of qubits
  num_qubit = last_used_in_gate.size();
  // Extract the list of qubits used
  setQubitsList(last_used_in_gate);

} // end of readInQBBData

void ProofReader::setQubitsList(
    std::unordered_map<std::string, unsigned> qubits_used) {
  for (const auto &pair : qubits_used) {
    qubits.push_back(pair.first);
  }

  std::sort(qubits.begin(), qubits.end(), nat_comp);
}

void ProofReader::sortByTopologicalOrder(
    std::unordered_map<unsigned, unsigned> &parents,
    std::unordered_map<unsigned, std::vector<unsigned>> &children,
    std::vector<GateToken> &all_gates) {

  std::deque<unsigned> ready_to_print;
  std::deque<unsigned> next_layer;
  std::vector<GateToken> to_print;

  for (auto &pair : parents) {
    if (pair.second == 0)
      ready_to_print.push_back(pair.first);
  }

  while (ready_to_print.size() > 0) {
    unsigned g = ready_to_print.front();

    to_print.push_back(all_gates[g]);
    ready_to_print.pop_front();

    for (auto child : children[g]) {
      parents[child]--;
      if (parents[child] == 0) {
        next_layer.push_back(child);
      }
    }

    if (ready_to_print.size() == 0) {
      // this will sort the trivially commuting gates into columns
      // with non-overlapping control lines, and update
      // the sorted columns into the output data structure
      sortByQubitsInterval(to_print);

      ready_to_print.insert(ready_to_print.begin(),
                            std::make_move_iterator(next_layer.begin()),
                            std::make_move_iterator(next_layer.end()));
      next_layer.clear();
      to_print.clear();
    }
  } // end of while there are gates left to print loop
} // end of sortByTopologicalOrder

void ProofReader::sortByQubitsInterval(std::vector<GateToken> &list_of_gates) {
  // sort the gates by starting qubit in ascending order
  // e.g:
  // gate1 is H on qreg_2, gate2 is CNOT on qreg0 and qreg3
  // after sorting, we have {gate2, gate1}

  std::sort(list_of_gates.begin(), list_of_gates.end(),
            [](const GateToken &lhs, const GateToken &rhs) {
              return nat_comp(lhs.start, rhs.start);
            });

  // temp map to hold the sorted gates
  auto gate_blocks =
      std::map<std::string, std::vector<GateToken>,
               std::function<bool(const std::string &, const std::string &)>>{
          [](const std::string &lhs, const std::string &rhs) {
            return nat_comp(lhs, rhs);
          }};

  for (auto &gate : list_of_gates) {
    std::string start = gate.getStartQubit();
    std::string finish = gate.getFinishQubit();

    // ending qubit index as keys
    if (gate_blocks.size() == 0) {
      gate_blocks[finish].push_back(gate);
    } else {
      // find the element that is strictly less than current starting qubit
      auto iter = gate_blocks.lower_bound(start);

      iter = (iter != gate_blocks.begin()) ? --iter : gate_blocks.end();

      if (iter == gate_blocks.end()) {
        // Add new a column to print
        gate_blocks[finish].push_back(gate);
      } else {
        // push back this gate to the one pointed to by iter
        gate_blocks[iter->first].push_back(gate);
        // update the finish time of iter if it's greater than the current key
        gate_blocks[finish] = iter->second;
        // Same ending qubits are strictly forbidden as this would imply that
        // not all gates trivially commute Should put in a check just to make
        // sure
        gate_blocks.erase(iter->first);
      }
    }
  } // end of for over list_of_gates

  // push the sorted gates into output data structure
  // Not a big fan of this side-effect type behaviour
  // but seems to be a logical thing to do right here
  for (auto &pair : gate_blocks) {
    gates.push_back(pair.second);
  }
} // end of sortByQubitsInterval

} // namespace proofreader
