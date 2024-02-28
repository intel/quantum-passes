#include <clang/Quantum/quintrinsics.h>

qbit q[5];
cbit c[7];

quantum_kernel void GHZ_5() {
  H(q[0]);
  CNOT(q[0], q[1]);
  CNOT(q[1], q[2]);
  CNOT(q[2], q[3]);
  CNOT(q[3], q[4]);
}

int main(){
  GHZ_5();
}

