//===--------- PrinterLaTex.cpp - Read in Quantum Circuits ----------------===//
//
// Copyright (C) 2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//
//===----------------------------------------------------------------------===//
//
// This file implements a class that prints the input circuit out as LaTex
// instructions to a file
//
//===----------------------------------------------------------------------===//

#include "IntelQuantumPasses/QuantumCompile/PrinterLaTex.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/Debug.h"

#include <fstream>
#include <iostream>
#include <map>
#define DEBUG_TYPE "latexInstructions"
extern cl::opt<std::string> comp_pos;
extern cl::list<std::string> quantum_debug;

PrinterLaTex::PrinterLaTex(proofreader::ProofReader *read) {
  this->reader = read;
}

void PrinterLaTex::insertColumnSymbol(std::vector<std::string> &str) {
  for (int i = 0; i < str.size(); i++) {
    str[i] += "&";
  }
}

void PrinterLaTex::insertSpace(std::vector<std::string> &str) {
  for (int i = 0; i < str.size(); i++) {
    str[i] += " ";
  }
}

void PrinterLaTex::insertQubitNames(std::vector<std::string> &str_vec,
                                    std::map<std::string, int> &qubit_lut) {
  // Insert the qubit register name and index on each row
  for (auto &q : qubit_lut) {
    std::string name = "";
    for (auto c : q.first) {
      if (c == '_')
        // Underscores must be escaped in LaTex
        // otherwise it will result in an invalid LaTex instruction
        name += "\\_";
      else
        name += c;
    }
    str_vec.at(q.second) += "\\lstick{\\ket{" + name + "}} ";
  }
}

void PrinterLaTex::writeToFile(std::vector<std::string> &str) {
  std::string source_name = reader->getSourceFileName();
  std::string qbb_name = reader->getQBBName();
  std::string time_str = this->getCurrentTimeStr();
  std::string file_name = this->getOutputDirectory() + source_name + "-" +
                          qbb_name + "-" + comp_pos + "-" + time_str + ".tex";

  // Write to output file
  std::fstream file;
  file.open(file_name, std::ios_base::out);
  if (!file.is_open()) {
    std::cout << "Unable to open file <" << file_name << ">. Exiting... \n";
    return;
  }

  std::string docclass = "\\documentclass{amsart} \n";
  std::string package_call = "\\usepackage[braket]{qcircuit} \n\\vfuzz2pt \n";
  std::string begin_doc = "\\begin{document} \n \\[ \n";
  std::string setting_spacing = "\\Qcircuit @C=1em @R=.7em @!R { \n";
  std::string end_braket = "\n} \n\\] \n\\end{document} \n";

  // Add the title rows
  file << docclass;
  file << package_call;
  file << begin_doc;
  file << setting_spacing;

  // write the instructions
  for (auto &row : str) {
    file << row;
  }

  // add the ending rows
  file << end_braket;
  file.close();
  std::cout << "Wrote LaTex file to <" << file_name << ">. \n \n";
}

