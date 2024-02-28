; RUN: opt -S %loadintelquantumpasses -p=q-annotations-to-json,spin-native-annotate,simple-qbit-placement,q-stats-print  -o - %s 2>&1 | FileCheck %s --dump-input fail
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
@q = dso_local global [3 x i16] zeroinitializer, align 2
@c = dso_local global [3 x i8] zeroinitializer, align 1
@llvm.global.annotations = appending global [24 x { ptr, ptr, ptr, i32, ptr }] [{ ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str, ptr @.str.1, i32 48, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.2, ptr @.str.1, i32 68, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.3, ptr @.str.1, i32 88, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.4, ptr @.str.1, i32 109, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.5, ptr @.str.1, i32 129, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.6, ptr @.str.1, i32 149, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.7, ptr @.str.1, i32 169, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.8, ptr @.str.1, i32 189, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.9, ptr @.str.1, i32 209, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.10, ptr @.str.1, i32 230, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.11, ptr @.str.1, i32 251, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.12, ptr @.str.1, i32 272, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.13, ptr @.str.1, i32 293, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.14, ptr @.str.1, i32 314, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.15, ptr @.str.1, i32 335, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.16, ptr @.str.1, i32 359, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.17, ptr @.str.1, i32 377, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.18, ptr @.str.1, i32 395, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.19, ptr @.str.1, i32 413, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.20, ptr @.str.1, i32 433, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.21, ptr @.str.1, i32 453, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.22, ptr @.str.1, i32 481, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.23, ptr @.str.1, i32 503, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.24, ptr @.str.1, i32 525, ptr null }], section "llvm.metadata"

; Function Attrs: mustprogress noinline nounwind uwtable
define dso_local void @_Z7PrepAllv.stub() local_unnamed_addr #0 section ".qbbs_text" {
entry:
  br label %aqcc.quantum

aqcc.quantum:                                     ; preds = %entry
  %arrayidx11 = getelementptr inbounds [3 x i16], ptr @q, i64 0, i64 0
  %0 = load i16, ptr %arrayidx11, align 2
  call void @llvm.quantum.rotxy(i16 %0, double 0x3FF921FB54442D18, double 0x4012D97C7F3321D2, i32 1)
  %arrayidx10 = getelementptr inbounds [3 x i16], ptr @q, i64 0, i64 0
  %1 = load i16, ptr %arrayidx10, align 2
  call void @llvm.quantum.prepz(i16 %1, i32 0)
  %arrayidx9 = getelementptr inbounds [3 x i16], ptr @q, i64 0, i64 0
  %2 = load i16, ptr %arrayidx9, align 2
  call void @llvm.quantum.rotxy(i16 %2, double 0x3FF921FB54442D18, double 0x3FF921FB54442D18, i32 0)
  %arrayidx8 = getelementptr inbounds [3 x i16], ptr @q, i64 0, i64 1
  %3 = load i16, ptr %arrayidx8, align 2
  call void @llvm.quantum.rotxy(i16 %3, double 0x3FF921FB54442D18, double 0x4012D97C7F3321D2, i32 0)
  %arrayidx7 = getelementptr inbounds [3 x i16], ptr @q, i64 0, i64 1
  %4 = load i16, ptr %arrayidx7, align 2
  call void @llvm.quantum.prepz(i16 %4, i32 0)
  %arrayidx6 = getelementptr inbounds [3 x i16], ptr @q, i64 0, i64 1
  %5 = load i16, ptr %arrayidx6, align 2
  call void @llvm.quantum.rotxy(i16 %5, double 0x3FF921FB54442D18, double 0x3FF921FB54442D18, i32 0)
  %arrayidx5 = getelementptr inbounds [3 x i16], ptr @q, i64 0, i64 2
  %6 = load i16, ptr %arrayidx5, align 2
  call void @llvm.quantum.rotxy(i16 %6, double 0x3FF921FB54442D18, double 0x4012D97C7F3321D2, i32 0)
  %arrayidx4 = getelementptr inbounds [3 x i16], ptr @q, i64 0, i64 2
  %7 = load i16, ptr %arrayidx4, align 2
  call void @llvm.quantum.prepz(i16 %7, i32 0)
  %arrayidx3 = getelementptr inbounds [3 x i16], ptr @q, i64 0, i64 2
  %8 = load i16, ptr %arrayidx3, align 2
  call void @llvm.quantum.rotxy(i16 %8, double 0x3FF921FB54442D18, double 0x3FF921FB54442D18, i32 2)
  br label %aqcc.meas.move.end

