#include <clang/Quantum/quintrinsics.h>

qbit q[5] = {5, 1, 2, 3, 4};
qbit anc = 0;

//This was a bug where custom placement was seg faulting on the
//scalar global qubit. 
//This variant demonstrates a known issue where scalars 
//initialized to 0 are treated as not mapped. 

quantum_kernel void test(){
  H(anc);
}