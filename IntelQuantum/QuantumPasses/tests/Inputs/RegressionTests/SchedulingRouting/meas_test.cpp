//===----------------------------------------------------------------------===//
//
// Copyright (C) 2021 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//
//===----------------------------------------------------------------------===//
//
// This file provides an example of quantum program which calls multiple
// quantum kernels as functions.
//
//===----------------------------------------------------------------------===//

#include <clang/Quantum/quintrinsics.h>
#include <vector>

// Global register of qubits
const int N = 7;
qbit q[N];
extern std::vector<bool> ClassicalBitRegister;
cbit c[N];

// Individual quantum kernels, i.e. blocks of quantum operations (prep, gates, meas)

/// Quantum kernel for state preparation
// It uses two equivalent operations:
// * __quantum_prepz
// * PrepZ
quantum_kernel void state_preparation() {
  PrepX(q[0]);
  PrepY(q[1]);
  PrepZ(q[2]);
  for (int i = 3; i < N; ++i)
      PrepZ(q[i]);
}

quantum_kernel void state_measurement() {
  MeasX(q[0], c[0]);
  MeasY(q[1], c[1]);
  MeasZ(q[2], c[2]);
  for (int i = 3; i < N; ++i)
      MeasZ(q[i], c[i]);
}

/// Main program calling multiple quantum kernels
int main() {

  state_preparation();
  state_measurement();

  return 0;
}
