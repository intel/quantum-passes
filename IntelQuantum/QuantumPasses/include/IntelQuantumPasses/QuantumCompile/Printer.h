//===- Printer.h ---------------------------------------------------------====//
//
// Copyright (C) 2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//
//===----------------------------------------------------------------------===//
//
// This file defines is the base class for printing circuit diagrams
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_QUANTUM_PRINTER_H
#define LLVM_QUANTUM_PRINTER_H

#include <ctime>
#include <string>

class Printer {
public:
  virtual void render() = 0; // =0 make it pure virtual and subclasses must
                             // implement

  std::string getCurrentTimeStr() {
    time_t t;
    struct tm *ttime;
    char buffer[80];
    time(&t);
    ttime = localtime(&t);
    if (nullptr == ttime)
      return "19700101-000000";
    strftime(buffer, 80, "%Y%m%d-%H%M%S", ttime);
    std::string time_str = buffer;
    return buffer;
  }

  void setOutputDirectory(std::string odir) { this->output_dir = odir; }
  std::string getOutputDirectory() { return this->output_dir; }

private:
  std::string output_dir;
};

#endif
