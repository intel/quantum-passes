#include <clang/Quantum/quintrinsics.h>

qbit q[2];


quantum_kernel void no_arg(){
  H(q[0]);
}

quantum_kernel void qbit_arg(qbit &arg){
  H(arg);
}

quantum_kernel void array_arg0(qbit arg[]){
  H(arg[0]);
}

quantum_kernel void array_arg1(qbit arg[]){
  H(arg[1]);
}

quantum_kernel void unused_arg(qbit &arg){
  H(q[0]);
}

quantum_kernel void index_arg(unsigned i){
  H(q[i]);
}

quantum_kernel void loop_on_arg(unsigned n){
  for(int i = 0; i < n; i++) H(q[i]);
}

quantum_kernel void branch_arg(bool br){
  if(br)
    H(q[0]);
  else
    H(q[1]);
}
