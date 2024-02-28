#include <clang/Quantum/quintrinsics.h>

qbit q[3];
cbit c[3];


quantum_kernel void PrepAllZ(){  
  PrepZ(q[0]);
  PrepZ(q[1]);
  PrepZ(q[2]);
}

quantum_kernel void ForwardRx(double arg){
  RX(q[0], arg);
  RX(q[1], arg);
  RX(q[2], arg);
}

quantum_kernel void BackwardRx(double arg){
  RX(q[0], arg);
  RX(q[1], arg);
  RX(q[2], arg);
}

quantum_kernel void MeasALLZ(){
  MeasZ(q[0], c[0]);
  MeasZ(q[1], c[1]);
  MeasZ(q[2], c[2]);
}

int main(){
  
  PrepAllZ();
  ForwardRx(1.1);
  BackwardRx(-1.1);
  MeasALLZ();

  return 0;
}