void PrinterLaTex::render() {
  std::vector<std::string> qubits = reader->getListOfQubits();

  std::map<std::string, int> qubit_lut;
  for (int i = 0; i < qubits.size(); i++) {
    qubit_lut[qubits.at(i)] = i;
  }

  // to_tex holds the LaTex instructions to be printed
  unsigned num_qubits = reader->getNumQubits();
  std::vector<std::string> to_tex(num_qubits, "");

  // construct the qubit name column
  PrinterLaTex::insertQubitNames(to_tex, qubit_lut);

  // construct LaTex instructions for each column of gates
  for (auto &gates : reader->getListOfGates()) {
    // construct each column of gates
    std::vector<std::string> current_column(num_qubits, "");
    insertColumnSymbol(current_column);
    insertSpace(current_column);

    for (auto &gate : gates) {
      std::vector<std::string> qubit_names;
      // Single qubit gates
      if (gate.symbol_map.size() == 1) {
        int index = qubit_lut[gate.symbol_map.begin()->first];

        if (gate.name == "X" || gate.name == "Z" || gate.name == "Y" ||
            gate.name == "H" || gate.name == "T" || gate.name == "S") {
          current_column[index] += "\\gate{" + gate.name + "}";
        } else if (gate.name == "Sdag") {
          current_column[index] += "\\gate{S^\\dag}";
        } else if (gate.name == "Tdag") {
          current_column[index] += "\\gate{T^\\dag}";
        } else if (gate.name.rfind("Meas", 0) == 0) {
          // else if (gate.name.starts_with("Meas")) {
          current_column[index] += "\\measure{\\mbox{";
          current_column[index] += &gate.name.back();
          current_column[index] += "}}";
        } else if (gate.name.rfind("Prep", 0) == 0) {
          // else if (gate.name.starts_with("Prep")) {
          // TODO: Decide if we want to print the gates out
          current_column[index] += "\\gate{" + gate.name + "}";
          // if (gate.name[-1] == 'X')
          //     // H prepz H; all other qubits should have a wire
          //     continue;
          // else if (gate.name[-1] == 'Z')
          //     // MeasZ, conditional X correction
          //     continue;
          // else if (gate.name[-1] == 'Y')
          //     // Y prepZ Y
          //     continue;
        }
        // Rotations
        else if (gate.name.rfind("R", 0) == 0) {
          // current_column[index] += "\\gate{" + gate.name + "}";
          current_column[index] += "\\gate{" + gate.name + gate.parameter + "}";
        } else {
          std::cout << gate.name
                    << " : Unknown single qubit gate. Exiting...\n";
          return;
        }
      }
      // Controlled two qubit gates and swap
      else if (gate.symbol_map.size() == 2) {
        if (gate.name == "SWAP") {
          std::vector<int> idx;
          for (auto qubit : gate.symbol_map) {
            idx.push_back(qubit_lut[qubit.first]);
          }
          current_column.at(idx[0]) += "\\qswap";
          current_column.at(idx[1]) += "\\qswap";
          current_column.at(idx[1]) +=
              " \\qwx[-" + std::to_string(idx[1] - idx[0]) + "]";
        } else if (gate.name == "SwapA") {
          std::vector<int> idx;
          for (auto &qubit : gate.symbol_map) {
            idx.push_back(qubit_lut[qubit.first]);
          }
          // current_column.at(idx[0]) += "\\gate{SwapA}";
          // current_column.at(idx[1]) += "\\gate{SwapA}";
          current_column.at(idx[0]) += "\\gate{SwapA" + gate.parameter + "}";
          current_column.at(idx[1]) += "\\gate{SwapA" + gate.parameter + "}";
          current_column.at(idx[1]) +=
              " \\qwx[-" + std::to_string(idx[1] - idx[0]) + "]";
        } else if (gate.name == "CNOT" || gate.name == "CZ" ||
                   gate.name == "CPhase") {
          // cnot, cz, cphase
          int target_idx, contrl_idx;
          std::string target_gate;
          if (gate.symbol_map.begin()->second == "o") {
            // the first qubit in the map is the control
            contrl_idx = qubit_lut[gate.symbol_map.begin()->first];
            target_idx = qubit_lut[std::prev(gate.symbol_map.end())->first];
            target_gate = std::prev(gate.symbol_map.end())->second;
          } else {
            // the first qubit in the map is the target
            target_idx = qubit_lut[gate.symbol_map.begin()->first];
            contrl_idx = qubit_lut[std::prev(gate.symbol_map.end())->first];
            target_gate = gate.symbol_map.begin()->second;
          }
          // Put the target directly into the instruction
          // Calculate the parameter needed for the control bits. Control option
          // uses curly brakets!
          if (gate.name == "CNOT")
            current_column.at(target_idx) += "\\targ";
          else if (gate.name == "CZ")
            current_column.at(target_idx) += "\\gate{Z}";
          else if (gate.name == "CPhase")
            // current_column.at(target_idx) += "\\gate{Phase}";
            current_column.at(target_idx) +=
                "\\gate{Phase" + gate.parameter + "}";

          signed pos_diff =
              target_idx - contrl_idx; // position parameter needed to print the
                                       // control lines
          if (pos_diff > 0)
            current_column.at(contrl_idx) +=
                "\\ctrl{" + std::to_string(pos_diff) + "}";
          else
            current_column.at(contrl_idx) +=
                "\\ctrl{-" + std::to_string(-1 * pos_diff) + "}";
        }
      } else if (gate.name == "Toffoli") {
        /*
        3 cases:
        case 1:     *  case 2: *   case 3:
        q0: - X -   *  - o -   *   - o -
              |     *    |     *     |
        q1: - o -   *  - o -   *   - X -
              |     *    |     *     |
        q2: - o -   *  - X -   *   - o -
        */
        std::map<int, std::string> index_symbol;
        for (auto qubit : gate.symbol_map) {
          int idx = qubit_lut[qubit.first];
          index_symbol[idx] = qubit.second;
        }

        // check which case is this Toffoli
        int targ_idx, ctrl1_idx, ctrl2_idx;
        std::string ctrl1_pos, ctrl2_pos;
        if (index_symbol.begin()->second == "X") {
          targ_idx = index_symbol.begin()->first;
          ctrl1_idx = (++index_symbol.begin())->first;
          ctrl2_idx = std::prev(index_symbol.end())->first;
          ctrl1_pos = "-" + std::to_string(ctrl1_idx - targ_idx);
          ctrl2_pos = "-" + std::to_string(ctrl2_idx - ctrl1_idx);
        } else if (std::prev(index_symbol.end())->second == "X") {
          targ_idx = std::prev(index_symbol.end())->first;
          ctrl1_idx = index_symbol.begin()->first;
          ctrl2_idx = (++index_symbol.begin())->first;
          ctrl1_pos = std::to_string(ctrl2_idx - ctrl1_idx);
          ctrl2_pos = std::to_string(targ_idx - ctrl2_idx);
        } else {
          targ_idx = (++index_symbol.begin())->first;
          ctrl1_idx = index_symbol.begin()->first;
          ctrl2_idx = std::prev(index_symbol.end())->first;
          ctrl1_pos = std::to_string(targ_idx - ctrl1_idx);
          ctrl2_pos = "-" + std::to_string(ctrl2_idx - targ_idx);
        }

        // Add instructions to the column
        current_column.at(targ_idx) += "\\targ";
        current_column.at(ctrl1_idx) += "\\ctrl{" + ctrl1_pos + "}";
        current_column.at(ctrl2_idx) += "\\ctrl{" + ctrl2_pos + "}";
      }
    }

    // Add a wire on qubits which do not have a gate
    for (int i = 0; i < current_column.size(); i++) {
      std::string last_char = &current_column[i].back();
      if (last_char == " ") {
        current_column[i] += "\\qw";
      }
    }

    // Add a space on each row
    insertSpace(current_column);

    // Add this column to the output string
    for (int i = 0; i < current_column.size(); i++) {
      to_tex[i] += current_column[i];
    }
  }

  // Add the wire symbol to the end of each row
  for (int i = 0; i < to_tex.size(); i++) {
    to_tex[i] += "& \\qw ";
  }

  // Add the row symbol to the end of each row, except the last row
  for (int i = 0; i < (int)to_tex.size() - 1; i++) {
    to_tex[i] += "\\\\ \n";
  }

  // Write to an output file
  writeToFile(to_tex);

  bool debug_print = false;
  if (!quantum_debug.empty()) {
    for (std::string &DebugPass : quantum_debug) {
      debug_print = (DebugPass == "PrintLatex");
    }
  }

  // The following code is for debug only
  DEBUG_WITH_TYPE("latexInstructions", dbgs() << "LaTex instructions: \n");
  if (debug_print)
    errs() << "LaTex instructions: \n";
  for (auto &t : to_tex) {
    DEBUG_WITH_TYPE("latexInstructions", dbgs() << t << "\n");
    if (debug_print)
      errs() << t << "\n";
  }
  DEBUG_WITH_TYPE("latexInstructions", dbgs() << "\n");
  if (debug_print) {
    errs() << "\n";
  }
};

#undef DEBUG_TYPE