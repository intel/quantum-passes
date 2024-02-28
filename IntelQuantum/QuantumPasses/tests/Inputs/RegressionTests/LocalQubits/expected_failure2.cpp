#include <clang/Quantum/quintrinsics.h>

qbit global_qreg[2];
cbit c[3];


// pass in a short by value and a qbit by value - illegal
void quantum_kernel passShortandQbitByValue(short num, qbit value) {
    short a;
    a = num + 5;

    H(value);
}

void quantum_kernel top_kernel() {
    qbit local[2]; 

    short x = 2; 
    passShortandQbitByValue(x, local[0]);

}

int main(){
    top_kernel();

	return 0;
}
