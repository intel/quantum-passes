//===- PrinterInterface.cpp - Manages and instantiate printer objects -----===//
//
// Copyright (C) 2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//
//===----------------------------------------------------------------------===//
//
// This file implements an interface to take command line options and create
// needed Printer objects to carry out the printing
//
//===----------------------------------------------------------------------===//

#include "IntelQuantumPasses/QuantumCompile/PrinterInterface.h"

#include <algorithm>

PrinterInterface::PrinterInterface(proofreader::ProofReader *read,
                                   std::string pos) {
  this->reader = read;
  comp_pos = pos;
}

void PrinterInterface::addOptions(std::string print_option) {
  if (std::find(this->options.begin(), this->options.end(), print_option) ==
      this->options.end()) {
    this->options.push_back(print_option);
  }
}

void PrinterInterface::setOutputDir(std::string output_dir) {
  this->output_dir = output_dir;
}

void PrinterInterface::run() {
  for (auto &option : this->options) {
    if (option == "console") {
      PrinterASCII printer_ascii(this->reader);
      printer_ascii.render();
    } else if (option == "tex") {
      PrinterLaTex printer_latex(this->reader, comp_pos);
      printer_latex.setOutputDirectory(this->output_dir);
      printer_latex.render();
    } else if (option == "json") {
      PrinterJson printer_json(this->reader, comp_pos);
      printer_json.setOutputDirectory(this->output_dir);
      printer_json.render();
    }
  }
}
