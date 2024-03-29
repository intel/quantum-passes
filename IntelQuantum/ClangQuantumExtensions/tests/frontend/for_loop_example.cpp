// NOTE: Assertions have been autogenerated by utils/update_cc_test_checks.py UTC_ARGS: --version 3
// RUN: %clang -S -emit-llvm %include_dirs %loadintelclangextensions  %s -o - | FileCheck %s --dump-input fail


#include "clang/Quantum/quintrinsics.h"

cbit c0, c1;

// CHECK-LABEL: define dso_local void @_Z13for_loop_testRtS_(
// CHECK-SAME: ptr noundef nonnull align 2 dereferenceable(2) [[Q0:%.*]], ptr noundef nonnull align 2 dereferenceable(2) [[Q1:%.*]]) #[[ATTR0:[0-9]+]] section ".qbbs_text" {
// CHECK:    [[Q0_ADDR:%.*]] = alloca ptr, align 8
// CHECK-NEXT:    [[Q1_ADDR:%.*]] = alloca ptr, align 8
// CHECK-NEXT:    [[N:%.*]] = alloca i32, align 4
// CHECK-NEXT:    [[I:%.*]] = alloca i32, align 4
// CHECK-NEXT:    store ptr [[Q0]], ptr [[Q0_ADDR]], align 8
// CHECK-NEXT:    store ptr [[Q1]], ptr [[Q1_ADDR]], align 8
// CHECK-NEXT:    store i32 3, ptr [[N]], align 4
// CHECK-NEXT:    [[TMP0:%.*]] = load ptr, ptr [[Q0_ADDR]], align 8
// CHECK-NEXT:    call void @_Z5PrepZRt(ptr noundef nonnull align 2 dereferenceable(2) [[TMP0]])
// CHECK-NEXT:    [[TMP1:%.*]] = load ptr, ptr [[Q1_ADDR]], align 8
// CHECK-NEXT:    call void @_Z5PrepZRt(ptr noundef nonnull align 2 dereferenceable(2) [[TMP1]])
// CHECK-NEXT:    store i32 0, ptr [[I]], align 4
// CHECK-NEXT:    br label %[[FOR_COND:.*]]
// CHECK:       [[FOR_COND]]:
// CHECK-NEXT:    [[TMP2:%.*]] = load i32, ptr [[I]], align 4
// CHECK-NEXT:    [[CMP:%.*]] = icmp slt i32 [[TMP2]], 3
// CHECK-NEXT:    br i1 [[CMP]], label %[[FOR_BODY:.*]], label %[[FOR_END:.*]]
// CHECK:       [[FOR_BODY]]:
// CHECK-NEXT:    [[TMP3:%.*]] = load ptr, ptr [[Q0_ADDR]], align 8
// CHECK-NEXT:    call void @_Z1XRt(ptr noundef nonnull align 2 dereferenceable(2) [[TMP3]])
// CHECK-NEXT:    [[TMP4:%.*]] = load ptr, ptr [[Q1_ADDR]], align 8
// CHECK-NEXT:    call void @_Z1XRt(ptr noundef nonnull align 2 dereferenceable(2) [[TMP4]])
// CHECK-NEXT:    [[TMP5:%.*]] = load ptr, ptr [[Q0_ADDR]], align 8
// CHECK-NEXT:    [[TMP6:%.*]] = load ptr, ptr [[Q1_ADDR]], align 8
// CHECK-NEXT:    call void @_Z4CNOTRtS_(ptr noundef nonnull align 2 dereferenceable(2) [[TMP5]], ptr noundef nonnull align 2 dereferenceable(2) [[TMP6]])
// CHECK-NEXT:    [[TMP7:%.*]] = load ptr, ptr [[Q0_ADDR]], align 8
// CHECK-NEXT:    call void @_Z1HRt(ptr noundef nonnull align 2 dereferenceable(2) [[TMP7]])
// CHECK-NEXT:    [[TMP8:%.*]] = load ptr, ptr [[Q1_ADDR]], align 8
// CHECK-NEXT:    call void @_Z1HRt(ptr noundef nonnull align 2 dereferenceable(2) [[TMP8]])
// CHECK-NEXT:    br label %[[FOR_INC:.*]]
// CHECK:       [[FOR_INC]]:
// CHECK-NEXT:    [[TMP9:%.*]] = load i32, ptr [[I]], align 4
// CHECK-NEXT:    [[INC:%.*]] = add nsw i32 [[TMP9]], 1
// CHECK-NEXT:    store i32 [[INC]], ptr [[I]], align 4
// CHECK-NEXT:    br label %[[FOR_COND]], !llvm.loop [[LOOP6:![0-9]+]]
// CHECK:       [[FOR_END]]:
// CHECK-NEXT:    [[TMP10:%.*]] = load ptr, ptr [[Q0_ADDR]], align 8
// CHECK-NEXT:    call void @_Z5MeasZRtRbb(ptr noundef nonnull align 2 dereferenceable(2) [[TMP10]], ptr noundef nonnull align 1 dereferenceable(1) @c0, i1 noundef zeroext false)
// CHECK-NEXT:    [[TMP11:%.*]] = load ptr, ptr [[Q1_ADDR]], align 8
// CHECK-NEXT:    call void @_Z5MeasZRtRbb(ptr noundef nonnull align 2 dereferenceable(2) [[TMP11]], ptr noundef nonnull align 1 dereferenceable(1) @c1, i1 noundef zeroext false)
// CHECK-NEXT:    ret void
//
quantum_kernel void for_loop_test(qbit &q0, qbit &q1) {
  const int N = 3;
  PrepZ(q0);
  PrepZ(q1);

  for (int i = 0; i < N; i++) {
    X(q0);
    X(q1);
    CNOT(q0, q1);
    H(q0);
    H(q1);
  }

  MeasZ(q0, c0);
  MeasZ(q1, c1);

  return;
}























