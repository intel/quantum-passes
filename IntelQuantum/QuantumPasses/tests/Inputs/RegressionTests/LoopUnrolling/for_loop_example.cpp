#include <clang/Quantum/quintrinsics.h>

cbit c0, c1;

quantum_kernel void for_loop_test(qbit &q0, qbit &q1) {
  const int N = 3;
  PrepZ(q0);
  PrepZ(q1);

  for (int i = 0; i < N; i++) {
    X(q0);
    X(q1);
    CNOT(q0, q1);
    H(q0);
    H(q1);
  }

  MeasZ(q0, c0);
  MeasZ(q1, c1);

  return;
}