aqcc.meas.move.end:                               ; preds = %aqcc.quantum
  ret void
}

; Function Attrs: mustprogress noinline nounwind uwtable
define dso_local void @_Z10VQE_anzatzPd.stub(ptr nocapture nofree noundef nonnull readonly align 8 dereferenceable(8) %angs) local_unnamed_addr #0 section ".qbbs_text" {
entry:
  %0 = load double, ptr %angs, align 8, !tbaa !0
  %arrayidx1 = getelementptr inbounds double, ptr %angs, i64 1
  %1 = load double, ptr %arrayidx1, align 8, !tbaa !0
  %arrayidx2 = getelementptr inbounds double, ptr %angs, i64 2
  %2 = load double, ptr %arrayidx2, align 8, !tbaa !0
  %arrayidx3 = getelementptr inbounds double, ptr %angs, i64 3
  %3 = load double, ptr %arrayidx3, align 8, !tbaa !0
  %arrayidx4 = getelementptr inbounds double, ptr %angs, i64 4
  %4 = load double, ptr %arrayidx4, align 8, !tbaa !0
  br label %aqcc.quantum

aqcc.quantum:                                     ; preds = %entry
  %arrayidx38 = getelementptr inbounds [3 x i16], ptr @q, i64 0, i64 0
  %5 = load i16, ptr %arrayidx38, align 2
  call void @llvm.quantum.rotxy(i16 %5, double %0, double 0.000000e+00, i32 1)
  %arrayidx37 = getelementptr inbounds [3 x i16], ptr @q, i64 0, i64 1
  %6 = load i16, ptr %arrayidx37, align 2
  call void @llvm.quantum.rotxy(i16 %6, double %1, double 0.000000e+00, i32 0)
  %arrayidx36 = getelementptr inbounds [3 x i16], ptr @q, i64 0, i64 2
  %7 = load i16, ptr %arrayidx36, align 2
  call void @llvm.quantum.rotxy(i16 %7, double %2, double 0.000000e+00, i32 0)
  %arrayidx35 = getelementptr inbounds [3 x i16], ptr @q, i64 0, i64 1
  %8 = load i16, ptr %arrayidx35, align 2
  call void @llvm.quantum.rotxy(i16 %8, double 0x3FF921FB54442D18, double 0x4012D97C7F3321D2, i32 0)
  %arrayidx33 = getelementptr inbounds [3 x i16], ptr @q, i64 0, i64 0
  %9 = load i16, ptr %arrayidx33, align 2
  %arrayidx34 = getelementptr inbounds [3 x i16], ptr @q, i64 0, i64 1
  %10 = load i16, ptr %arrayidx34, align 2
  call void @llvm.quantum.cphase(i16 %9, i16 %10, double 0x400921FB54442D18, i32 0)
  %arrayidx32 = getelementptr inbounds [3 x i16], ptr @q, i64 0, i64 1
  %11 = load i16, ptr %arrayidx32, align 2
  call void @llvm.quantum.rotxy(i16 %11, double 0x3FF921FB54442D18, double 0x3FF921FB54442D18, i32 0)
  %arrayidx31 = getelementptr inbounds [3 x i16], ptr @q, i64 0, i64 1
  %12 = load i16, ptr %arrayidx31, align 2
  call void @llvm.quantum.rotxy(i16 %12, double %3, double 0x3FF921FB54442D18, i32 0)
  %arrayidx30 = getelementptr inbounds [3 x i16], ptr @q, i64 0, i64 2
  %13 = load i16, ptr %arrayidx30, align 2
  call void @llvm.quantum.rotxy(i16 %13, double 0x3FF921FB54442D18, double 0x4012D97C7F3321D2, i32 0)
  %arrayidx28 = getelementptr inbounds [3 x i16], ptr @q, i64 0, i64 1
  %14 = load i16, ptr %arrayidx28, align 2
  %arrayidx29 = getelementptr inbounds [3 x i16], ptr @q, i64 0, i64 2
  %15 = load i16, ptr %arrayidx29, align 2
  call void @llvm.quantum.cphase(i16 %14, i16 %15, double 0x400921FB54442D18, i32 0)
  %arrayidx27 = getelementptr inbounds [3 x i16], ptr @q, i64 0, i64 2
  %16 = load i16, ptr %arrayidx27, align 2
  call void @llvm.quantum.rotxy(i16 %16, double 0x3FF921FB54442D18, double 0x3FF921FB54442D18, i32 0)
  %arrayidx26 = getelementptr inbounds [3 x i16], ptr @q, i64 0, i64 2
  %17 = load i16, ptr %arrayidx26, align 2
  call void @llvm.quantum.rotxy(i16 %17, double 0x400921FB54442D18, double 0.000000e+00, i32 0)
  %arrayidx25 = getelementptr inbounds [3 x i16], ptr @q, i64 0, i64 2
  %18 = load i16, ptr %arrayidx25, align 2
  %19 = fmul double %4, 5.000000e-01
  call void @llvm.quantum.rotxy(i16 %18, double 0x400921FB54442D18, double %19, i32 0)
  %arrayidx24 = getelementptr inbounds [3 x i16], ptr @q, i64 0, i64 2
  %20 = load i16, ptr %arrayidx24, align 2
  call void @llvm.quantum.rotxy(i16 %20, double 0x3FF921FB54442D18, double 0x4012D97C7F3321D2, i32 0)
  %arrayidx22 = getelementptr inbounds [3 x i16], ptr @q, i64 0, i64 1
  %21 = load i16, ptr %arrayidx22, align 2
  %arrayidx23 = getelementptr inbounds [3 x i16], ptr @q, i64 0, i64 2
  %22 = load i16, ptr %arrayidx23, align 2
  call void @llvm.quantum.cphase(i16 %21, i16 %22, double 0x400921FB54442D18, i32 0)
  %arrayidx21 = getelementptr inbounds [3 x i16], ptr @q, i64 0, i64 2
  %23 = load i16, ptr %arrayidx21, align 2
  call void @llvm.quantum.rotxy(i16 %23, double 0x3FF921FB54442D18, double 0x3FF921FB54442D18, i32 0)
  %arrayidx20 = getelementptr inbounds [3 x i16], ptr @q, i64 0, i64 1
  %24 = load i16, ptr %arrayidx20, align 2
  call void @llvm.quantum.rotxy(i16 %24, double 0x3FF921FB54442D18, double 0x4012D97C7F3321D2, i32 0)
  %arrayidx18 = getelementptr inbounds [3 x i16], ptr @q, i64 0, i64 0
  %25 = load i16, ptr %arrayidx18, align 2
  %arrayidx19 = getelementptr inbounds [3 x i16], ptr @q, i64 0, i64 1
  %26 = load i16, ptr %arrayidx19, align 2
  call void @llvm.quantum.cphase(i16 %25, i16 %26, double 0x400921FB54442D18, i32 0)
  %arrayidx17 = getelementptr inbounds [3 x i16], ptr @q, i64 0, i64 1
  %27 = load i16, ptr %arrayidx17, align 2
  call void @llvm.quantum.rotxy(i16 %27, double 0x3FF921FB54442D18, double 0x3FF921FB54442D18, i32 2)
  br label %aqcc.meas.move.end

