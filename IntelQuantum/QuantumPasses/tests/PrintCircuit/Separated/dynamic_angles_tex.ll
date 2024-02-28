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
@q = dso_local global [5 x i16] [i16 0, i16 1, i16 2, i16 3, i16 4], align 2
@check = dso_local local_unnamed_addr global i16 5, align 2
@c = dso_local local_unnamed_addr global [5 x i8] zeroinitializer, align 1
@c_check = dso_local local_unnamed_addr global i8 0, align 1
@llvm.global.annotations = appending global [24 x { ptr, ptr, ptr, i32, ptr }] [{ ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str, ptr @.str.1, i32 48, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.2, ptr @.str.1, i32 68, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.3, ptr @.str.1, i32 88, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.4, ptr @.str.1, i32 109, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.5, ptr @.str.1, i32 129, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.6, ptr @.str.1, i32 149, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.7, ptr @.str.1, i32 169, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.8, ptr @.str.1, i32 189, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.9, ptr @.str.1, i32 209, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.10, ptr @.str.1, i32 230, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.11, ptr @.str.1, i32 251, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.12, ptr @.str.1, i32 272, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.13, ptr @.str.1, i32 293, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.14, ptr @.str.1, i32 314, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.15, ptr @.str.1, i32 335, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.16, ptr @.str.1, i32 359, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.17, ptr @.str.1, i32 377, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.18, ptr @.str.1, i32 395, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.19, ptr @.str.1, i32 413, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.20, ptr @.str.1, i32 433, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.21, ptr @.str.1, i32 453, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.22, ptr @.str.1, i32 481, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.23, ptr @.str.1, i32 503, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.24, ptr @.str.1, i32 525, ptr null }], section "llvm.metadata"
@aqcc.qsda = global [65536 x double] zeroinitializer #0

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(none) uwtable
define dso_local noundef nofpclass(nzero) double @_Z9fake_atand(double noundef %a) local_unnamed_addr #1 {
entry:
  %add = fadd double %a, 1.000000e+00
  ret double %add
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(none) uwtable
define dso_local noundef double @_Z6funct1d(double noundef %a) local_unnamed_addr #1 {
entry:
  %add = fadd double %a, 5.000000e-01
  %div = fdiv double %a, 1.000000e+01
  %sub = fsub double 1.000000e+00, %div
  %mul = fmul double %add, %sub
  ret double %mul
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(none) uwtable
define dso_local noundef double @_Z6funct2d(double noundef %a) local_unnamed_addr #1 {
entry:
  %sub = fsub double 2.500000e+00, %a
  %div = fmul double %sub, 5.000000e-01
  %add = fadd double %div, 1.500000e+00
  %0 = tail call double @llvm.fmuladd.f64(double noundef %add, double noundef 2.000000e+00, double noundef 5.000000e-01)
  ret double %0
}

; Function Attrs: nocallback nofree nosync nounwind speculatable willreturn memory(none)
declare double @llvm.fmuladd.f64(double, double, double) #2

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(none) uwtable
define dso_local noundef double @_Z6funct3d(double noundef %a) local_unnamed_addr #1 {
entry:
  %sub = fsub double 2.500000e+00, %a
  %mul = fmul double %sub, %a
  ret double %mul
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(none) uwtable
define dso_local noundef double @_Z6funct4d(double noundef %a) local_unnamed_addr #1 {
entry:
  %sub = fsub double 2.500000e+00, %a
  %mul = fmul double %sub, %a
  %mul1 = fmul double %mul, %a
  ret double %mul1
}

; Function Attrs: mustprogress noinline nounwind uwtable
define dso_local void @_Z8two_joind.stub(double noundef %ang) local_unnamed_addr #3 {
entry:
  store double %ang, ptr getelementptr inbounds ([65536 x double], ptr @aqcc.qsda, i64 0, i64 4), align 8
  br label %aqcc.quantum

aqcc.quantum:                                     ; preds = %entry
  %QRTcall = call i32 @_ZN4qrt014scheduleKernelEPFvvEj(ptr @"_Z23two_join(double).QBB.0.v.stub", i32 8)
  br label %aqcc.meas.move.end

aqcc.meas.move.end:                               ; preds = %aqcc.quantum
  store i16 5, ptr @check, align 2
  store [5 x i16] [i16 0, i16 1, i16 2, i16 3, i16 4], ptr @q, align 2
  ret void
}

