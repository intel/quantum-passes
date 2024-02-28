//===- PrinterASCII.h ----------------------------------------------------====//
//
// Copyright (C) 2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//
//===----------------------------------------------------------------------===//
//
// This file defines a class that prints circuit diagram into the console
// using ASCII characters
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_QUANTUM_PRINTERASCII_H
#define LLVM_QUANTUM_PRINTERASCII_H

#include "Printer.h"
#include "ProofReader.h"

#include <string>
#include <vector>

class PrinterASCII : public Printer {
public:
  // Constructor and destructor
  PrinterASCII(proofreader::ProofReader *read);
  PrinterASCII() = delete;
  ~PrinterASCII() = default;

  // Method to print the circuit
  void render() override;

private:
  proofreader::ProofReader *reader;

  /// Insert "-" symbols to the given row
  void insertLineToRow(std::string &str, signed int num);

  /// Insert symbol "-" to all even rows between row index [begin, end) in a
  /// given column Insert " " to odd rows to make sure vertical alignment is
  /// perserved
  void insertLineToColumn(std::vector<std::string> &str, unsigned begin,
                          unsigned end);

  /// Insert control lines between two row indexes (begin, end)
  void insertVerticalLine(std::vector<std::string> &str_vec, unsigned begin,
                          unsigned end);

  /// Insert " " symbol to the given row
  void insertSpaceToRow(std::string &str, signed int num);

  /// Insert the column to show qubit names
  void insertQubitNames(std::vector<std::string> &str_vec,
                        std::unordered_map<std::string, int> &qubit_lut);
};

#endif
