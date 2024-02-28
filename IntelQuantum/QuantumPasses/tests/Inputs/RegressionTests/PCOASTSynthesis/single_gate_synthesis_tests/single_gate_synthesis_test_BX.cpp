#include <clang/Quantum/quintrinsics.h>

  qbit q[5];
  cbit c[5];

quantum_kernel void BX(qbit q1, qbit q2){
  Sdag(q1);
  H(q1);
  CNOT(q1, q2);
  H(q1);
  S(q1);
}

quantum_kernel void BXTest(){
  BX(q[0], q[1]);
}