; Function Attrs: mustprogress noinline nounwind uwtable
define dso_local void @_Z10three_joind.stub(double noundef %ang) local_unnamed_addr #3 {
entry:
  %add.i = fadd double %ang, 1.000000e+00
  store double %ang, ptr getelementptr inbounds ([65536 x double], ptr @aqcc.qsda, i64 0, i64 4), align 8
  store double %add.i, ptr getelementptr inbounds ([65536 x double], ptr @aqcc.qsda, i64 0, i64 5), align 8
  br label %aqcc.quantum

aqcc.quantum:                                     ; preds = %entry
  %QRTcall = call i32 @_ZN4qrt014scheduleKernelEPFvvEj(ptr @"_Z25three_join(double).QBB.1.v.stub", i32 7)
  br label %aqcc.meas.move.end

aqcc.meas.move.end:                               ; preds = %aqcc.quantum
  store i16 5, ptr @check, align 2
  store [5 x i16] [i16 0, i16 1, i16 2, i16 3, i16 4], ptr @q, align 2
  ret void
}

; Function Attrs: mustprogress noinline nounwind uwtable
define dso_local void @_Z12four_join_RZd.stub(double noundef %ang) local_unnamed_addr #3 {
entry:
  %add = fadd double %ang, 1.300000e+00
  %0 = fmul double %ang, 5.000000e-01
  store double %0, ptr getelementptr inbounds ([65536 x double], ptr @aqcc.qsda, i64 0, i64 4), align 8
  %1 = fmul double %add, 5.000000e-01
  store double %1, ptr getelementptr inbounds ([65536 x double], ptr @aqcc.qsda, i64 0, i64 5), align 8
  br label %aqcc.quantum

aqcc.quantum:                                     ; preds = %entry
  %QRTcall = call i32 @_ZN4qrt014scheduleKernelEPFvvEj(ptr @"_Z27four_join_RZ(double).QBB.2.v.stub", i32 6)
  br label %aqcc.meas.move.end

aqcc.meas.move.end:                               ; preds = %aqcc.quantum
  store i16 5, ptr @check, align 2
  store [5 x i16] [i16 0, i16 1, i16 2, i16 3, i16 4], ptr @q, align 2
  ret void
}

; Function Attrs: mustprogress noinline nounwind uwtable
define dso_local void @_Z16CPhaseSwapA_joind.stub(double noundef %ang) local_unnamed_addr #3 {
entry:
  store double %ang, ptr getelementptr inbounds ([65536 x double], ptr @aqcc.qsda, i64 0, i64 4), align 8
  br label %aqcc.quantum

aqcc.quantum:                                     ; preds = %entry
  %QRTcall = call i32 @_ZN4qrt014scheduleKernelEPFvvEj(ptr @"_Z31CPhaseSwapA_join(double).QBB.3.v.stub", i32 2)
  br label %aqcc.meas.move.end

aqcc.meas.move.end:                               ; preds = %aqcc.quantum
  store i16 5, ptr @check, align 2
  store [5 x i16] [i16 0, i16 1, i16 2, i16 3, i16 4], ptr @q, align 2
  ret void
}

; Function Attrs: mustprogress noinline nounwind uwtable
define dso_local void @_Z6more_Rd.stub(double noundef %ang) local_unnamed_addr #3 {
entry:
  %add.i = fadd double %ang, 5.000000e-01
  %div.i = fdiv double %ang, 1.000000e+01
  %sub.i = fsub double 1.000000e+00, %div.i
  %mul.i = fmul double %add.i, %sub.i
  %add = fadd double %mul.i, %ang
  %sub.i13 = fsub double 2.500000e+00, %ang
  %div.i14 = fmul double %sub.i13, 5.000000e-01
  %add.i15 = fadd double %div.i14, 1.500000e+00
  %0 = tail call double @llvm.fmuladd.f64(double noundef %add.i15, double noundef 2.000000e+00, double noundef 5.000000e-01)
  %mul.i17 = fmul double %sub.i13, %ang
  %mul1.i = fmul double %mul.i17, %ang
  %sub.i20 = fsub double 2.500000e+00, %mul.i
  %div.i21 = fmul double %sub.i20, 5.000000e-01
  %add.i22 = fadd double %div.i21, 1.500000e+00
  %1 = tail call double @llvm.fmuladd.f64(double noundef %add.i22, double noundef 2.000000e+00, double noundef 5.000000e-01)
  %sub.i23 = fsub double 2.500000e+00, %1
  %mul.i24 = fmul double %1, %sub.i23
  %sub.i25 = fsub double 2.500000e+00, %mul.i24
  %mul.i26 = fmul double %mul.i24, %sub.i25
  %mul1.i27 = fmul double %mul.i24, %mul.i26
  %2 = fmul double %mul.i, 5.000000e-01
  store double %2, ptr getelementptr inbounds ([65536 x double], ptr @aqcc.qsda, i64 0, i64 4), align 8
  %3 = fmul double %add, 5.000000e-01
  store double %3, ptr getelementptr inbounds ([65536 x double], ptr @aqcc.qsda, i64 0, i64 5), align 8
  %4 = fmul double %0, 5.000000e-01
  store double %4, ptr getelementptr inbounds ([65536 x double], ptr @aqcc.qsda, i64 0, i64 6), align 8
  %5 = fmul double %mul.i17, 5.000000e-01
  store double %5, ptr getelementptr inbounds ([65536 x double], ptr @aqcc.qsda, i64 0, i64 7), align 8
  %6 = fmul double %mul1.i, 5.000000e-01
  store double %6, ptr getelementptr inbounds ([65536 x double], ptr @aqcc.qsda, i64 0, i64 8), align 8
  %7 = fmul double %mul1.i27, 5.000000e-01
  store double %7, ptr getelementptr inbounds ([65536 x double], ptr @aqcc.qsda, i64 0, i64 9), align 8
  br label %aqcc.quantum

