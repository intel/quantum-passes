//===----------------------------------------------------------------------===//
//
// Copyright (C) 2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//
//===----------------------------------------------------------------------===//

/// NOTE: Pick an include path depending on whether running in development
/// enviromnment or production environment
//===----------------------------------------------------------------------===//
#include <clang/Quantum/quintrinsics.h>
#include <cassert>
#include <cmath>
#include <functional>
#include <iostream>
#include <math.h>
#include <quantum_service_apis.h>
#include <vector>

// Note: need to use 255 qubit config file
// (release_utils/IQSDK_IQS_Configuration_255Q_HumanReadable.json)

const int N = 16;
qbit q[N];
cbit c[N];

quantum_kernel void ghz() {

  for (int i = 0; i < N; i++) {
    PrepZ(q[i]);
  }

  H(q[0]);

  for (int i = 0; i < N - 1; i++) {
    CNOT(q[i], q[i + 1]);
  }
}

int main() {
  TensorNetworkConfig tensor_network_config(N);
  TensorNetworkSimulator tensor_network(tensor_network_config);
  iqsdk::QRT_ERROR_T status = tensor_network.ready();
  assert(status == iqsdk::QRT_ERROR_SUCCESS);
  ghz();

  std::vector<std::reference_wrapper<qbit>> qids;
  for (int i = 0; i < N; ++i) {
    qids.push_back(std::ref(q[i]));
  }

  std::string zero_vector = "";
  std::string one_vector = "";

  for (int i = 0; i < N; ++i) {
    zero_vector += "0";
    one_vector += "1";
  }

  std::cout << "Printing amplitudes of GHZ state" << std::endl;

  std::complex<double> amplitude_zero =
      tensor_network.getAmplitude(qids, zero_vector);
  std::cout << "|" << zero_vector << ">"
            << " " << amplitude_zero << std::endl;
  std::complex<double> amplitude_one =
      tensor_network.getAmplitude(qids, one_vector);
  std::cout << "|" << one_vector << ">"
            << " " << amplitude_one << std::endl;
  // The amplitude of this should be ~0
  std::string state = "1111111111000000000000000000000000000000000000000000000000000000000000111111111111111111111100001111";
  std::complex<double> amplitude_state =
      tensor_network.getAmplitude(qids, state);
  std::cout << "|" << state << ">"
            << " " << amplitude_state << std::endl;
}
