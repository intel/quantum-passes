#include <clang/Quantum/quintrinsics.h>

  qbit q[5];
  cbit c[5];

quantum_kernel void SdagTest(){
  Sdag(q[0]);
}