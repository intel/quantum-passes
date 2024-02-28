#include <clang/Quantum/quintrinsics.h>

qbit qreg[5];
// qbit octopus[5];
// qbit watermelon[5];
cbit c[3];


void quantum_kernel gates(){
    CNOT(qreg[0],qreg[4]);
    CZ(qreg[1],qreg[3]);
    H(qreg[2]);
    SWAP(qreg[0], qreg[1]);
    SWAP(qreg[3],qreg[4]);
    Z(qreg[2]);
}


int main(){
    gates();
	return 0;
}