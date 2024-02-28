#include <clang/Quantum/quintrinsics.h>

  qbit q[5];
  cbit c[5];

quantum_kernel void AY(qbit q1, qbit q2){
  H(q1);
  Sdag(q2);
  CNOT(q1, q2);
  H(q1);
  S(q2);
}

quantum_kernel void AYTest(){
  AY(q[0], q[1]);
}

