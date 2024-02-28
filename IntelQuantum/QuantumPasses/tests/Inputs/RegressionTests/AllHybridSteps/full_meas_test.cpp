#include <clang/Quantum/quintrinsics.h>

const int N =4;
qbit q[N];
qbit q_check;
cbit result1[3];
cbit result2[3];
cbit c_check;

quantum_kernel void PrepQs(){

  PrepZ(q[0]);
  PrepZ(q[1]);
  PrepZ(q[2]);
  PrepZ(q[3]);
  PrepX(q_check);

}

quantum_kernel void MidCircMeas(){

  CNOT(q[0], q[1]);
  RZ(q[1], 0.1);
  CNOT(q[1], q[2]);
  RY(q[2], 0.2);
  CZ(q[2], q_check);
  MeasZ(q_check, c_check);
  CNOT(q[1], q[2]);
  CNOT(q[0], q[1]);

  MeasY(q[0], result1[0]);
  MeasY(q[1], result1[1]);
  MeasY(q[2], result1[2]);

}

quantum_kernel void CondTrue(){

  CNOT(q[1], q[3]);
  RZ(q[3], 0.3);
  CNOT(q[2], q[3]);
  RZ(q[3], 0.4);
  CZ(q[0], q[3]);
  RX(q[3], 0.5);
  
}

quantum_kernel void MeasureFinal(){
  
  MeasY(q[1], result2[0]);
  MeasY(q[2], result2[1]);
  MeasY(q[3], result2[2]);

}

int main(){

  PrepQs();
  MidCircMeas();

  if(c_check){
    CondTrue();
  }

  MeasureFinal();

  return 0;

}
