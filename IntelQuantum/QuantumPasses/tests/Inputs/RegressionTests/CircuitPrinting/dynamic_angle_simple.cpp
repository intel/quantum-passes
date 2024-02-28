#include <clang/Quantum/quintrinsics.h>

const unsigned N = 5;
qbit q[N];
qbit check;

cbit c[N];
cbit c_check;

double fake_atan(double a) {return a + 1.;}


quantum_kernel void two_join(double ang){

  CNOT(q[0], q[1]); 
  RX(q[1], ang);
  RX(q[1], 0.2);
  CNOT(q[0], q[1]);

}

quantum_kernel void three_join(double ang){

  CZ(q[0], q[2]);
  RY(q[2], ang);
  RY(q[2], ang + 2.0);
  RY(q[2], 3.0 - ang);
  RXY(q[0], 1.1, 2.2);
  // RY(q[0], ang + ang - .5 + 1.5 * 2.0 * ang);
  // RY(q[0], ang + ang - .5 + 1.5 * 2.0 );

}

quantum_kernel void four_join_RZ(double ang){
  double temp = fake_atan(ang);
  RZ(q[2], ang);
  RZ(q[2], 0.2);
  RZ(q[2], ang + temp);  
}


int main() {

  two_join(0.1);
  three_join(0.2);
  four_join_RZ(0.3);


  return 0;

}
