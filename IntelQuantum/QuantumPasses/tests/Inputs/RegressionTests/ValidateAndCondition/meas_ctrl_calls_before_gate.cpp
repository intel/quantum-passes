#include <clang/Quantum/quintrinsics.h>

qbit q[2];
qbit anc;

quantum_kernel double call_before_gate(){

  cbit c;

  CNOT(q[0], q[1]);
  MeasY(q[0], c);

  release_quantum_state();
  
  CNOT(q[0], q[1]); 

  double temp = (double) c;
  temp = temp * 0.5;

  return temp;
}

int main(){

  call_before_gate();


}
