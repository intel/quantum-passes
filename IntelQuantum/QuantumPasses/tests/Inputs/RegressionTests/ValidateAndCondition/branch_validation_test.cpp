#include <clang/Quantum/quintrinsics.h>

const int N =4;
qbit q[N];
cbit c;
double paras[2];

quantum_kernel void BranchTest(bool br, unsigned pow){

  double temp;

  if(br) temp = paras[0];
  else temp = paras[1];

  RZ(q[0], temp);

  for(int i =0; i<pow; i++) temp *= temp;
  
   RZ(q[0], temp);

}

int main(){
  BranchTest(false, 10);
  return 0;
}

