#include <clang/Quantum/quintrinsics.h>

#include <cmath>

const unsigned N = 5;
qbit q[N];

quantum_kernel void prep_transform_meas(double ang){
  cbit c;
  
  PrepZ(q[0]);
  Toffoli(q[0], q[1], q[2]);
  RZ(q[2], ang);
  Toffoli(q[0], q[1], q[2]);
  MeasX(q[1], c);

  release_quantum_state();

}

quantum_kernel void scramble(double angs[]){
  cbit c[N];

  PrepZ(q[0]);
  PrepZ(q[1]);
  PrepZ(q[2]);
  PrepZ(q[3]);
  PrepZ(q[4]);

  H(q[2]);
  S(q[4]);
  CNOT(q[2], q[4]);
  RX(q[2], angs[0]);
  CZ(q[4], q[1]);
  CZ(q[1], q[0]);
  double temp = std::atan(angs[1]);
  RY(q[0], temp);
  Toffoli(q[2], q[3], q[4]);
  CZ(q[4], q[0]);
  Toffoli(q[2], q[3], q[4]);
  RX(q[0], angs[2] + angs[3]);
  H(q[0]);
  Toffoli(q[0], q[1], q[2]);


  MeasZ(q[0], c[0]);
  MeasZ(q[1], c[1]);
  MeasZ(q[2], c[2]);
  MeasZ(q[3], c[3]);
  MeasZ(q[4], c[4]);

  release_quantum_state();

}

int main(){
  double ang = 0.1;
  double angs[5] = {0.2, 0.3, 0.4, 0.5, 0.6};

  prep_transform_meas(ang);
  scramble(angs);

}


