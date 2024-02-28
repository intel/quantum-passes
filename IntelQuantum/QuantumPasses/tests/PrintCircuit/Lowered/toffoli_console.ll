; RUN: opt -S %loadintelquantumpasses -p=q-annotations-to-json,spin-native-annotate,print-circuit-qbb --print-format console  -o /dev/null %s  | FileCheck %s --dump-input fail
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
@cat = dso_local global [5 x i16] zeroinitializer, align 2
@basil = dso_local local_unnamed_addr global [5 x i16] zeroinitializer, align 2
@c = dso_local local_unnamed_addr global [3 x i8] zeroinitializer, align 1
@llvm.global.annotations = appending global [24 x { ptr, ptr, ptr, i32, ptr }] [{ ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str, ptr @.str.1, i32 48, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.2, ptr @.str.1, i32 68, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.3, ptr @.str.1, i32 88, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.4, ptr @.str.1, i32 109, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.5, ptr @.str.1, i32 129, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.6, ptr @.str.1, i32 149, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.7, ptr @.str.1, i32 169, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.8, ptr @.str.1, i32 189, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.9, ptr @.str.1, i32 209, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.10, ptr @.str.1, i32 230, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.11, ptr @.str.1, i32 251, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.12, ptr @.str.1, i32 272, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.13, ptr @.str.1, i32 293, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.14, ptr @.str.1, i32 314, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.15, ptr @.str.1, i32 335, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.16, ptr @.str.1, i32 359, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.17, ptr @.str.1, i32 377, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.18, ptr @.str.1, i32 395, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.19, ptr @.str.1, i32 413, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.20, ptr @.str.1, i32 433, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.21, ptr @.str.1, i32 453, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.22, ptr @.str.1, i32 481, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.23, ptr @.str.1, i32 503, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.24, ptr @.str.1, i32 525, ptr null }], section "llvm.metadata"

; Function Attrs: mustprogress noinline nounwind uwtable
define dso_local void @_Z15multiQubitGatesv.stub() local_unnamed_addr #0 section ".qbbs_text" {
entry:
  br label %aqcc.quantum

