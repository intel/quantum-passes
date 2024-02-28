#include <clang/Quantum/quintrinsics.h>

#include <cmath>

const unsigned N = 5;
qbit q[N];

quantum_kernel void prep_tof(){
  
  PrepZ(q[0]);
  Toffoli(q[0], q[1], q[2]);

}

quantum_kernel void prep_c_cliffords(){
  PrepZ(q[0]);
  CZ(q[0], q[1]);
  CNOT(q[0], q[2]);
  S(q[3]);
  CNOT(q[0], q[3]);
  Sdag(q[3]);
}

quantum_kernel void prep_after_circuit(){
  CNOT(q[0], q[1]);
  RZ(q[1], 0.1);
  CZ(q[1], q[2]);
  RX(q[2], 0.2);
  CNOT(q[2], q[3]); 
  S(q[3]);
  CNOT(q[0], q[3]);
  Sdag(q[3]);

  PrepX(q[0]);
  PrepX(q[1]);
  PrepX(q[2]);
  PrepX(q[3]);
  PrepX(q[4]);
  
}

int main(){
  prep_tof();
  prep_c_cliffords();
  prep_after_circuit();
}