aqcc.quantum:                                     ; preds = %entry
  %QRTcall = call i32 @_ZN4qrt014scheduleKernelEPFvvEj(ptr @"_Z21more_R(double).QBB.4.v.stub", i32 12)
  br label %aqcc.meas.move.end

aqcc.meas.move.end:                               ; preds = %aqcc.quantum
  store i16 5, ptr @check, align 2
  store [5 x i16] [i16 0, i16 1, i16 2, i16 3, i16 4], ptr @q, align 2
  ret void
}

; Function Attrs: mustprogress norecurse nounwind uwtable
define dso_local noundef i32 @main() local_unnamed_addr #4 {
entry:
  tail call void @_Z8two_joind.stub(double noundef nofpclass(nan inf zero sub nnorm) 1.000000e-01) #5
  tail call void @_Z10three_joind.stub(double noundef nofpclass(nan inf zero sub nnorm) 2.000000e-01) #5
  tail call void @_Z12four_join_RZd.stub(double noundef nofpclass(nan inf zero sub nnorm) 3.000000e-01) #5
  tail call void @_Z16CPhaseSwapA_joind.stub(double noundef nofpclass(nan inf zero sub nnorm) 4.000000e-01) #5
  ret i32 0
}

declare i32 @_ZN4qrt021initializeQrtSystem64EPdPcS1_(ptr, ptr, ptr)

declare i1 @_ZN4qrt013isInitializedEv()

declare void @_ZN4qrt020updateMeasurementMapEPbmb(ptr, i64, i1)

declare i32 @_ZN4qrt014scheduleKernelEPFvvEj(ptr, i32)

declare i32 @_ZN4qrt018dismantleQrtSystemEv()

; Function Attrs: nounwind
declare void @llvm.quantum.rotz(i16, double, i32 immarg) #5

; Function Attrs: nounwind
declare void @llvm.quantum.rotxy(i16, double, double, i32 immarg) #5

; Function Attrs: nounwind
declare void @llvm.quantum.cphase(i16, i16, double, i32 immarg) #5

; Function Attrs: nounwind
declare void @llvm.quantum.swapalp(i16, i16, double, i32 immarg) #5

; Function Attrs: nounwind
declare void @llvm.quantum.measz(i16, ptr, i1, i32 immarg) #5

; Function Attrs: nounwind
declare void @llvm.quantum.prepz(i16, i32 immarg) #5

; Function Attrs: nounwind
declare void @llvm.quantum.rotz.imm(i16 immarg, double immarg, i32 immarg) #5

; Function Attrs: nounwind
declare void @llvm.quantum.rotxy.imm(i16 immarg, double immarg, double immarg, i32 immarg) #5

; Function Attrs: nounwind
declare void @llvm.quantum.cphase.imm(i16 immarg, i16 immarg, double immarg, i32 immarg) #5

; Function Attrs: nounwind
declare void @llvm.quantum.swapalp.imm(i16 immarg, i16 immarg, double immarg, i32 immarg) #5

; Function Attrs: nounwind
declare void @llvm.quantum.measz.imm(i16 immarg, i32 immarg) #5

; Function Attrs: nounwind
declare void @llvm.quantum.prepz.imm(i16 immarg, i32 immarg) #5

; Function Attrs: nounwind
declare void @llvm.quantum.rotz.imm.index(i16 immarg, i32 immarg, i32 immarg) #5

; Function Attrs: nounwind
declare void @llvm.quantum.rotxy.imm.index(i16 immarg, i32 immarg, i32 immarg, i32 immarg) #5

