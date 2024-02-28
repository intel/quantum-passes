#include <clang/Quantum/quintrinsics.h>

qbit q[5] = {0, 1, 2, 3, 4};
qbit anc = 5;

//This was a bug where custom placement was seg faulting on the
//scalar global qubit. 

quantum_kernel void test(){
  H(anc);
}