aqcc.meas.move.end:                               ; preds = %aqcc.quantum
  ret void
}

; Function Attrs: mustprogress noinline nounwind uwtable
define dso_local void @_Z5MeasHv.stub() local_unnamed_addr #0 section ".qbbs_text" {
entry:
  br label %aqcc.quantum

aqcc.quantum:                                     ; preds = %entry
  %arrayidx9 = getelementptr inbounds [3 x i16], ptr @q, i64 0, i64 0
  %0 = load i16, ptr %arrayidx9, align 2
  call void @llvm.quantum.measz(i16 %0, ptr @c, i1 false, i32 1)
  %arrayidx8 = getelementptr inbounds [3 x i16], ptr @q, i64 0, i64 1
  %1 = load i16, ptr %arrayidx8, align 2
  call void @llvm.quantum.rotxy(i16 %1, double 0x3FF921FB54442D18, double 0x4012D97C7F3321D2, i32 0)
  %arrayidx7 = getelementptr inbounds [3 x i16], ptr @q, i64 0, i64 1
  %2 = load i16, ptr %arrayidx7, align 2
  call void @llvm.quantum.measz(i16 %2, ptr getelementptr inbounds ([3 x i8], ptr @c, i64 0, i64 1), i1 false, i32 0)
  %arrayidx6 = getelementptr inbounds [3 x i16], ptr @q, i64 0, i64 1
  %3 = load i16, ptr %arrayidx6, align 2
  call void @llvm.quantum.rotxy(i16 %3, double 0x3FF921FB54442D18, double 0x3FF921FB54442D18, i32 0)
  %arrayidx5 = getelementptr inbounds [3 x i16], ptr @q, i64 0, i64 2
  %4 = load i16, ptr %arrayidx5, align 2
  call void @llvm.quantum.rotxy(i16 %4, double 0x3FF921FB54442D18, double 0.000000e+00, i32 0)
  %arrayidx4 = getelementptr inbounds [3 x i16], ptr @q, i64 0, i64 2
  %5 = load i16, ptr %arrayidx4, align 2
  call void @llvm.quantum.measz(i16 %5, ptr getelementptr inbounds ([3 x i8], ptr @c, i64 0, i64 2), i1 false, i32 0)
  %arrayidx3 = getelementptr inbounds [3 x i16], ptr @q, i64 0, i64 2
  %6 = load i16, ptr %arrayidx3, align 2
  call void @llvm.quantum.rotxy(i16 %6, double 0x3FF921FB54442D18, double 0x400921FB54442D18, i32 2)
  br label %aqcc.meas.move.end

