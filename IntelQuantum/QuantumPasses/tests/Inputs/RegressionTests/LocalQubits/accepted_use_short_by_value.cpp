#include <clang/Quantum/quintrinsics.h>

// pass in a short by value - accepted behaviour
void quantum_kernel passInShortByValue(short num) {
    short a;
    a = num + 5;

    qbit q; // declare a local qubit inside
    H(q);
}

void quantum_kernel top_kernel() {
    passInShortByValue(5);
}

int main(){

	return 0;
}
