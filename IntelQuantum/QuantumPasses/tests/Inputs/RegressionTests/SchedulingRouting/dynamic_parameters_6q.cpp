//===----------------------------------------------------------------------===//
//
// Copyright (C) 2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//
//===----------------------------------------------------------------------===//
//
// This file provides an example of quantum program using dynamic parameters
// like rotation angles defined at runtime.
//
//===----------------------------------------------------------------------===//

/// Circuit that we want to schedule:
///
///     |      prep   |    slice 1      |         slice 2         |
/// 
/// q0:  |0> ---H----------*----*--- * ----T----Rx(1)----*----
///                        |    |    |                   |
/// q1:  |0> ---H----------)----)----)-------------------X---- D~
///                        |    |    |   
/// q2:  |0> --Rx45--------)----X----)-----H------------------
///                        |         |    
/// q3:  |0> --Ry45--------)----X----*----Rx(-2)-------X-------- D~
///                        |    |                      |
/// q4:  |0> --------------X----)----*----Ry(0)---X----)-------- D~
///                             |    |            |    |
/// q5:  |0> ---T--------Rx(0)--*----X------------*----*--------

// NOTE: Pick an include path depending on whether running in development
//       enviromnment or production environment
#include <clang/Quantum/quintrinsics.h>

#include <cmath>

const int N = 6;
qbit q[N];
cbit c[N];

double angle_inversion(double a) {return -a;}

//===----------------------------------------------------------------------===//

quantum_kernel void slice_2(double par0, double par1, double par2) {

  T(q[0]);
  H(q[2]);
  RY(q[4], par0);
  double temp_par2 = angle_inversion(par2);
  RX(q[3], temp_par2);
  CNOT(q[5], q[4]);
  CNOT(q[5], q[3]);
  RX(q[0], par1);
  CNOT(q[0], q[1]);

  MeasZ(q[4], c[0]);
  MeasZ(q[1], c[1]);
  MeasZ(q[3], c[3]);

  release_quantum_state();
}

//===----------------------------------------------------------------------===//

quantum_kernel void preparation() {

  for (int i = 0; i < N; i++) {
      PrepZ(q[i]);
  }
  H(q[1]);
  H(q[0]);
  RX(q[2], 45);
  RY(q[3], 45);
  T(q[5]);
}

//===----------------------------------------------------------------------===//

quantum_kernel void slice_1(double par0) {

  RX(q[5], par0);
  CNOT(q[0], q[4]);
  CNOT(q[0], q[2]);
  CNOT(q[5], q[3]);

  CZ(q[3], q[0]);
  CNOT(q[4], q[5]);
}

//===----------------------------------------------------------------------===//

int main() {

  preparation();

  double par0 = M_PI;
  
  slice_1(par0);
  double pars[3] = {M_PI, 0.5, 0.3*M_PI};
  slice_2(pars[0], pars[1], pars[2]);

  return 0;
}
