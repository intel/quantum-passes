#include <clang/Quantum/quintrinsics.h>

// Pass by reference - accepted behaviour
void quantum_kernel passQubitByRef(qbit &q){
    Z(q); 
}

void quantum_kernel nested_call() {
    qbit q;
    passQubitByRef(q);
}

void quantum_kernel top_kernel() {

    nested_call();
}

int main(){

	return 0;
}
