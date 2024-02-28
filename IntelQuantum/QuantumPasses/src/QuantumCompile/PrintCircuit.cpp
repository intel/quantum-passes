//===--------- PrintCircuit.cpp - Read and Print Quantum Circuits ---------===//
//
// Copyright (C) 2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//
//===----------------------------------------------------------------------===//
//
// This file implements a module pass to read quantum IR and print the circuit
//   representation.
//
// The purpose is to offer an example of using the Intel QIter class to treat
//   code that represents a quantum circuit, as well as to offer examples of
//   llvm idioms.
//
// Printing options available:
//   * ASCII - this has the merit of giving immediate feedback with no extra
//       implementation, redirectable at commandline, and comes with a retro
//       computing vibe.
//   * .json - this is a simple print out of the key information needed to make
//       a circuit diagram, written to circuit.json.
//===----------------------------------------------------------------------===//

#define DEBUG_TYPE ""

#include "IntelQuantumPasses/QuantumAnalysis/QuantumAnnotationsToJson.h"
#include "IntelQuantumPasses/QuantumAnalysis/QuantumGateIdentifiers.h"
#include "IntelQuantumPasses/QuantumAnalysis/QuantumIterator.h"
#include "IntelQuantumPasses/QuantumAnalysis/QuantumRefs.h"

#include "llvm/IR/LegacyPassManager.h"
#include <fstream>
#include <iostream>
#include <string>

using namespace llvm;

extern cl::opt<bool> debug_print;

namespace {
struct PrintCircuit : public ModulePass {
  static char ID;
  PrintCircuit() : ModulePass(ID) {}

