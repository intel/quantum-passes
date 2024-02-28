// NOTE: Assertions have been autogenerated by utils/update_cc_test_checks.py UTC_ARGS: --version 3
// RUN: %clang -S -emit-llvm %include_dirs %loadintelclangextensions  %s -o - | FileCheck %s --dump-input fail


#define quantum_kernel \
  __attribute__((section(".qbbs_text"))) __attribute__((noinline))

using qbit = unsigned short int;
using cbit = bool;

void __quantum_qubit(qbit *q);
void __quantum_cbit(cbit *c);
void __quantum_param(double d);
void release_quantum_state();

// CHECK-LABEL: define dso_local void @_Z14MyHadamardGateRt(
// CHECK-SAME: ptr noundef nonnull align 2 dereferenceable(2) [[Q:%.*]]) #[[ATTR0:[0-9]+]] {
// CHECK:    [[Q_ADDR:%.*]] = alloca ptr, align 8
// CHECK-NEXT:    store ptr [[Q]], ptr [[Q_ADDR]], align 8
// CHECK-NEXT:    [[TMP0:%.*]] = load ptr, ptr [[Q_ADDR]], align 8
// CHECK-NEXT:    call void @_Z15__quantum_qubitPt(ptr noundef [[TMP0]])
// CHECK-NEXT:    ret void
//
__attribute__((noinline)) void MyHadamardGate(qbit &q) __attribute__((annotate("{ \
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

// CHECK-LABEL: define dso_local void @_Z12MyCustomGateRt(
// CHECK-SAME: ptr noundef nonnull align 2 dereferenceable(2) [[Q:%.*]]) #[[ATTR0]] {
// CHECK:    [[Q_ADDR:%.*]] = alloca ptr, align 8
// CHECK-NEXT:    store ptr [[Q]], ptr [[Q_ADDR]], align 8
// CHECK-NEXT:    [[TMP0:%.*]] = load ptr, ptr [[Q_ADDR]], align 8
// CHECK-NEXT:    call void @_Z15__quantum_qubitPt(ptr noundef [[TMP0]])
// CHECK-NEXT:    ret void
//
__attribute__((noinline)) void MyCustomGate(qbit &q) __attribute__((annotate("{ \
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

// CHECK-LABEL: define dso_local void @_Z16custom_gate_testv(
// CHECK-SAME: ) #[[ATTR0]] section ".qbbs_text" {
// CHECK:    call void @_Z14MyHadamardGateRt(ptr noundef nonnull align 2 dereferenceable(2) @q)
// CHECK-NEXT:    call void @_Z12MyCustomGateRt(ptr noundef nonnull align 2 dereferenceable(2) getelementptr inbounds ([2 x i16], ptr @q, i64 0, i64 1))
// CHECK-NEXT:    ret void
//
__attribute__((noinline)) quantum_kernel void custom_gate_test() {
  MyHadamardGate(q[0]);
  MyCustomGate(q[1]);

  /// NOTE: 04/13/2022 - Commented out measurement operation(s) since they are
  ///       being redesigned and measurement is not necessary for this test.
  // __quantum_measz(q[0], 0);
  // __quantum_measz(q[1], 0);

  return;
}

























