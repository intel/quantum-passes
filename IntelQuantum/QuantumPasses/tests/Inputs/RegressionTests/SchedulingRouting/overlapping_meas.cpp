#include <clang/Quantum/quintrinsics.h>

qbit q[3];

quantum_kernel void multi_meas(){

  cbit c[9];
  MeasZ(q[0], c[0]);
  MeasZ(q[1], c[1]);
  MeasZ(q[2], c[2]);
  MeasZ(q[0], c[3]);
  MeasZ(q[1], c[4]);
  MeasZ(q[2], c[5]);
  MeasZ(q[0], c[6]);
  MeasZ(q[1], c[7]);
  MeasZ(q[2], c[8]);
  
}

int main(){
  multi_meas();
}