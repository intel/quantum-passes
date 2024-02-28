#include <clang/Quantum/quintrinsics.h>

qbit global_qreg[2];
cbit c[3];



// Illegal 
void quantum_kernel passQubitByValue(qbit q) {
    X(q);
}

void quantum_kernel nested_call() {
    qbit q;
    passQubitByValue(q); 
}

void quantum_kernel top_kernel() {
    qbit local[2]; 
    qbit self; 

    passQubitByValue(local[0]);
    passQubitByValue(global_qreg[1]);
    nested_call();

}


int main(){
    top_kernel();

	return 0;
}
