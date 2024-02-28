#include <clang/Quantum/quintrinsics.h>

const unsigned N = 15;
qbit q[N];

quantum_kernel void basicOperation() {
    CNOT(q[0], q[1]);
}

int main() {
    for (int i = 0; i < N; i++){
        basicOperation();
    }

    return 0;
}
