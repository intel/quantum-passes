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

quantum_kernel double call_before_branch(unsigned pow, double ang){
 
  cbit c;
  double temp = ang;

  CNOT(q[0], q[1]);
  MeasY(q[0], c);

  release_quantum_state(); 

  CNOT(q[0], q[1]);

  for(int i =0; i<pow; i++) temp *= temp;
  
   RZ(q[0], temp);

  temp = (double) c;
  temp = temp * 0.5;

  return temp;

}

quantum_kernel double call_after_branch(unsigned pow, double ang){
 
  cbit c;
  double temp = ang;

  CNOT(q[0], q[1]);
  MeasY(q[0], c);
  CNOT(q[0], q[1]);

  for(int i =0; i<pow; i++) temp *= temp;
  
  release_quantum_state(); 
  RZ(q[0], temp);

  temp = (double) c;
  temp = temp * 0.5;

  return temp;

}

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

  call_before_gate();
  call_after_gate();
  call_end();


}
