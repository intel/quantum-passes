; RUN: opt -S %loadintelquantumpasses -p=q-annotations-to-json,spin-native-annotate,print-circuit-qbb --print-format tex --quantum-debug=PrintLatex -o /dev/null %s 2>&1 | FileCheck %s --dump-input fail
; ModuleID = 'test.ll'
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

@.str = private unnamed_addr constant [325 x i8] c"{     \22matrix_real\22 : \221/sqrt(2)*[1, 1, 1, -1]\22,     \22matrix_imag\22 : [0, 0, 0, 0],     \22matrix_order\22 : \22rm\22,     \22is_hermitian\22 : true,     \22is_unitary\22 : true,     \22is_mutable\22 : true,     \22qubit_list\22 : [0],     \22parametric_list\22 : [],     \22control_qubit_list\22 : [],     \22local_basis_list\22 : [4],     \22identifier\22 : 3   }\00", section "llvm.metadata"
@.str.1 = private unnamed_addr constant [37 x i8] c"include/clang/Quantum/quintrinsics.h\00", section "llvm.metadata"
@.str.2 = private unnamed_addr constant [313 x i8] c"{     \22matrix_real\22 : [0, 1, 1, 0],     \22matrix_imag\22 : [0, 0, 0, 0],     \22matrix_order\22 : \22rm\22,     \22is_hermitian\22 : true,     \22is_unitary\22 : true,     \22is_mutable\22 : true,     \22qubit_list\22 : [0],     \22parametric_list\22 : [],     \22control_qubit_list\22 : [],     \22local_basis_list\22 : [1],     \22identifier\22 : 12   }\00", section "llvm.metadata"
@.str.3 = private unnamed_addr constant [314 x i8] c"{     \22matrix_real\22 : [0, 0, 0, 0],     \22matrix_imag\22 : [0, -1, 1, 0],     \22matrix_order\22 : \22rm\22,     \22is_hermitian\22 : true,     \22is_unitary\22 : true,     \22is_mutable\22 : true,     \22qubit_list\22 : [0],     \22parametric_list\22 : [],     \22control_qubit_list\22 : [],     \22local_basis_list\22 : [2],     \22identifier\22 : 18   }\00", section "llvm.metadata"
@.str.4 = private unnamed_addr constant [313 x i8] c"{     \22matrix_real\22 : [1, 0, 0, -1],     \22matrix_imag\22 : [0, 0, 0, 0],     \22matrix_order\22 : \22rm\22,     \22is_hermitian\22 : true,     \22is_unitary\22 : true,     \22is_mutable\22 : true,     \22qubit_list\22 : [0],     \22parametric_list\22 : [],     \22control_qubit_list\22 : [],     \22local_basis_list\22 : [3],     \22identifier\22 : 6   }\00", section "llvm.metadata"
@.str.5 = private unnamed_addr constant [313 x i8] c"{     \22matrix_real\22 : [1, 0, 0, 0],     \22matrix_imag\22 : [0, 0, 0, 1],     \22matrix_order\22 : \22rm\22,     \22is_hermitian\22 : false,     \22is_unitary\22 : true,     \22is_mutable\22 : true,     \22qubit_list\22 : [0],     \22parametric_list\22 : [],     \22control_qubit_list\22 : [],     \22local_basis_list\22 : [3],     \22identifier\22 : 5   }\00", section "llvm.metadata"
@.str.6 = private unnamed_addr constant [315 x i8] c"{     \22matrix_real\22 : [1, 0, 0, 0],     \22matrix_imag\22 : [0, 0, 0, -1],     \22matrix_order\22 : \22rm\22,     \22is_hermitian\22 : false,     \22is_unitary\22 : true,     \22is_mutable\22 : true,     \22qubit_list\22 : [0],     \22parametric_list\22 : [],     \22control_qubit_list\22 : [],     \22local_basis_list\22 : [3],     \22identifier\22 : 11   }\00", section "llvm.metadata"
@.str.7 = private unnamed_addr constant [334 x i8] c"{     \22matrix_real\22 : \22[1, 0, 0, cos(pi/4)]\22,     \22matrix_imag\22 : \22[0, 0, 0, sin(pi/4)]\22,     \22matrix_order\22 : \22rm\22,     \22is_hermitian\22 : false,     \22is_unitary\22 : true,     \22is_mutable\22 : true,     \22qubit_list\22 : [0],     \22parametric_list\22 : [],     \22control_qubit_list\22 : [],     \22local_basis_list\22 : [3],     \22identifier\22 : 24   }\00", section "llvm.metadata"
@.str.8 = private unnamed_addr constant [335 x i8] c"{     \22matrix_real\22 : \22[1, 0, 0, cos(pi/4)]\22,     \22matrix_imag\22 : \22[0, 0, 0, -sin(pi/4)]\22,     \22matrix_order\22 : \22rm\22,     \22is_hermitian\22 : false,     \22is_unitary\22 : true,     \22is_mutable\22 : true,     \22qubit_list\22 : [0],     \22parametric_list\22 : [],     \22control_qubit_list\22 : [],     \22local_basis_list\22 : [3],     \22identifier\22 : 25   }\00", section "llvm.metadata"
@.str.9 = private unnamed_addr constant [364 x i8] c"{     \22matrix_real\22 : \22[cos(angle/2), 0, 0, cos(angle/2)]\22,     \22matrix_imag\22 : \22[0, -sin(angle/2), sin(angle/2), 0]\22,     \22matrix_order\22 : \22rm\22,     \22is_hermitian\22 : false,     \22is_unitary\22 : true,     \22is_mutable\22 : true,     \22qubit_list\22 : [0],     \22parametric_list\22 : [1],     \22control_qubit_list\22 : [],     \22local_basis_list\22 : [1],     \22identifier\22 : 26   }\00", section "llvm.metadata"
@.str.10 = private unnamed_addr constant [362 x i8] c"{     \22matrix_real\22 : \22[cos(angle/2), -sin(angle/2), sin(angle/2), cos(angle/2)]\22,     \22matrix_imag\22 : [0, 0, 0, 0],     \22matrix_order\22 : \22rm\22,     \22is_hermitian\22 : false,     \22is_unitary\22 : true,     \22is_mutable\22 : true,     \22qubit_list\22 : [0],     \22parametric_list\22 : [1],     \22control_qubit_list\22 : [],     \22local_basis_list\22 : [2],     \22identifier\22 : 27   }\00", section "llvm.metadata"
@.str.11 = private unnamed_addr constant [364 x i8] c"{     \22matrix_real\22 : \22[cos(angle/2), 0, 0, cos(angle/2)]\22,     \22matrix_imag\22 : \22[-sin(angle/2), 0, 0, sin(angle/2)]\22,     \22matrix_order\22 : \22rm\22,     \22is_hermitian\22 : false,     \22is_unitary\22 : true,     \22is_mutable\22 : true,     \22qubit_list\22 : [0],     \22parametric_list\22 : [1],     \22control_qubit_list\22 : [],     \22local_basis_list\22 : [3],     \22identifier\22 : 28   }\00", section "llvm.metadata"
@.str.12 = private unnamed_addr constant [396 x i8] c"{     \22matrix_real\22 : [1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, -1],     \22matrix_imag\22 : [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0],     \22matrix_order\22 : \22rm\22,     \22is_hermitian\22 : true,     \22is_unitary\22 : true,     \22is_mutable\22 : true,     \22qubit_list\22 : [0, 1],     \22parametric_list\22 : [],     \22control_qubit_list\22 : [0, 1],     \22local_basis_list\22 : [3, 3],     \22identifier\22 : 37   }\00", section "llvm.metadata"
@.str.13 = private unnamed_addr constant [392 x i8] c"{     \22matrix_real\22 : [1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0],     \22matrix_imag\22 : [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0],     \22matrix_order\22 : \22rm\22,     \22is_hermitian\22 : true,     \22is_unitary\22 : true,     \22is_mutable\22 : true,     \22qubit_list\22 : [0, 1],     \22parametric_list\22 : [],     \22control_qubit_list\22 : [0],     \22local_basis_list\22 : [3, 1],     \22identifier\22 : 35   }\00", section "llvm.metadata"
@.str.14 = private unnamed_addr constant [391 x i8] c"{     \22matrix_real\22 : [1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1],     \22matrix_imag\22 : [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0],     \22matrix_order\22 : \22rm\22,     \22is_hermitian\22 : true,     \22is_unitary\22 : true,     \22is_mutable\22 : true,     \22qubit_list\22 : [0, 1],     \22parametric_list\22 : [],     \22control_qubit_list\22 : [],     \22local_basis_list\22 : [0, 0],     \22identifier\22 : 38   }\00", section "llvm.metadata"
@.str.15 = private unnamed_addr constant [689 x i8] c"{     \22matrix_real\22 : [1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0],     \22matrix_imag\22 : [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0],     \22matrix_order\22 : \22rm\22,     \22is_hermitian\22 : true,     \22is_unitary\22 : true,     \22is_mutable\22 : true,     \22qubit_list\22 : [0, 1, 2],     \22parametric_list\22 : [],     \22control_qubit_list\22 : [0, 1],     \22local_basis_list\22 : [3, 3, 1],     \22identifier\22 : 39   }\00", section "llvm.metadata"
@.str.16 = private unnamed_addr constant [417 x i8] c"{     \22matrix_real\22 : \22{qt(prob0) : [0.5, 0.5, 0.5, 0.5], qt(prob1) : [0.5, -0.5, 0.5, -0.5]}\22,     \22matrix_imag\22 : \22{qt(prob0) : [0, 0, 0, 0], qt(prob1) : [0, 0, 0, 0]}\22,     \22matrix_order\22 : \22rm\22,     \22is_hermitian\22 : false,     \22is_unitary\22 : false,     \22is_mutable\22 : true,     \22qubit_list\22 : [0],     \22parametric_list\22 : [],     \22control_qubit_list\22 : [],     \22local_basis_list\22 : [1],     \22identifier\22 : 40   }\00", section "llvm.metadata"
@.str.17 = private unnamed_addr constant [417 x i8] c"{     \22matrix_real\22 : \22{qt(prob0) : [0.5, 0, 0, 0.5], qt(prob1) : [0.5, 0, 0, -0.5]}\22,     \22matrix_imag\22 : \22{qt(prob0) : [0, 0.5, -0.5, 0], qt(prob1) : [0, 0.5, 0.5, 0]}\22,     \22matrix_order\22 : \22rm\22,     \22is_hermitian\22 : false,     \22is_unitary\22 : false,     \22is_mutable\22 : true,     \22qubit_list\22 : [0],     \22parametric_list\22 : [],     \22control_qubit_list\22 : [],     \22local_basis_list\22 : [2],     \22identifier\22 : 41   }\00", section "llvm.metadata"
@.str.18 = private unnamed_addr constant [399 x i8] c"{     \22matrix_real\22 : \22{qt(prob0) : [1, 0, 0, 0], qt(prob1) : [0, 1, 0, 0]}\22,     \22matrix_imag\22 : \22{qt(prob0) : [0, 0, 0, 0], qt(prob1) : [0, 0, 0, 0]}\22,     \22matrix_order\22 : \22rm\22,     \22is_hermitian\22 : false,     \22is_unitary\22 : false,     \22is_mutable\22 : true,     \22qubit_list\22 : [0],     \22parametric_list\22 : [],     \22control_qubit_list\22 : [],     \22local_basis_list\22 : [3],     \22identifier\22 : 42   }\00", section "llvm.metadata"
@.str.19 = private unnamed_addr constant [421 x i8] c"{     \22matrix_real\22 : \22{qt(prob0) : [0.5, 0.5, 0.5, 0.5], qt(prob1) : [0.5, -0.5, -0.5, 0.5]}\22,     \22matrix_imag\22 : \22{qt(prob0) : [0, 0, 0, 0], qt(prob1) : [0, 0, 0, 0]}\22,     \22matrix_order\22 : \22rm\22,     \22is_hermitian\22 : false,     \22is_unitary\22 : false,     \22is_mutable\22 : true,     \22qubit_list\22 : [0],     \22parametric_list\22 : [1, 2],     \22control_qubit_list\22 : [],     \22local_basis_list\22 : [1],     \22identifier\22 : 43   }\00", section "llvm.metadata"
@.str.20 = private unnamed_addr constant [421 x i8] c"{     \22matrix_real\22 : \22{qt(prob0) : [0.5, 0, 0, 0.5], qt(prob1) : [0.5, 0, 0, 0.5]}\22,     \22matrix_imag\22 : \22{qt(prob0) : [0, 0.5, -0.5, 0], qt(prob1) : [0, -0.5, 0.5, 0]}\22,     \22matrix_order\22 : \22rm\22,     \22is_hermitian\22 : false,     \22is_unitary\22 : false,     \22is_mutable\22 : true,     \22qubit_list\22 : [0],     \22parametric_list\22 : [1, 2],     \22control_qubit_list\22 : [],     \22local_basis_list\22 : [2],     \22identifier\22 : 44   }\00", section "llvm.metadata"
@.str.21 = private unnamed_addr constant [403 x i8] c"{     \22matrix_real\22 : \22{qt(prob0) : [1, 0, 0, 0], qt(prob1) : [0, 0, 0, 1]}\22,     \22matrix_imag\22 : \22{qt(prob0) : [0, 0, 0, 0], qt(prob1) : [0, 0, 0, 0]}\22,     \22matrix_order\22 : \22rm\22,     \22is_hermitian\22 : false,     \22is_unitary\22 : false,     \22is_mutable\22 : true,     \22qubit_list\22 : [0],     \22parametric_list\22 : [1, 2],     \22control_qubit_list\22 : [],     \22local_basis_list\22 : [3],     \22identifier\22 : 45   }\00", section "llvm.metadata"
@.str.22 = private unnamed_addr constant [420 x i8] c"{     \22matrix_real\22 : \22[1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, cos(angle)]\22,     \22matrix_imag\22 : \22[0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -sin(angle)]\22,     \22matrix_order\22 : \22rm\22,     \22is_hermitian\22 : false,     \22is_unitary\22 : true,     \22is_mutable\22 : true,     \22qubit_list\22 : [0, 1],     \22parametric_list\22 : [2],     \22control_qubit_list\22 : [0, 1],     \22local_basis_list\22 : [3, 3],     \22identifier\22 : 46   }\00", section "llvm.metadata"
@.str.23 = private unnamed_addr constant [423 x i8] c"{     \22matrix_real\22 : \22[cos(theta/2), -sin(theta/2) sin(phi), sin(theta/2) sin(phi), cos(theta/2)]\22,     \22matrix_imag\22 : \22[0, -sin(theta/2)cos(phi), sin(theta/2)cos(phi), 0]\22,     \22matrix_order\22 : \22rm\22,     \22is_hermitian\22 : false,     \22is_unitary\22 : true,     \22is_mutable\22 : true,     \22qubit_list\22 : [0],     \22parametric_list\22 : [1,2],     \22control_qubit_list\22 : [],     \22local_basis_list\22 : [0],     \22identifier\22 : 47   }\00", section "llvm.metadata"
@.str.24 = private unnamed_addr constant [503 x i8] c"{     \22matrix_real\22 : \22[1, 0, 0, 0, 0, (1+cos(angle))/2, (1-cos(angle))/2, 0, 0, (1-cos(angle))/2, (1+cos(angle))/2, 0, 0, 0, 0, 1]\22,     \22matrix_imag\22 : \22[1, 0, 0, 0, 0, sin(angle)/2, -sin(angle)/2, 0, 0, -sin(angle)/2, sin(angle)/2, 0, 0, 0, 0, 1]\22,     \22matrix_order\22 : \22rm\22,     \22is_hermitian\22 : false,     \22is_unitary\22 : true,     \22is_mutable\22 : true,     \22qubit_list\22 : [0, 1],     \22parametric_list\22 : [2],     \22control_qubit_list\22 : [],     \22local_basis_list\22 : [0, 0],     \22identifier\22 : 53   }\00", section "llvm.metadata"
@q = dso_local global [3 x i16] [i16 0, i16 1, i16 2], align 2
@c = dso_local global [3 x i8] zeroinitializer, align 1
@llvm.global.annotations = appending global [24 x { ptr, ptr, ptr, i32, ptr }] [{ ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str, ptr @.str.1, i32 48, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.2, ptr @.str.1, i32 68, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.3, ptr @.str.1, i32 88, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.4, ptr @.str.1, i32 109, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.5, ptr @.str.1, i32 129, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.6, ptr @.str.1, i32 149, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.7, ptr @.str.1, i32 169, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.8, ptr @.str.1, i32 189, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.9, ptr @.str.1, i32 209, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.10, ptr @.str.1, i32 230, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.11, ptr @.str.1, i32 251, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.12, ptr @.str.1, i32 272, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.13, ptr @.str.1, i32 293, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.14, ptr @.str.1, i32 314, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.15, ptr @.str.1, i32 335, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.16, ptr @.str.1, i32 359, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.17, ptr @.str.1, i32 377, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.18, ptr @.str.1, i32 395, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.19, ptr @.str.1, i32 413, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.20, ptr @.str.1, i32 433, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.21, ptr @.str.1, i32 453, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.22, ptr @.str.1, i32 481, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.23, ptr @.str.1, i32 503, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.24, ptr @.str.1, i32 525, ptr null }], section "llvm.metadata"
@aqcc.qsda = global [65536 x double] zeroinitializer #0

