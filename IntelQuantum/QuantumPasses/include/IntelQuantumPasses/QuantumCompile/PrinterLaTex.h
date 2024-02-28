//===- PrinterLaTex.h ----------------------------------------------------====//
//
// Copyright (C) 2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//
//===----------------------------------------------------------------------===//
//
// This file defines a class that prints circuit diagram as LaTex instructions
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_QUANTUM_PRINTERLATEX_H
#define LLVM_QUANTUM_PRINTERLATEX_H

#include "Printer.h"
#include "ProofReader.h"

#include <string>
#include <vector>

class PrinterLaTex : public Printer {
public:
  // Constructor and destructor
  PrinterLaTex(proofreader::ProofReader *read);
  PrinterLaTex() = delete;
  ~PrinterLaTex() = default;

  // Method to print the circuit
  void render() override;

private:
  proofreader::ProofReader *reader;

  /// Insert the column to show qubit names
  void insertQubitNames(std::vector<std::string> &str_vec,
                        std::map<std::string, int> &qubit_lut);

  /// Insert symbol "-" to all even rows between row index [begin, end) in a
  /// given column Insert " " to odd rows to make sure vertical alignment is
  /// perserved
  void insertColumnSymbol(std::vector<std::string> &str);

  /// Insert " " symbol to the given row
  void insertSpace(std::vector<std::string> &str);

  /// Write to a file
  void writeToFile(std::vector<std::string> &str);
};

#endif
