//===- PrinterJson.h ----------------------------------------------------====//
//
// Copyright (C) 2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//
//===----------------------------------------------------------------------===//
//
// This file defines a class that prints circuit diagram as a json file
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_QUANTUM_PRINTERJSON_H
#define LLVM_QUANTUM_PRINTERJSON_H

#include "Printer.h"
#include "ProofReader.h"

#include <string>
#include <vector>

class PrinterJson : public Printer {
public:
  // Constructor and destructor
  PrinterJson(proofreader::ProofReader *read);
  PrinterJson() = delete;
  ~PrinterJson() = default;

  // Method to print the circuit
  void render() override;

private:
  proofreader::ProofReader *reader;

  // Write to a file
  void writeToFile(std::vector<std::vector<std::string>> &str);
};

#endif
