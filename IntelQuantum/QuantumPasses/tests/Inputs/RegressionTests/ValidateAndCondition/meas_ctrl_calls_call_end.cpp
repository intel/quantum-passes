#include <clang/Quantum/quintrinsics.h>

qbit q[2];
qbit anc;

quantum_kernel double call_end(){

  cbit c;

  CNOT(q[0], q[1]);
  MeasY(q[0], c);
  CNOT(q[0], q[1]);

  double temp = (double) c;
  temp = temp * 0.5;

  release_quantum_state(); 

  return temp;
}

int main(){
  call_end();


}