; Function Attrs: mustprogress noinline nounwind uwtable
define dso_local void @_Z3allv.stub() local_unnamed_addr #1 {
entry:
  br label %aqcc.quantum

aqcc.quantum:                                     ; preds = %entry
  %QRTcall = call i32 @_ZN4qrt014scheduleKernelEPFvvEj(ptr @"_Z12all().QBB.0.v.stub", i32 48)
  br label %aqcc.meas.move.end

aqcc.meas.move.end:                               ; preds = %aqcc.quantum
  %QRTcast2 = bitcast ptr @c to ptr
  call void @_ZN4qrt020updateMeasurementMapEPbmb(ptr %QRTcast2, i64 0, i1 false)
  %QRTcast1 = bitcast ptr getelementptr inbounds ([3 x i8], ptr @c, i64 0, i64 2) to ptr
  call void @_ZN4qrt020updateMeasurementMapEPbmb(ptr %QRTcast1, i64 2, i1 false)
  %QRTcast = bitcast ptr getelementptr inbounds ([3 x i8], ptr @c, i64 0, i64 1) to ptr
  call void @_ZN4qrt020updateMeasurementMapEPbmb(ptr %QRTcast, i64 1, i1 false)
  store [3 x i16] [i16 0, i16 1, i16 2], ptr @q, align 2
  ret void
}

