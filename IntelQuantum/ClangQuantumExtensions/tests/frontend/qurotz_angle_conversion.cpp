// NOTE: Assertions have been autogenerated by utils/update_cc_test_checks.py UTC_ARGS: --version 3
// RUN: %clang -S -emit-llvm %include_dirs %loadintelclangextensions  %s -o - | FileCheck %s --dump-input fail


#include "clang/Quantum/quintrinsics.h"

//<------------------------------------------------------------------------->//
/* simple arithmetic on a constant angle */
// CHECK-LABEL: define dso_local void @_Z11angle_test1t(
// CHECK-SAME: i16 noundef zeroext [[Q0:%.*]]) #[[ATTR0:[0-9]+]] section ".qbbs_text" {
// CHECK:    [[Q0_ADDR:%.*]] = alloca i16, align 2
// CHECK-NEXT:    [[ANGLE:%.*]] = alloca double, align 8
// CHECK-NEXT:    store i16 [[Q0]], ptr [[Q0_ADDR]], align 2
// CHECK-NEXT:    store double 0x400921FB5444261E, ptr [[ANGLE]], align 8
// CHECK-NEXT:    call void @_Z2RZRtd(ptr noundef nonnull align 2 dereferenceable(2) [[Q0_ADDR]], double noundef 0x400921FB5444261E)
// CHECK-NEXT:    call void @_Z2RZRtd(ptr noundef nonnull align 2 dereferenceable(2) [[Q0_ADDR]], double noundef 0x3FF921FB5444261E)
// CHECK-NEXT:    call void @_Z2RZRtd(ptr noundef nonnull align 2 dereferenceable(2) [[Q0_ADDR]], double noundef 0x401921FB5444261E)
// CHECK-NEXT:    ret void
//
quantum_kernel void angle_test1(qbit q0) {
  const double angle = 3.141592653589;
  RZ(q0, angle);
  RZ(q0, angle / 2);
  RZ(q0, angle * 2);
}











//<------------------------------------------------------------------------->//
/* simple arithmetic on two constant angles */
// CHECK-LABEL: define dso_local void @_Z11angle_test2t(
// CHECK-SAME: i16 noundef zeroext [[Q0:%.*]]) #[[ATTR0]] section ".qbbs_text" {
// CHECK:    [[Q0_ADDR:%.*]] = alloca i16, align 2
// CHECK-NEXT:    [[A:%.*]] = alloca double, align 8
// CHECK-NEXT:    [[B:%.*]] = alloca double, align 8
// CHECK-NEXT:    store i16 [[Q0]], ptr [[Q0_ADDR]], align 2
// CHECK-NEXT:    store double 0x3FDD902159BFCF34, ptr [[A]], align 8
// CHECK-NEXT:    store double 0x3FFFED7E3BBA9DCD, ptr [[B]], align 8
// CHECK-NEXT:    call void @_Z2RZRtd(ptr noundef nonnull align 2 dereferenceable(2) [[Q0_ADDR]], double noundef 0xBFF88975E54AAA00)
// CHECK-NEXT:    call void @_Z2RZRtd(ptr noundef nonnull align 2 dereferenceable(2) [[Q0_ADDR]], double noundef 0x4003A8C3491548CD)
// CHECK-NEXT:    call void @_Z2RZRtd(ptr noundef nonnull align 2 dereferenceable(2) [[Q0_ADDR]], double noundef 0x401147A1F16B29C5)
// CHECK-NEXT:    ret void
//
quantum_kernel void angle_test2(qbit q0) {
  const double A = 0.461922013898618;
  const double B = 1.995481713596621;
  RZ(q0, A - B);
  RZ(q0, A + B);
  RZ(q0, B / A);
}











