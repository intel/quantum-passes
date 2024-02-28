#include <clang/Quantum/quintrinsics.h>

const unsigned N = 5;
qbit q[N];

quantum_kernel void nestedLoop() {
    for (int i = 0; i < N; i++) {
        for (int j = i + 1; j < N; j++) {
            CNOT(q[i], q[j]);
        }
    }
}
