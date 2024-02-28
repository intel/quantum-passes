#include <clang/Quantum/quintrinsics.h>

qbit global_qreg[3];


void quantum_kernel kernel1(){
    qbit local_1[2]; 

    X(local_1[0]);
    Z(local_1[1]);
    
}



int main(){
	return 0;
}
