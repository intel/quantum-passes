#include <clang/Quantum/quintrinsics.h>

qbit q[3];
qbit c_a_t[5];
qbit basil_leaf[5];
cbit c[3];


void quantum_kernel multiQubitGates(){
    CNOT(basil_leaf[0],c_a_t[1]);
    CZ(c_a_t[1], c_a_t[0]);
    Toffoli(basil_leaf[0], c_a_t[1], c_a_t[2]);
    SWAP(basil_leaf[1], basil_leaf[2]);
    CPhase(c_a_t[0], basil_leaf[1], 1.5);
    SwapA(c_a_t[1], basil_leaf[0], 1.2);
    CNOT(basil_leaf[0],c_a_t[1]);
    CZ(c_a_t[1], c_a_t[0]);
    Toffoli(basil_leaf[0], c_a_t[1], c_a_t[2]);
    SWAP(basil_leaf[1], basil_leaf[2]);
    CPhase(c_a_t[0], basil_leaf[1], 1.5);
    SwapA(c_a_t[1], basil_leaf[0], 1.2);
    CNOT(basil_leaf[0],c_a_t[1]);
    CZ(c_a_t[1], c_a_t[0]);
}


int main(){
	return 0;
}