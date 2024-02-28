#include <clang/Quantum/quintrinsics.h>

qbit qubit_reg[5];
cbit c[3];


void quantum_kernel multiQubitGates(){
    RX(qubit_reg[0], 0.0);
    RXY(qubit_reg[3], 23456, 12);
    RY(qubit_reg[2], 123.1);
    RZ(qubit_reg[3], 5);
    CPhase(qubit_reg[4], qubit_reg[0],0.123456789123456);
}


int main(){
	return 0;
}