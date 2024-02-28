//===----------------------------------------------------------------------===//
//
// Copyright (C) 2021 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//
//===----------------------------------------------------------------------===//
//
// MBL
// A single Trotter step of a 3-qubit version of the Many Body Localization
// (MBL) Algorithm.
//
// References:
//
// Theory:
//
// 1. S. Johri, R. Nandkishore, and R. N. Bhatt, "Many-body localization in
// imperfectly isolated quantum systems," Phys. Rev. Lett., vol. 114, Mar. 2015,
// Art. no. 117401, doi: 10.1103/PhysRevLett.114.117401.
//
// 2. S. D. Geraedts, R. Nandkishore, and N. Regnault, "Many-body localization
// and thermalization: Insights from the entanglement spectrum," Phys. Rev. B,
// vol. 93, May 2016, Art. no. 174202, doi: 10.1103/PhysRevB.93.174202.
//
// Architectural study:
//
// 1. X. Zou et al., "Enhancing a Near-Term Quantum Accelerator's Instruction
// Set Architecture for Materials Science Applications," in IEEE Transactions on
// Quantum Engineering, vol. 1, pp. 1-7, 2020, Art no. 4500307,
// doi: 10.1109/TQE.2020.2965810.
//===----------------------------------------------------------------------===//

/// NOTE: Pick an include path depending on whether running in development
/// enviromnment or production environment
//===----------------------------------------------------------------------===//
#include <clang/Quantum/quintrinsics.h>

#include <cstdlib>
#include <cassert>
#include <functional>
#include <iostream>
#include <math.h>
#include <string.h>

const int N = 3;
qbit q[N];
cbit c[N];

quantum_kernel void mbl_q3_1ts() {

  for (int i = 0; i < N; i++) {
    PrepZ(q[i]);
  }

  X(q[0]);
  X(q[2]);

  CNOT(q[0], q[1]);
  RZ(q[0], 9.563581772879);
  RZ(q[1], 5.2);
  H(q[0]);              
  CNOT(q[0], q[1]);     
  RZ(q[0], 5.2);
  RZ(q[1], -5.2);
  CNOT(q[0], q[1]);     
  H(q[0]);              
  CNOT(q[0], q[1]);     

  RZ(q[2], 1.415202001624);
  CNOT(q[1], q[2]);     
  RZ(q[1], 7.070381830323);
  RZ(q[2], 5.2);
  H(q[1]);              
  CNOT(q[1], q[2]);     
  RZ(q[1], 5.2);
  RZ(q[2], -5.2);
  CNOT(q[1], q[2]);     
  H(q[1]);              
  CNOT(q[1], q[2]);     

  RX(q[0], 4.94709917593);
  RX(q[1], 5.041840372001);
  RX(q[2], 2.56278001524);

  // for (int i = 0; i < N; i++) {
  //   MeasZ(q[i], c[i]);
  // }
}

quantum_kernel void cnot01() {
  RY(q[1], -0.5 * M_PI);
  CZ(q[0], q[1]);
  RXY(q[1], 0.5 * M_PI, -0.1);
}

quantum_kernel void cnot10() {
  RY(q[0], -0.5 * M_PI);
  CZ(q[1], q[0]);
  RXY(q[0], 0.5 * M_PI, 0.1);
}

quantum_kernel void cnot12() {
  RY(q[2], -0.5 * M_PI);
  CZ(q[1], q[2]);
  RXY(q[2], 0.5 * M_PI, -0.1);
}

quantum_kernel void cnot21() {
  RY(q[1], -0.5 * M_PI);
  CZ(q[2], q[1]);
  RXY(q[1], 0.5 * M_PI, 0.1);
}

quantum_kernel void mbl_q3_1ts_cpp_QDSIM() {
  for (int i = 0; i < N; i++) {
    PrepZ(q[i]);
  }

  X(q[0]);
  X(q[2]);

  cnot01();
  RZ(q[0], 9.563581772879);
  RZ(q[1], 5.2);
  H(q[0]);              
  cnot01();     
  RZ(q[0], 5.2);
  RZ(q[1], -5.2);
  cnot01();     
  H(q[0]);              
  cnot01();     

  RZ(q[2], 1.415202001624);
  cnot12();     
  RZ(q[1], 7.070381830323);
  RZ(q[2], 5.2);
  H(q[1]);              
  cnot12();     
  RZ(q[1], 5.2);
  RZ(q[2], -5.2);
  cnot12();     
  H(q[1]);              
  cnot12();     

  RX(q[0], 4.94709917593);
  RX(q[1], 5.041840372001);
  RX(q[2], 2.56278001524);

#if 0
  for (int i=0; i<N; i++)
    MeasZ(q[i], c[i]);
#elif 0
  for (int i=0; i<N; i++)
    MeasZ(q[i], c[i]);

  release_quantum_state();
#endif
}


// Should change intel-quantum-sdk.json to use Quantum_SDK_Prototype_7q_configuration_python_qd_sim.json configuration
// This example is only for internal development purpose, and Service_Quantum_Device will not get built in the release build.
int main() {

  std::cout << "\n\n----- Running mbl_q3_1t -----\n";
  mbl_q3_1ts();

  // --------------------------------------------------------------------------
  // --------------------------------------------------------------------------

  std::cout << "\n\n----- Running mbl_q3_1t_cpp_QDSIM -----\n";
  mbl_q3_1ts_cpp_QDSIM();

}
