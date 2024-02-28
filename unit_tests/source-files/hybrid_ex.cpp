#include "clang/Quantum/quintrinsics.h"
//#include <iostream>

qbit q[3];
cbit c[3];

quantum_kernel void PrepAll(){
  PrepX(q[0]);
  PrepX(q[1]);
  PrepX(q[2]);
}

quantum_kernel void VQE_anzatz(double angs[]){
  RX(q[0], angs[0]);
  RX(q[1], angs[1]);
  RX(q[2], angs[2]);
  CNOT(q[0], q[1]);
  RY(q[1], angs[3]);
  CNOT(q[1], q[2]);
  RZ(q[2], angs[4]);
  CNOT(q[1], q[2]);
  CNOT(q[0], q[1]);
}

bool optimize_fake(double angs[], double cost){
  return cost > 0;
}

double Energy(){
    return 0.23*(1-2*(double)c[0]) - 1.34*(1-2*(double)c[1]) + 2.14*(1-2*(double)c[2]) + 0.13*(1-2*(double)c[2])*(1-2*(double)c[0]);
}

quantum_kernel void MeasH(){
  MeasZ(q[0], c[0]);
  MeasX(q[1], c[1]);
  MeasY(q[2], c[2]);
}

int main(){

    double angs[] = {1., 1., 1., 1., 1.};

    while(optimize_fake(angs, Energy())){
        PrepAll();
        VQE_anzatz(angs);
        MeasH();
    }

    //print result but not actually doing that.
    return 0;
}