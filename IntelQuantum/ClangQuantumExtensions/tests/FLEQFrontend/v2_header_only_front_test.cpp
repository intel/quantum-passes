// NOTE: Assertions have been autogenerated by utils/update_cc_test_checks.py UTC_ARGS: --version 2
// RUN: %clang %include_dirs -O1 -S -emit-llvm %loadintelclangextensions -o - %s | FileCheck %s --dump-input fail
#include <clang/Quantum/quintrinsics.h>
#include <clang/Quantum/qexpr.h>
#include <clang/Quantum/qlist.h>

qbit q1;
qbit q2;
qbit q3[2];

using namespace qexpr;

// CHECK-LABEL: define dso_local noundef i32 @main
// CHECK-SAME: () local_unnamed_addr #[[ATTR3:[0-9]+]] {
// CHECK:    ret i32 0
//
int main() {

}
