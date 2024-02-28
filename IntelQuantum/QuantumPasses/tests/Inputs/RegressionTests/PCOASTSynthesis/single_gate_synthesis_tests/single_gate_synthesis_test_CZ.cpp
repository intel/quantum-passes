#include <clang/Quantum/quintrinsics.h>

  qbit q[5];
  cbit c[5];

quantum_kernel void CZTest(){
  CZ(q[0], q[1]);
}

