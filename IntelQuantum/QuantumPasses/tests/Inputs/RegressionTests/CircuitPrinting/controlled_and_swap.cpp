#include <clang/Quantum/quintrinsics.h>

qbit q[3];
qbit cat[5];
qbit basil[5];
cbit c[3];


void quantum_kernel multiQubitGates(){
    CNOT(basil[0],cat[1]);
    CZ(cat[1], cat[0]);
    Toffoli(basil[0], cat[1], cat[2]);
    SWAP(basil[1], basil[2]);
    CPhase(cat[0], basil[1], 1.5);
    SwapA(cat[1], basil[0], 1.2);
}


int main(){
	return 0;
}