#include <clang/Quantum/quintrinsics.h>

qbit q[3];
cbit c;

quantum_kernel void HCall(){
  H(q[0]);
}

quantum_kernel void XCall(){
  X(q[0]);
}

quantum_kernel void YCall(){
  Y(q[0]);
}

quantum_kernel void ZCall(){
  Z(q[0]);
}

quantum_kernel void SCall(){
  S(q[0]);
}

quantum_kernel void SdagCall(){
  Sdag(q[0]);
}

quantum_kernel void TCall(){
  T(q[0]);
}

quantum_kernel void TdagCall(){
  Tdag(q[0]);
}

quantum_kernel void RXCall(double ang){
  RX(q[0], 0.1);
  RX(q[0], ang);
}

quantum_kernel void RYCall(double ang){
  RY(q[0], 0.2);
  RY(q[0], ang);
}

quantum_kernel void RZCall(double ang){
  RZ(q[0], 0.3);
  RZ(q[0], ang);
}

quantum_kernel void CZCall(){
  CZ(q[0], q[1]);
}

quantum_kernel void CNOTCall(){
  CNOT(q[0], q[1]);
}

quantum_kernel void SWAPCall(){
  SWAP(q[0], q[1]);
}

quantum_kernel void ToffoliCall(){
  Toffoli(q[0], q[1], q[2]);
}

quantum_kernel void PrepXCall(){
  PrepX(q[0]);
}

quantum_kernel void PrepYCall(){
  PrepY(q[0]);
}

quantum_kernel void PrepZCall(){
  PrepZ(q[0]);
}

quantum_kernel void MeasXCall(cbit &carg){
  MeasX(q[0], carg);
  MeasX(q[1], c);
}

quantum_kernel void MeasYCall(cbit &carg){
  MeasY(q[0], carg);
  MeasY(q[1], c);
}

quantum_kernel void MeasZCall(cbit &carg){
  MeasZ(q[0], carg);
  MeasZ(q[1], c);
}

quantum_kernel void CPhaseCall(double arg){
  CPhase(q[0], q[1], arg);
  CPhase(q[0], q[1], .3);
}

quantum_kernel void RXYCall(double arg1, double arg2){
  RXY(q[0], arg1, arg2);
  RXY(q[0], arg1, 0.4);
  RXY(q[0], 0.5, arg2);
  RXY(q[0], 0.6, 0.7);
}

quantum_kernel void SwapACall(double arg){
  SwapA(q[0], q[1], arg);
  SwapA(q[0], q[1], .3);
}

int main(){
  
  HCall();
  XCall();
  YCall();
  SCall();
  SdagCall();
  TCall();
  TdagCall();

  RXCall(1.1);
  RYCall(1.2);
  RZCall(1.3);

  CZCall();
  CNOTCall();
  ToffoliCall();

  PrepXCall();
  PrepYCall();
  PrepZCall();

  cbit cloc;

  MeasXCall(cloc);
  MeasYCall(cloc);
  MeasZCall(cloc);

  return 0;
}