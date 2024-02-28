#include <clang/Quantum/quintrinsics.h>

  qbit q[5];
  cbit c[5];

quantum_kernel void ZHSdagTest(){
  Sdag(q[0]);
  H(q[0]);
  Z(q[0]);
}
