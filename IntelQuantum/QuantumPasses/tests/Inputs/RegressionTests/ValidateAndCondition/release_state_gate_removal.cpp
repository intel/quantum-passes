#include <clang/Quantum/quintrinsics.h>

qbit global_0;
qbit global_1;

quantum_kernel void kernel_1() {
   CNOT(global_0, global_1);
   release_quantum_state();
}

quantum_kernel void kernel_2() {
   kernel_1();
   PrepX(global_1);
   H(global_1);
}