aqcc.meas.move.end:                               ; preds = %aqcc.quantum
  ret void
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(none) uwtable
define dso_local noundef zeroext i1 @_Z13optimize_fakePdd(ptr nocapture nofree noundef readnone %angs, double noundef %cost) local_unnamed_addr #1 {
entry:
  %cmp = fcmp ogt double %cost, 0.000000e+00
  ret i1 %cmp
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(read, argmem: none, inaccessiblemem: none) uwtable
define dso_local noundef double @_Z6Energyv() local_unnamed_addr #2 {
entry:
  %0 = load i8, ptr @c, align 1, !tbaa !4, !range !6, !noundef !7
  %tobool = icmp ne i8 %0, 0
  %conv = uitofp i1 %tobool to double
  %1 = tail call double @llvm.fmuladd.f64(double noundef %conv, double noundef -2.000000e+00, double noundef 1.000000e+00)
  %2 = load i8, ptr getelementptr inbounds ([3 x i8], ptr @c, i64 0, i64 1), align 1, !tbaa !4, !range !6, !noundef !7
  %tobool1 = icmp ne i8 %2, 0
  %conv2 = uitofp i1 %tobool1 to double
  %3 = tail call double @llvm.fmuladd.f64(double noundef %conv2, double noundef -2.000000e+00, double noundef 1.000000e+00)
  %neg = fmul double %3, -1.340000e+00
  %4 = tail call double @llvm.fmuladd.f64(double noundef %1, double noundef 2.300000e-01, double noundef %neg)
  %5 = load i8, ptr getelementptr inbounds ([3 x i8], ptr @c, i64 0, i64 2), align 1, !tbaa !4, !range !6, !noundef !7
  %tobool5 = icmp ne i8 %5, 0
  %conv6 = uitofp i1 %tobool5 to double
  %6 = tail call double @llvm.fmuladd.f64(double noundef %conv6, double noundef -2.000000e+00, double noundef 1.000000e+00)
  %7 = tail call double @llvm.fmuladd.f64(double noundef %6, double noundef 2.140000e+00, double noundef %4)
  %mul = fmul double %6, 1.300000e-01
  %8 = tail call double @llvm.fmuladd.f64(double noundef %mul, double noundef %1, double noundef %7)
  ret double %8
}

; Function Attrs: nocallback nofree nosync nounwind speculatable willreturn memory(none)
declare double @llvm.fmuladd.f64(double, double, double) #3

