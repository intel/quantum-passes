#include <clang/Quantum/quintrinsics.h>

  qbit q[5];
  cbit c[5];


quantum_kernel void XTest(){
  X(q[0]);
}
