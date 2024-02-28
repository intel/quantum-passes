#include <clang/Quantum/quintrinsics.h>

qbit qreg[5];
// qbit octopus[5];
// qbit watermelon[5];
cbit c[3];


void quantum_kernel gates(){
    RX(qreg[0],2.9);
    RZ(qreg[1],0.6);
    RXY(qreg[2], 1.1, 1.5);
    RY(qreg[3], 1.6);
    SwapA(qreg[0], qreg[1], 0.5);
    CPhase(qreg[1], qreg[3], 1.2);
}


int main(){
    gates();
	return 0;
}