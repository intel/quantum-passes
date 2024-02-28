#include <clang/Quantum/quintrinsics.h>

qbit q[2];

quantum_kernel void bad_CNOT(){
  CNOT(q[0], q[0]);
}

int main(){
  bad_CNOT();
  return 0;
}