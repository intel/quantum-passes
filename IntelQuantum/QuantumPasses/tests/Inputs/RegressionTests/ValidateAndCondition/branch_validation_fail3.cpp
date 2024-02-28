#include <clang/Quantum/quintrinsics.h>

qbit q;

quantum_kernel void BranchTest_fail(int s) {

  PrepZ(q);

  switch(s){
    case 0:
      break;
    case 1:
      H(q);
      break;
    case 2:
      S(q);
      break;
    case 3:
      H(q);
      Sdag(q);
      H(q);
      break;
    case 4:
      H(q);
      S(q);
      break;
    case 5:
      Sdag(q);
      H(q);
      break;
    
  }

}

int main(){
  BranchTest_fail(5);
  return 0;
}
