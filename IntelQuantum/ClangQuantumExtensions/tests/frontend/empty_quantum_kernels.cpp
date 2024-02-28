// NOTE: Assertions have been autogenerated by utils/update_cc_test_checks.py UTC_ARGS: --version 3
// RUN: %clang -S -emit-llvm %include_dirs %loadintelclangextensions  %s -o - | FileCheck %s --dump-input fail


#define quantum_kernel \
  __attribute__((section(".qbbs_text"))) __attribute__((noinline))

// CHECK-LABEL: define dso_local void @_Z5test1v(
// CHECK-SAME: ) #[[ATTR0:[0-9]+]] section ".qbbs_text" {
// CHECK:    ret void
//
quantum_kernel void test1() {
}






// CHECK-LABEL: define dso_local noundef i32 @_Z5test2v(
// CHECK-SAME: ) #[[ATTR0]] section ".qbbs_text" {
// CHECK:    ret i32 0
//
quantum_kernel int test2() {
  return 0;
}





