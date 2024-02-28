//===- PrinterInterface.h ------------------------------------------------====//
//
// Copyright (C) 2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//
//===----------------------------------------------------------------------===//
//
// This file defines a class which provides the interface between the
// PrinterPass and the various printer classes
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_QUANTUM_PRINTERINTERFACE_H
#define LLVM_QUANTUM_PRINTERINTERFACE_H

#include "PrinterASCII.h"
#include "PrinterJson.h"
#include "PrinterLaTex.h"
#include "ProofReader.h"

#include <vector>

class PrinterInterface {
public:
  PrinterInterface(proofreader::ProofReader *read);
  void addOptions(std::string option);
  void setOutputDir(std::string output_dir);
  void run();

private:
  proofreader::ProofReader *reader;
  std::vector<std::string> options;
  std::string output_dir;
};

#endif