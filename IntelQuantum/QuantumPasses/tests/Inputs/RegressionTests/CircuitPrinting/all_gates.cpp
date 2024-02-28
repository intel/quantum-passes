#include <clang/Quantum/quintrinsics.h>

qbit q[3];
cbit c[3];


quantum_kernel void all(){
  PrepX(q[0]);
  PrepZ(q[1]);
  PrepY(q[2]);
  X(q[0]);
  H(q[0]);
  Y(q[1]);
  Z(q[0]);
  S(q[2]);
  Sdag(q[0]);
  T(q[1]);
  Tdag(q[2]);
  RX(q[0], 0.1);
  RZ(q[1], 0.3);
  RY(q[2], 0.8);
  RXY(q[2], 0.5, 0.6);
  CZ(q[0], q[1]);
  CNOT(q[2], q[1]);
  SwapA(q[0], q[1], .3);
  Toffoli(q[0], q[1], q[2]);
  SWAP(q[0], q[2]);
  MeasX(q[1], c[1]);
  MeasZ(q[2], c[2]);
  MeasY(q[0], c[0]);
}



int main(){
  all();
  return 0;
}