; Function Attrs: mustprogress norecurse nounwind uwtable
define dso_local noundef i32 @main() local_unnamed_addr #2 {
entry:
  tail call void @_Z3allv.stub() #3
  ret i32 0
}

declare i32 @_ZN4qrt021initializeQrtSystem64EPdPcS1_(ptr, ptr, ptr)

declare i1 @_ZN4qrt013isInitializedEv()

declare void @_ZN4qrt020updateMeasurementMapEPbmb(ptr, i64, i1)

declare i32 @_ZN4qrt014scheduleKernelEPFvvEj(ptr, i32)

declare i32 @_ZN4qrt018dismantleQrtSystemEv()

; Function Attrs: nounwind
declare void @llvm.quantum.rotz(i16, double, i32 immarg) #3

; Function Attrs: nounwind
declare void @llvm.quantum.rotxy(i16, double, double, i32 immarg) #3

; Function Attrs: nounwind
declare void @llvm.quantum.cphase(i16, i16, double, i32 immarg) #3

; Function Attrs: nounwind
declare void @llvm.quantum.swapalp(i16, i16, double, i32 immarg) #3

; Function Attrs: nounwind
declare void @llvm.quantum.measz(i16, ptr, i1, i32 immarg) #3

; Function Attrs: nounwind
declare void @llvm.quantum.prepz(i16, i32 immarg) #3

