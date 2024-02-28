#include <clang/Quantum/quintrinsics.h>
#include <clang/Quantum/qexpr.h>

using namespace qexpr;

qbit q[3];

quantum_kernel void PrepAll(){

  for (int i = 0; i < 3; i++)
    PrepZ(q[i]);
}

int main() {

  eval_hold(
    convert<PrepAll>()
  );

  return 0;

}
