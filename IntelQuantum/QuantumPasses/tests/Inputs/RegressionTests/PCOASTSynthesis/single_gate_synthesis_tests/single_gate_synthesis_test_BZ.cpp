#include <clang/Quantum/quintrinsics.h>

  qbit q[5];
  cbit c[5];

quantum_kernel void BZ(qbit q1, qbit q2){
  Sdag(q1);
  CNOT(q2, q1);
  S(q1);
}

quantum_kernel void BZTest(){
  BZ(q[0], q[1]);
}