; Function Attrs: nounwind
declare void @llvm.quantum.rotz.imm(i16 immarg, double immarg, i32 immarg) #3

; Function Attrs: nounwind
declare void @llvm.quantum.rotxy.imm(i16 immarg, double immarg, double immarg, i32 immarg) #3

; Function Attrs: nounwind
declare void @llvm.quantum.cphase.imm(i16 immarg, i16 immarg, double immarg, i32 immarg) #3

; Function Attrs: nounwind
declare void @llvm.quantum.swapalp.imm(i16 immarg, i16 immarg, double immarg, i32 immarg) #3

; Function Attrs: nounwind
declare void @llvm.quantum.measz.imm(i16 immarg, i32 immarg) #3

; Function Attrs: nounwind
declare void @llvm.quantum.prepz.imm(i16 immarg, i32 immarg) #3

; Function Attrs: nounwind
declare void @llvm.quantum.rotz.imm.index(i16 immarg, i32 immarg, i32 immarg) #3

; Function Attrs: nounwind
declare void @llvm.quantum.rotxy.imm.index(i16 immarg, i32 immarg, i32 immarg, i32 immarg) #3

; Function Attrs: nounwind
declare void @llvm.quantum.cphase.imm.index(i16 immarg, i16 immarg, i32 immarg, i32 immarg) #3

