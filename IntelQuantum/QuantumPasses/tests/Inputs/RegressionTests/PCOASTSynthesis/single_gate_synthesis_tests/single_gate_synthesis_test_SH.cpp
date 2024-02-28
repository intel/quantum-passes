#include <clang/Quantum/quintrinsics.h>

  qbit q[5];
  cbit c[5];

quantum_kernel void SHTest(){
  H(q[0]);
  S(q[0]);
}

