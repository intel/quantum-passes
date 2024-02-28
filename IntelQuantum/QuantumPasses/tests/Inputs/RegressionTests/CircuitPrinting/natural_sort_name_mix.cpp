#include <clang/Quantum/quintrinsics.h>

qbit qubit_0_a[1];
qbit qubit_1_b[1];
qbit qubit_10_a[1];
qbit qubit_0_c[1];
qbit qubit_2_b[1];
cbit c[3];


void quantum_kernel multiQubitGates(){
    CNOT(qubit_0_a[0],qubit_1_b[0]);
    CNOT(qubit_10_a[0],qubit_0_c[0]);
    CNOT(qubit_0_c[0],qubit_2_b[0]);
    CNOT(qubit_1_b[0],qubit_2_b[0]);
}


int main(){
	return 0;
}