; Function Attrs: nounwind
declare void @llvm.quantum.swapalp.imm.index(i16 immarg, i16 immarg, i32 immarg, i32 immarg) #3

; Function Attrs: nounwind
declare void @llvm.quantum.measz.imm.index(i16 immarg, i32 immarg, i32 immarg) #3

define void @"_Z12all().QBB.0.v.stub"() section ".qbbs_text" {
aqcc.quantum:
  call void @llvm.quantum.rotxy.imm(i16 0, double 0x3FF921FB54442D18, double 0x4012D97C7F3321D2, i32 1)
  call void @llvm.quantum.prepz.imm(i16 0, i32 0)
  call void @llvm.quantum.rotxy.imm(i16 0, double 0x3FF921FB54442D18, double 0x3FF921FB54442D18, i32 0)
  call void @llvm.quantum.prepz.imm(i16 1, i32 0)
  call void @llvm.quantum.rotxy.imm(i16 2, double 0x3FF921FB54442D18, double 0.000000e+00, i32 0)
  call void @llvm.quantum.prepz.imm(i16 2, i32 0)
  call void @llvm.quantum.rotxy.imm(i16 2, double 0x3FF921FB54442D18, double 0x400921FB54442D18, i32 0)
  call void @llvm.quantum.rotxy.imm(i16 0, double 0x400921FB54442D18, double 0.000000e+00, i32 0)
  call void @llvm.quantum.rotxy.imm(i16 0, double 0x3FF921FB54442D18, double 0x3FF921FB54442D18, i32 0)
  call void @llvm.quantum.rotxy.imm(i16 0, double 0x400921FB54442D18, double 0.000000e+00, i32 0)
  call void @llvm.quantum.rotxy.imm(i16 1, double 0x400921FB54442D18, double 0x3FF921FB54442D18, i32 0)
  call void @llvm.quantum.rotxy.imm(i16 0, double 0x400921FB54442D18, double 0.000000e+00, i32 0)
  call void @llvm.quantum.rotxy.imm(i16 0, double 0x400921FB54442D18, double 0x3FF921FB54442D18, i32 0)
  call void @llvm.quantum.rotxy.imm(i16 2, double 0x400921FB54442D18, double 0.000000e+00, i32 0)
  call void @llvm.quantum.rotxy.imm(i16 2, double 0x400921FB54442D18, double 0x3FE921FB54442D18, i32 0)
  call void @llvm.quantum.rotxy.imm(i16 0, double 0x400921FB54442D18, double 0.000000e+00, i32 0)
  call void @llvm.quantum.rotxy.imm(i16 0, double 0x400921FB54442D18, double 0x4002D97C7F3321D2, i32 0)
  call void @llvm.quantum.rotxy.imm(i16 1, double 0x400921FB54442D18, double 0.000000e+00, i32 0)
  call void @llvm.quantum.rotxy.imm(i16 1, double 0x400921FB54442D18, double 0x3FD921FB54442D18, i32 0)
  call void @llvm.quantum.rotxy.imm(i16 2, double 0x400921FB54442D18, double 0.000000e+00, i32 0)
  call void @llvm.quantum.rotxy.imm(i16 2, double 0x400921FB54442D18, double 0x4005FDBBE9BBA775, i32 0)
  call void @llvm.quantum.rotxy.imm(i16 0, double 1.000000e-01, double 0.000000e+00, i32 0)
  call void @llvm.quantum.rotxy.imm(i16 1, double 0x400921FB54442D18, double 0.000000e+00, i32 0)
  call void @llvm.quantum.rotxy.imm(i16 1, double 0x400921FB54442D18, double 1.500000e-01, i32 0)
  call void @llvm.quantum.rotxy.imm(i16 2, double 8.000000e-01, double 0x3FF921FB54442D18, i32 0)
  call void @llvm.quantum.rotxy.imm(i16 2, double 5.000000e-01, double 6.000000e-01, i32 0)
  call void @llvm.quantum.cphase.imm(i16 0, i16 1, double 0x400921FB54442D18, i32 0)
  call void @llvm.quantum.rotxy.imm(i16 1, double 0x3FF921FB54442D18, double 0x4012D97C7F3321D2, i32 0)
  call void @llvm.quantum.cphase.imm(i16 2, i16 1, double 0x400921FB54442D18, i32 0)
  call void @llvm.quantum.rotxy.imm(i16 1, double 0x3FF921FB54442D18, double 0x3FF921FB54442D18, i32 0)
  call void @llvm.quantum.swapalp.imm(i16 0, i16 1, double 3.000000e-01, i32 0)
  call void @llvm.quantum.rotxy.imm(i16 2, double 0x3FF921FB54442D18, double 0x4012D97C7F3321D2, i32 0)
  call void @llvm.quantum.cphase.imm(i16 0, i16 1, double 0x3FF921FB54442D18, i32 0)
  call void @llvm.quantum.cphase.imm(i16 1, i16 2, double 0x3FF921FB54442D18, i32 0)
  call void @llvm.quantum.rotxy.imm(i16 2, double 0x3FF921FB54442D18, double 0x3FF921FB54442D18, i32 0)
  call void @llvm.quantum.cphase.imm(i16 0, i16 2, double 0x400921FB54442D18, i32 0)
  call void @llvm.quantum.rotxy.imm(i16 2, double 0x3FF921FB54442D18, double 0x4012D97C7F3321D2, i32 0)
  call void @llvm.quantum.cphase.imm(i16 1, i16 2, double 0x4012D97C7F3321D2, i32 0)
  call void @llvm.quantum.rotxy.imm(i16 2, double 0x3FF921FB54442D18, double 0x3FF921FB54442D18, i32 0)
  call void @llvm.quantum.cphase.imm(i16 0, i16 2, double 0x400921FB54442D18, i32 0)
  call void @llvm.quantum.swapalp.imm(i16 0, i16 2, double 0x400921FB54442D18, i32 0)
  call void @llvm.quantum.rotxy.imm(i16 1, double 0x3FF921FB54442D18, double 0x4012D97C7F3321D2, i32 0)
  call void @llvm.quantum.measz.imm.index(i16 1, i32 1, i32 0)
  call void @llvm.quantum.rotxy.imm(i16 1, double 0x3FF921FB54442D18, double 0x3FF921FB54442D18, i32 0)
  call void @llvm.quantum.measz.imm.index(i16 2, i32 2, i32 0)
  call void @llvm.quantum.rotxy.imm(i16 0, double 0x3FF921FB54442D18, double 0.000000e+00, i32 0)
  call void @llvm.quantum.measz.imm.index(i16 0, i32 0, i32 0)
  call void @llvm.quantum.rotxy.imm(i16 0, double 0x3FF921FB54442D18, double 0x400921FB54442D18, i32 2)
  ret void
}

