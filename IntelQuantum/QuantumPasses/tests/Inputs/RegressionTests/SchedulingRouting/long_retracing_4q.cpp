//===----------------------------------------------------------------------===//
//
// Copyright (C) 2021 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//
//===----------------------------------------------------------------------===//
//
// This file provides an example of basic quantum program in which a QBB
// has a very long forward scheduling but simply needs a short backward one.
//
//===----------------------------------------------------------------------===//

/// Circuit that we want to schedule:
///
/// q0: |0> ---H--*--*--X--Z--*--*--X--Z--*--*--X--Z--- D~
///               |  |  |     |  |  |     |  |  |    
/// q1: |0> ---H--X--)--)--Z--X--)--)--Z--X--)--)--Z--- D~
///                  |  |        |  |        |  |
/// q2: |0> ---X--X--X--)--H--X--X--)--H--X--X--)--H--- D~
///               |     |     |     |     |     |
/// q3: |0> ---H--*-----*--H--*-----*--H--*-----*--H--- D~
///
///              |__________|
///           these gates are repeated other 2 times

#include <clang/Quantum/quintrinsics.h>

const int N = 4;
qbit q[N];
cbit c[N];

quantum_kernel void circuit() {
  PrepZ(q[0]);
  PrepZ(q[1]);
  PrepZ(q[2]);
  PrepZ(q[3]);

  H(q[0]);
  H(q[1]);
  Z(q[2]);
  H(q[3]);
  
  for (int i=0; i<3; ++i)
  {
    CNOT(q[0], q[1]);
    CNOT(q[3], q[2]);
    CNOT(q[0], q[2]);
    CNOT(q[0], q[3]);
    Z(q[0]);
    Z(q[1]);
    H(q[2]);
    H(q[3]);
  }
  
  for (int i = 0; i < N; i++)
    MeasZ(q[i], c[i]);
}

//===----------------------------------------------------------------------===//

int main() {

  circuit();
  return 0;

}
