// NOTE: Assertions have been autogenerated by utils/update_cc_test_checks.py UTC_ARGS: --version 3
// RUN: %clang -S -emit-llvm %include_dirs %loadintelclangextensions  %s -o - | FileCheck %s --dump-input fail


#include "clang/Quantum/quintrinsics.h"
#include <math.h>
#include <vector>

const int N = 4;
qbit QubitReg[N];
cbit c[N];
quantum_shared_double_array QuantumVariableParams[8];

// CHECK-LABEL: define dso_local void @_Z15simpleRangeTestv(
// CHECK-SAME: ) #[[ATTR0:[0-9]+]] section ".qbbs_text" {
// CHECK:    [[INDEX:%.*]] = alloca i32, align 4
// CHECK-NEXT:    [[INDEX1:%.*]] = alloca i32, align 4
// CHECK-NEXT:    store i32 0, ptr [[INDEX]], align 4
// CHECK-NEXT:    br label %[[FOR_COND:.*]]
// CHECK:       [[FOR_COND]]:
// CHECK-NEXT:    [[TMP0:%.*]] = load i32, ptr [[INDEX]], align 4
// CHECK-NEXT:    [[CMP:%.*]] = icmp slt i32 [[TMP0]], 4
// CHECK-NEXT:    br i1 [[CMP]], label %[[FOR_BODY:.*]], label %[[FOR_END:.*]]
// CHECK:       [[FOR_BODY]]:
// CHECK-NEXT:    [[TMP1:%.*]] = load i32, ptr [[INDEX]], align 4
// CHECK-NEXT:    [[IDXPROM:%.*]] = sext i32 [[TMP1]] to i64
// CHECK-NEXT:    [[ARRAYIDX:%.*]] = getelementptr inbounds [4 x i16], ptr @QubitReg, i64 0, i64 [[IDXPROM]]
// CHECK-NEXT:    call void @_Z5PrepZRt(ptr noundef nonnull align 2 dereferenceable(2) [[ARRAYIDX]])
// CHECK-NEXT:    br label %[[FOR_INC:.*]]
// CHECK:       [[FOR_INC]]:
// CHECK-NEXT:    [[TMP2:%.*]] = load i32, ptr [[INDEX]], align 4
// CHECK-NEXT:    [[INC:%.*]] = add nsw i32 [[TMP2]], 1
// CHECK-NEXT:    store i32 [[INC]], ptr [[INDEX]], align 4
// CHECK-NEXT:    br label %[[FOR_COND]], !llvm.loop [[LOOP6:![0-9]+]]
// CHECK:       [[FOR_END]]:
// CHECK-NEXT:    call void @_Z2RXRtd(ptr noundef nonnull align 2 dereferenceable(2) @QubitReg, double noundef 0x400921FB54442D18)
// CHECK-NEXT:    call void @_Z2RYRtd(ptr noundef nonnull align 2 dereferenceable(2) getelementptr inbounds ([4 x i16], ptr @QubitReg, i64 0, i64 1), double noundef 0x401F6A7A2955385E)
// CHECK-NEXT:    call void @_Z2RZRtd(ptr noundef nonnull align 2 dereferenceable(2) getelementptr inbounds ([4 x i16], ptr @QubitReg, i64 0, i64 2), double noundef 0x4022D97C7F3321D2)
// CHECK-NEXT:    [[TMP3:%.*]] = load double, ptr @QuantumVariableParams, align 16
// CHECK-NEXT:    call void @_Z2RXRtd(ptr noundef nonnull align 2 dereferenceable(2) @QubitReg, double noundef [[TMP3]])
// CHECK-NEXT:    [[TMP4:%.*]] = load double, ptr getelementptr inbounds ([8 x double], ptr @QuantumVariableParams, i64 0, i64 1), align 8
// CHECK-NEXT:    call void @_Z2RYRtd(ptr noundef nonnull align 2 dereferenceable(2) getelementptr inbounds ([4 x i16], ptr @QubitReg, i64 0, i64 1), double noundef [[TMP4]])
// CHECK-NEXT:    [[TMP5:%.*]] = load double, ptr getelementptr inbounds ([8 x double], ptr @QuantumVariableParams, i64 0, i64 7), align 8
// CHECK-NEXT:    call void @_Z2RZRtd(ptr noundef nonnull align 2 dereferenceable(2) getelementptr inbounds ([4 x i16], ptr @QubitReg, i64 0, i64 2), double noundef [[TMP5]])
// CHECK-NEXT:    store i32 0, ptr [[INDEX1]], align 4
// CHECK-NEXT:    br label %[[FOR_COND2:.*]]
// CHECK:       [[FOR_COND2]]:
// CHECK-NEXT:    [[TMP6:%.*]] = load i32, ptr [[INDEX1]], align 4
// CHECK-NEXT:    [[CMP3:%.*]] = icmp slt i32 [[TMP6]], 4
// CHECK-NEXT:    br i1 [[CMP3]], label %[[FOR_BODY4:.*]], label %[[FOR_END11:.*]]
// CHECK:       [[FOR_BODY4]]:
// CHECK-NEXT:    [[TMP7:%.*]] = load i32, ptr [[INDEX1]], align 4
// CHECK-NEXT:    [[IDXPROM5:%.*]] = sext i32 [[TMP7]] to i64
// CHECK-NEXT:    [[ARRAYIDX6:%.*]] = getelementptr inbounds [4 x i16], ptr @QubitReg, i64 0, i64 [[IDXPROM5]]
// CHECK-NEXT:    [[TMP8:%.*]] = load i32, ptr [[INDEX1]], align 4
// CHECK-NEXT:    [[IDXPROM7:%.*]] = sext i32 [[TMP8]] to i64
// CHECK-NEXT:    [[ARRAYIDX8:%.*]] = getelementptr inbounds [4 x i8], ptr @c, i64 0, i64 [[IDXPROM7]]
// CHECK-NEXT:    call void @_Z5MeasZRtRbb(ptr noundef nonnull align 2 dereferenceable(2) [[ARRAYIDX6]], ptr noundef nonnull align 1 dereferenceable(1) [[ARRAYIDX8]], i1 noundef zeroext false)
// CHECK-NEXT:    br label %[[FOR_INC9:.*]]
// CHECK:       [[FOR_INC9]]:
// CHECK-NEXT:    [[TMP9:%.*]] = load i32, ptr [[INDEX1]], align 4
// CHECK-NEXT:    [[INC10:%.*]] = add nsw i32 [[TMP9]], 1
// CHECK-NEXT:    store i32 [[INC10]], ptr [[INDEX1]], align 4
// CHECK-NEXT:    br label %[[FOR_COND2]], !llvm.loop [[LOOP8:![0-9]+]]
// CHECK:       [[FOR_END11]]:
// CHECK-NEXT:    ret void
//
quantum_kernel void simpleRangeTest() {
  for (int Index = 0; Index < N; Index++)
    PrepZ(QubitReg[Index]);

  RX(QubitReg[0], M_PI);
  RY(QubitReg[1], 2.5 * M_PI);
  RZ(QubitReg[2], 3 * M_PI);

  RX(QubitReg[0], QuantumVariableParams[0]);
  RY(QubitReg[1], QuantumVariableParams[1]);
  RZ(QubitReg[2], QuantumVariableParams[7]);

  for (int Index = 0; Index < N; Index++)
    MeasZ(QubitReg[Index], c[Index]);
}

