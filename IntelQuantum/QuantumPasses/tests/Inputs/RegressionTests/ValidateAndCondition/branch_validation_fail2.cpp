#include <clang/Quantum/quintrinsics.h>

qbit q;

quantum_kernel bool BranchTest_fail(bool b) {
  if (b) {
    PrepZ(q);
    return true;
  } else {
    PrepZ(q);
    X(q);
    return false;
  }
}

int main(){
  BranchTest_fail(false);
  return 0;
}
