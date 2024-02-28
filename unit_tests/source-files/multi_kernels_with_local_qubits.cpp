#include "clang/Quantum/quintrinsics.h"


void quantum_kernel kernel3(){
    qbit local_3[5]; 
    RY(local_3[0], 0.5);
    RY(local_3[1], 0.5);
    RY(local_3[2], 0.5);
    RY(local_3[3], 0.5);
    RY(local_3[4], 0.5);
}

void quantum_kernel kernel2(){
    qbit local_2[5]; 

    H(local_2[0]);
    H(local_2[1]);
    H(local_2[2]);
    H(local_2[3]);
    H(local_2[4]);
}

void quantum_kernel kernel1(){
    qbit local_1[2]; 

    X(local_1[0]);
    Z(local_1[1]);
}



int main(){
	return 0;
}