aqcc.quantum:                                     ; preds = %entry
  %arrayidx50 = getelementptr inbounds [5 x i16], ptr @cat, i64 0, i64 0
  %0 = load i16, ptr %arrayidx50, align 2
  call void @llvm.quantum.rotxy(i16 %0, double 0x4012D97C7F3321D2, double 0x3FF921FB54442D18, i32 1)
  %arrayidx48 = getelementptr inbounds [5 x i16], ptr @cat, i64 0, i64 2
  %1 = load i16, ptr %arrayidx48, align 2
  %arrayidx49 = getelementptr inbounds [5 x i16], ptr @cat, i64 0, i64 1
  %2 = load i16, ptr %arrayidx49, align 2
  call void @llvm.quantum.cphase(i16 %1, i16 %2, double 0x3FF921FB54442D18, i32 0)
  %arrayidx46 = getelementptr inbounds [5 x i16], ptr @cat, i64 0, i64 1
  %3 = load i16, ptr %arrayidx46, align 2
  %arrayidx47 = getelementptr inbounds [5 x i16], ptr @cat, i64 0, i64 0
  %4 = load i16, ptr %arrayidx47, align 2
  call void @llvm.quantum.cphase(i16 %3, i16 %4, double 0x3FF921FB54442D18, i32 0)
  %arrayidx45 = getelementptr inbounds [5 x i16], ptr @cat, i64 0, i64 0
  %5 = load i16, ptr %arrayidx45, align 2
  call void @llvm.quantum.rotxy(i16 %5, double 0x3FF921FB54442D18, double 0x3FF921FB54442D18, i32 0)
  %arrayidx43 = getelementptr inbounds [5 x i16], ptr @cat, i64 0, i64 2
  %6 = load i16, ptr %arrayidx43, align 2
  %arrayidx44 = getelementptr inbounds [5 x i16], ptr @cat, i64 0, i64 0
  %7 = load i16, ptr %arrayidx44, align 2
  call void @llvm.quantum.cphase(i16 %6, i16 %7, double 0x400921FB54442D18, i32 0)
  %arrayidx42 = getelementptr inbounds [5 x i16], ptr @cat, i64 0, i64 0
  %8 = load i16, ptr %arrayidx42, align 2
  call void @llvm.quantum.rotxy(i16 %8, double 0x4012D97C7F3321D2, double 0x3FF921FB54442D18, i32 0)
  %arrayidx40 = getelementptr inbounds [5 x i16], ptr @cat, i64 0, i64 1
  %9 = load i16, ptr %arrayidx40, align 2
  %arrayidx41 = getelementptr inbounds [5 x i16], ptr @cat, i64 0, i64 0
  %10 = load i16, ptr %arrayidx41, align 2
  call void @llvm.quantum.cphase(i16 %9, i16 %10, double 0x4012D97C7F3321D2, i32 0)
  %arrayidx39 = getelementptr inbounds [5 x i16], ptr @cat, i64 0, i64 0
  %11 = load i16, ptr %arrayidx39, align 2
  call void @llvm.quantum.rotxy(i16 %11, double 0x3FF921FB54442D18, double 0x3FF921FB54442D18, i32 0)
  %arrayidx37 = getelementptr inbounds [5 x i16], ptr @cat, i64 0, i64 2
  %12 = load i16, ptr %arrayidx37, align 2
  %arrayidx38 = getelementptr inbounds [5 x i16], ptr @cat, i64 0, i64 0
  %13 = load i16, ptr %arrayidx38, align 2
  call void @llvm.quantum.cphase(i16 %12, i16 %13, double 0x400921FB54442D18, i32 0)
  %arrayidx36 = getelementptr inbounds [5 x i16], ptr @cat, i64 0, i64 4
  %14 = load i16, ptr %arrayidx36, align 2
  call void @llvm.quantum.rotxy(i16 %14, double 0x4012D97C7F3321D2, double 0x3FF921FB54442D18, i32 0)
  %arrayidx34 = getelementptr inbounds [5 x i16], ptr @cat, i64 0, i64 2
  %15 = load i16, ptr %arrayidx34, align 2
  %arrayidx35 = getelementptr inbounds [5 x i16], ptr @cat, i64 0, i64 0
  %16 = load i16, ptr %arrayidx35, align 2
  call void @llvm.quantum.cphase(i16 %15, i16 %16, double 0x3FF921FB54442D18, i32 0)
  %arrayidx32 = getelementptr inbounds [5 x i16], ptr @cat, i64 0, i64 0
  %17 = load i16, ptr %arrayidx32, align 2
  %arrayidx33 = getelementptr inbounds [5 x i16], ptr @cat, i64 0, i64 4
  %18 = load i16, ptr %arrayidx33, align 2
  call void @llvm.quantum.cphase(i16 %17, i16 %18, double 0x3FF921FB54442D18, i32 0)
  %arrayidx31 = getelementptr inbounds [5 x i16], ptr @cat, i64 0, i64 4
  %19 = load i16, ptr %arrayidx31, align 2
  call void @llvm.quantum.rotxy(i16 %19, double 0x3FF921FB54442D18, double 0x3FF921FB54442D18, i32 0)
  %arrayidx29 = getelementptr inbounds [5 x i16], ptr @cat, i64 0, i64 2
  %20 = load i16, ptr %arrayidx29, align 2
  %arrayidx30 = getelementptr inbounds [5 x i16], ptr @cat, i64 0, i64 4
  %21 = load i16, ptr %arrayidx30, align 2
  call void @llvm.quantum.cphase(i16 %20, i16 %21, double 0x400921FB54442D18, i32 0)
  %arrayidx28 = getelementptr inbounds [5 x i16], ptr @cat, i64 0, i64 4
  %22 = load i16, ptr %arrayidx28, align 2
  call void @llvm.quantum.rotxy(i16 %22, double 0x4012D97C7F3321D2, double 0x3FF921FB54442D18, i32 0)
  %arrayidx26 = getelementptr inbounds [5 x i16], ptr @cat, i64 0, i64 0
  %23 = load i16, ptr %arrayidx26, align 2
  %arrayidx27 = getelementptr inbounds [5 x i16], ptr @cat, i64 0, i64 4
  %24 = load i16, ptr %arrayidx27, align 2
  call void @llvm.quantum.cphase(i16 %23, i16 %24, double 0x4012D97C7F3321D2, i32 0)
  %arrayidx25 = getelementptr inbounds [5 x i16], ptr @cat, i64 0, i64 4
  %25 = load i16, ptr %arrayidx25, align 2
  call void @llvm.quantum.rotxy(i16 %25, double 0x3FF921FB54442D18, double 0x3FF921FB54442D18, i32 0)
  %arrayidx23 = getelementptr inbounds [5 x i16], ptr @cat, i64 0, i64 2
  %26 = load i16, ptr %arrayidx23, align 2
  %arrayidx24 = getelementptr inbounds [5 x i16], ptr @cat, i64 0, i64 4
  %27 = load i16, ptr %arrayidx24, align 2
  call void @llvm.quantum.cphase(i16 %26, i16 %27, double 0x400921FB54442D18, i32 0)
  %arrayidx22 = getelementptr inbounds [5 x i16], ptr @cat, i64 0, i64 2
  %28 = load i16, ptr %arrayidx22, align 2
  call void @llvm.quantum.rotxy(i16 %28, double 0x4012D97C7F3321D2, double 0x3FF921FB54442D18, i32 0)
  %arrayidx20 = getelementptr inbounds [5 x i16], ptr @cat, i64 0, i64 3
  %29 = load i16, ptr %arrayidx20, align 2
  %arrayidx21 = getelementptr inbounds [5 x i16], ptr @cat, i64 0, i64 0
  %30 = load i16, ptr %arrayidx21, align 2
  call void @llvm.quantum.cphase(i16 %29, i16 %30, double 0x3FF921FB54442D18, i32 0)
  %arrayidx18 = getelementptr inbounds [5 x i16], ptr @cat, i64 0, i64 0
  %31 = load i16, ptr %arrayidx18, align 2
  %arrayidx19 = getelementptr inbounds [5 x i16], ptr @cat, i64 0, i64 2
  %32 = load i16, ptr %arrayidx19, align 2
  call void @llvm.quantum.cphase(i16 %31, i16 %32, double 0x3FF921FB54442D18, i32 0)
  %arrayidx17 = getelementptr inbounds [5 x i16], ptr @cat, i64 0, i64 2
  %33 = load i16, ptr %arrayidx17, align 2
  call void @llvm.quantum.rotxy(i16 %33, double 0x3FF921FB54442D18, double 0x3FF921FB54442D18, i32 0)
  %arrayidx15 = getelementptr inbounds [5 x i16], ptr @cat, i64 0, i64 3
  %34 = load i16, ptr %arrayidx15, align 2
  %arrayidx16 = getelementptr inbounds [5 x i16], ptr @cat, i64 0, i64 2
  %35 = load i16, ptr %arrayidx16, align 2
  call void @llvm.quantum.cphase(i16 %34, i16 %35, double 0x400921FB54442D18, i32 0)
  %arrayidx14 = getelementptr inbounds [5 x i16], ptr @cat, i64 0, i64 2
  %36 = load i16, ptr %arrayidx14, align 2
  call void @llvm.quantum.rotxy(i16 %36, double 0x4012D97C7F3321D2, double 0x3FF921FB54442D18, i32 0)
  %arrayidx12 = getelementptr inbounds [5 x i16], ptr @cat, i64 0, i64 0
  %37 = load i16, ptr %arrayidx12, align 2
  %arrayidx13 = getelementptr inbounds [5 x i16], ptr @cat, i64 0, i64 2
  %38 = load i16, ptr %arrayidx13, align 2
  call void @llvm.quantum.cphase(i16 %37, i16 %38, double 0x4012D97C7F3321D2, i32 0)
  %arrayidx11 = getelementptr inbounds [5 x i16], ptr @cat, i64 0, i64 2
  %39 = load i16, ptr %arrayidx11, align 2
  call void @llvm.quantum.rotxy(i16 %39, double 0x3FF921FB54442D18, double 0x3FF921FB54442D18, i32 0)
  %arrayidx9 = getelementptr inbounds [5 x i16], ptr @cat, i64 0, i64 3
  %40 = load i16, ptr %arrayidx9, align 2
  %arrayidx10 = getelementptr inbounds [5 x i16], ptr @cat, i64 0, i64 2
  %41 = load i16, ptr %arrayidx10, align 2
  call void @llvm.quantum.cphase(i16 %40, i16 %41, double 0x400921FB54442D18, i32 2)
  br label %aqcc.meas.move.end

