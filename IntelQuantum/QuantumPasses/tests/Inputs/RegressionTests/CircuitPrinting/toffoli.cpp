#include <clang/Quantum/quintrinsics.h>

qbit cat[5];
qbit basil[5];
cbit c[3];


void quantum_kernel multiQubitGates(){
    Toffoli(cat[2], cat[1], cat[0]);
    Toffoli(cat[2], cat[0], cat[4]);
    Toffoli(cat[3], cat[0], cat[2]);
}


int main(){
	return 0;
}