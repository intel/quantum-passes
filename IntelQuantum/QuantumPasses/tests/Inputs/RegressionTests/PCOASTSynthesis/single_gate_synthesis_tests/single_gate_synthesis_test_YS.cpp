#include <clang/Quantum/quintrinsics.h>

  qbit q[5];
  cbit c[5];

quantum_kernel void YSTest(){
  S(q[0]);
  Y(q[0]);
}