// CHECK-LABEL: define dso_local noundef i32 @main(
// CHECK-SAME: ) #[[ATTR2:[0-9]+]] {
// CHECK:    [[RETVAL:%.*]] = alloca i32, align 4
// CHECK-NEXT:    store i32 0, ptr [[RETVAL]], align 4
// CHECK-NEXT:    store double 0x400921FB54442D18, ptr @QuantumVariableParams, align 16
// CHECK-NEXT:    store double 0x401F6A7A2955385E, ptr getelementptr inbounds ([8 x double], ptr @QuantumVariableParams, i64 0, i64 1), align 8
// CHECK-NEXT:    store double 0x4022D97C7F3321D2, ptr getelementptr inbounds ([8 x double], ptr @QuantumVariableParams, i64 0, i64 7), align 8
// CHECK-NEXT:    call void @_Z15simpleRangeTestv()
// CHECK-NEXT:    ret i32 0
//
int main() {
  QuantumVariableParams[0] = M_PI;
  QuantumVariableParams[1] = 2.5 * M_PI;
  QuantumVariableParams[7] = 3 * M_PI;

  simpleRangeTest();
  return 0;
}

/* Testing of the intrinsics is covered in other Clang tests */
/* NOTE: IEE754 representation of double values :
  M_PI        = 3.14159265358979323846 --> 0x400921FB54442D18
  2.5 * M_PI  = 7.85398163397448309615 --> 0x401F6A7A2955385E
  3 * M_PI    = 9.42477796076937971538 --> 0x4022D97C7F3321D2
*/











































