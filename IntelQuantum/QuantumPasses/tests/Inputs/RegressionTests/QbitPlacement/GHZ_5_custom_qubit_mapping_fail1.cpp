#include <clang/Quantum/quintrinsics.h>


// assuming the json has 7 qubits, this is a valid qubit mapping assignment 
// qubits do not need to be consecutive nor in order
qbit q[5]  = {1, 2, 3, 0, 16}; // should cause an error if config only has 12 qubits
cbit c[7];

// examples of invalid mapping assignment 
// qbit q[5]  = {1, 2, 3, 6, 9};  // invalid because index 9 is out of bound when json has only 7 qubits
// qbit q[5]  = {1, 1, 1, 1, 1};  // invalid because 1 is assigned to multiple qubits 
// qbit q[5];  // invalid because -C flag is set yet no qubit mapping is provided in source file 


quantum_kernel void GHZ_5() {
  H(q[0]);
  CNOT(q[0], q[1]);
  CNOT(q[1], q[2]);
  CNOT(q[2], q[3]);
  CNOT(q[3], q[4]);
}

int main(){
  GHZ_5();
}

