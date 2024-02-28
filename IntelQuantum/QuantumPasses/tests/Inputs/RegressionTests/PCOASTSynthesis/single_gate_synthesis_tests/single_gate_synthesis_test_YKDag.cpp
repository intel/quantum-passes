#include <clang/Quantum/quintrinsics.h>

  qbit q[5];
  cbit c[5];

quantum_kernel void YKDagTest(){
  H(q[0]);
  Sdag(q[0]);
  H(q[0]);
  Y(q[0]);
}

