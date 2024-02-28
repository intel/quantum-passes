//===--------- PrinterJson.cpp - Prints the circuit to a json file --------===//
//
// Copyright (C) 2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//
//===----------------------------------------------------------------------===//
//
// This file implements a class that prints the input circuit out as a json file
//
//===----------------------------------------------------------------------===//

#include "IntelQuantumPasses/QuantumCompile/PrinterJson.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/Debug.h"

#include <dirent.h>
#include <fstream>
#include <iostream>
#include <map>
#include <nlohmann/json.hpp>
#include <sys/stat.h>

#define DEBUG_TYPE "json"

using jsonAPI = nlohmann::ordered_json; // preserve the insertion order
extern cl::opt<std::string> comp_pos;
extern cl::list<std::string> quantum_debug;

PrinterJson::PrinterJson(proofreader::ProofReader *read) {
  this->reader = read;
}

void PrinterJson::writeToFile(std::vector<std::vector<std::string>> &str) {
  std::string source_name = reader->getSourceFileName();
  std::string qbb_name = reader->getQBBName();
  std::string time_str = this->getCurrentTimeStr();
  std::string visual_path = this->getOutputDirectory() + "Visualization";

  DIR *dir = opendir(visual_path.c_str());
  if (!dir) {
    int status =
        mkdir(visual_path.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    if (status == -1) {
      std::cout << "WARNING: Quantum SDK - QuantumPrinterPass says: Unable to "
                   "create the Visualization directory to store the json "
                   "output. Skipping printing to json files...\n";
    }
  } else {
    closedir(dir);
  }

  bool debug_print = false;
  if (!quantum_debug.empty()) {
    for (std::string &DebugPass : quantum_debug) {
      debug_print = (DebugPass == "PrintJson");
    }
  }

  std::string file_name = visual_path + "/" + source_name + "-" + qbb_name +
                          "-" + comp_pos + "-" + time_str + ".json";

  signed int pos = reader->getCompilationPositionID();

  jsonAPI j;
  // Adding title rows
  j["IntelQuantumID"] = "Circuit-v1";
  j["title"] = qbb_name;
  j["numQbits"] = reader->getNumQubits();

  // add the qubit ID and qubit name map
  std::vector<std::string> qubits = reader->getListOfQubits();
  jsonAPI j_array(qubits);
  j["qbitNames"] = qubits;

  // write the circuit
  j["gates"] = jsonAPI::array();
  for (int i = 0; i < str.size(); i++) {
    // get the qubit index
    std::stringstream qubit_numbers(str[i].at(1));
    std::vector<unsigned> q_numbers;
    while (qubit_numbers.good()) {
      std::string qubit_num;
      std::getline(qubit_numbers, qubit_num, ',');
      q_numbers.push_back(std::stoi(qubit_num));
    }
    jsonAPI j_array(q_numbers);

    jsonAPI gate;
    gate["name"] = str[i].at(0);
    gate["qubits"] = q_numbers;
    j["gates"].emplace_back(gate);
  }

  // Write to output file
  std::fstream file;
  file.open(file_name, std::ios::out);
  if (!file.is_open()) {
    std::cout << "Unable to open file <" << file_name << ">. Exiting... \n";
    return;
  }

  file << std::setw(4) << j << std::endl;
  file.close();
  std::cout << "Wrote Json file to <" << file_name << ">. \n \n";

  // The following code is for debug only
  DEBUG_WITH_TYPE("json", dbgs() << "Json instructions: \n");
  if (debug_print)
    errs() << "Json instructions: \n";
  std::string jstring = j.dump(4);
  for (auto json_inst : jstring) {
    DEBUG_WITH_TYPE("json", dbgs() << json_inst);
    if (debug_print)
      errs() << json_inst;
  }
  DEBUG_WITH_TYPE("json", dbgs() << "\n");
  if (debug_print)
    errs() << "\n";
}

void PrinterJson::render() {
  std::vector<std::string> qubits = reader->getListOfQubits();

  // qubit_lut holds the integer to string mapping of qubit IDs
  std::map<std::string, int> qubit_lut;
  for (int i = 0; i < qubits.size(); i++) {
    qubit_lut[qubits.at(i)] = i;
  }

  // construct Json instructions for each column of gates
  std::vector<std::vector<std::string>> circuit_str;
  for (auto gates : reader->getListOfGates()) {
    for (auto gate : gates) {
      std::vector<std::string> gate_str(2, "");
      // position 0 holds the name
      gate_str.at(0) += gate.name;

      if (gate.symbol_map.size() == 1) {
        gate_str.at(1) +=
            std::to_string(qubit_lut[gate.symbol_map.begin()->first]);
      } else if (gate.symbol_map.size() == 2) {
        if (gate.symbol_map.begin()->second == "o") {
          gate_str.at(1) +=
              std::to_string(qubit_lut[gate.symbol_map.begin()->first]);
          gate_str.at(1) += ",";
          gate_str.at(1) += std::to_string(
              qubit_lut[std::prev(gate.symbol_map.end())->first]);
        } else {
          // second qubit is the control or swap
          gate_str.at(1) += std::to_string(
              qubit_lut[std::prev(gate.symbol_map.end())->first]);
          gate_str.at(1) += ",";
          gate_str.at(1) +=
              std::to_string(qubit_lut[gate.symbol_map.begin()->first]);
        }
      } else if (gate.symbol_map.size() == 3) {
        std::string ctrl1 = "";
        std::string ctrl2 = "";
        std::string targ = "";
        for (auto q : gate.symbol_map) {
          if (q.second != "o")
            targ += q.first;
          else if (q.second == "o" && ctrl1 == "")
            ctrl1 += q.first;
          else
            ctrl2 += q.first;
        }
        gate_str.at(1) += std::to_string(qubit_lut[ctrl1]) + "," +
                          std::to_string(qubit_lut[ctrl2]) + "," +
                          std::to_string(qubit_lut[targ]);
      }

      circuit_str.push_back(gate_str);
    }
  }

  // Write to an output file
  writeToFile(circuit_str);
};

#undef DEBUG_TYPE
