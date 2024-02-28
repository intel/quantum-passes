#include <clang/Quantum/quintrinsics.h>

void MyHadamardGate(qbit &q) __attribute__((annotate("{ \
    \"matrix_real\" : \"1/sqrt(2)*[1, 1, 1, -1]\", \
    \"matrix_imag\" : [0, 0, 0, 0], \
    \"is_hermitian\" : true, \
    \"is_unitary\" : true, \
    \"is_mutable\" : true, \
    \"qubit_list\" : [0], \
    \"parametric_list\" : [], \
    \"control_qubit_list\" : [], \
  }"))) {
  __quantum_qubit(&q);
}

void MyCustomGate(qbit &q) __attribute__((annotate("{ \
    \"matrix_real\" : [0, 1, 0, 0], \
    \"matrix_imag\" : [0, 0, j, 0], \
    \"is_hermitian\" : false, \
    \"is_unitary\" : true, \
    \"is_mutable\" : true, \
    \"qubit_list\" : [0], \
    \"parametric_list\" : [], \
    \"control_qubit_list\" : [], \
  }"))) {
  __quantum_qubit(&q);
}

qbit q[2];

quantum_kernel void custom_gate_test() {
  MyHadamardGate(q[0]);
  MyCustomGate(q[1]);
  return;
}
