#include <clang/Quantum/quintrinsics.h>

  qbit q[5];
  cbit c[5];

quantum_kernel void AX(qbit q1, qbit q2){
  H(q1);
  H(q2);
  CZ(q1, q2);
  H(q1);
  H(q2);
}

quantum_kernel void AXTest(){
  AX(q[0], q[1]);
}

