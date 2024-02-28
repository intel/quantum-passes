#include <clang/Quantum/quintrinsics.h>

// Pass by pointer - accepted behaviour
void quantum_kernel passQubitArrayByPtr(qbit *qubit_vec, int num_ele){
    for (int i=0; i < num_ele; i++)
        H(qubit_vec[i]);
}

void quantum_kernel top_kernel() {
    qbit local[3];
     
    passQubitArrayByPtr(local, 3);
}

int main(){

	return 0;
}
