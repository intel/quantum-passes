#include <clang/Quantum/quintrinsics.h>

  qbit q[5];
  cbit c[5];

quantum_kernel void XSHTest(){
  H(q[0]);
  S(q[0]);
  X(q[0]);
}

