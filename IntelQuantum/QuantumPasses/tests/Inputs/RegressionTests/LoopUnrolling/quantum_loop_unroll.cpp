#include <clang/Quantum/quintrinsics.h>

const unsigned N = 10;
qbit q[N];

quantum_kernel void simpleLoop() {
    for (int i = 0; i < N; i++) {
        PrepX(q[i]);
    }
    CNOT(q[0], q[1]);
    CNOT(q[2], q[3]);
    RX(q[4], 0.1);
}