; Function Attrs: mustprogress norecurse nounwind uwtable
define dso_local noundef i32 @main() local_unnamed_addr #4 {
entry:
  %angs = alloca [5 x double], align 16
  call void @llvm.lifetime.start.p0(i64 noundef 40, ptr noundef nonnull align 16 dereferenceable(40) %angs) #6
  store <2 x double> <double 1.000000e+00, double 1.000000e+00>, ptr %angs, align 16
  %0 = getelementptr inbounds [5 x double], ptr %angs, i64 0, i64 2
  store <2 x double> <double 1.000000e+00, double 1.000000e+00>, ptr %0, align 16
  %1 = getelementptr inbounds [5 x double], ptr %angs, i64 0, i64 4
  store double 1.000000e+00, ptr %1, align 16
  br label %while.cond

while.cond:                                       ; preds = %while.body, %entry
  %2 = load i8, ptr @c, align 1, !tbaa !4, !range !6, !noundef !7
  %tobool.i = icmp ne i8 %2, 0
  %conv.i = uitofp i1 %tobool.i to double
  %3 = tail call double @llvm.fmuladd.f64(double noundef %conv.i, double noundef -2.000000e+00, double noundef 1.000000e+00)
  %4 = load i8, ptr getelementptr inbounds ([3 x i8], ptr @c, i64 0, i64 1), align 1, !tbaa !4, !range !6, !noundef !7
  %tobool1.i = icmp ne i8 %4, 0
  %conv2.i = uitofp i1 %tobool1.i to double
  %5 = tail call double @llvm.fmuladd.f64(double noundef %conv2.i, double noundef -2.000000e+00, double noundef 1.000000e+00)
  %neg.i = fmul double %5, -1.340000e+00
  %6 = tail call double @llvm.fmuladd.f64(double noundef %3, double noundef 2.300000e-01, double noundef %neg.i)
  %7 = load i8, ptr getelementptr inbounds ([3 x i8], ptr @c, i64 0, i64 2), align 1, !tbaa !4, !range !6, !noundef !7
  %tobool5.i = icmp ne i8 %7, 0
  %conv6.i = uitofp i1 %tobool5.i to double
  %8 = tail call double @llvm.fmuladd.f64(double noundef %conv6.i, double noundef -2.000000e+00, double noundef 1.000000e+00)
  %9 = tail call double @llvm.fmuladd.f64(double noundef %8, double noundef 2.140000e+00, double noundef %6)
  %mul.i = fmul double %8, 1.300000e-01
  %10 = tail call double @llvm.fmuladd.f64(double noundef %mul.i, double noundef %3, double noundef %9)
  %cmp.i = fcmp ogt double %10, 0.000000e+00
  br i1 %cmp.i, label %while.body, label %while.end

while.body:                                       ; preds = %while.cond
  tail call void @_Z7PrepAllv.stub() #6
  call void @_Z10VQE_anzatzPd.stub(ptr noalias nocapture nofree noundef nonnull readonly align 16 dereferenceable(40) %angs) #6
  tail call void @_Z5MeasHv.stub() #6
  br label %while.cond, !llvm.loop !8

while.end:                                        ; preds = %while.cond
  call void @llvm.lifetime.end.p0(i64 noundef 40, ptr noundef nonnull align 16 dereferenceable(40) %angs) #6
  ret i32 0
}

; Function Attrs: nocallback nofree nosync nounwind willreturn memory(argmem: readwrite)
declare void @llvm.lifetime.start.p0(i64 immarg, ptr nocapture) #5

; Function Attrs: nocallback nofree nosync nounwind willreturn memory(argmem: readwrite)
declare void @llvm.lifetime.end.p0(i64 immarg, ptr nocapture) #5

declare i32 @_ZN4qrt021initializeQrtSystem64EPdPcS1_(ptr, ptr, ptr)

declare i1 @_ZN4qrt013isInitializedEv()

declare void @_ZN4qrt020updateMeasurementMapEPbmb(ptr, i64, i1)

declare i32 @_ZN4qrt014scheduleKernelEPFvvEj(ptr, i32)

declare i32 @_ZN4qrt018dismantleQrtSystemEv()

; Function Attrs: nounwind
declare void @llvm.quantum.rotz(i16, double, i32 immarg) #6

; Function Attrs: nounwind
declare void @llvm.quantum.rotxy(i16, double, double, i32 immarg) #6

; Function Attrs: nounwind
declare void @llvm.quantum.cphase(i16, i16, double, i32 immarg) #6

