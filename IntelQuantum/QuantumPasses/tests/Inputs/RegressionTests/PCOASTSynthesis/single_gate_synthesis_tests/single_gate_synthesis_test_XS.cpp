#include <clang/Quantum/quintrinsics.h>

  qbit q[5];
  cbit c[5];

quantum_kernel void XSTest(){
  S(q[0]);
  X(q[0]);
}