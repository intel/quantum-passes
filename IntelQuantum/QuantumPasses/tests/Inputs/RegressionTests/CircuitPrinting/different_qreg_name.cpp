#include <clang/Quantum/quintrinsics.h>

qbit bee[5];
qbit octopus[5];
qbit watermelon[5];
cbit c[3];


void quantum_kernel singleQubitGates(){
    T(bee[1]);
    Tdag(bee[3]);
    Sdag(octopus[0]);
    H(octopus[3]);
    X(watermelon[3]);
    Z(watermelon[1]);
    Y(watermelon[0]);
    S(octopus[3]);
}


int main(){
    singleQubitGates();
	return 0;
}