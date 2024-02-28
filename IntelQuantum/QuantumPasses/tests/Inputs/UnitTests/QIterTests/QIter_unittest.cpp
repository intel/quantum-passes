#include <clang/Quantum/quintrinsics.h>

qbit q1_glob;
qbit q2_glob[2];

void CtorTest1() {

  qbit q1;
  qbit q2;

  H(q1);
  T(q1);
  CZ(q1, q2);

}

void CtorTest2() {

  qbit q1;
  qbit q2;

  CNOT(q1, q2);
  RZ(q2, 10.1);
  Sdag(q2);
  H(q1);

}

void CtorTest3() {
//Empty function
}

int CtorTest4(){
  int a=1;
  int b=1;
  return a+b;
}

void MoversTest() {

  qbit q1;
  qbit q2;
  qbit q3;
  qbit q4;

  H(q1);
  CZ(q1, q2);
  H(q2);
  H(q4);
  CZ(q1, q4);
  CZ(q2, q3);
  H(q3);
  H(q1);
  H(q4);
  CZ(q2, q1);
  H(q3);

}

void GettersTest() {

  qbit q1;
  qbit q2;

  CNOT(q1, q2);
  RZ(q2, 10.1);
  Sdag(q2);
  H(q1);
  
}

void CommutesTest() {

  qbit q1;
  qbit q2;
  qbit q3;

  H(q1);
  H(q2);
  H(q1);
  CNOT(q1, q2);
  RX(q1, 1.234);
  X(q2);
  RZ(q2, 1.1234);
  Z(q1);
  CZ(q1, q2);
  CNOT(q1, q3);
  CNOT(q3, q2);
  Toffoli(q1, q2, q3);
}

void IRManipulationTest(qbit q1, qbit q2[2]) {

  qbit q3;
  qbit q4[2];
  double ang1;;
  double ang2[2];
  unsigned idx =1;

}

void GlobalQbitRefIRManipulationTest(int idx){

  qbit q1_loc;
  qbit q2_loc;

  CNOT(q1_loc, q2_loc);
  RX(q1_loc, 0.2);
  H(q1_glob);
  H(q2_glob[idx]);

}

void FullExampleTest() {

  qbit q[4];

  CNOT(q[0], q[1]);
  RZ(q[1], 1.1234);
  H(q[1]);
  H(q[2]);
  CNOT(q[1], q[2]);
  RX(q[2], 0.35);
  CNOT(q[2], q[3]);
  H(q[3]);
  CNOT(q[1], q[2]);
  RY(q[2], 0.2);
  H(q[2]);

}
