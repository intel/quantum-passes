#include <clang/Quantum/quintrinsics.h>

qbit global_qreg[2];
cbit c[3];


// Pass by pointer - accepted behaviour
void quantum_kernel passQubitArrayByPtr(qbit qubit_vec[], int num_ele){
    for (int i=0; i < num_ele; i++)
        H(qubit_vec[i]);

}

// Pass by pointer - accepted behaviour
void quantum_kernel passQubitArrayByPtr2(qbit *qubit_vec, int num_ele){
    for (int i=0; i < num_ele; i++)
        H(qubit_vec[i]);
}

// Pass by reference - accepted behaviour
void quantum_kernel passQubitByRef(qbit &q){
    Z(q); 
}

// pass in a short by value - accepted behaviour
void quantum_kernel passInShortByValue(short num) {
    short a;
    a = num + 5;

    qbit q; // declare a local qubit inside
    H(q);
}

void quantum_kernel nested_call() {
    qbit q;
    passQubitByRef(q);
}

void quantum_kernel top_kernel() {
    qbit local[3];
    short x = 5;
     
    passQubitArrayByPtr(local, 3);
    passQubitArrayByPtr2(local, 3);
    passQubitByRef(local[0]);
    passInShortByValue(5);
    nested_call();
}



int main(){

	return 0;
}
