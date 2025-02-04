//===----------------------------------------------------------------------===//
// INTEL CONFIDENTIAL
//
// Copyright 2024 Intel Corporation.
//
// This software and the related documents are Intel copyrighted materials, and
// your use of them is governed by the express license under which they were
// provided to you ("License"). Unless the License provides otherwise, you may
// not use, modify, copy, publish, distribute, disclose or transmit this
// software or the related documents without Intel's prior written permission.
//
// This software and the related documents are provided as is, with no express
// or implied warranties, other than those that are expressly stated in the
// License.
//===----------------------------------------------------------------------===//
//
// QFT
// A often used subroutine which transforms between the computatioanl basis and
// the Fourier basis.
//
// QFT can be performed by applying a repeating sequence of Hadamard gate and
// two-qubit controlled phase gates, followed by a series of SWAP gates at the
// end.
//
//===----------------------------------------------------------------------===//

/// Production mode
#include <clang/Quantum/quintrinsics.h>

const int N = 3;
qbit QubitReg[N];
cbit CReg[N];

quantum_kernel void empty_kernel() {
  return;
}

quantum_kernel void simple_intrinsic_block() {
  PrepZ(QubitReg[0]);
}

quantum_kernel void simple_canonical_block() {
  PrepZ(QubitReg[0]);
}

quantum_kernel void simple_kernel_call_block() {
  // Initialization of the qubits
  simple_canonical_block();
}

quantum_kernel void simple_kernel_call_canonical_block() {
  PrepZ(QubitReg[0]);
  PrepZ(QubitReg[1]);
  simple_canonical_block();
  H(QubitReg[0]);
  H(QubitReg[1]);
  H(QubitReg[2]);
}

quantum_kernel void qbitref_dependency() {
  CNOT(QubitReg[0], QubitReg[1]);
  H(QubitReg[0]);
  H(QubitReg[1]);
  CNOT(QubitReg[0], QubitReg[1]);
}

quantum_kernel void qid_dependency() {
  CPhase(QubitReg[0], QubitReg[1], 0);
  RXY(QubitReg[0], 0, 0);
  RXY(QubitReg[1], 0, 0);
  CPhase(QubitReg[0], QubitReg[1], 0);
}

quantum_kernel void basic_line() {
  PrepZ(QubitReg[0]);
  H(QubitReg[0]);
  return;
}

quantum_kernel void basic_line_parallel() {
  PrepZ(QubitReg[0]);
  H(QubitReg[0]);
  PrepZ(QubitReg[1]);
  return;
}

quantum_kernel void basic_loop() {
  for (int Index = 0; Index < N; Index++) {
    PrepZ(QubitReg[Index]);
    H(QubitReg[Index]);
  }
}

quantum_kernel void basic_loop_parallel() {
  for (int Index = 0; Index < N; Index++) {
    PrepZ(QubitReg[Index]);
    H(QubitReg[Index]);
    PrepZ(QubitReg[1]);
  }
}

quantum_kernel void multi_block_loop() {
  for (int Index = 0; Index < N; Index++) {
    PrepZ(QubitReg[Index]);
    H(QubitReg[Index]);
  }
}

quantum_kernel void multi_block_loop_parallel() {
  for (int Index = 0; Index < N; Index++) {
    PrepZ(QubitReg[Index]);
    H(QubitReg[Index]);
    PrepZ(QubitReg[1]);
  }
}

quantum_kernel void basic_branch(bool test) {
  PrepZ(QubitReg[0]);
  H(QubitReg[0]);
  if (test) {
    Z(QubitReg[0]);
  } else {
    X(QubitReg[0]);
  }
}

quantum_kernel void basic_branch_parallel(bool test) {
  PrepZ(QubitReg[0]);
  H(QubitReg[0]);
  PrepZ(QubitReg[1]);
  if (test) {
    Z(QubitReg[0]);
    X(QubitReg[0]);
  } else {
    X(QubitReg[0]);
    Z(QubitReg[1]);
  }
}

quantum_kernel void basic_line_limit() {
  PrepZ(QubitReg[0]);
  PrepZ(QubitReg[1]);
  CNOT(QubitReg[0], QubitReg[1]);
  return;
}

quantum_kernel void kernel_with_double(double test) {
  RZ(QubitReg[0], test);
}

quantum_kernel void kernel_with_int_and_double(int test1, double test2) {
  RZ(QubitReg[0], test2);
}

quantum_kernel void kernel_with_local_qubit(double test2) {
  qbit q_local[2];
  RZ(q_local[0], 3.14);
}

quantum_kernel void kernel_with_qubit_arg_1(qbit *q_arg) {
  RZ(q_arg[0], 3.14);
}

quantum_kernel void kernel_with_qubit_arg_2(qbit *q_arg) {
  RX(qbit[0], 3.14);
}
