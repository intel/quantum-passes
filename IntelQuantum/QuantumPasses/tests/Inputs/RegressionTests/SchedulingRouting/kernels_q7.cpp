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

/// Circuit that we want to schedule, divided in kernels:
///
///     |prep | slice 1 |   slice 2   | slice 3| meas
///
/// q0:   |0> -----X--------*------------- * ---- D~
///                         |              |
/// q1:   |0> --------------|---X--------- X ---- D~
///                         |   |
/// q2:   |0> -----X--------|---|--*------------- D~
///                         |   |  |
/// q3:   |0> --------------X---|--|------------- D~
///                             |  |
/// q4:   |0> -----X------------*--|------------- D~
///                                |
/// q5:   |0> ---------------------X-------X----- D~
///                                        |
/// q6:   |0> -----X-----------------------*----- D~

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
  __quantum_prepz(q[0]);
  __quantum_prepz(q[1]);
  __quantum_prepz(q[2]);
  for (int i = 3; i < N; ++i)
      PrepZ(q[i]);
}

/// Quantum kernel with 1-qubit gates
quantum_kernel void slice_1() {
  X(q[0]);
  X(q[2]);
  X(q[4]);
  X(q[6]);
}

/// Quantum kernel with 2-qubit gates (notice that it corresponds to the third slice!)
quantum_kernel void slice_3() {
  CNOT(q[0], q[1]);
  CNOT(q[6], q[5]);
}

/// Quantum kernel with 2-qubit gates (notice that it corresponds to the second slice!)
quantum_kernel void slice_2() {
  CNOT(q[0], q[3]);
  CNOT(q[4], q[1]);
  CNOT(q[2], q[5]);
}

/// Quantum kernel for state measurement
// It uses two equivalent operations:
// * __quantum_measz
// * MrepZ
quantum_kernel void state_measurement() {
  __quantum_measz(q[0]);
  for (int i = 1; i < N; ++i)
      MeasZ(q[i]);
}


/// Main program calling multiple quantum kernels
int main() {

  state_preparation();
  slice_1();
  slice_2();
  slice_3();
  state_measurement();
  
  c[0] = ClassicalBitRegister[0];

  if(c[0]) state_preparation();

  return 0;
}