aqcc.meas.move.end:                               ; preds = %aqcc.quantum
  ret void
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(none) uwtable
define dso_local noundef i32 @main() local_unnamed_addr #1 {
entry:
  ret i32 0
}

declare i32 @_ZN4qrt021initializeQrtSystem64EPdPcS1_(ptr, ptr, ptr)

declare i1 @_ZN4qrt013isInitializedEv()

declare void @_ZN4qrt020updateMeasurementMapEPbmb(ptr, i64, i1)

declare i32 @_ZN4qrt014scheduleKernelEPFvvEj(ptr, i32)

declare i32 @_ZN4qrt018dismantleQrtSystemEv()

; Function Attrs: nounwind
declare void @llvm.quantum.rotz(i16, double, i32 immarg) #2

; Function Attrs: nounwind
declare void @llvm.quantum.rotxy(i16, double, double, i32 immarg) #2

; Function Attrs: nounwind
declare void @llvm.quantum.cphase(i16, i16, double, i32 immarg) #2

; Function Attrs: nounwind
declare void @llvm.quantum.swapalp(i16, i16, double, i32 immarg) #2

; Function Attrs: nounwind
declare void @llvm.quantum.measz(i16, ptr, i1, i32 immarg) #2

; Function Attrs: nounwind
declare void @llvm.quantum.prepz(i16, i32 immarg) #2

