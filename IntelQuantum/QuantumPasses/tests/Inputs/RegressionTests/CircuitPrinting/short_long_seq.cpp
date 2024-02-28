#include <clang/Quantum/quintrinsics.h>

qbit q[3];
qbit cat[5];
qbit basil[5];
qbit dog[35];
cbit c[3];


void quantum_kernel multiQubitGates(){
    CNOT(basil[0],cat[1]);
    CZ(cat[1], cat[0]);
    Toffoli(basil[0], cat[1], cat[2]);
    SWAP(basil[1], basil[2]);
    CPhase(cat[0], basil[1], 1.5);
    SwapA(cat[1], basil[0], 1.2);
    CNOT(basil[0],cat[1]);
    CZ(cat[1], cat[0]);
    Toffoli(basil[0], cat[1], cat[2]);
    SWAP(basil[1], basil[2]);
    CPhase(cat[0], basil[1], 1.5);
    SwapA(cat[1], basil[0], 1.2);
    CNOT(basil[0],cat[1]);
    CZ(cat[1], cat[0]);
    Toffoli(basil[0], cat[1], cat[2]);
    SWAP(basil[1], basil[2]);
    CPhase(cat[0], basil[1], 1.5);
    SwapA(cat[1], basil[0], 1.2);
    CNOT(basil[0],cat[1]);
    CZ(cat[1], cat[0]);
    Toffoli(basil[0], cat[1], cat[2]);
    SWAP(basil[1], basil[2]);
    CPhase(cat[0], basil[1], 1.5);
    SwapA(cat[1], basil[0], 1.2);
    CNOT(basil[0],cat[1]);
    CZ(cat[1], cat[0]);
    Toffoli(basil[0], cat[1], cat[2]);
    SWAP(basil[1], basil[2]);
    CPhase(cat[0], basil[1], 1.5);
    SwapA(cat[1], basil[0], 1.2);
    CNOT(basil[0],cat[1]);
    CZ(cat[1], cat[0]);
    Toffoli(basil[0], cat[1], cat[2]);
    SWAP(basil[1], basil[2]);
    CPhase(cat[0], basil[1], 1.5);
    SwapA(cat[1], basil[0], 1.2);
    CNOT(basil[0],cat[1]);
    CZ(cat[1], cat[0]);
    Toffoli(basil[0], cat[1], cat[2]);
    SWAP(basil[1], basil[2]);
    CPhase(cat[0], basil[1], 1.5);
    SwapA(cat[1], basil[0], 1.2);
}

void quantum_kernel multiQubitGates2(){
    CNOT(basil[0],cat[1]);
    CZ(cat[1], cat[0]);
    Toffoli(basil[0], cat[1], cat[2]);
    SWAP(basil[1], basil[2]);
    CPhase(cat[0], basil[1], 1.5);
    SwapA(cat[1], basil[0], 1.2);
    CNOT(basil[0],cat[1]);
    CZ(cat[1], cat[0]);
    Toffoli(basil[0], cat[1], cat[2]);
    SWAP(basil[1], basil[2]);
    CPhase(cat[0], basil[1], 1.5);
    SwapA(cat[1], basil[0], 1.2);
    CNOT(basil[0],cat[1]);
    CZ(cat[1], cat[0]);
    Toffoli(basil[0], cat[1], cat[2]);
}

void quantum_kernel wide(){
    
    X(dog[0]);
    X(dog[1]);
    X(dog[2]);
    X(dog[3]);
    X(dog[4]);
    X(dog[5]);
    X(dog[6]);
    X(dog[7]);
    X(dog[8]);
    X(dog[9]);
    X(dog[10]);
    X(dog[11]);
    X(dog[12]);
    X(dog[13]);
    X(dog[14]);
    X(dog[15]);
    X(dog[16]);
    X(dog[17]);
    X(dog[18]);
    X(dog[19]);
    X(dog[20]);
    X(dog[21]);
    X(dog[22]);
    X(dog[23]);
    X(dog[24]);
    X(dog[25]);
    X(dog[26]);
    X(dog[27]);
    X(dog[28]);
}


int main(){
	return 0;
}