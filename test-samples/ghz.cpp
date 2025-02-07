//===----------------------------------------------------------------------===//
//
// Copyright (C) 2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//
//===----------------------------------------------------------------------===//
//
//      Generating the Greenberger-Horne-Zeilinger (GHZ) state
//      The n-qubit GHZ state is: (|000...> + |111...>) / sqrt(2)
//
//      For 5 qubits the circuit looks like:
//      |qumem_0> : - PrepZ --- H --- o --------------------
//                                    |
//      |qumem_1> : - PrepZ --------- X --- o --------------
//                                          |
//      |qumem_2> : - PrepZ --------------- X --- o --------
//                                                |
//      |qumem_3> : - PrepZ --------------------- X --- o --
//                                                      |
//      |qumem_4> : - PrepZ --------------------------- X --
//===----------------------------------------------------------------------===//

#include <clang/Quantum/quintrinsics.h>
#include <quantum_full_state_simulator_backend.h>

#include <algorithm>
#include <cassert>
#include <iostream>
#include <vector>

// Allocate 15 global qubits.
const int MAX_NUM_QUBITS = 12;
qbit Qumem[MAX_NUM_QUBITS];
cbit Cmem[MAX_NUM_QUBITS];

// Function to prep N qubits
template <int N> quantum_kernel void prepQubits(qbit q[]) {
  for (int i = 0; i < N; ++i)
    PrepZ(q[i]);
}

template <int N> quantum_kernel void GHZ(qbit q[], cbit c[]) {
  prepQubits<N>(q);
  H(q[0]);
  for (int i = 0; i < (N - 1); ++i)
    CNOT(q[i], q[i + 1]);
}

template <int N> quantum_kernel void callGHZ() { GHZ<N>(Qumem, Cmem); }

template <int N> void runGHZ(iqsdk::FullStateSimulator &device) {
  callGHZ<N>();
  std::vector<std::reference_wrapper<qbit>> qbit_refs;
  for (size_t qubit = 0; qubit < N; ++qubit) {
    qbit_refs.push_back(Qumem[qubit]);
  }
  std::string zero_vector;
  std::string one_vector;
  for (size_t qubit = 0; qubit < N; ++qubit) {
    zero_vector += "0";
    one_vector += "1";
  }
  std::cout << "------- " << N << " qubit GHZ state -------" << std::endl;
  auto probability_map = device.getProbabilities(
      qbit_refs, {iqsdk::QssIndex(zero_vector), iqsdk::QssIndex(one_vector)});
  iqsdk::FullStateSimulator::displayProbabilities(probability_map);
  auto amplitude_map = device.getAmplitudes(
      qbit_refs, {iqsdk::QssIndex(zero_vector), iqsdk::QssIndex(one_vector)});
  iqsdk::FullStateSimulator::displayAmplitudes(amplitude_map);
}

// Supply template for template parameters N <= 0 (do nothing)
template <int N>
void runGHZ_recursive(iqsdk::FullStateSimulator &device,
                      typename std::enable_if<(N <= 0)>::type * = 0) {}

// Supply template for template parameters N > 0
template <int N>
void runGHZ_recursive(iqsdk::FullStateSimulator &device,
                      typename std::enable_if<(N > 0)>::type * = 0) {
  //runGHZ_recursive<N - 5>(device);
  runGHZ<N>(device);
}

int main(int argc, char *argv[]) {
  iqsdk::IqsConfig iqs_config;
  iqsdk::FullStateSimulator iqs_device(iqs_config);
  iqsdk::QRT_ERROR_T status = iqs_device.ready();
  assert(status == iqsdk::QRT_ERROR_SUCCESS);
  runGHZ_recursive<MAX_NUM_QUBITS>(iqs_device);
}
