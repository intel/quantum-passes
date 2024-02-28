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

quantum_kernel void CircuitTest1(){
 
  CNOT(q[0], q[1]);
  CNOT(q[1], q[2]);
  CNOT(q[2], q[3]);
  CNOT(q[3], q[4]);
  RZ(q[4], 0.4); //exp(-i 0.4/2 Z0 Z1 Z2 Z3 Z4 )
  CNOT(q[3], q[4]);
  CNOT(q[2], q[3]);
  CNOT(q[1], q[2]);
  CNOT(q[0], q[1]);

  CZ(q[0], q[1]);
  CZ(q[1], q[2]);
  CZ(q[2], q[3]);
  CZ(q[3], q[4]);
  RX(q[3], 0.5); //exp(-i 0.5/2 Z2 X3 Z4 ) 
  CZ(q[3], q[4]);
  CZ(q[2], q[3]);
  CZ(q[1], q[2]);
  CZ(q[0], q[1]);

  BY(q[0], q[1]);
  BY(q[1], q[2]);
  BY(q[2], q[3]);
  BY(q[3], q[4]);
  RX(q[3], 0.6); //exp(-i 0.6/2 Y2 X3 Y4 ) 
  RY(q[3], 0.7); //exp(-i 0.7/2 Y3);
  BY(q[3], q[4]);
  BY(q[2], q[3]);
  BY(q[1], q[2]);
  BY(q[0], q[1]);

}

quantum_kernel void CircuitTest2(){

PrepZ(q[0]); //as is
PrepZ(q[1]); //as is
PrepZ(q[2]); //as is
AY(q[2], q[1]);
BX(q[0], q[2]);
RX(q[2], 0.8); //exp (-i 0.8/2 X2)
CZ(q[1], q[2]);
RY(q[1], 0.9); //exp (-i 0.9/2 Y0 Z2)
CNOT(q[0], q[2]);
RX(q[0], 1.0); //exp (-i 1.0/2 X0 Z1 X2)
BZ(q[1], q[0]);
AX(q[0],q[2]);
RY(q[2], 1.1); // exp(-i 1.1/2 Y0 Y1)
BZ(q[0], q[1]);
MeasZ(q[0], c[0]); // Meas(X2)

      //End Frame                 ( -----HSdag--x----  ) -1
      // [       X2        Y2]    |             |      |
      // [ Z0 Z1     X0 Z1   ]  = | --Y---------x--x-- |
      // [-Y0 X1        Z1   ]    |   |            |   |
      //                          ( --X---ZH-------x-- )

}


