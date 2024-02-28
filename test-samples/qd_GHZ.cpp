//===----------------------------------------------------------------------===//
//
// Copyright (C) 2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//
//===----------------------------------------------------------------------===//

#include <clang/Quantum/quintrinsics.h>

/// Quantum Runtime Library APIs
#include <quantum_service_apis.h>

const int total_qubits = 2;
qbit qubit_register[total_qubits];

quantum_kernel void ghz_total_qubits() {
  for (int i = 0; i < total_qubits; i++) {
    PrepZ(qubit_register[i]);
  }

  H(qubit_register[0]);

  for (int i = 0; i < total_qubits - 1; i++) {
    CNOT(qubit_register[i], qubit_register[i + 1]);
  }
}

int main() {
  iqsdk::DeviceConfig qd_config("QD_SIM");
  iqsdk::FullStateSimulator quantum_8086(qd_config);
  if (iqsdk::QRT_ERROR_SUCCESS != quantum_8086.ready())
    return 1;

  // get references to qbits
  std::vector<std::reference_wrapper<qbit>> qids;
  for (int id = 0; id < total_qubits; ++id) {
    qids.push_back(std::ref(qubit_register[id]));
  }

  ghz_total_qubits();

  // use string constructor of Quantum State Space index to choose which
  // basis states' data is retrieved
  std::vector<iqsdk::QssIndex> bases = {iqsdk::QssIndex("|00>"),
                                        iqsdk::QssIndex("|11>")};
  iqsdk::QssMap<double> probability_map;
  probability_map = quantum_8086.getProbabilities(qids, bases);

  // perform a double-check on our results
  double total_probability = 0.0;
  for (auto const &key_value : probability_map) {
    total_probability += key_value.second;
  }

  std::cout << "Sum of probability to measure fully entangled state: "
            << total_probability << std::endl;
  quantum_8086.displayProbabilities(probability_map);

  // use sampling technique to simulate the results of many runs
  std::vector<std::vector<bool>> measurement_samples;
  unsigned total_samples = 1000;
  measurement_samples = quantum_8086.getSamples(total_samples, qids);

  // build a distribution of states
  iqsdk::QssMap<unsigned int> distribution =
      iqsdk::FullStateSimulator::samplesToHistogram(measurement_samples);

  // print out the results
  std::cout << "Using " << total_samples
            << " samples, the distribution of states is:" << std::endl;
  for (const auto &entry : distribution) {
    double weight = entry.second;
    weight = weight / total_samples;

    std::cout << entry.first << " : " << weight << std::endl;
  }

  return 0;
}
