#include <clang/Quantum/quintrinsics.h>

qbit q[3];

quantum_kernel void single_Toffoli(){
  Toffoli(q[0], q[1], q[2]);
}

int main(){
  single_Toffoli();
}