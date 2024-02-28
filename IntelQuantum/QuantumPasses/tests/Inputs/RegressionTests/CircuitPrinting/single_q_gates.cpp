#include <clang/Quantum/quintrinsics.h>

qbit q[3];
qbit cat[5];
qbit basil[5];
cbit c[3];


void quantum_kernel singleQubitGates(){
    T(cat[1]);
    Tdag(cat[1]);
    Sdag(basil[0]);
    H(cat[2]);
    X(cat[3]);
    Z(cat[2]);
    Y(cat[0]);
    S(basil[3]);
}

void quantum_kernel angle(){
    RZ(cat[0], 1.1);
    RY(cat[1], 1.1);
    RX(cat[2],1.1);  
    RXY(cat[2],1.1, 1.1);  
}

int main(){
	return 0;
}