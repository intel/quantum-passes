// NOTE: Assertions have been autogenerated by utils/update_cc_test_checks.py UTC_ARGS: --version 3
// RUN: %clang -S -emit-llvm %include_dirs %loadintelclangextensions  %s -o - | FileCheck %s --dump-input fail

// TODO: Expand this test with FileCheck



#include "clang/Quantum/quintrinsics.h"
#include <string>
#define ODD_OR_EVEN 0


// CHECK-LABEL: define dso_local noundef i32 @main(
// CHECK-SAME: i32 noundef [[ARGC:%.*]], ptr noundef [[ARGV:%.*]]) #[[ATTR2:[0-9]+]] {
// CHECK:    [[RETVAL:%.*]] = alloca i32, align 4
// CHECK-NEXT:    [[ARGC_ADDR:%.*]] = alloca i32, align 4
// CHECK-NEXT:    [[ARGV_ADDR:%.*]] = alloca ptr, align 8
// CHECK-NEXT:    [[Q:%.*]] = alloca [2 x i16], align 2
// CHECK-NEXT:    [[C:%.*]] = alloca [2 x i8], align 1
// CHECK-NEXT:    [[I:%.*]] = alloca i32, align 4
// CHECK-NEXT:    store i32 0, ptr [[RETVAL]], align 4
// CHECK-NEXT:    store i32 [[ARGC]], ptr [[ARGC_ADDR]], align 4
// CHECK-NEXT:    store ptr [[ARGV]], ptr [[ARGV_ADDR]], align 8
// CHECK-NEXT:    [[ARRAYIDX:%.*]] = getelementptr inbounds [2 x i16], ptr [[Q]], i64 0, i64 0
// CHECK-NEXT:    call void @_Z5PrepZRt(ptr noundef nonnull align 2 dereferenceable(2) [[ARRAYIDX]])
// CHECK-NEXT:    [[ARRAYIDX1:%.*]] = getelementptr inbounds [2 x i16], ptr [[Q]], i64 0, i64 1
// CHECK-NEXT:    call void @_Z5PrepZRt(ptr noundef nonnull align 2 dereferenceable(2) [[ARRAYIDX1]])
// CHECK-NEXT:    [[ARRAYIDX2:%.*]] = getelementptr inbounds [2 x i16], ptr [[Q]], i64 0, i64 1
// CHECK-NEXT:    call void @_Z1XRt(ptr noundef nonnull align 2 dereferenceable(2) [[ARRAYIDX2]])
// CHECK-NEXT:    [[ARRAYIDX3:%.*]] = getelementptr inbounds [2 x i16], ptr [[Q]], i64 0, i64 0
// CHECK-NEXT:    [[ARRAYIDX4:%.*]] = getelementptr inbounds [2 x i8], ptr [[C]], i64 0, i64 0
// CHECK-NEXT:    call void @_Z5MeasZRtRbb(ptr noundef nonnull align 2 dereferenceable(2) [[ARRAYIDX3]], ptr noundef nonnull align 1 dereferenceable(1) [[ARRAYIDX4]], i1 noundef zeroext false)
// CHECK-NEXT:    [[ARRAYIDX5:%.*]] = getelementptr inbounds [2 x i16], ptr [[Q]], i64 0, i64 1
// CHECK-NEXT:    [[ARRAYIDX6:%.*]] = getelementptr inbounds [2 x i8], ptr [[C]], i64 0, i64 1
// CHECK-NEXT:    call void @_Z5MeasZRtRbb(ptr noundef nonnull align 2 dereferenceable(2) [[ARRAYIDX5]], ptr noundef nonnull align 1 dereferenceable(1) [[ARRAYIDX6]], i1 noundef zeroext false)
// CHECK-NEXT:    store i32 0, ptr [[I]], align 4
// CHECK-NEXT:    br label %[[FOR_COND:.*]]
// CHECK:       [[FOR_COND]]:
// CHECK-NEXT:    [[TMP0:%.*]] = load i32, ptr [[I]], align 4
// CHECK-NEXT:    [[CMP:%.*]] = icmp slt i32 [[TMP0]], 2
// CHECK-NEXT:    br i1 [[CMP]], label %[[FOR_BODY:.*]], label %[[FOR_END:.*]]
// CHECK:       [[FOR_BODY]]:
// CHECK-NEXT:    [[TMP1:%.*]] = load i32, ptr [[I]], align 4
// CHECK-NEXT:    [[IDXPROM:%.*]] = sext i32 [[TMP1]] to i64
// CHECK-NEXT:    [[ARRAYIDX7:%.*]] = getelementptr inbounds [2 x i8], ptr [[C]], i64 0, i64 [[IDXPROM]]
// CHECK-NEXT:    [[TMP2:%.*]] = load i8, ptr [[ARRAYIDX7]], align 1
// CHECK-NEXT:    [[TOBOOL:%.*]] = trunc i8 [[TMP2]] to i1
// CHECK-NEXT:    [[CONV:%.*]] = zext i1 [[TOBOOL]] to i32
// CHECK-NEXT:    [[CMP8:%.*]] = icmp eq i32 [[CONV]], 0
// CHECK-NEXT:    br i1 [[CMP8]], label %[[IF_THEN:.*]], label %[[IF_ELSE:.*]]
// CHECK:       [[IF_THEN]]:
// CHECK-NEXT:    [[TMP3:%.*]] = load i32, ptr [[I]], align 4
// CHECK-NEXT:    [[CALL:%.*]] = call i32 (ptr, ...) @printf(ptr noundef @.str.25, i32 noundef [[TMP3]])
// CHECK-NEXT:    br label %[[IF_END:.*]]
// CHECK:       [[IF_ELSE]]:
// CHECK-NEXT:    [[TMP4:%.*]] = load i32, ptr [[I]], align 4
// CHECK-NEXT:    [[CALL9:%.*]] = call i32 (ptr, ...) @printf(ptr noundef @.str.26, i32 noundef [[TMP4]])
// CHECK-NEXT:    br label %[[IF_END]]
// CHECK:       [[IF_END]]:
// CHECK-NEXT:    br label %[[FOR_INC:.*]]
// CHECK:       [[FOR_INC]]:
// CHECK-NEXT:    [[TMP5:%.*]] = load i32, ptr [[I]], align 4
// CHECK-NEXT:    [[INC:%.*]] = add nsw i32 [[TMP5]], 1
// CHECK-NEXT:    store i32 [[INC]], ptr [[I]], align 4
// CHECK-NEXT:    br label %[[FOR_COND]], !llvm.loop [[LOOP6:![0-9]+]]
// CHECK:       [[FOR_END]]:
// CHECK-NEXT:    ret i32 0
//
int main(int argc, char *argv[]) {
  qbit q[2];
  cbit c[2];

  PrepZ(q[0]);
  PrepZ(q[1]);

  if (ODD_OR_EVEN) {
    X(q[0]);
  } else {
    X(q[1]);
  }

  MeasZ(q[0], c[0]);
  MeasZ(q[1], c[1]);

  for (int i = 0; i < 2; i++) {
    // std::cout << "Bit#" << i << " was" << (c[i] == 0 ? " not" : "") << " flipped.\n";
    if (c[i] == 0) {
      printf("Bit# %d was not flipped.\n", i);
    } else {
      printf("Bit# %d was flipped.\n", i);
    }
  }

  return 0;
}
