#include <clang/Quantum/quintrinsics.h>

qbit q[5];
cbit c[7];

#include <cmath>

const double PIby2 = std::asin(1);

quantum_kernel void All_Consensus() {
  
  RX(q[0], 0.1);
  CNOT(q[0], q[1]);
  RZ(q[1], 0.2);
  CNOT(q[0], q[1]);

  MeasZ(q[0], c[0]);
  CNOT(q[0], q[1]);
  MeasZ(q[1], c[1]);
  CNOT(q[1], q[2]);
  MeasZ(q[2], c[2]);
  CNOT(q[2], q[3]);
  MeasZ(q[3],c[3]);
  CNOT(q[3], q[4]);
  MeasZ(q[4], c[4]);

  release_quantum_state();

}

quantum_kernel void Some_Consensus(){

  RX(q[0], 0.1);
  CNOT(q[0], q[1]);
  RZ(q[1], 0.2);
  CNOT(q[0], q[1]);

  MeasZ(q[2], c[0]);
  CNOT(q[0], q[1]);
  MeasX(q[0], c[1]);
  RX(q[1], PIby2);
  CNOT(q[1], q[2]);
  MeasZ(q[2], c[2]);

  release_quantum_state();

}

quantum_kernel void No_Consensus(){

  RX(q[0], 0.1);
  CNOT(q[0], q[1]);
  RZ(q[1], 0.2);
  CNOT(q[0], q[1]);

  CNOT(q[0], q[1]);
  CNOT(q[1], q[2]);
  CZ(q[0], q[2]);
  MeasY(q[0],c[0]);
  MeasY(q[1], c[1]);
  MeasZ(q[2], c[2]);

  release_quantum_state();

}

quantum_kernel void Dependency(){

  RX(q[0], 0.1);
  CNOT(q[0], q[1]);
  RZ(q[1], 0.2);
  CNOT(q[0], q[1]);

  CNOT(q[0], q[1]);
  MeasX(q[0], c[0]);
  MeasZ(q[1], c[1]);
  CZ(q[0], q[1]);
  MeasX(q[0], c[2]);

  release_quantum_state();

}

int main(){
  All_Consensus();
  Some_Consensus();
  No_Consensus();
  Dependency();
}

