#include <clang/Quantum/quintrinsics.h>

  qbit q[5];
  cbit c[5];

quantum_kernel void XHSdagTest(){
  Sdag(q[0]);
  H(q[0]);
  X(q[0]);
}
