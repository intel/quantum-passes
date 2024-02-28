#include <clang/Quantum/quintrinsics.h>

// Pass by reference - accepted behaviour
void quantum_kernel passQubitByRef(qbit &q){
    Z(q); 
}

void quantum_kernel top_kernel() {
    qbit local[3];

    passQubitByRef(local[0]);
}

int main(){

	return 0;
}
