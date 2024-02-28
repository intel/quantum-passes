#include <clang/Quantum/quintrinsics.h>

const int N =4;
qbit q[N];
cbit c;
double paras[2];

quantum_kernel void BranchTest_fail(bool br){

  double temp;

  if(br){ 
    CNOT(q[0], q[1]);
    RZ(q[0], paras[0]);
    CNOT(q[0], q[1]);
  }
  else{
    CZ(q[0], q[1]);
    RX(q[0], paras[1]);
    CZ(q[0], q[1]);
  }

}

int main(){
  BranchTest_fail(false);
}