; Function Attrs: nounwind
declare void @llvm.quantum.cphase.imm.index(i16 immarg, i16 immarg, i32 immarg, i32 immarg) #5

; Function Attrs: nounwind
declare void @llvm.quantum.swapalp.imm.index(i16 immarg, i16 immarg, i32 immarg, i32 immarg) #5

; Function Attrs: nounwind
declare void @llvm.quantum.measz.imm.index(i16 immarg, i32 immarg, i32 immarg) #5

define void @"_Z23two_join(double).QBB.0.v.stub"() section ".qbbs_text" {
aqcc.quantum:
  call void @llvm.quantum.rotxy.imm(i16 1, double 0x3FF921FB54442D18, double 0x4012D97C7F3321D2, i32 1)
  call void @llvm.quantum.cphase.imm(i16 0, i16 1, double 0x400921FB54442D18, i32 0)
  call void @llvm.quantum.rotxy.imm(i16 1, double 0x3FF921FB54442D18, double 0x3FF921FB54442D18, i32 0)
  call void @llvm.quantum.rotxy.imm.index(i16 1, i32 4, i32 0, i32 0)
  call void @llvm.quantum.rotxy.imm(i16 1, double 2.000000e-01, double 0.000000e+00, i32 0)
  call void @llvm.quantum.rotxy.imm(i16 1, double 0x3FF921FB54442D18, double 0x4012D97C7F3321D2, i32 0)
  call void @llvm.quantum.cphase.imm(i16 0, i16 1, double 0x400921FB54442D18, i32 0)
  call void @llvm.quantum.rotxy.imm(i16 1, double 0x3FF921FB54442D18, double 0x3FF921FB54442D18, i32 2)
  ret void
}

define void @"_Z25three_join(double).QBB.1.v.stub"() section ".qbbs_text" {
aqcc.quantum:
  call void @llvm.quantum.cphase.imm(i16 0, i16 2, double 0x400921FB54442D18, i32 1)
  call void @llvm.quantum.rotxy.imm.index(i16 2, i32 4, i32 1, i32 0)
  call void @llvm.quantum.rotxy.imm(i16 2, double 0x400921FB54442D18, double 0.000000e+00, i32 0)
  call void @llvm.quantum.rotxy.imm.index(i16 2, i32 5, i32 1, i32 0)
  call void @llvm.quantum.rotxy.imm(i16 2, double 0x400921FB54442D18, double 0.000000e+00, i32 0)
  call void @llvm.quantum.rotxy.imm(i16 2, double 2.000000e-01, double 0x3FF921FB54442D18, i32 0)
  call void @llvm.quantum.cphase.imm(i16 0, i16 2, double 0x400921FB54442D18, i32 2)
  ret void
}

define void @"_Z27four_join_RZ(double).QBB.2.v.stub"() section ".qbbs_text" {
aqcc.quantum:
  call void @llvm.quantum.rotxy.imm(i16 2, double 0x400921FB54442D18, double 0.000000e+00, i32 1)
  call void @llvm.quantum.rotxy.imm.index(i16 2, i32 2, i32 4, i32 0)
  call void @llvm.quantum.rotxy.imm(i16 2, double 0x400921FB54442D18, double 0.000000e+00, i32 0)
  call void @llvm.quantum.rotxy.imm(i16 2, double 0x400921FB54442D18, double 1.000000e-01, i32 0)
  call void @llvm.quantum.rotxy.imm(i16 2, double 0x400921FB54442D18, double 0.000000e+00, i32 0)
  call void @llvm.quantum.rotxy.imm.index(i16 2, i32 2, i32 5, i32 2)
  ret void
}

define void @"_Z31CPhaseSwapA_join(double).QBB.3.v.stub"() section ".qbbs_text" {
aqcc.quantum:
  call void @llvm.quantum.cphase.imm.index(i16 0, i16 1, i32 4, i32 1)
  call void @llvm.quantum.swapalp.imm(i16 0, i16 1, double 8.640000e-01, i32 2)
  ret void
}

