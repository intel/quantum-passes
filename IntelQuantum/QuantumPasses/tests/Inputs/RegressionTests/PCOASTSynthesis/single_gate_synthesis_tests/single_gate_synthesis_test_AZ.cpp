#include <clang/Quantum/quintrinsics.h>

  qbit q[5];
  cbit c[5];

quantum_kernel void AZTest(){
  CNOT(q[1], q[0]);
}

