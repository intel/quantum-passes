#include <clang/Quantum/quintrinsics.h>

qbit q[3];

quantum_kernel void prep_meas_hold() {
  cbit c[3];

  PrepZ(q[0]);
  PrepZ(q[1]);
  PrepZ(q[2]);

  X(q[1]);

  MeasZ(q[0], c[0]);
  MeasZ(q[1], c[1]);
  MeasZ(q[2], c[2]);

}

quantum_kernel void prep_meas_release() {
  cbit c[3];

  PrepZ(q[0]);
  PrepZ(q[1]);
  PrepZ(q[2]);

  MeasZ(q[0], c[0]);
  MeasZ(q[1], c[1]);
  MeasZ(q[2], c[2]);

  release_quantum_state();

}

int main(){
  prep_meas_hold();
  prep_meas_release();
}

