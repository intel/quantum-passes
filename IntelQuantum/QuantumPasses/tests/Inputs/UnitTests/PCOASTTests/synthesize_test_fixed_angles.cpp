#include <clang/Quantum/quintrinsics.h>

const unsigned N = 5;
qbit q[N];
qbit check;

cbit c[N];
cbit c_check;

//only meant to represent a hypothetical inversion meta-function
typedef void(*q_kern)(); 

q_kern quantum_invert(q_kern circ){ return circ; }
q_kern quantum_control(qbit q, q_kern circ){ return circ; }

quantum_kernel void PrepAll(){

  PrepZ(q[0]);
  PrepX(q[1]);
  PrepX(q[2]);
  PrepX(q[3]);
  PrepZ(q[4]);
  PrepX(check);

}

quantum_kernel void circ1(){

  CNOT(q[0], q[4]);
  CNOT(q[1], q[2]);
  CNOT(q[3], q[4]);

  RZ(q[4], 0.1);
  RX(q[2], 0.2);

  CNOT(q[2], q[3]);
  CZ(q[3], q[1]);
  CZ(q[1], check);

  MeasZ(check, c_check);

}

quantum_kernel void cond_true(){

  CNOT(q[2], q[4]);
  CNOT(q[3], q[4]);

  RY(q[3], 0.3);
  RZ(q[2], 0.4);

  CNOT(q[2], q[3]);
  CZ(q[2], q[4]);

}

quantum_kernel void cond_false(){

  Toffoli(q[0], q[2], q[4]);
  CZ(q[1], q[4]);

  RZ(q[4], 0.5);
  RX(q[1], 0.6);

  CNOT(q[2], q[4]);
  CNOT(q[1], q[2]);

}

quantum_kernel void u_to_ctl(){

  CNOT(q[1], q[2]);
  RZ(q[2], 0.7);
  CNOT(q[2], q[3]);
  CZ(q[3], q[4]);
  RY(q[4], 0.8);
  CNOT(q[0], q[4]);
  CZ(q[4], q[2]);
  CNOT(q[2], q[0]);

}

quantum_kernel void MeasAll(){

  MeasX(q[0], c[0]);
  MeasY(q[1], c[1]);
  MeasZ(q[2], c[2]);
  MeasX(q[3], c[3]);
  MeasY(q[4], c[4]);

}


int main() {

  PrepAll();
  circ1();

  if(c_check) cond_true();
  else cond_false();

  quantum_invert(&circ1);

  quantum_control(check, &u_to_ctl);

  MeasAll();

  return 0;

}
