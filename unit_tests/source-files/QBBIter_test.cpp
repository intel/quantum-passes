#include "clang/Quantum/quintrinsics.h"

qbit q[2];
cbit test;
double para[2];

quantum_kernel void MultipleQBBTest(){

  H(q[0]);
  CNOT(q[0], q[1]);
  RZ(q[1], para[0]);
  MeasZ(q[0], test);

  if(test){
    CNOT(q[0], q[1]);
    H(q[0]);
  }

}