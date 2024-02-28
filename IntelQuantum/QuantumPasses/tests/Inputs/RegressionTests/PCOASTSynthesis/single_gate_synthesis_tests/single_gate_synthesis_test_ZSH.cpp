#include <clang/Quantum/quintrinsics.h>

  qbit q[5];
  cbit c[5];

quantum_kernel void ZSHTest(){
  H(q[0]);
  S(q[0]);
  Z(q[0]);
}

