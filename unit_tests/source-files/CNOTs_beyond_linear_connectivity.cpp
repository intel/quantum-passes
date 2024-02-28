#include "clang/Quantum/quintrinsics.h"

qbit q[5];
cbit c[2];

quantum_kernel void circuit() {
  H(q[0]);
  CNOT(q[0], q[1]);
  CNOT(q[1], q[2]);
  CNOT(q[3], q[0]);
  CNOT(q[1], q[4]);
  CNOT(q[4], q[2]);
  CNOT(q[2], q[3]);
  // Comment or uncomment next line to change the scheduler behavior
  // for the backward routing.
  //release_quantum_state();
}

int main(){
  circuit();
}

