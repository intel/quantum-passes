#include <clang/Quantum/quintrinsics.h>

//<------------------------------------------------------------------------->//
/* simple arithmetic on a constant angle */
quantum_kernel void angle_test1(qbit q0) {
  const double angle = 3.141592653589;
  RZ(q0, angle);
  RZ(q0, angle / 2);
  RZ(q0, angle * 2);
}

//<------------------------------------------------------------------------->//
/* simple arithmetic on two constant angles */
quantum_kernel void angle_test2(qbit q0) {
  const double A = 0.461922013898618;
  const double B = 1.995481713596621;
  RZ(q0, A - B);
  RZ(q0, A + B);
  RZ(q0, B / A);
}

//<------------------------------------------------------------------------->//
/* array of constant angles */
quantum_kernel void angle_test3(qbit q0) {
  const double arr[2] = {0.5, 3.5};
  RZ(q0, arr[0]);
  RZ(q0, arr[1]);
  RZ(q0, arr[0] + arr[1]);
}

//<------------------------------------------------------------------------->//
/* for-loop with an array of constant angles */
const int N = 2;
qbit q[N];
const double arr[N] = {1.57, 3.14};

int main() {
  for (int i = 0; i < N; i++) {
    RZ(q[i], arr[i]);
  }
  return 0;
}
