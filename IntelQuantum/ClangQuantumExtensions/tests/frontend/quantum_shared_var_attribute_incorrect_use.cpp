// NOTE: Assertions have been autogenerated by utils/update_cc_test_checks.py UTC_ARGS: --version 3
// RUN: %clang -S -emit-llvm %include_dirs %loadintelclangextensions  %s -o - | FileCheck %s --dump-input fail

#include "clang/Quantum/quintrinsics.h"

#define quantum_shared_var __attribute__((quantum_shared_var))

// CHECK-LABEL: define dso_local void @_Z3foov(
// CHECK-SAME: ) #[[ATTR2:[0-9]+]] {
// CHECK:    [[BAR:%.*]] = alloca float, align 4
// CHECK-NEXT:    store float 0x400921FA00000000, ptr [[BAR]], align 4
// CHECK-NEXT:    ret void
//
void foo() {
  quantum_shared_var float bar = 3.14159;
}


