#include <clang/Quantum/quintrinsics.h>

qbit q;

void getPara(double &);

quantum_kernel void reuse(){

  double a = 0.;

  getPara(a);
  RZ(q, a);
  getPara(a);
  RZ(q, a);

}

int main(){

  reuse();

  return 0;
}