define void @"_Z21more_R(double).QBB.4.v.stub"() section ".qbbs_text" {
aqcc.quantum:
  call void @llvm.quantum.rotxy.imm(i16 0, double 0x400921FB54442D18, double 0.000000e+00, i32 1)
  call void @llvm.quantum.rotxy.imm.index(i16 0, i32 2, i32 4, i32 0)
  call void @llvm.quantum.rotxy.imm(i16 1, double 0x400921FB54442D18, double 0.000000e+00, i32 0)
  call void @llvm.quantum.rotxy.imm.index(i16 1, i32 2, i32 5, i32 0)
  call void @llvm.quantum.rotxy.imm(i16 2, double 0x400921FB54442D18, double 0.000000e+00, i32 0)
  call void @llvm.quantum.rotxy.imm.index(i16 2, i32 2, i32 6, i32 0)
  call void @llvm.quantum.rotxy.imm(i16 3, double 0x400921FB54442D18, double 0.000000e+00, i32 0)
  call void @llvm.quantum.rotxy.imm.index(i16 3, i32 2, i32 7, i32 0)
  call void @llvm.quantum.rotxy.imm(i16 4, double 0x400921FB54442D18, double 0.000000e+00, i32 0)
  call void @llvm.quantum.rotxy.imm.index(i16 4, i32 2, i32 8, i32 0)
  call void @llvm.quantum.rotxy.imm(i16 0, double 0x400921FB54442D18, double 0.000000e+00, i32 0)
  call void @llvm.quantum.rotxy.imm.index(i16 0, i32 2, i32 9, i32 2)
  ret void
}

attributes #0 = { "quantum_shared_var" }
attributes #1 = { mustprogress nofree norecurse nosync nounwind willreturn memory(none) uwtable "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #2 = { nocallback nofree nosync nounwind speculatable willreturn memory(none) }
attributes #3 = { mustprogress noinline nounwind uwtable "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #4 = { mustprogress norecurse nounwind uwtable "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #5 = { nounwind }
; CHECK: LaTex instructions: 
; CHECK: \lstick{\ket{qid\_0}} & \qw & \ctrl{1} & \qw & \qw & \qw & \qw & \ctrl{1} & \qw & \qw \\ 
; CHECK: \lstick{\ket{qid\_1}} & \gate{RXY(1.571, 4.712)} & \gate{Z} & \gate{RXY(1.571, 1.571)} & \gate{RXY(quantum_shared_double_array[4], 0)} & \gate{RXY(0.2, 0.)} & \gate{RXY(1.571, 4.712)} & \gate{Z} & \gate{RXY(1.571, 1.571)} & \qw 
; CHECK: LaTex instructions: 
; CHECK: \lstick{\ket{qid\_0}} & \ctrl{1} & \qw & \qw & \qw & \qw & \qw & \ctrl{1} & \qw \\ 
; CHECK: \lstick{\ket{qid\_2}} & \gate{Z} & \gate{RXY(quantum_shared_double_array[4], 1.570796)} & \gate{RXY(3.142, 0.)} & \gate{RXY(quantum_shared_double_array[5], 1.570796)} & \gate{RXY(3.142, 0.)} & \gate{RXY(0.2, 1.571)} & \gate{Z} & \qw 
; CHECK: LaTex instructions: 
; CHECK: \lstick{\ket{qid\_2}} & \gate{RXY(3.142, 0.)} & \gate{RXY(3.141593, quantum_shared_double_array[4])} & \gate{RXY(3.142, 0.)} & \gate{RXY(3.142, 0.1)} & \gate{RXY(3.142, 0.)} & \gate{RXY(3.141593, quantum_shared_double_array[5])} & \qw 
; CHECK: LaTex instructions: 
; CHECK: \lstick{\ket{qid\_0}} & \ctrl{1} & \gate{SwapA(0.864)} & \qw \\ 
; CHECK: \lstick{\ket{qid\_1}} & \gate{Phase(quantum_shared_double_array[4])} & \gate{SwapA(0.864)} \qwx[-1] & \qw 
; CHECK: LaTex instructions: 
; CHECK: \lstick{\ket{qid\_0}} & \gate{RXY(3.142, 0.)} & \gate{RXY(3.141593, quantum_shared_double_array[4])} & \gate{RXY(3.142, 0.)} & \gate{RXY(3.141593, quantum_shared_double_array[9])} & \qw \\ 
; CHECK: \lstick{\ket{qid\_1}} & \gate{RXY(3.142, 0.)} & \gate{RXY(3.141593, quantum_shared_double_array[5])} & \qw & \qw & \qw \\ 
; CHECK: \lstick{\ket{qid\_2}} & \gate{RXY(3.142, 0.)} & \gate{RXY(3.141593, quantum_shared_double_array[6])} & \qw & \qw & \qw \\ 
; CHECK: \lstick{\ket{qid\_3}} & \gate{RXY(3.142, 0.)} & \gate{RXY(3.141593, quantum_shared_double_array[7])} & \qw & \qw & \qw \\ 
; CHECK: \lstick{\ket{qid\_4}} & \gate{RXY(3.142, 0.)} & \gate{RXY(3.141593, quantum_shared_double_array[8])} & \qw & \qw & \qw 