attributes #0 = { "quantum_shared_var" }
attributes #1 = { mustprogress noinline nounwind uwtable "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #2 = { mustprogress norecurse nounwind uwtable "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #3 = { nounwind }
; CHECK: LaTex instructions: 
; CHECK: \lstick{\ket{qid\_0}} & \gate{RXY(1.571, 4.712)} & \gate{PrepZ} & \gate{RXY(1.571, 1.571)} & \gate{RXY(3.142, 0.)} & \gate{RXY(1.571, 1.571)} & \gate{RXY(3.142, 0.)} & \gate{RXY(3.142, 0.)} & \gate{RXY(3.142, 1.571)} & \gate{RXY(3.142, 0.)} & \gate{RXY(3.142, 2.356)} & \gate{RXY(0.1, 0.)} & \ctrl{1} & \qw & \qw & \qw & \gate{SwapA(0.3)} & \ctrl{1} & \qw & \qw & \ctrl{2} & \qw & \qw & \qw & \qw & \ctrl{2} & \qw & \qswap & \gate{RXY(1.571, 0.)} & \measure{\mbox{Z}} & \gate{RXY(1.571, 3.142)} & \qw \\ 
; CHECK: \lstick{\ket{qid\_1}} & \gate{PrepZ} & \gate{RXY(3.142, 1.571)} & \gate{RXY(3.142, 0.)} & \gate{RXY(3.142, 0.3927)} & \gate{RXY(3.142, 0.)} & \gate{RXY(3.142, 0.15)} & \qw & \qw & \qw & \qw & \qw & \gate{Z} & \gate{RXY(1.571, 4.712)} & \gate{Z} & \gate{RXY(1.571, 1.571)} & \gate{SwapA(0.3)} \qwx[-1] & \gate{Phase(1.571)} & \ctrl{1} & \qw & \qw & \qw & \ctrl{1} & \gate{RXY(1.571, 4.712)} & \measure{\mbox{Z}} & \qw & \gate{RXY(1.571, 1.571)} & \qw & \qw & \qw & \qw & \qw \\ 
; CHECK: \lstick{\ket{qid\_2}} & \gate{RXY(1.571, 0.)} & \gate{PrepZ} & \gate{RXY(1.571, 3.142)} & \gate{RXY(3.142, 0.)} & \gate{RXY(3.142, 0.7854)} & \gate{RXY(3.142, 0.)} & \gate{RXY(3.142, 2.749)} & \gate{RXY(0.8, 1.571)} & \gate{RXY(0.5, 0.6)} & \qw & \qw & \qw & \qw & \ctrl{-1} & \gate{RXY(1.571, 4.712)} & \qw & \qw & \gate{Phase(1.571)} & \gate{RXY(1.571, 1.571)} & \gate{Z} & \gate{RXY(1.571, 4.712)} & \gate{Phase(4.712)} & \gate{RXY(1.571, 1.571)} & \qw & \gate{Z} & \qw & \qswap \qwx[-2] & \measure{\mbox{Z}} & \qw & \qw & \qw 