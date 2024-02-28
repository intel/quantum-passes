//===- PrinterASCII.cpp - Read in Quantum Circuits ------------------------===//
//
// Copyright (C) 2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//
//===----------------------------------------------------------------------===//
//
// This file implements a class that prints the input circuit out as ASCII
// to the console
//
//===----------------------------------------------------------------------===//

#include "IntelQuantumPasses/QuantumCompile/PrinterASCII.h"

#include <iostream>
#include <map>

#include <stdio.h>
#include <sys/ioctl.h>
#include <unistd.h>

#define MAX_RENDERED_QUBITS 30
#define MAX_COLUMNS 300

PrinterASCII::PrinterASCII(proofreader::ProofReader *read) {
  this->reader = read;
}

void PrinterASCII::insertLineToRow(std::string &str, signed int num) {
  for (int i = 0; i < num; i++) {
    str += "-";
  }
}

void PrinterASCII::insertLineToColumn(std::vector<std::string> &str,
                                      unsigned begin, unsigned end) {
  for (int i = begin; i < end; i++) {
    if (i % 2 == 0)
      str[i] += "-";
    else
      str[i] += " ";
  }
}

void PrinterASCII::insertVerticalLine(std::vector<std::string> &str_vec,
                                      unsigned row1, unsigned row2) {
  // adding control lines between qubits
  unsigned begin, end;
  if (row1 < row2) {
    begin = row1;
    end = row2;
  } else {
    begin = row2;
    end = row1;
  }

  for (int i = begin + 1; i < end; i++)
    str_vec[i] += " | ";
}

void PrinterASCII::insertSpaceToRow(std::string &str, signed int num) {
  for (int i = 0; i < num; i++) {
    str += " ";
  }
}

void PrinterASCII::insertQubitNames(
    std::vector<std::string> &str_vec,
    std::unordered_map<std::string, int> &qubit_lut) {
  unsigned num_to_pad = 0;
  // Insert the qubit register name and index on even rows
  for (auto &q : qubit_lut) {
    str_vec[q.second] += "|" + q.first + ">";
    num_to_pad = (str_vec[q.second].size() > num_to_pad)
                     ? str_vec[q.second].size()
                     : num_to_pad;
  }

  // pad space to all rows to ensure vertical alignment
  for (int i = 0; i < str_vec.size(); i++) {
    // +1 is to create a white space immediately following the
    // ket sign. Design choice.
    unsigned pad_num = num_to_pad - str_vec[i].size() + 1;
    insertSpaceToRow(str_vec[i], pad_num);
    // if the row is a qubit row, we add a colon sign
    // otherwise, we add spaces
    str_vec[i] += (i % 2 == 0) ? ": " : "  ";
  }
}

void PrinterASCII::render() {
  // if the number of qubits or number of gates exceeds our limits
  // we will not print to the console
  unsigned num_qubits = reader->getNumQubits();
  unsigned num_columns = reader->getListOfGates().size();
  if (num_qubits > MAX_RENDERED_QUBITS || num_columns > MAX_COLUMNS) {
    std::cout << "WARNING: Quantum SDK - The size of the circuit "
              << reader->getQBBName()
              << " exceeds the printable size. \nPrintable upper limits: "
              << MAX_RENDERED_QUBITS << " qubits, " << MAX_COLUMNS
              << " gates.\nSkipping printing...\n";
    return;
  }

  std::vector<std::string> qubits = reader->getListOfQubits();

  std::unordered_map<std::string, int> qubit_lut;
  for (int i = 0; i < qubits.size(); i++) {
    qubit_lut[qubits.at(i)] = i * 2;
  }

  // to_console holds the ASCII char to be printed
  unsigned num_rows = 2 * num_qubits;
  std::vector<std::string> to_console(num_rows, "");

  // get the window width
  struct winsize window;
  ioctl(STDOUT_FILENO, TIOCGWINSZ, &window);

  // construct the qubit name column
  PrinterASCII::insertQubitNames(to_console, qubit_lut);

  std::cout << " Circuit for QBB in quantum_kernel - '" << reader->getQBBName()
            << "'\n\n";
  // construct ASCII strings for each column of gates
  for (auto &gates : reader->getListOfGates()) {
    std::vector<std::string> current_column(num_rows, "");
    insertLineToColumn(current_column, 0, current_column.size());

    signed max_symbol_len = 0;
    for (auto &gate : gates) {
      std::vector<std::string> qubit_names;
      for (auto &qubit : gate.symbol_map) {
        std::string q_gate = " " + qubit.second;
        // add the angle string if this qubit has an angle on it
        if (gate.parameter.size() > 0 && qubit.second != "o") { /*&&
            gate.name.rfind("Meas", 0) != 0)*/
          q_gate += gate.parameter;
        }

        max_symbol_len =
            (q_gate.size() > max_symbol_len) ? q_gate.size() : max_symbol_len;
        q_gate += " ";
        current_column[qubit_lut[qubit.first]] += q_gate;

        qubit_names.push_back(qubit.first);
      }

      // Add control lines
      for (int i = 0; i < qubit_names.size() - 1; i++) {
        insertVerticalLine(current_column, qubit_lut[qubit_names.at(i)],
                           qubit_lut[qubit_names.at(i + 1)]);
      }
    }

    // Pad to the right to maintain alignment
    for (int i = 0; i < current_column.size(); i++) {
      // +3 accounts for the two extra spaces added to each symbol
      signed num_to_pad = max_symbol_len + 3 - current_column[i].size();
      if (num_to_pad < 0) {
        llvm::errs()
            << "WARNING: Quantum SDK - attempting to pad nagative number of "
               "spaces. Unable to generate ASCII represenstation.\n";
        return;
      }

      if (i % 2 == 0)
        // qubits and gates are written on even rows
        insertLineToRow(current_column[i], num_to_pad);
      else
        // odd rows are reserved for space and control lines
        insertSpaceToRow(current_column[i], num_to_pad);
    }

    // Add two columns of "-"
    insertLineToColumn(current_column, 0, current_column.size());
    insertLineToColumn(current_column, 0, current_column.size());

    // if the length of the string exceeds the window width,
    // flush the current content to the console and start printing
    // on a new section
    if (current_column.at(0).size() + to_console.at(0).size() > window.ws_col &&
        window.ws_col != 0) {
      // if the window size is too narrow, show warning
      if (to_console.at(0).size() > window.ws_col) {
        llvm::errs() << "WARNING: Quantum SDK - console too narrow. Circuit "
                        "might not be shown properly.\nPlease consider making "
                        "the window wider and try again.\n";
      }

      for (auto &row : to_console) {
        std::cout << row << "\n";
        row = "";
      }
      std::cout << "\n";
    }
    // add the qubit names if we are printing to a new section
    if (to_console.at(0).size() == 0) {
      PrinterASCII::insertQubitNames(to_console, qubit_lut);
    }
    // add the current column to the output string
    for (int i = 0; i < current_column.size(); i++) {
      to_console[i] += current_column[i];
    }
  }

  // Print the remaining part to terminal
  for (auto &row : to_console) {
    std::cout << row << "\n";
  }
  std::cout << "\n";
};
