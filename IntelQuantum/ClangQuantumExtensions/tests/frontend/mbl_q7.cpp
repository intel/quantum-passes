// NOTE: Assertions have been autogenerated by utils/update_cc_test_checks.py UTC_ARGS: --version 3
// RUN: %clang -S -emit-llvm %include_dirs %loadintelclangextensions  %s -o - | FileCheck %s --dump-input fail


#include "clang/Quantum/quintrinsics.h"

qbit q[7];
cbit c[7];

// CHECK-LABEL: define dso_local noundef i32 @main(
// CHECK-SAME: ) #[[ATTR2:[0-9]+]] {
// CHECK:    [[RETVAL:%.*]] = alloca i32, align 4
// CHECK-NEXT:    store i32 0, ptr [[RETVAL]], align 4
// CHECK-NEXT:    call void @_Z1XRt(ptr noundef nonnull align 2 dereferenceable(2) @q)
// CHECK-NEXT:    call void @_Z1XRt(ptr noundef nonnull align 2 dereferenceable(2) getelementptr inbounds ([7 x i16], ptr @q, i64 0, i64 2))
// CHECK-NEXT:    call void @_Z1XRt(ptr noundef nonnull align 2 dereferenceable(2) getelementptr inbounds ([7 x i16], ptr @q, i64 0, i64 4))
// CHECK-NEXT:    call void @_Z1XRt(ptr noundef nonnull align 2 dereferenceable(2) getelementptr inbounds ([7 x i16], ptr @q, i64 0, i64 6))
// CHECK-NEXT:    call void @_Z4CNOTRtS_(ptr noundef nonnull align 2 dereferenceable(2) @q, ptr noundef nonnull align 2 dereferenceable(2) getelementptr inbounds ([7 x i16], ptr @q, i64 0, i64 1))
// CHECK-NEXT:    call void @_Z4CNOTRtS_(ptr noundef nonnull align 2 dereferenceable(2) getelementptr inbounds ([7 x i16], ptr @q, i64 0, i64 2), ptr noundef nonnull align 2 dereferenceable(2) getelementptr inbounds ([7 x i16], ptr @q, i64 0, i64 3))
// CHECK-NEXT:    call void @_Z4CNOTRtS_(ptr noundef nonnull align 2 dereferenceable(2) getelementptr inbounds ([7 x i16], ptr @q, i64 0, i64 4), ptr noundef nonnull align 2 dereferenceable(2) getelementptr inbounds ([7 x i16], ptr @q, i64 0, i64 5))
// CHECK-NEXT:    call void @_Z2RZRtd(ptr noundef nonnull align 2 dereferenceable(2) getelementptr inbounds ([7 x i16], ptr @q, i64 0, i64 6), double noundef 0x4027B0228ED0F7FD)
// CHECK-NEXT:    call void @_Z2RZRtd(ptr noundef nonnull align 2 dereferenceable(2) @q, double noundef 0x4023208DCA464625)
// CHECK-NEXT:    call void @_Z2RZRtd(ptr noundef nonnull align 2 dereferenceable(2) getelementptr inbounds ([7 x i16], ptr @q, i64 0, i64 1), double noundef 8.000000e+00)
// CHECK-NEXT:    call void @_Z2RZRtd(ptr noundef nonnull align 2 dereferenceable(2) getelementptr inbounds ([7 x i16], ptr @q, i64 0, i64 2), double noundef 0x3FF6A4AADAA35752)
// CHECK-NEXT:    call void @_Z2RZRtd(ptr noundef nonnull align 2 dereferenceable(2) getelementptr inbounds ([7 x i16], ptr @q, i64 0, i64 3), double noundef 8.000000e+00)
// CHECK-NEXT:    call void @_Z2RZRtd(ptr noundef nonnull align 2 dereferenceable(2) getelementptr inbounds ([7 x i16], ptr @q, i64 0, i64 4), double noundef 0x40225AB85AA5E40E)
// CHECK-NEXT:    call void @_Z2RZRtd(ptr noundef nonnull align 2 dereferenceable(2) getelementptr inbounds ([7 x i16], ptr @q, i64 0, i64 5), double noundef 8.000000e+00)
// CHECK-NEXT:    call void @_Z1HRt(ptr noundef nonnull align 2 dereferenceable(2) @q)
// CHECK-NEXT:    call void @_Z1HRt(ptr noundef nonnull align 2 dereferenceable(2) getelementptr inbounds ([7 x i16], ptr @q, i64 0, i64 2))
// CHECK-NEXT:    call void @_Z1HRt(ptr noundef nonnull align 2 dereferenceable(2) getelementptr inbounds ([7 x i16], ptr @q, i64 0, i64 4))
// CHECK-NEXT:    call void @_Z2RZRtd(ptr noundef nonnull align 2 dereferenceable(2) @q, double noundef 0x3FFB7812AEEF4459)
// CHECK-NEXT:    call void @_Z2RZRtd(ptr noundef nonnull align 2 dereferenceable(2) getelementptr inbounds ([7 x i16], ptr @q, i64 0, i64 2), double noundef 0x3FFB7812AEEF4459)
// CHECK-NEXT:    call void @_Z2RZRtd(ptr noundef nonnull align 2 dereferenceable(2) getelementptr inbounds ([7 x i16], ptr @q, i64 0, i64 4), double noundef 0x3FFB7812AEEF4459)
// CHECK-NEXT:    call void @_Z4CNOTRtS_(ptr noundef nonnull align 2 dereferenceable(2) getelementptr inbounds ([7 x i16], ptr @q, i64 0, i64 1), ptr noundef nonnull align 2 dereferenceable(2) @q)
// CHECK-NEXT:    call void @_Z4CNOTRtS_(ptr noundef nonnull align 2 dereferenceable(2) getelementptr inbounds ([7 x i16], ptr @q, i64 0, i64 3), ptr noundef nonnull align 2 dereferenceable(2) getelementptr inbounds ([7 x i16], ptr @q, i64 0, i64 2))
// CHECK-NEXT:    call void @_Z4CNOTRtS_(ptr noundef nonnull align 2 dereferenceable(2) getelementptr inbounds ([7 x i16], ptr @q, i64 0, i64 5), ptr noundef nonnull align 2 dereferenceable(2) getelementptr inbounds ([7 x i16], ptr @q, i64 0, i64 4))
// CHECK-NEXT:    call void @_Z2RZRtd(ptr noundef nonnull align 2 dereferenceable(2) @q, double noundef 0x4025B2F8FE66442C)
// CHECK-NEXT:    call void @_Z2RZRtd(ptr noundef nonnull align 2 dereferenceable(2) getelementptr inbounds ([7 x i16], ptr @q, i64 0, i64 2), double noundef 0x4025B2F8FE66442C)
// CHECK-NEXT:    call void @_Z2RZRtd(ptr noundef nonnull align 2 dereferenceable(2) getelementptr inbounds ([7 x i16], ptr @q, i64 0, i64 4), double noundef 0x4025B2F8FE66442C)
// CHECK-NEXT:    call void @_Z4CNOTRtS_(ptr noundef nonnull align 2 dereferenceable(2) getelementptr inbounds ([7 x i16], ptr @q, i64 0, i64 1), ptr noundef nonnull align 2 dereferenceable(2) @q)
// CHECK-NEXT:    call void @_Z4CNOTRtS_(ptr noundef nonnull align 2 dereferenceable(2) getelementptr inbounds ([7 x i16], ptr @q, i64 0, i64 3), ptr noundef nonnull align 2 dereferenceable(2) getelementptr inbounds ([7 x i16], ptr @q, i64 0, i64 2))
// CHECK-NEXT:    call void @_Z4CNOTRtS_(ptr noundef nonnull align 2 dereferenceable(2) getelementptr inbounds ([7 x i16], ptr @q, i64 0, i64 5), ptr noundef nonnull align 2 dereferenceable(2) getelementptr inbounds ([7 x i16], ptr @q, i64 0, i64 4))
// CHECK-NEXT:    call void @_Z1HRt(ptr noundef nonnull align 2 dereferenceable(2) @q)
// CHECK-NEXT:    call void @_Z1HRt(ptr noundef nonnull align 2 dereferenceable(2) getelementptr inbounds ([7 x i16], ptr @q, i64 0, i64 2))
// CHECK-NEXT:    call void @_Z1HRt(ptr noundef nonnull align 2 dereferenceable(2) getelementptr inbounds ([7 x i16], ptr @q, i64 0, i64 4))
// CHECK-NEXT:    call void @_Z4CNOTRtS_(ptr noundef nonnull align 2 dereferenceable(2) @q, ptr noundef nonnull align 2 dereferenceable(2) getelementptr inbounds ([7 x i16], ptr @q, i64 0, i64 1))
// CHECK-NEXT:    call void @_Z4CNOTRtS_(ptr noundef nonnull align 2 dereferenceable(2) getelementptr inbounds ([7 x i16], ptr @q, i64 0, i64 2), ptr noundef nonnull align 2 dereferenceable(2) getelementptr inbounds ([7 x i16], ptr @q, i64 0, i64 3))
// CHECK-NEXT:    call void @_Z4CNOTRtS_(ptr noundef nonnull align 2 dereferenceable(2) getelementptr inbounds ([7 x i16], ptr @q, i64 0, i64 4), ptr noundef nonnull align 2 dereferenceable(2) getelementptr inbounds ([7 x i16], ptr @q, i64 0, i64 5))
// CHECK-NEXT:    call void @_Z2RXRtd(ptr noundef nonnull align 2 dereferenceable(2) @q, double noundef 0x4013C9D45DCAC069)
// CHECK-NEXT:    call void @_Z4CNOTRtS_(ptr noundef nonnull align 2 dereferenceable(2) getelementptr inbounds ([7 x i16], ptr @q, i64 0, i64 1), ptr noundef nonnull align 2 dereferenceable(2) getelementptr inbounds ([7 x i16], ptr @q, i64 0, i64 2))
// CHECK-NEXT:    call void @_Z4CNOTRtS_(ptr noundef nonnull align 2 dereferenceable(2) getelementptr inbounds ([7 x i16], ptr @q, i64 0, i64 3), ptr noundef nonnull align 2 dereferenceable(2) getelementptr inbounds ([7 x i16], ptr @q, i64 0, i64 4))
// CHECK-NEXT:    call void @_Z4CNOTRtS_(ptr noundef nonnull align 2 dereferenceable(2) getelementptr inbounds ([7 x i16], ptr @q, i64 0, i64 5), ptr noundef nonnull align 2 dereferenceable(2) getelementptr inbounds ([7 x i16], ptr @q, i64 0, i64 6))
// CHECK-NEXT:    call void @_Z2RZRtd(ptr noundef nonnull align 2 dereferenceable(2) getelementptr inbounds ([7 x i16], ptr @q, i64 0, i64 1), double noundef 0x401C48122CADE12F)
// CHECK-NEXT:    call void @_Z2RZRtd(ptr noundef nonnull align 2 dereferenceable(2) getelementptr inbounds ([7 x i16], ptr @q, i64 0, i64 2), double noundef 8.000000e+00)
// CHECK-NEXT:    call void @_Z2RZRtd(ptr noundef nonnull align 2 dereferenceable(2) getelementptr inbounds ([7 x i16], ptr @q, i64 0, i64 3), double noundef 0x3FFE7C72183E883B)
// CHECK-NEXT:    call void @_Z2RZRtd(ptr noundef nonnull align 2 dereferenceable(2) getelementptr inbounds ([7 x i16], ptr @q, i64 0, i64 4), double noundef 8.000000e+00)
// CHECK-NEXT:    call void @_Z2RZRtd(ptr noundef nonnull align 2 dereferenceable(2) getelementptr inbounds ([7 x i16], ptr @q, i64 0, i64 5), double noundef 0x4018F92744F1072F)
// CHECK-NEXT:    call void @_Z2RZRtd(ptr noundef nonnull align 2 dereferenceable(2) getelementptr inbounds ([7 x i16], ptr @q, i64 0, i64 6), double noundef 8.000000e+00)
// CHECK-NEXT:    call void @_Z1HRt(ptr noundef nonnull align 2 dereferenceable(2) getelementptr inbounds ([7 x i16], ptr @q, i64 0, i64 1))
// CHECK-NEXT:    call void @_Z1HRt(ptr noundef nonnull align 2 dereferenceable(2) getelementptr inbounds ([7 x i16], ptr @q, i64 0, i64 3))
// CHECK-NEXT:    call void @_Z1HRt(ptr noundef nonnull align 2 dereferenceable(2) getelementptr inbounds ([7 x i16], ptr @q, i64 0, i64 5))
// CHECK-NEXT:    call void @_Z2RZRtd(ptr noundef nonnull align 2 dereferenceable(2) getelementptr inbounds ([7 x i16], ptr @q, i64 0, i64 1), double noundef 0x3FFB7812AEEF4459)
// CHECK-NEXT:    call void @_Z2RZRtd(ptr noundef nonnull align 2 dereferenceable(2) getelementptr inbounds ([7 x i16], ptr @q, i64 0, i64 3), double noundef 0x3FFB7812AEEF4459)
// CHECK-NEXT:    call void @_Z2RZRtd(ptr noundef nonnull align 2 dereferenceable(2) getelementptr inbounds ([7 x i16], ptr @q, i64 0, i64 5), double noundef 0x3FFB7812AEEF4459)
// CHECK-NEXT:    call void @_Z4CNOTRtS_(ptr noundef nonnull align 2 dereferenceable(2) getelementptr inbounds ([7 x i16], ptr @q, i64 0, i64 2), ptr noundef nonnull align 2 dereferenceable(2) getelementptr inbounds ([7 x i16], ptr @q, i64 0, i64 1))
// CHECK-NEXT:    call void @_Z4CNOTRtS_(ptr noundef nonnull align 2 dereferenceable(2) getelementptr inbounds ([7 x i16], ptr @q, i64 0, i64 4), ptr noundef nonnull align 2 dereferenceable(2) getelementptr inbounds ([7 x i16], ptr @q, i64 0, i64 3))
// CHECK-NEXT:    call void @_Z4CNOTRtS_(ptr noundef nonnull align 2 dereferenceable(2) getelementptr inbounds ([7 x i16], ptr @q, i64 0, i64 6), ptr noundef nonnull align 2 dereferenceable(2) getelementptr inbounds ([7 x i16], ptr @q, i64 0, i64 5))
// CHECK-NEXT:    call void @_Z2RZRtd(ptr noundef nonnull align 2 dereferenceable(2) getelementptr inbounds ([7 x i16], ptr @q, i64 0, i64 1), double noundef 0x4025B2F8FE66442C)
// CHECK-NEXT:    call void @_Z2RZRtd(ptr noundef nonnull align 2 dereferenceable(2) getelementptr inbounds ([7 x i16], ptr @q, i64 0, i64 3), double noundef 0x4025B2F8FE66442C)
// CHECK-NEXT:    call void @_Z2RZRtd(ptr noundef nonnull align 2 dereferenceable(2) getelementptr inbounds ([7 x i16], ptr @q, i64 0, i64 5), double noundef 0x4025B2F8FE66442C)
// CHECK-NEXT:    call void @_Z4CNOTRtS_(ptr noundef nonnull align 2 dereferenceable(2) getelementptr inbounds ([7 x i16], ptr @q, i64 0, i64 2), ptr noundef nonnull align 2 dereferenceable(2) getelementptr inbounds ([7 x i16], ptr @q, i64 0, i64 1))
// CHECK-NEXT:    call void @_Z4CNOTRtS_(ptr noundef nonnull align 2 dereferenceable(2) getelementptr inbounds ([7 x i16], ptr @q, i64 0, i64 4), ptr noundef nonnull align 2 dereferenceable(2) getelementptr inbounds ([7 x i16], ptr @q, i64 0, i64 3))
// CHECK-NEXT:    call void @_Z4CNOTRtS_(ptr noundef nonnull align 2 dereferenceable(2) getelementptr inbounds ([7 x i16], ptr @q, i64 0, i64 6), ptr noundef nonnull align 2 dereferenceable(2) getelementptr inbounds ([7 x i16], ptr @q, i64 0, i64 5))
// CHECK-NEXT:    call void @_Z1HRt(ptr noundef nonnull align 2 dereferenceable(2) getelementptr inbounds ([7 x i16], ptr @q, i64 0, i64 1))
// CHECK-NEXT:    call void @_Z1HRt(ptr noundef nonnull align 2 dereferenceable(2) getelementptr inbounds ([7 x i16], ptr @q, i64 0, i64 3))
// CHECK-NEXT:    call void @_Z1HRt(ptr noundef nonnull align 2 dereferenceable(2) getelementptr inbounds ([7 x i16], ptr @q, i64 0, i64 5))
// CHECK-NEXT:    call void @_Z4CNOTRtS_(ptr noundef nonnull align 2 dereferenceable(2) getelementptr inbounds ([7 x i16], ptr @q, i64 0, i64 1), ptr noundef nonnull align 2 dereferenceable(2) getelementptr inbounds ([7 x i16], ptr @q, i64 0, i64 2))
// CHECK-NEXT:    call void @_Z4CNOTRtS_(ptr noundef nonnull align 2 dereferenceable(2) getelementptr inbounds ([7 x i16], ptr @q, i64 0, i64 3), ptr noundef nonnull align 2 dereferenceable(2) getelementptr inbounds ([7 x i16], ptr @q, i64 0, i64 4))
// CHECK-NEXT:    call void @_Z4CNOTRtS_(ptr noundef nonnull align 2 dereferenceable(2) getelementptr inbounds ([7 x i16], ptr @q, i64 0, i64 5), ptr noundef nonnull align 2 dereferenceable(2) getelementptr inbounds ([7 x i16], ptr @q, i64 0, i64 6))
// CHECK-NEXT:    call void @_Z2RXRtd(ptr noundef nonnull align 2 dereferenceable(2) getelementptr inbounds ([7 x i16], ptr @q, i64 0, i64 1), double noundef 0x40142AD833D5964B)
// CHECK-NEXT:    call void @_Z2RXRtd(ptr noundef nonnull align 2 dereferenceable(2) getelementptr inbounds ([7 x i16], ptr @q, i64 0, i64 2), double noundef 0x3FF412DB519D4FE8)
// CHECK-NEXT:    call void @_Z2RXRtd(ptr noundef nonnull align 2 dereferenceable(2) getelementptr inbounds ([7 x i16], ptr @q, i64 0, i64 3), double noundef 0x4028C148BD7CD19C)
// CHECK-NEXT:    call void @_Z2RXRtd(ptr noundef nonnull align 2 dereferenceable(2) getelementptr inbounds ([7 x i16], ptr @q, i64 0, i64 4), double noundef 0x40206E02BC4AC2D2)
// CHECK-NEXT:    call void @_Z2RXRtd(ptr noundef nonnull align 2 dereferenceable(2) getelementptr inbounds ([7 x i16], ptr @q, i64 0, i64 5), double noundef 0x4024EA7BEB762BBB)
// CHECK-NEXT:    call void @_Z5MeasZRtRbb(ptr noundef nonnull align 2 dereferenceable(2) @q, ptr noundef nonnull align 1 dereferenceable(1) @c, i1 noundef zeroext false)
// CHECK-NEXT:    call void @_Z5MeasZRtRbb(ptr noundef nonnull align 2 dereferenceable(2) getelementptr inbounds ([7 x i16], ptr @q, i64 0, i64 1), ptr noundef nonnull align 1 dereferenceable(1) getelementptr inbounds ([7 x i8], ptr @c, i64 0, i64 1), i1 noundef zeroext false)
// CHECK-NEXT:    call void @_Z5MeasZRtRbb(ptr noundef nonnull align 2 dereferenceable(2) getelementptr inbounds ([7 x i16], ptr @q, i64 0, i64 2), ptr noundef nonnull align 1 dereferenceable(1) getelementptr inbounds ([7 x i8], ptr @c, i64 0, i64 2), i1 noundef zeroext false)
// CHECK-NEXT:    call void @_Z5MeasZRtRbb(ptr noundef nonnull align 2 dereferenceable(2) getelementptr inbounds ([7 x i16], ptr @q, i64 0, i64 3), ptr noundef nonnull align 1 dereferenceable(1) getelementptr inbounds ([7 x i8], ptr @c, i64 0, i64 3), i1 noundef zeroext false)
// CHECK-NEXT:    call void @_Z5MeasZRtRbb(ptr noundef nonnull align 2 dereferenceable(2) getelementptr inbounds ([7 x i16], ptr @q, i64 0, i64 4), ptr noundef nonnull align 1 dereferenceable(1) getelementptr inbounds ([7 x i8], ptr @c, i64 0, i64 4), i1 noundef zeroext false)
// CHECK-NEXT:    call void @_Z5MeasZRtRbb(ptr noundef nonnull align 2 dereferenceable(2) getelementptr inbounds ([7 x i16], ptr @q, i64 0, i64 5), ptr noundef nonnull align 1 dereferenceable(1) getelementptr inbounds ([7 x i8], ptr @c, i64 0, i64 5), i1 noundef zeroext false)
// CHECK-NEXT:    call void @_Z5MeasZRtRbb(ptr noundef nonnull align 2 dereferenceable(2) getelementptr inbounds ([7 x i16], ptr @q, i64 0, i64 6), ptr noundef nonnull align 1 dereferenceable(1) getelementptr inbounds ([7 x i8], ptr @c, i64 0, i64 6), i1 noundef zeroext false)
// CHECK-NEXT:    ret i32 0
//
int main() {

  X(q[0]);
  X(q[2]);
  X(q[4]);
  X(q[6]);

  CNOT(q[0], q[1]);
  CNOT(q[2], q[3]);
  CNOT(q[4], q[5]);

  RZ(q[6], 11.844013655671);
  RZ(q[0], 9.563581772879);
  RZ(q[1], 8.0);
  RZ(q[2], 1.415202001624);
  RZ(q[3], 8.0);
  RZ(q[4], 9.177187760115);
  RZ(q[5], 8.0);

  H(q[0]);
  H(q[2]);
  H(q[4]);

  RZ(q[0], 1.71681469282);
  RZ(q[2], 1.71681469282);
  RZ(q[4], 1.71681469282);

  CNOT(q[1], q[0]);
  CNOT(q[3], q[2]);
  CNOT(q[5], q[4]);

  RZ(q[0], 10.849555921539);
  RZ(q[2], 10.849555921539);
  RZ(q[4], 10.849555921539);

  CNOT(q[1], q[0]);
  CNOT(q[3], q[2]);
  CNOT(q[5], q[4]);

  H(q[0]);
  H(q[2]);
  H(q[4]);

  CNOT(q[0], q[1]);
  CNOT(q[2], q[3]);
  CNOT(q[4], q[5]);

  RX(q[0], 4.94709917593);

  CNOT(q[1], q[2]);
  CNOT(q[3], q[4]);
  CNOT(q[5], q[6]);

  RZ(q[1], 7.070381830323);
  RZ(q[2], 8.0);
  RZ(q[3], 1.905382246689);
  RZ(q[4], 8.0);
  RZ(q[5], 6.243313863002);
  RZ(q[6], 8.0);

  H(q[1]);
  H(q[3]);
  H(q[5]);

  RZ(q[1], 1.71681469282);
  RZ(q[3], 1.71681469282);
  RZ(q[5], 1.71681469282);

  CNOT(q[2], q[1]);
  CNOT(q[4], q[3]);
  CNOT(q[6], q[5]);

  RZ(q[1], 10.849555921539);
  RZ(q[3], 10.849555921539);
  RZ(q[5], 10.849555921539);

  CNOT(q[2], q[1]);
  CNOT(q[4], q[3]);
  CNOT(q[6], q[5]);

  H(q[1]);
  H(q[3]);
  H(q[5]);

  CNOT(q[1], q[2]);
  CNOT(q[3], q[4]);
  CNOT(q[5], q[6]);

  RX(q[1], 5.041840372001);
  RX(q[2], 1.254603689963);
  RX(q[3], 12.377508088576);
  RX(q[4], 8.214864620329);
  RX(q[5], 10.45797668283);

  MeasZ(q[0], c[0]);
  MeasZ(q[1], c[1]);
  MeasZ(q[2], c[2]);
  MeasZ(q[3], c[3]);
  MeasZ(q[4], c[4]);
  MeasZ(q[5], c[5]);
  MeasZ(q[6], c[6]);

  return 0;
}

/* Testing of the intrinsics is covered in other Clang tests */


















































































