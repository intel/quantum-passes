//===----------------------------------------------------------------------===//
//
// Copyright (C) 2021 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//
//===----------------------------------------------------------------------===//
//
// This file provides an example of quantum program using measurements
// in the middle of a quantum kernel.
//
//===----------------------------------------------------------------------===//

/// Circuit that we want to schedule:
///
///     |      prep   |    slice 1      |         slice 2         |
/// 
/// q0:  |0> ---X--------- * ------- * ----T---H----*-------
///                        |         |              |
/// q1:  |0> -- H -------- * -- * ---)------------- X ------ D~
///                             |    |   
/// q2:  |0> --Rx45------- * -- * ---)-----H-- D~ ---Rx45--- D~ 
///                        |         |    
/// q3:  |0> -- H ---------)--- * -- * ---Ry45----- * ------ D~
///                        |    |                   |
/// q4:  |0> --------------X--- * ---*-----T---X----)------- D~
///                                  |         |    |
/// q5:  |0> ---T--------- X ---T----)---------)----)------- 
///                        |         |         |    |
/// q6:  |0> --Rym45------ * ------- X ------- * -- X ------ D~

#include <clang/Quantum/quintrinsics.h>

const int N = 7;
qbit q[N];
cbit c[N];

//===----------------------------------------------------------------------===//

quantum_kernel void preparation() {

  for (int i = 0; i < N; i++) {
      PrepZ(q[i]);
  }
  H(q[1]);
  H(q[3]);
  X(q[0]);
  RX(q[2], 45);
  T(q[5]);
  RY(q[6],-45);
}

//===----------------------------------------------------------------------===//

quantum_kernel void slice_1() {

  CZ(q[0], q[1]);
  CNOT(q[2], q[4]);
  CNOT(q[6], q[5]);
  CZ(q[2], q[1]);
  CZ(q[3], q[4]);
  T(q[5]);
  CZ(q[3], q[0]);
  CNOT(q[4], q[6]);
}

//===----------------------------------------------------------------------===//

quantum_kernel void slice_2() {

  T(q[0]);
  H(q[0]);
  CNOT(q[0], q[1]);
  H(q[2]);
  T(q[4]);

  MeasZ(q[2], c[0]); // mid-circuit meas

  RY(q[3], 45);
  CNOT(q[6], q[4]);
  RX(q[2], 45); // op after meas
  CNOT(q[3], q[6]);

  for (int i = 1; i < 5; i++) {
      MeasZ(q[i], c[i]);
  }
  MeasZ(q[6], c[6]);

  release_quantum_state();
}

//===----------------------------------------------------------------------===//

int main() {

  preparation();
  slice_1();
  slice_2();
  return 0;
}
