#include <clang/Quantum/quintrinsics.h>

qbit q[3];
qbit qubit_reg[15];
cbit c[3];


void quantum_kernel multiQubitGates(){
    CNOT(qubit_reg[0],qubit_reg[1]);
    CNOT(qubit_reg[1],qubit_reg[11]);
    CNOT(qubit_reg[11],qubit_reg[3]);
    CNOT(qubit_reg[0],qubit_reg[5]);
    CNOT(qubit_reg[0],qubit_reg[10]);
    CNOT(qubit_reg[0],qubit_reg[9]);
    CNOT(qubit_reg[0],qubit_reg[2]);
    CNOT(qubit_reg[0],qubit_reg[4]);
    CNOT(qubit_reg[0],qubit_reg[6]);
    CNOT(qubit_reg[12],qubit_reg[1]);
    CNOT(qubit_reg[14],qubit_reg[1]);
    CNOT(qubit_reg[8],qubit_reg[1]);

}


int main(){
	return 0;
}