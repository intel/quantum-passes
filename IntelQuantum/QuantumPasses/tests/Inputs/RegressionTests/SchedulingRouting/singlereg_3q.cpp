//===----------------------------------------------------------------------===//
//
// Copyright (C) 2021 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//
//===----------------------------------------------------------------------===//
//
// This file provides an example of basic quantum program with a single
// qubit register and all quantum operations in the main().
//
//===----------------------------------------------------------------------===//

/// Circuit that we want to schedule:
///
/// q0: |0> -- H -- X -- X -- Z ------- D~
///                      |    |    
/// q1: |0> -- Z ------- * -- * -- X -- D~
///                                |
/// q2: |+> -- Z -- H ------- Y -- * -- D~
///
/// We provide the gate in an order different from the temporal one,
/// to test the greedy algorithm.
///
/// No need of routing for a linear connectivity and trivial placement.

#include <clang/Quantum/quintrinsics.h>

const int N = 3;
qbit q[N];
cbit c[N];

quantum_kernel void circuit() {
//  for (int i = 0; i <2; i++)
//    PrepZ(q[i]);
  PrepZ(q[0]);
  PrepZ(q[1]);
  PrepX(q[N-1]);

  H(q[0]);
  X(q[0]);
  Z(q[1]);
  CNOT(q[1], q[0]);
  CZ(q[1], q[0]);

  Z(q[2]);
  H(q[2]);
  Y(q[2]);
  CNOT(q[2], q[1]);

  MeasZ(q[2], c[2]);
  MeasZ(q[1], c[1]);
  MeasZ(q[0], c[0]);
}

//===----------------------------------------------------------------------===//

int main() {

  circuit();
  return 0;

}
