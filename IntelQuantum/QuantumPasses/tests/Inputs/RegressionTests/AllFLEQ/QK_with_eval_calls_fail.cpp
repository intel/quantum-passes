
#include <clang/Quantum/qexpr.h>
#include <clang/Quantum/qlist.h>
#include <clang/Quantum/quintrinsics.h>

// Define the number of qubits needed for compilation
const int N_sub = 4; // Number of qubits in subsystem (thermal state size)

qbit listable(qreg_subA, N_sub);
qbit listable(qreg_subB, N_sub);

QExpr PrepZAll(qlist::QList reg) {
  return qexpr::cIf(size(reg) == 0, qexpr::identity(),
                    qexpr::_PrepZ(reg[0]) * PrepZAll(reg + 1));
}

//should fail due to mixed paradigms
quantum_kernel void PrepZAll(){
  for(int i = 0; i < N_sub; i++){
    PrepZ(qreg_subA[i]);
  }

  qexpr::eval_hold(PrepZAll(qreg_subB));
}

int main() {
  PrepZAll();
  return 0;
}