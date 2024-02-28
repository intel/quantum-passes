#include <clang/Quantum/quintrinsics.h>

const int N =4;
qbit q[N];
cbit c;
double paras[2];

quantum_kernel void BranchTest_fail(unsigned pow){

  double temp =0.5;

  for(int i =0; i<pow; i++){ 
    temp *= temp;
    RZ(q[0], temp);
  }

}

int main(){
  BranchTest_fail(10);
}

