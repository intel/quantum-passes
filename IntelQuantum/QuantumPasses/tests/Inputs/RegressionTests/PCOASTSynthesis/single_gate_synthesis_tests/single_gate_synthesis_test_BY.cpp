#include <clang/Quantum/quintrinsics.h>

  qbit q[5];
  cbit c[5];

quantum_kernel void BY(qbit q1, qbit q2){
 Sdag(q1);
 Sdag(q2);
  H(q1);
  CNOT(q1, q2);
  H(q1);
  S(q1);
  S(q2);
}

quantum_kernel void BYTest(){
  BY(q[0], q[1]);
}

