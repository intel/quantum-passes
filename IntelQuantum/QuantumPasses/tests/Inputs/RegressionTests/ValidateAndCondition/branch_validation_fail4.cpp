#include <clang/Quantum/quintrinsics.h>

qbit q;

quantum_kernel bool BranchTest_fail(bool a, bool b, bool c) {

  PrepZ(q);

  if(a){
    H(q);
    if(b){
      S(q);
      if(c){
        H(q);
        return true;
      }
      else
        return false;
    }
    else
      return false;
  }
  else
    return false;

}

int main(){
  BranchTest_fail(true, true, true);
  return 0;
}