  bool runOnModule(Module &M) override {
    std::vector<aqcc::QIter> circuit;
    std::vector<unsigned> qubit_order; // ordered as they are encountered in IR
                                       // values refer to position in register

    // read program and store gates in circuit
    for (Function &F : M) {
      if (F.getName() == "main") {

        aqcc::QIter qi(F); // quantum iterator to step through the gates

        for (qi.gotoBegin(); !qi.isEnd(); ++qi) {
          // use QIter and QbitRef methods to find where this gate's qubits are
          // in the absolute total qubit register
          std::vector<aqcc::QbitRef> wires = qi.getQubitOperands();

          for (auto gate_qubit : wires) {
            if (!gate_qubit.isResolved()) {
              if (!gate_qubit.updateToSimplified()) {
                std::cout << "Print Circuit Pass can't resolve qubit indices."
                          << std::endl;
                return false; // change to break and print what is in circuit
              }
            }
            // try again in the positive because the inner-if may have succeeded
            if (gate_qubit.isResolved()) {
              unsigned qubit_val = gate_qubit.getIndexResolved().value();
              bool first_encounter =
                  std::find(qubit_order.begin(), qubit_order.end(),
                            qubit_val) == qubit_order.end();
              if (first_encounter) {
                qubit_order.push_back(qubit_val);
              }
            }
          } // end for over gate's qubits

          circuit.push_back(qi);
        } // end for over QIter
      }   // end if main
    }     // end for F in M

    // print .json
    std::ofstream json_out;
    std::string filename = "circuit_diagram.json";
    json_out.open(filename);

    json_out << "{\n\t\"title\" : \"Sample Circuit Title\",\n";
    json_out << "\t\"numQbits\" : " << qubit_order.size() << "," << std::endl;
    json_out << "\t\"gates\" : [\n";

    for (unsigned i = 0; i < circuit.size(); i++) {
      auto &gate = circuit.at(i);
      std::string abbrev = GetNameFromGateIdentifier(gate.getIdentifier());
      json_out << "\t\t{\n";
      json_out << "\t\t\t\"name\" : \"" << abbrev << "\",\n";

      json_out << "\t\t\t\"qubits\" : [";
      std::vector<aqcc::QbitRef> wires = gate.getQubitOperands();
      if (wires.size() == 1) {
        unsigned this_qubit_val = wires.at(0).getIndexResolved().value();
        json_out << this_qubit_val << "]\n";
      } else {
        auto last = wires.cend();
        --last;
        for (auto it = wires.begin(); it != wires.end(); ++it) {
          if (it == last) {
            json_out << it->getIndexResolved().value();
          } else {
            json_out << it->getIndexResolved().value() << ", ";
          }
        }
        json_out << "]\n";
      }
      if (i == circuit.size() - 1) {
        json_out << "\t\t}\n";
      } else {
        json_out << "\t\t},\n";
      }
    }
    json_out << "\t]\n"
             << "}" << std::endl;

    // print the ASCII circuit
    unsigned maxrows = 4 * qubit_order.size();
    std::vector<std::string> sPrintOutLines(maxrows, "");
    unsigned timeSlice = 0;
    for (auto &gate : circuit) {
      std::string abbrev = GetNameFromGateIdentifier(gate.getIdentifier());
      std::vector<aqcc::QbitRef> wires = gate.getQubitOperands();

      if (wires.size() == 1) {
        unsigned gate_qbit_val = wires.at(0).getIndexResolved().value();

        for (std::vector<unsigned>::size_type i = 0; i < qubit_order.size();
             ++i) {
          unsigned current_qubit = qubit_order.at(i);
          bool gate_on_current = current_qubit == gate_qbit_val;
          if (gate_on_current) {
            sPrintOutLines.at(4 * i + 0) += "  ----  ";
            sPrintOutLines.at(4 * i + 1) += "==|" + abbrev + " |==";
            sPrintOutLines.at(4 * i + 2) += "  ----  ";
            sPrintOutLines.at(4 * i + 3) += "        ";
          } else {
            sPrintOutLines.at(4 * i + 0) += "        ";
            sPrintOutLines.at(4 * i + 1) += "========";
            sPrintOutLines.at(4 * i + 2) += "        ";
            sPrintOutLines.at(4 * i + 3) += "        ";
          }
        }
      } else if (wires.size() == 2) {
        std::vector<unsigned> gate_qbit_vals;
        for (std::vector<aqcc::QbitRef>::size_type i = 0; i < wires.size();
             ++i) {
          gate_qbit_vals.push_back(wires.at(i).getIndexResolved().value());
        }
        bool bControlOn = false;
        for (std::vector<std::string>::size_type i = 0; i < qubit_order.size();
             ++i) {
          // 4 possibilities
          if (qubit_order.at(i) == gate_qbit_vals.at(0)) { // gate
            sPrintOutLines.at(4 * i + 0) += "  ----  ";
            sPrintOutLines.at(4 * i + 1) += "==|" + abbrev + "|==";
            sPrintOutLines.at(4 * i + 2) += "  ----  ";
            sPrintOutLines.at(4 * i + 3) += "   :    ";
            bControlOn = !bControlOn;
          } else if (bControlOn && qubit_order.at(i) == gate_qbit_vals.at(1)) {
            sPrintOutLines.at(4 * i + 0) += "   :    ";
            sPrintOutLines.at(4 * i + 1) += "===*====";
            sPrintOutLines.at(4 * i + 2) += "        ";
            sPrintOutLines.at(4 * i + 3) += "        ";
            bControlOn = !bControlOn;
          } else if (qubit_order.at(i) == gate_qbit_vals.at(1)) { // control
            sPrintOutLines.at(4 * i + 0) += "        ";
            sPrintOutLines.at(4 * i + 1) += "===*====";
            sPrintOutLines.at(4 * i + 2) += "   :    ";
            sPrintOutLines.at(4 * i + 3) += "   :    ";
            bControlOn = !bControlOn;
          } else if (bControlOn) { // print wire & control
            sPrintOutLines.at(4 * i + 0) += "   :    ";
            sPrintOutLines.at(4 * i + 1) += "========";
            sPrintOutLines.at(4 * i + 2) += "   :    ";
            sPrintOutLines.at(4 * i + 3) += "   :    ";
          } else { // need to print wire
            sPrintOutLines.at(4 * i + 0) += "        ";
            sPrintOutLines.at(4 * i + 1) += "========";
            sPrintOutLines.at(4 * i + 2) += "        ";
            sPrintOutLines.at(4 * i + 3) += "        ";
          }
        }
      }
      ++timeSlice;

      // Write and Flush
      bool lineDone = (timeSlice % 9) == 0;
      if (lineDone || timeSlice == circuit.size()) {
        for (auto row : sPrintOutLines) {
          // Publish
          std::cout << row << std::endl;
          row = "";
        }
      }
    }

    return true;

  } // end runOnModule
};  // end struct PrintCircuit
} // end anonymous namespace

char PrintCircuit::ID = 0;
static RegisterPass<PrintCircuit> X("print_circuit",
                                    "Print Quantum Circuit Pass",
                                    false /* Only looks at CFG */,
                                    false /* Analysis Pass */);
#undef DEBUG_TYPE
