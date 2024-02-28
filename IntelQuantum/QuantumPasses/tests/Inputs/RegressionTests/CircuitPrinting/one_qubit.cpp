#include <clang/Quantum/quintrinsics.h>

qbit bee[5];
qbit octopus[5];
qbit watermelon[5];
cbit c[3];


void quantum_kernel singleQubitGates(){
    T(bee[1]);
    Tdag(bee[1]);
    Sdag(bee[1]);
    H(bee[1]);
    X(bee[1]);
    Z(bee[1]);
    S(bee[1]);
    MeasX(bee[1],c[0]);
}


int main(){
    singleQubitGates();
	return 0;
}