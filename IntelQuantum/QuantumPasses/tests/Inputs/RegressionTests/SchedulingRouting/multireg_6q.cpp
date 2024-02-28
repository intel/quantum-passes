//===----------------------------------------------------------------------===//
//
// Copyright (C) 2021 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//
//===----------------------------------------------------------------------===//
//
// This file provides an example of quantum program using multiple qubit
// registers: here the qubit array 'q' and the single qubit 'ancilla'.
//
//===----------------------------------------------------------------------===//

/// Circuit that we want to schedule:
///
/// anc: |0> ------- H ------------------ D~
///              
///  q0: |0> ------- * ------- * --------
///                  |         |   
///  q1: |0> -- H -- * -- * ---)--- H --- D~
///                       |    |   
///  q2: |0> ------- * -- * ---)---------
///                  |         |    
///  q3: |0> -- H -- * -- * -- * -- H --- D~
///                       |        
///  q4: |0> ------------ * -------------
///
///                           ^^^
///                       2 SWAPs are required for a linear
///                       connectivity and trivial placement
///
/// Notice that the quantum state is released and thus it is not
/// necessary adding any "backward routing".

#include <clang/Quantum/quintrinsics.h>

const int N = 5;
qbit q[N];
qbit ancilla;

cbit c[N];
cbit ca;

quantum_kernel void circuit() {

  for (int i = 0; i < N; i++) {
    PrepZ(q[i]);
  }

  H(q[1]);
  H(q[3]);

  CZ(q[0], q[1]);
  CZ(q[2], q[3]);
  CZ(q[1], q[2]);
  CZ(q[3], q[4]);
  CZ(q[0], q[3]);

  H(q[1]);
  H(q[3]);

  MeasZ(q[1], c[1]);
  MeasZ(q[3], c[3]);

  PrepZ(ancilla);
  H(ancilla);
  MeasZ(ancilla, ca);

  release_quantum_state();
}

//===----------------------------------------------------------------------===//

int main() {

  circuit();
  return 0;
}
