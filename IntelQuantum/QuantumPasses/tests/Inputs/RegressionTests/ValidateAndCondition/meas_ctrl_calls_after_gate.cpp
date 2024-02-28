#include <clang/Quantum/quintrinsics.h>

qbit q[2];
qbit anc;

quantum_kernel double call_after_gate(){

  cbit c;

  CNOT(q[0], q[1]);
  MeasY(q[0], c);
  CNOT(q[0], q[1]);

  release_quantum_state(); 

  double temp = (double) c;
  temp = temp * 0.5;

  return temp;
}

int main(){

  call_after_gate();


}
