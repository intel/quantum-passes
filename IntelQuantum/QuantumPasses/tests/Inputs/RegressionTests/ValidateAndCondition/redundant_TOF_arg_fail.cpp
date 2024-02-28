#include <clang/Quantum/quintrinsics.h>

qbit q[2];

quantum_kernel void bad_TOF(){
  Toffoli(q[0], q[1], q[1]);
}

int main(){
  bad_TOF();
  return 0;
}