; Function Attrs: nounwind
declare void @llvm.quantum.swapalp(i16, i16, double, i32 immarg) #6

; Function Attrs: nounwind
declare void @llvm.quantum.measz(i16, ptr, i1, i32 immarg) #6

; Function Attrs: nounwind
declare void @llvm.quantum.prepz(i16, i32 immarg) #6

; Function Attrs: nounwind
declare void @llvm.quantum.rotz.imm(i16 immarg, double immarg, i32 immarg) #6

; Function Attrs: nounwind
declare void @llvm.quantum.rotxy.imm(i16 immarg, double immarg, double immarg, i32 immarg) #6

; Function Attrs: nounwind
declare void @llvm.quantum.cphase.imm(i16 immarg, i16 immarg, double immarg, i32 immarg) #6

; Function Attrs: nounwind
declare void @llvm.quantum.swapalp.imm(i16 immarg, i16 immarg, double immarg, i32 immarg) #6

; Function Attrs: nounwind
declare void @llvm.quantum.measz.imm(i16 immarg, i32 immarg) #6

; Function Attrs: nounwind
declare void @llvm.quantum.prepz.imm(i16 immarg, i32 immarg) #6

; Function Attrs: nounwind
declare void @llvm.quantum.rotz.imm.index(i16 immarg, i32 immarg, i32 immarg) #6

; Function Attrs: nounwind
declare void @llvm.quantum.rotxy.imm.index(i16 immarg, i32 immarg, i32 immarg, i32 immarg) #6

; Function Attrs: nounwind
declare void @llvm.quantum.cphase.imm.index(i16 immarg, i16 immarg, i32 immarg, i32 immarg) #6

; Function Attrs: nounwind
declare void @llvm.quantum.swapalp.imm.index(i16 immarg, i16 immarg, i32 immarg, i32 immarg) #6

; Function Attrs: nounwind
declare void @llvm.quantum.measz.imm.index(i16 immarg, i32 immarg, i32 immarg) #6

attributes #0 = { mustprogress noinline nounwind uwtable "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #1 = { mustprogress nofree norecurse nosync nounwind willreturn memory(none) uwtable "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #2 = { mustprogress nofree norecurse nosync nounwind willreturn memory(read, argmem: none, inaccessiblemem: none) uwtable "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #3 = { nocallback nofree nosync nounwind speculatable willreturn memory(none) }
attributes #4 = { mustprogress norecurse nounwind uwtable "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #5 = { nocallback nofree nosync nounwind willreturn memory(argmem: readwrite) }
attributes #6 = { nounwind }

!0 = !{!1, !1, i64 0}
!1 = !{!"double", !2, i64 0}
!2 = !{!"omnipotent char", !3, i64 0}
!3 = !{!"Simple C++ TBAA"}
!4 = !{!5, !5, i64 0}
!5 = !{!"bool", !2, i64 0}
!6 = !{i8 0, i8 2}
!7 = !{}
!8 = distinct !{!8, !9, !10}
!9 = !{!"llvm.loop.mustprogress"}
!10 = !{!"llvm.loop.unroll.disable"}
; CHECK:  Stats for QBB in quantum_kernel - 'PrepAll()'
; CHECK: 	Total number of gates: 9
; CHECK: 		Total number of SpinRotXY gates: 6
; CHECK: 		Total number of SpinPrepZ gates: 3
; CHECK: 	Approximate depth: 3 time units
; CHECK: 		(with an average gate time of 1 time units)
; CHECK:  Stats for QBB in quantum_kernel - 'VQE_anzatz(double*)'
; CHECK: 	Total number of gates: 18
; CHECK: 		Total number of SpinRotXY gates: 14
; CHECK: 		Total number of SpinCPhase gates: 4
; CHECK: 	Approximate depth: 14 time units
; CHECK: 		(with an average gate time of 1 time units)
; CHECK:  Stats for QBB in quantum_kernel - 'MeasH()'
; CHECK: 	Total number of gates: 7
; CHECK: 		Total number of SpinRotXY gates: 4
; CHECK: 		Total number of SpinMeasZ gates: 3
; CHECK: 	Approximate depth: 3 time units
; CHECK: 		(with an average gate time of 1 time units)
