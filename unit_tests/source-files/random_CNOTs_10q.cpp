#include "clang/Quantum/quintrinsics.h"

qbit q[10];
cbit c[1];

quantum_kernel void circuit() {
  H(q[0]);
  CNOT(q[0], q[1]);
  CNOT(q[1], q[2]);
  H(q[3]);
  CNOT(q[3], q[1]);
  CNOT(q[3], q[6]);
  H(q[5]);
  CNOT(q[5], q[8]);
  CNOT(q[1], q[5]);
  H(q[6]);
  CNOT(q[6], q[2]);
  CNOT(q[6], q[9]);
  H(q[9]);
  CNOT(q[9], q[0]);
  CNOT(q[8], q[9]);
  CNOT(q[4], q[7]);
  CNOT(q[5], q[3]);
  H(q[2]);
  CNOT(q[2], q[3]);
  CNOT(q[1], q[6]);
  H(q[1]);
  CNOT(q[1], q[0]);
  CNOT(q[9], q[7]);
  CNOT(q[4], q[3]);
  CNOT(q[5], q[7]);
  H(q[4]);
  CNOT(q[4], q[7]);
  CNOT(q[1], q[0]);
  H(q[4]);
  CNOT(q[3], q[2]);
  CNOT(q[5], q[4]);

  //release_quantum_state();
}

int main(){
  circuit();
}

