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

  double temp = fake_atan(ang);

  CZ(q[0], q[2]);

  RY(q[2], ang);
  X(q[2]);
  RY(q[2], temp);
  X(q[2]);
  RY(q[2], 0.2);

  CZ(q[0], q[2]);
}

quantum_kernel void four_join_RZ(double ang){

  double temp = fake_atan(ang);

  RZ(q[2], ang);
  RZ(q[2], temp);
  RZ(q[2], 0.2);
  RZ(q[2], ang + fake_atan(0.3));
  
}

quantum_kernel void CPhaseSwapA_join(double ang){

  CPhase(q[0], q[1], ang);
  SwapA(q[0], q[1], fake_atan(0.4));

}

int main() {

  two_join(0.1);
  three_join(0.2);
  four_join_RZ(0.3);
  CPhaseSwapA_join(0.4);
 
  return 0;

}
