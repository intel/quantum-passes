#include <clang/Quantum/quintrinsics.h>

qbit q[2];
qbit anc;

quantum_kernel double double_call_fail(){

  cbit c;

  CNOT(q[0], q[1]);
  MeasY(q[0], c);

  release_quantum_state();
  
  CNOT(q[0], q[1]); 

  double temp = (double) c;
  temp = temp * 0.5;

  release_quantum_state();

  return temp;
}

int main(){

  double_call_fail();
  return 0;

}