; Function Attrs: nounwind
declare void @llvm.quantum.rotz.imm(i16 immarg, double immarg, i32 immarg) #2

; Function Attrs: nounwind
declare void @llvm.quantum.rotxy.imm(i16 immarg, double immarg, double immarg, i32 immarg) #2

; Function Attrs: nounwind
declare void @llvm.quantum.cphase.imm(i16 immarg, i16 immarg, double immarg, i32 immarg) #2

; Function Attrs: nounwind
declare void @llvm.quantum.swapalp.imm(i16 immarg, i16 immarg, double immarg, i32 immarg) #2

; Function Attrs: nounwind
declare void @llvm.quantum.measz.imm(i16 immarg, i32 immarg) #2

; Function Attrs: nounwind
declare void @llvm.quantum.prepz.imm(i16 immarg, i32 immarg) #2

; Function Attrs: nounwind
declare void @llvm.quantum.rotz.imm.index(i16 immarg, i32 immarg, i32 immarg) #2

; Function Attrs: nounwind
declare void @llvm.quantum.rotxy.imm.index(i16 immarg, i32 immarg, i32 immarg, i32 immarg) #2

; Function Attrs: nounwind
declare void @llvm.quantum.cphase.imm.index(i16 immarg, i16 immarg, i32 immarg, i32 immarg) #2

; Function Attrs: nounwind
declare void @llvm.quantum.swapalp.imm.index(i16 immarg, i16 immarg, i32 immarg, i32 immarg) #2

; Function Attrs: nounwind
declare void @llvm.quantum.measz.imm.index(i16 immarg, i32 immarg, i32 immarg) #2

attributes #0 = { mustprogress noinline nounwind uwtable "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #1 = { mustprogress nofree norecurse nosync nounwind willreturn memory(none) uwtable "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #2 = { nounwind }
; CHECK:  Circuit for QBB in quantum_kernel - 'multiQubitGates'
; CHECK: |cat_0> : - RXY(4.712, 1.571) ---- Phase(1.571) ---- RXY(1.571, 1.571) ---- Z ---- RXY(4.712, 1.571) ---- Phase(4.712) ---- RXY(1.571, 1.571) ---- Z ---- Phase(1.571) ---- o -------------------------------------------------------------------- o --------------- Phase(1.571) --------------------------------------- o -------------------------------------------------------------------- o --------------------------------------------
; CHECK:                                    |                                        |                             |                                        |      |                 |                                                                      |                 |                                                    |                                                                      |                                             
; CHECK: |cat_1> : - Phase(1.571) --------- o -------------------------------------- | --------------------------- o -------------------------------------- | ---- | --------------- | -------------------------------------------------------------------- | --------------- | -------------------------------------------------- | -------------------------------------------------------------------- | --------------------------------------------
; CHECK:             |                                                               |                                                                      |      |                 |                                                                      |                 |                                                    |                                                                      |                                             
; CHECK: |cat_2> : - o ------------------------------------------------------------- o -------------------------------------------------------------------- o ---- o --------------- | -------------------------------------- o --------------------------- | --------------- | -------------------- o ---- RXY(4.712, 1.571) ---- Phase(1.571) ---- RXY(1.571, 1.571) ---- Z ---- RXY(4.712, 1.571) ---- Phase(4.712) ---- RXY(1.571, 1.571) ---- Z ---
; CHECK:                                                                                                                                                                             |                                        |                             |                 |                      |                                                                      |                                                                      |    
; CHECK: |cat_3> : ----------------------------------------------------------------------------------------------------------------------------------------------------------------- | -------------------------------------- | --------------------------- | --------------- o -------------------- | -------------------------------------------------------------------- o -------------------------------------------------------------------- o ---
; CHECK:                                                                                                                                                                             |                                        |                             |                                        |                                                                                                                                                  
; CHECK: |cat_4> : - RXY(4.712, 1.571) --------------------------------------------------------------------------------------------------------------------------------------------- Phase(1.571) ---- RXY(1.571, 1.571) ---- Z ---- RXY(4.712, 1.571) ---- Phase(4.712) ---- RXY(1.571, 1.571) ---- Z -------------------------------------------------------------------------------------------------------------------------------------------------
