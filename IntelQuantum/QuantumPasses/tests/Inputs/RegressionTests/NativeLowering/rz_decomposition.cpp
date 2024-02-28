#include <clang/Quantum/quintrinsics.h>

const int N = 6;
qbit Q[N];

quantum_kernel void gatesWithRz() {
  RZ(Q[0], 3.141592653589793);
  Z(Q[1]);
  S(Q[2]);
  Sdag(Q[3]);
  T(Q[4]);
  Tdag(Q[5]);
}

int main() {
  gatesWithRz();
  return 0;
}

