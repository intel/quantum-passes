#include <clang/Quantum/quintrinsics.h>

  qbit q[5];
  cbit c[5];

quantum_kernel void HSdagTest(){
  Sdag(q[0]);
  H(q[0]);
}

quantum_kernel void SHTest(){
  H(q[0]);
  S(q[0]);
}

quantum_kernel void HTest(){
  H(q[0]);
}

quantum_kernel void KDagTest(){
  H(q[0]);
  Sdag(q[0]);
  H(q[0]);
}

quantum_kernel void STest(){
  S(q[0]);
}

quantum_kernel void ZTest(){
  Z(q[0]);
}

quantum_kernel void ZHSdagTest(){
  Sdag(q[0]);
  H(q[0]);
  Z(q[0]);
}

quantum_kernel void ZSHTest(){
  H(q[0]);
  S(q[0]);
  Z(q[0]);
}

quantum_kernel void ZHTest(){
  H(q[0]);
  Z(q[0]);
}

quantum_kernel void ZKDagTest(){
  H(q[0]);
  Sdag(q[0]);
  H(q[0]);
  Z(q[0]);
}

quantum_kernel void SdagTest(){
  Sdag(q[0]);
}

quantum_kernel void XTest(){
  X(q[0]);
}

quantum_kernel void XHSdagTest(){
  Sdag(q[0]);
  H(q[0]);
  X(q[0]);
}

quantum_kernel void XSHTest(){
  H(q[0]);
  S(q[0]);
  X(q[0]);
}

quantum_kernel void XHTest(){
  H(q[0]);
  X(q[0]);
}

quantum_kernel void XKDagTest(){
  H(q[0]);
  S(q[0]);
  H(q[0]);
}

quantum_kernel void XSTest(){
  S(q[0]);
  X(q[0]);
}

quantum_kernel void YTest(){
  Y(q[0]);
}

quantum_kernel void YHSdagTest(){
  Sdag(q[0]);
  H(q[0]);
  Y(q[0]);
}

quantum_kernel void YSHTest(){
  H(q[0]);
  S(q[0]);
  Y(q[0]);
}

quantum_kernel void YHTest(){
  H(q[0]);
  Y(q[0]);
}

quantum_kernel void YKDagTest(){
  H(q[0]);
  Sdag(q[0]);
  H(q[0]);
  Y(q[0]);
}

quantum_kernel void YSTest(){
  S(q[0]);
  Y(q[0]);
}

quantum_kernel void TTest(){
  T(q[0]);
}

quantum_kernel void TdagTest(){
  Tdag(q[0]);
}

quantum_kernel void RXTest(){
  RX(q[0], 0.1);
}

quantum_kernel void RYTest(){
  RY(q[0], 0.2);
}

quantum_kernel void RZTest(){
  RZ(q[0], 0.3);
}

quantum_kernel void AX(qbit q1, qbit q2){
  H(q1);
  H(q2);
  CZ(q1, q2);
  H(q1);
  H(q2);
}

quantum_kernel void AXTest(){
  AX(q[0], q[1]);
}

quantum_kernel void AY(qbit q1, qbit q2){
  H(q1);
  Sdag(q2);
  CNOT(q1, q2);
  H(q1);
  S(q2);
}

quantum_kernel void AYTest(){
  AY(q[0], q[1]);
}

quantum_kernel void AZTest(){
  CNOT(q[1], q[0]);
}

quantum_kernel void BX(qbit q1, qbit q2){
  Sdag(q1);
  H(q1);
  CNOT(q1, q2);
  H(q1);
  S(q1);
}

quantum_kernel void BXTest(){
  BX(q[0], q[1]);
}

quantum_kernel void BY(qbit q1, qbit q2){
 Sdag(q1);
 Sdag(q2);
  H(q1);
  CNOT(q1, q2);
  H(q1);
  S(q1);
  S(q2);
}

quantum_kernel void BYTest(){
  BY(q[0], q[1]);
}

quantum_kernel void BZ(qbit q1, qbit q2){
  Sdag(q1);
  CNOT(q2, q1);
  S(q1);
}

quantum_kernel void BZTest(){
  BZ(q[0], q[1]);
}

quantum_kernel void CXTest(){
  CNOT(q[0], q[1]);
}

quantum_kernel void CY(qbit q1, qbit q2){
  Sdag(q2);
  H(q2);
  CZ(q1, q2);
  H(q2);
  S(q2);
}

quantum_kernel void CYTest(){
  CY(q[0], q[1]);
}

quantum_kernel void CZTest(){
  CZ(q[0], q[1]);
}

quantum_kernel void SWAPTest(){
  SWAP(q[0], q[1]);
}

quantum_kernel void ToffoliTest(){
  Toffoli(q[0], q[1], q[2]);
}

quantum_kernel void MeasXTest(){
  MeasX(q[0], c[0]);
}

quantum_kernel void MeasYTest(){
  MeasY(q[0], c[0]);
}

quantum_kernel void MeasZTest(){
  MeasZ(q[0], c[0]);
}

quantum_kernel void PrepXTest(){
  PrepX(q[0]);
}

quantum_kernel void PrepYTest(){
  PrepY(q[0]);
}

quantum_kernel void PrepZTest(){
  PrepZ(q[0]);
}