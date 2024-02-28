#include <clang/Quantum/quintrinsics.h>

#include <cmath>

const unsigned N = 5;
qbit q[N];

quantum_kernel void prep_meas_0(){
  cbit c[2];
  
  PrepZ(q[0]);
  CNOT(q[0], q[1]);
  MeasX(q[0], c[0]);
  MeasZ(q[1], c[1]);

  release_quantum_state();

}

quantum_kernel void prep_meas_0_alt(){
  cbit c[2];
  
  PrepZ(q[0]);
  CNOT(q[1], q[0]);
  MeasX(q[1], c[0]);
  MeasZ(q[0], c[1]);

  release_quantum_state();

}

quantum_kernel void prep_meas_1(){
  cbit c[N];
  
  PrepZ(q[0]);
  PrepZ(q[2]);
  PrepY(q[4]);

  CNOT(q[0], q[1]);
  CNOT(q[1], q[2]);
  CZ(q[2], q[3]);
  CNOT(q[4], q[3]);

  MeasX(q[0], c[0]);
  MeasZ(q[1], c[1]);
  MeasY(q[2], c[2]);
  MeasX(q[3], c[3]);
  MeasY(q[4], c[4]);

  release_quantum_state();

}

quantum_kernel void prep_rot_meas(){
  cbit c[N];
  
  PrepZ(q[0]);
  PrepZ(q[2]);
  PrepY(q[4]);

  CNOT(q[1], q[0]);
  RY(q[1], 0.1);
  CNOT(q[1], q[0]);

  CZ(q[3], q[4]);
  RX(q[4], 0.2);
  CZ(q[3], q[4]);


  CNOT(q[0], q[1]);
  CNOT(q[1], q[2]);
  CZ(q[2], q[3]);
  CNOT(q[4], q[3]);

  MeasX(q[0], c[0]);
  MeasZ(q[1], c[1]);
  MeasY(q[2], c[2]);
  MeasX(q[3], c[3]);
  MeasY(q[4], c[4]);

  release_quantum_state();

}


int main(){

prep_meas_0();

}