//<------------------------------------------------------------------------->//
/* array of constant angles */
// CHECK-LABEL: define dso_local void @_Z11angle_test3t(
// CHECK-SAME: i16 noundef zeroext [[Q0:%.*]]) #[[ATTR0]] section ".qbbs_text" {
// CHECK:    [[Q0_ADDR:%.*]] = alloca i16, align 2
// CHECK-NEXT:    [[ARR:%.*]] = alloca [2 x double], align 16
// CHECK-NEXT:    store i16 [[Q0]], ptr [[Q0_ADDR]], align 2
// CHECK-NEXT:    call void @llvm.memcpy.p0.p0.i64(ptr align 16 [[ARR]], ptr align 16 @__const._Z11angle_test3t.arr, i64 16, i1 false)
// CHECK-NEXT:    [[ARRAYIDX:%.*]] = getelementptr inbounds [2 x double], ptr [[ARR]], i64 0, i64 0
// CHECK-NEXT:    [[TMP0:%.*]] = load double, ptr [[ARRAYIDX]], align 16
// CHECK-NEXT:    call void @_Z2RZRtd(ptr noundef nonnull align 2 dereferenceable(2) [[Q0_ADDR]], double noundef [[TMP0]])
// CHECK-NEXT:    [[ARRAYIDX1:%.*]] = getelementptr inbounds [2 x double], ptr [[ARR]], i64 0, i64 1
// CHECK-NEXT:    [[TMP1:%.*]] = load double, ptr [[ARRAYIDX1]], align 8
// CHECK-NEXT:    call void @_Z2RZRtd(ptr noundef nonnull align 2 dereferenceable(2) [[Q0_ADDR]], double noundef [[TMP1]])
// CHECK-NEXT:    [[ARRAYIDX2:%.*]] = getelementptr inbounds [2 x double], ptr [[ARR]], i64 0, i64 0
// CHECK-NEXT:    [[TMP2:%.*]] = load double, ptr [[ARRAYIDX2]], align 16
// CHECK-NEXT:    [[ARRAYIDX3:%.*]] = getelementptr inbounds [2 x double], ptr [[ARR]], i64 0, i64 1
// CHECK-NEXT:    [[TMP3:%.*]] = load double, ptr [[ARRAYIDX3]], align 8
// CHECK-NEXT:    [[ADD:%.*]] = fadd double [[TMP2]], [[TMP3]]
// CHECK-NEXT:    call void @_Z2RZRtd(ptr noundef nonnull align 2 dereferenceable(2) [[Q0_ADDR]], double noundef [[ADD]])
// CHECK-NEXT:    ret void
//
quantum_kernel void angle_test3(qbit q0) {
  const double arr[2] = {0.5, 3.5};
  RZ(q0, arr[0]);
  RZ(q0, arr[1]);
  RZ(q0, arr[0] + arr[1]);
}











//<------------------------------------------------------------------------->//
/* for-loop with an array of constant angles */
const int N = 2;
qbit q[N];
const double arr[N] = {1.57, 3.14};

// CHECK-LABEL: define dso_local noundef i32 @main(
// CHECK-SAME: ) #[[ATTR3:[0-9]+]] {
// CHECK:    [[RETVAL:%.*]] = alloca i32, align 4
// CHECK-NEXT:    [[I:%.*]] = alloca i32, align 4
// CHECK-NEXT:    store i32 0, ptr [[RETVAL]], align 4
// CHECK-NEXT:    store i32 0, ptr [[I]], align 4
// CHECK-NEXT:    br label %[[FOR_COND:.*]]
// CHECK:       [[FOR_COND]]:
// CHECK-NEXT:    [[TMP0:%.*]] = load i32, ptr [[I]], align 4
// CHECK-NEXT:    [[CMP:%.*]] = icmp slt i32 [[TMP0]], 2
// CHECK-NEXT:    br i1 [[CMP]], label %[[FOR_BODY:.*]], label %[[FOR_END:.*]]
// CHECK:       [[FOR_BODY]]:
// CHECK-NEXT:    [[TMP1:%.*]] = load i32, ptr [[I]], align 4
// CHECK-NEXT:    [[IDXPROM:%.*]] = sext i32 [[TMP1]] to i64
// CHECK-NEXT:    [[ARRAYIDX:%.*]] = getelementptr inbounds [2 x i16], ptr @q, i64 0, i64 [[IDXPROM]]
// CHECK-NEXT:    [[TMP2:%.*]] = load i32, ptr [[I]], align 4
// CHECK-NEXT:    [[IDXPROM1:%.*]] = sext i32 [[TMP2]] to i64
// CHECK-NEXT:    [[ARRAYIDX2:%.*]] = getelementptr inbounds [2 x double], ptr @_ZL3arr, i64 0, i64 [[IDXPROM1]]
// CHECK-NEXT:    [[TMP3:%.*]] = load double, ptr [[ARRAYIDX2]], align 8
// CHECK-NEXT:    call void @_Z2RZRtd(ptr noundef nonnull align 2 dereferenceable(2) [[ARRAYIDX]], double noundef [[TMP3]])
// CHECK-NEXT:    br label %[[FOR_INC:.*]]
// CHECK:       [[FOR_INC]]:
// CHECK-NEXT:    [[TMP4:%.*]] = load i32, ptr [[I]], align 4
// CHECK-NEXT:    [[INC:%.*]] = add nsw i32 [[TMP4]], 1
// CHECK-NEXT:    store i32 [[INC]], ptr [[I]], align 4
// CHECK-NEXT:    br label %[[FOR_COND]], !llvm.loop [[LOOP6:![0-9]+]]
// CHECK:       [[FOR_END]]:
// CHECK-NEXT:    ret i32 0
//
int main() {
  for (int i = 0; i < N; i++) {
    RZ(q[i], arr[i]);
  }
  return 0;
}















