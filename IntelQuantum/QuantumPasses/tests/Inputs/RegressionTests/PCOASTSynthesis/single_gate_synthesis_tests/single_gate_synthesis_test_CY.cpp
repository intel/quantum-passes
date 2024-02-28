#include <clang/Quantum/quintrinsics.h>

  qbit q[5];
  cbit c[5];

quantum_kernel void CY(qbit q1, qbit q2){
  Sdag(q2);
  H(q2);
  CZ(q1, q2);
  H(q2);
  S(q2);
}

quantum_kernel void CYTest(){
  CY(q[0], q[1]);
}

