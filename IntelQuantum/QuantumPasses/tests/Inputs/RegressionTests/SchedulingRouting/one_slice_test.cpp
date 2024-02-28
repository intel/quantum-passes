#include <clang/Quantum/quintrinsics.h>

qbit q[5];
cbit c;

quantum_kernel void EmptyKernel(){}

quantum_kernel void OneGateKernel(){
  MeasZ(q[0], c);
}

quantum_kernel void SomeGateKernel(){
  H(q[0]);
  CNOT(q[0], q[1]);
}

quantum_kernel void ManyGateKernel(){
  PrepZ(q[0]);
  PrepX(q[1]);
  H(q[0]);
  CNOT(q[0], q[1]);
  S(q[2]);
  RX(q[0], 0.1);
  CNOT(q[0], q[1]);
  MeasZ(q[0], c);
}

int main(){
  
  EmptyKernel();
  OneGateKernel();
  SomeGateKernel();
  ManyGateKernel();

  return 0;
  
}