; RUN: opt -S %loadintelquantumpasses -p=q-annotations-to-json,spin-native-annotate,print-circuit-qbb --print-format console  -o /dev/null %s  | FileCheck %s --dump-input fail
; ModuleID = 'test.ll'
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

%"class.std::ios_base::Init" = type { i8 }
%class.Foo = type { double }

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
@_ZStL8__ioinit = internal global %"class.std::ios_base::Init" zeroinitializer, align 1
@__dso_handle = external hidden global i8
@q = dso_local global [9 x i16] zeroinitializer, align 16
@c = dso_local global [5 x i8] zeroinitializer, align 1
@csig = dso_local global i8 0, align 1
@i = dso_local local_unnamed_addr global i32 0, align 4
@A = dso_local global %class.Foo zeroinitializer, align 8
@angle = dso_local local_unnamed_addr global double 0.000000e+00, align 8
@angles = dso_local local_unnamed_addr global [4 x double] zeroinitializer, align 16
@llvm.global_ctors = appending global [1 x { i32, ptr, ptr }] [{ i32, ptr, ptr } { i32 65535, ptr @_GLOBAL__sub_I_parameter_printing.cpp, ptr null }]
@llvm.global.annotations = appending global [24 x { ptr, ptr, ptr, i32, ptr }] [{ ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str, ptr @.str.1, i32 48, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.2, ptr @.str.1, i32 68, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.3, ptr @.str.1, i32 88, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.4, ptr @.str.1, i32 109, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.5, ptr @.str.1, i32 129, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.6, ptr @.str.1, i32 149, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.7, ptr @.str.1, i32 169, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.8, ptr @.str.1, i32 189, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.9, ptr @.str.1, i32 209, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.10, ptr @.str.1, i32 230, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.11, ptr @.str.1, i32 251, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.12, ptr @.str.1, i32 272, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.13, ptr @.str.1, i32 293, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.14, ptr @.str.1, i32 314, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.15, ptr @.str.1, i32 335, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.16, ptr @.str.1, i32 359, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.17, ptr @.str.1, i32 377, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.18, ptr @.str.1, i32 395, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.19, ptr @.str.1, i32 413, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.20, ptr @.str.1, i32 433, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.21, ptr @.str.1, i32 453, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.22, ptr @.str.1, i32 481, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.23, ptr @.str.1, i32 503, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.24, ptr @.str.1, i32 525, ptr null }], section "llvm.metadata"

declare void @_ZNSt8ios_base4InitC1Ev(ptr noundef nonnull align 1 dereferenceable(1)) unnamed_addr #0

; Function Attrs: nounwind
declare void @_ZNSt8ios_base4InitD1Ev(ptr noundef nonnull align 1 dereferenceable(1)) unnamed_addr #1

; Function Attrs: nofree nounwind
declare i32 @__cxa_atexit(ptr, ptr, ptr) local_unnamed_addr #2

; Function Attrs: mustprogress noinline nounwind uwtable
define dso_local void @_Z12const_anglesv.stub() local_unnamed_addr #3 section ".qbbs_text" {
entry:
  br label %aqcc.quantum

aqcc.quantum:                                     ; preds = %entry
  %arrayidx26 = getelementptr inbounds [9 x i16], ptr @q, i64 0, i64 0
  %0 = load i16, ptr %arrayidx26, align 16
  call void @llvm.quantum.rotxy(i16 %0, double 0x400921FB54442D18, double 0.000000e+00, i32 1)
  %arrayidx25 = getelementptr inbounds [9 x i16], ptr @q, i64 0, i64 0
  %1 = load i16, ptr %arrayidx25, align 16
  call void @llvm.quantum.rotxy(i16 %1, double 0x400921FB54442D18, double 0x4122D687E3D70A3D, i32 0)
  %arrayidx24 = getelementptr inbounds [9 x i16], ptr @q, i64 0, i64 1
  %2 = load i16, ptr %arrayidx24, align 16
  call void @llvm.quantum.rotxy(i16 %2, double 0x400921FB54442D18, double 0.000000e+00, i32 0)
  %arrayidx23 = getelementptr inbounds [9 x i16], ptr @q, i64 0, i64 1
  %3 = load i16, ptr %arrayidx23, align 16
  call void @llvm.quantum.rotxy(i16 %3, double 0x400921FB54442D18, double 0x40B81CD6E631F8A1, i32 0)
  %arrayidx22 = getelementptr inbounds [9 x i16], ptr @q, i64 0, i64 2
  %4 = load i16, ptr %arrayidx22, align 16
  call void @llvm.quantum.rotxy(i16 %4, double 0x400921FB54442D18, double 0.000000e+00, i32 0)
  %arrayidx21 = getelementptr inbounds [9 x i16], ptr @q, i64 0, i64 2
  %5 = load i16, ptr %arrayidx21, align 16
  call void @llvm.quantum.rotxy(i16 %5, double 0x400921FB54442D18, double 0x404EDD3C07EE0B0B, i32 0)
  %arrayidx20 = getelementptr inbounds [9 x i16], ptr @q, i64 0, i64 3
  %6 = load i16, ptr %arrayidx20, align 16
  call void @llvm.quantum.rotxy(i16 %6, double 0x400921FB54442D18, double 0.000000e+00, i32 0)
  %arrayidx19 = getelementptr inbounds [9 x i16], ptr @q, i64 0, i64 3
  %7 = load i16, ptr %arrayidx19, align 16
  call void @llvm.quantum.rotxy(i16 %7, double 0x400921FB54442D18, double 0x3FE3C0CA4283DE1B, i32 0)
  %arrayidx18 = getelementptr inbounds [9 x i16], ptr @q, i64 0, i64 4
  %8 = load i16, ptr %arrayidx18, align 16
  call void @llvm.quantum.rotxy(i16 %8, double 0x400921FB54442D18, double 0.000000e+00, i32 0)
  %arrayidx17 = getelementptr inbounds [9 x i16], ptr @q, i64 0, i64 4
  %9 = load i16, ptr %arrayidx17, align 16
  call void @llvm.quantum.rotxy(i16 %9, double 0x400921FB54442D18, double 0x3F7948B0F8FAB5E6, i32 0)
  %arrayidx16 = getelementptr inbounds [9 x i16], ptr @q, i64 0, i64 5
  %10 = load i16, ptr %arrayidx16, align 16
  call void @llvm.quantum.rotxy(i16 %10, double 0x400921FB54442D18, double 0.000000e+00, i32 0)
  %arrayidx15 = getelementptr inbounds [9 x i16], ptr @q, i64 0, i64 5
  %11 = load i16, ptr %arrayidx15, align 16
  call void @llvm.quantum.rotxy(i16 %11, double 0x400921FB54442D18, double 0x3F102E85BE111841, i32 0)
  %arrayidx14 = getelementptr inbounds [9 x i16], ptr @q, i64 0, i64 6
  %12 = load i16, ptr %arrayidx14, align 16
  call void @llvm.quantum.rotxy(i16 %12, double 0x400921FB54442D18, double 0.000000e+00, i32 0)
  %arrayidx13 = getelementptr inbounds [9 x i16], ptr @q, i64 0, i64 6
  %13 = load i16, ptr %arrayidx13, align 16
  call void @llvm.quantum.rotxy(i16 %13, double 0x400921FB54442D18, double 0x3EA4B66DC015E19B, i32 0)
  %arrayidx12 = getelementptr inbounds [9 x i16], ptr @q, i64 0, i64 7
  %14 = load i16, ptr %arrayidx12, align 16
  call void @llvm.quantum.rotxy(i16 %14, double 0x400921FB54442D18, double 0.000000e+00, i32 0)
  %arrayidx11 = getelementptr inbounds [9 x i16], ptr @q, i64 0, i64 7
  %15 = load i16, ptr %arrayidx11, align 16
  call void @llvm.quantum.rotxy(i16 %15, double 0x400921FB54442D18, double 0xBFE3C0CA4283DE1B, i32 0)
  %arrayidx10 = getelementptr inbounds [9 x i16], ptr @q, i64 0, i64 8
  %16 = load i16, ptr %arrayidx10, align 16
  call void @llvm.quantum.rotxy(i16 %16, double 0x400921FB54442D18, double 0.000000e+00, i32 0)
  %arrayidx9 = getelementptr inbounds [9 x i16], ptr @q, i64 0, i64 8
  %17 = load i16, ptr %arrayidx9, align 16
  call void @llvm.quantum.rotxy(i16 %17, double 0x400921FB54442D18, double 0.000000e+00, i32 2)
  br label %aqcc.meas.move.end

aqcc.meas.move.end:                               ; preds = %aqcc.quantum
  ret void
}

; Function Attrs: mustprogress noinline uwtable
define dso_local void @_Z15function_anglesv.stub() local_unnamed_addr #4 section ".qbbs_text" {
entry:
  %call = tail call noundef double @_Z8getAnglev()
  %call1 = tail call noundef double @_Z8getAngled(double noundef 1.000000e-01)
  %call2 = tail call noundef double @_Z8getAnglev()
  %add = fadd double %call2, 1.000000e-01
  %call3 = tail call noundef double @_Z8getAngled(double noundef %add)
  %call4 = tail call noundef double @_Z8getAngledi(double noundef 1.000000e-01, i32 noundef 2)
  %call5 = tail call noundef double @_Z8getAnglev()
  %0 = load i32, ptr @i, align 4, !tbaa !0
  %add6 = add nsw i32 %0, 2
  %call7 = tail call noundef double @_Z8getAngledi(double noundef %call5, i32 noundef %add6)
  %call8 = tail call noundef double @_ZN3Foo3getEv(ptr noundef nonnull align 8 dereferenceable(8) @A)
  br label %aqcc.quantum

aqcc.quantum:                                     ; preds = %entry
  %arrayidx17 = getelementptr inbounds [9 x i16], ptr @q, i64 0, i64 0
  %1 = load i16, ptr %arrayidx17, align 16
  call void @llvm.quantum.rotxy(i16 %1, double 0x400921FB54442D18, double 0.000000e+00, i32 1)
  %arrayidx16 = getelementptr inbounds [9 x i16], ptr @q, i64 0, i64 0
  %2 = load i16, ptr %arrayidx16, align 16
  %3 = fmul double %call, 5.000000e-01
  call void @llvm.quantum.rotxy(i16 %2, double 0x400921FB54442D18, double %3, i32 0)
  %arrayidx15 = getelementptr inbounds [9 x i16], ptr @q, i64 0, i64 1
  %4 = load i16, ptr %arrayidx15, align 16
  call void @llvm.quantum.rotxy(i16 %4, double 0x400921FB54442D18, double 0.000000e+00, i32 0)
  %arrayidx14 = getelementptr inbounds [9 x i16], ptr @q, i64 0, i64 1
  %5 = load i16, ptr %arrayidx14, align 16
  %6 = fmul double %call1, 5.000000e-01
  call void @llvm.quantum.rotxy(i16 %5, double 0x400921FB54442D18, double %6, i32 0)
  %arrayidx13 = getelementptr inbounds [9 x i16], ptr @q, i64 0, i64 2
  %7 = load i16, ptr %arrayidx13, align 16
  call void @llvm.quantum.rotxy(i16 %7, double 0x400921FB54442D18, double 0.000000e+00, i32 0)
  %arrayidx12 = getelementptr inbounds [9 x i16], ptr @q, i64 0, i64 2
  %8 = load i16, ptr %arrayidx12, align 16
  %9 = fmul double %call3, 5.000000e-01
  call void @llvm.quantum.rotxy(i16 %8, double 0x400921FB54442D18, double %9, i32 0)
  %arrayidx11 = getelementptr inbounds [9 x i16], ptr @q, i64 0, i64 3
  %10 = load i16, ptr %arrayidx11, align 16
  call void @llvm.quantum.rotxy(i16 %10, double 0x400921FB54442D18, double 0.000000e+00, i32 0)
  %arrayidx10 = getelementptr inbounds [9 x i16], ptr @q, i64 0, i64 3
  %11 = load i16, ptr %arrayidx10, align 16
  %12 = fmul double %call4, 5.000000e-01
  call void @llvm.quantum.rotxy(i16 %11, double 0x400921FB54442D18, double %12, i32 0)
  %arrayidx9 = getelementptr inbounds [9 x i16], ptr @q, i64 0, i64 4
  %13 = load i16, ptr %arrayidx9, align 16
  call void @llvm.quantum.rotxy(i16 %13, double 0x400921FB54442D18, double 0.000000e+00, i32 0)
  %arrayidx8 = getelementptr inbounds [9 x i16], ptr @q, i64 0, i64 4
  %14 = load i16, ptr %arrayidx8, align 16
  %15 = fmul double %call7, 5.000000e-01
  call void @llvm.quantum.rotxy(i16 %14, double 0x400921FB54442D18, double %15, i32 0)
  %arrayidx7 = getelementptr inbounds [9 x i16], ptr @q, i64 0, i64 5
  %16 = load i16, ptr %arrayidx7, align 16
  call void @llvm.quantum.rotxy(i16 %16, double 0x400921FB54442D18, double 0.000000e+00, i32 0)
  %arrayidx6 = getelementptr inbounds [9 x i16], ptr @q, i64 0, i64 5
  %17 = load i16, ptr %arrayidx6, align 16
  %18 = fmul double %call8, 5.000000e-01
  call void @llvm.quantum.rotxy(i16 %17, double 0x400921FB54442D18, double %18, i32 2)
  br label %aqcc.meas.move.end

aqcc.meas.move.end:                               ; preds = %aqcc.quantum
  ret void
}

declare noundef double @_Z8getAnglev() local_unnamed_addr #0

declare noundef double @_Z8getAngled(double noundef) local_unnamed_addr #0

declare noundef double @_Z8getAngledi(double noundef, i32 noundef) local_unnamed_addr #0

declare noundef double @_ZN3Foo3getEv(ptr noundef nonnull align 8 dereferenceable(8)) local_unnamed_addr #0

; Function Attrs: mustprogress noinline nounwind uwtable
define dso_local void @_Z13scalar_anglesdPd.stub(double noundef %a, ptr nocapture nofree noundef readonly %b) local_unnamed_addr #3 section ".qbbs_text" {
entry:
  %0 = load double, ptr @angle, align 8, !tbaa !4
  %1 = load double, ptr getelementptr inbounds ([4 x double], ptr @angles, i64 0, i64 2), align 16, !tbaa !4
  %2 = load i32, ptr @i, align 4, !tbaa !0
  %add = add nsw i32 %2, 1
  %idxprom = sext i32 %add to i64
  %arrayidx = getelementptr inbounds [4 x double], ptr @angles, i64 0, i64 %idxprom
  %3 = load double, ptr %arrayidx, align 8, !tbaa !4
  %arrayidx1 = getelementptr inbounds double, ptr %b, i64 2
  %4 = load double, ptr %arrayidx1, align 8, !tbaa !4
  %5 = load i32, ptr @i, align 4, !tbaa !0
  %add2 = add nsw i32 %5, 1
  %idxprom3 = sext i32 %add2 to i64
  %arrayidx4 = getelementptr inbounds double, ptr %b, i64 %idxprom3
  %6 = load double, ptr %arrayidx4, align 8, !tbaa !4
  br label %aqcc.quantum

aqcc.quantum:                                     ; preds = %entry
  %arrayidx20 = getelementptr inbounds [9 x i16], ptr @q, i64 0, i64 0
  %7 = load i16, ptr %arrayidx20, align 16
  call void @llvm.quantum.rotxy(i16 %7, double 0x400921FB54442D18, double 0.000000e+00, i32 1)
  %arrayidx19 = getelementptr inbounds [9 x i16], ptr @q, i64 0, i64 0
  %8 = load i16, ptr %arrayidx19, align 16
  %9 = fmul double %0, 5.000000e-01
  call void @llvm.quantum.rotxy(i16 %8, double 0x400921FB54442D18, double %9, i32 0)
  %arrayidx18 = getelementptr inbounds [9 x i16], ptr @q, i64 0, i64 1
  %10 = load i16, ptr %arrayidx18, align 16
  call void @llvm.quantum.rotxy(i16 %10, double 0x400921FB54442D18, double 0.000000e+00, i32 0)
  %arrayidx17 = getelementptr inbounds [9 x i16], ptr @q, i64 0, i64 1
  %11 = load i16, ptr %arrayidx17, align 16
  %12 = fmul double %1, 5.000000e-01
  call void @llvm.quantum.rotxy(i16 %11, double 0x400921FB54442D18, double %12, i32 0)
  %arrayidx16 = getelementptr inbounds [9 x i16], ptr @q, i64 0, i64 2
  %13 = load i16, ptr %arrayidx16, align 16
  call void @llvm.quantum.rotxy(i16 %13, double 0x400921FB54442D18, double 0.000000e+00, i32 0)
  %arrayidx15 = getelementptr inbounds [9 x i16], ptr @q, i64 0, i64 2
  %14 = load i16, ptr %arrayidx15, align 16
  %15 = fmul double %3, 5.000000e-01
  call void @llvm.quantum.rotxy(i16 %14, double 0x400921FB54442D18, double %15, i32 0)
  %arrayidx14 = getelementptr inbounds [9 x i16], ptr @q, i64 0, i64 3
  %16 = load i16, ptr %arrayidx14, align 16
  call void @llvm.quantum.rotxy(i16 %16, double 0x400921FB54442D18, double 0.000000e+00, i32 0)
  %arrayidx13 = getelementptr inbounds [9 x i16], ptr @q, i64 0, i64 3
  %17 = load i16, ptr %arrayidx13, align 16
  %18 = fmul double %a, 5.000000e-01
  call void @llvm.quantum.rotxy(i16 %17, double 0x400921FB54442D18, double %18, i32 0)
  %arrayidx12 = getelementptr inbounds [9 x i16], ptr @q, i64 0, i64 4
  %19 = load i16, ptr %arrayidx12, align 16
  call void @llvm.quantum.rotxy(i16 %19, double 0x400921FB54442D18, double 0.000000e+00, i32 0)
  %arrayidx11 = getelementptr inbounds [9 x i16], ptr @q, i64 0, i64 4
  %20 = load i16, ptr %arrayidx11, align 16
  %21 = fmul double %4, 5.000000e-01
  call void @llvm.quantum.rotxy(i16 %20, double 0x400921FB54442D18, double %21, i32 0)
  %arrayidx10 = getelementptr inbounds [9 x i16], ptr @q, i64 0, i64 5
  %22 = load i16, ptr %arrayidx10, align 16
  call void @llvm.quantum.rotxy(i16 %22, double 0x400921FB54442D18, double 0.000000e+00, i32 0)
  %arrayidx9 = getelementptr inbounds [9 x i16], ptr @q, i64 0, i64 5
  %23 = load i16, ptr %arrayidx9, align 16
  %24 = fmul double %6, 5.000000e-01
  call void @llvm.quantum.rotxy(i16 %23, double 0x400921FB54442D18, double %24, i32 2)
  br label %aqcc.meas.move.end

aqcc.meas.move.end:                               ; preds = %aqcc.quantum
  ret void
}

; Function Attrs: mustprogress noinline nounwind uwtable
define dso_local void @_Z16binary_op_anglesddd.stub(double noundef %a, double noundef %b, double noundef %c) local_unnamed_addr #3 section ".qbbs_text" {
entry:
  %mul = fmul double %a, %b
  %div = fdiv double %a, %b
  %add = fadd double %a, %b
  %sub = fsub double %a, %b
  %0 = tail call double @llvm.fmuladd.f64(double noundef %a, double noundef %b, double noundef %c)
  %add2 = fadd double %b, %c
  %mul3 = fmul double %add2, %a
  %add5 = fadd double %add, %c
  %div6 = fdiv double %add5, 2.500000e+00
  %fneg = fneg double %add5
  br label %aqcc.quantum

aqcc.quantum:                                     ; preds = %entry
  %arrayidx23 = getelementptr inbounds [9 x i16], ptr @q, i64 0, i64 0
  %1 = load i16, ptr %arrayidx23, align 16
  call void @llvm.quantum.rotxy(i16 %1, double 0x400921FB54442D18, double 0.000000e+00, i32 1)
  %arrayidx22 = getelementptr inbounds [9 x i16], ptr @q, i64 0, i64 0
  %2 = load i16, ptr %arrayidx22, align 16
  %3 = fmul double %mul, 5.000000e-01
  call void @llvm.quantum.rotxy(i16 %2, double 0x400921FB54442D18, double %3, i32 0)
  %arrayidx21 = getelementptr inbounds [9 x i16], ptr @q, i64 0, i64 1
  %4 = load i16, ptr %arrayidx21, align 16
  call void @llvm.quantum.rotxy(i16 %4, double 0x400921FB54442D18, double 0.000000e+00, i32 0)
  %arrayidx20 = getelementptr inbounds [9 x i16], ptr @q, i64 0, i64 1
  %5 = load i16, ptr %arrayidx20, align 16
  %6 = fmul double %div, 5.000000e-01
  call void @llvm.quantum.rotxy(i16 %5, double 0x400921FB54442D18, double %6, i32 0)
  %arrayidx19 = getelementptr inbounds [9 x i16], ptr @q, i64 0, i64 2
  %7 = load i16, ptr %arrayidx19, align 16
  call void @llvm.quantum.rotxy(i16 %7, double 0x400921FB54442D18, double 0.000000e+00, i32 0)
  %arrayidx18 = getelementptr inbounds [9 x i16], ptr @q, i64 0, i64 2
  %8 = load i16, ptr %arrayidx18, align 16
  %9 = fmul double %add, 5.000000e-01
  call void @llvm.quantum.rotxy(i16 %8, double 0x400921FB54442D18, double %9, i32 0)
  %arrayidx17 = getelementptr inbounds [9 x i16], ptr @q, i64 0, i64 3
  %10 = load i16, ptr %arrayidx17, align 16
  call void @llvm.quantum.rotxy(i16 %10, double 0x400921FB54442D18, double 0.000000e+00, i32 0)
  %arrayidx16 = getelementptr inbounds [9 x i16], ptr @q, i64 0, i64 3
  %11 = load i16, ptr %arrayidx16, align 16
  %12 = fmul double %sub, 5.000000e-01
  call void @llvm.quantum.rotxy(i16 %11, double 0x400921FB54442D18, double %12, i32 0)
  %arrayidx15 = getelementptr inbounds [9 x i16], ptr @q, i64 0, i64 4
  %13 = load i16, ptr %arrayidx15, align 16
  call void @llvm.quantum.rotxy(i16 %13, double 0x400921FB54442D18, double 0.000000e+00, i32 0)
  %arrayidx14 = getelementptr inbounds [9 x i16], ptr @q, i64 0, i64 4
  %14 = load i16, ptr %arrayidx14, align 16
  %15 = fmul double %0, 5.000000e-01
  call void @llvm.quantum.rotxy(i16 %14, double 0x400921FB54442D18, double %15, i32 0)
  %arrayidx13 = getelementptr inbounds [9 x i16], ptr @q, i64 0, i64 5
  %16 = load i16, ptr %arrayidx13, align 16
  call void @llvm.quantum.rotxy(i16 %16, double 0x400921FB54442D18, double 0.000000e+00, i32 0)
  %arrayidx12 = getelementptr inbounds [9 x i16], ptr @q, i64 0, i64 5
  %17 = load i16, ptr %arrayidx12, align 16
  %18 = fmul double %mul3, 5.000000e-01
  call void @llvm.quantum.rotxy(i16 %17, double 0x400921FB54442D18, double %18, i32 0)
  %arrayidx11 = getelementptr inbounds [9 x i16], ptr @q, i64 0, i64 6
  %19 = load i16, ptr %arrayidx11, align 16
  call void @llvm.quantum.rotxy(i16 %19, double 0x400921FB54442D18, double 0.000000e+00, i32 0)
  %arrayidx10 = getelementptr inbounds [9 x i16], ptr @q, i64 0, i64 6
  %20 = load i16, ptr %arrayidx10, align 16
  %21 = fmul double %div6, 5.000000e-01
  call void @llvm.quantum.rotxy(i16 %20, double 0x400921FB54442D18, double %21, i32 0)
  %arrayidx9 = getelementptr inbounds [9 x i16], ptr @q, i64 0, i64 7
  %22 = load i16, ptr %arrayidx9, align 16
  call void @llvm.quantum.rotxy(i16 %22, double 0x400921FB54442D18, double 0.000000e+00, i32 0)
  %arrayidx8 = getelementptr inbounds [9 x i16], ptr @q, i64 0, i64 7
  %23 = load i16, ptr %arrayidx8, align 16
  %24 = fmul double %fneg, 5.000000e-01
  call void @llvm.quantum.rotxy(i16 %23, double 0x400921FB54442D18, double %24, i32 2)
  br label %aqcc.meas.move.end

aqcc.meas.move.end:                               ; preds = %aqcc.quantum
  ret void
}

; Function Attrs: nocallback nofree nosync nounwind speculatable willreturn memory(none)
declare double @llvm.fmuladd.f64(double, double, double) #5

; Function Attrs: mustprogress noinline nounwind uwtable
define dso_local void @_Z10meas_cbitsRb.stub(ptr noundef nonnull align 1 dereferenceable(1) "cbit_ref" %arg) local_unnamed_addr #3 section ".qbbs_text" {
entry:
  %0 = load i32, ptr @i, align 4, !tbaa !0
  %add = add nsw i32 %0, 2
  %idxprom = sext i32 %add to i64
  %arrayidx = getelementptr inbounds [5 x i8], ptr @c, i64 0, i64 %idxprom
  br label %aqcc.quantum

aqcc.quantum:                                     ; preds = %entry
  %arrayidx8 = getelementptr inbounds [9 x i16], ptr @q, i64 0, i64 0
  %1 = load i16, ptr %arrayidx8, align 16
  call void @llvm.quantum.measz(i16 %1, ptr @csig, i1 false, i32 1)
  %arrayidx7 = getelementptr inbounds [9 x i16], ptr @q, i64 0, i64 1
  %2 = load i16, ptr %arrayidx7, align 16
  call void @llvm.quantum.measz(i16 %2, ptr getelementptr inbounds ([5 x i8], ptr @c, i64 0, i64 1), i1 false, i32 0)
  %arrayidx6 = getelementptr inbounds [9 x i16], ptr @q, i64 0, i64 2
  %3 = load i16, ptr %arrayidx6, align 16
  call void @llvm.quantum.measz(i16 %3, ptr %arrayidx, i1 false, i32 0)
  %arrayidx5 = getelementptr inbounds [9 x i16], ptr @q, i64 0, i64 3
  %4 = load i16, ptr %arrayidx5, align 16
  call void @llvm.quantum.measz(i16 %4, ptr %arg, i1 false, i32 2)
  br label %aqcc.meas.move.end

aqcc.meas.move.end:                               ; preds = %aqcc.quantum
  ret void
}

; Function Attrs: mustprogress noinline nounwind uwtable
define dso_local void @_Z11comparisonsjj.stub(i32 noundef %a, i32 noundef %b) local_unnamed_addr #3 section ".qbbs_text" {
entry:
  %cmp = icmp eq i32 %a, %b
  %conv = uitofp i1 %cmp to double
  %cmp1 = icmp ne i32 %a, %b
  %conv2 = uitofp i1 %cmp1 to double
  %cmp3 = icmp ult i32 %a, %b
  %conv4 = uitofp i1 %cmp3 to double
  %cmp5 = icmp ule i32 %a, %b
  %conv6 = uitofp i1 %cmp5 to double
  %cmp7 = icmp ugt i32 %a, %b
  %conv8 = uitofp i1 %cmp7 to double
  %cmp9 = icmp uge i32 %a, %b
  %conv10 = uitofp i1 %cmp9 to double
  br label %aqcc.quantum

aqcc.quantum:                                     ; preds = %entry
  %arrayidx17 = getelementptr inbounds [9 x i16], ptr @q, i64 0, i64 0
  %0 = load i16, ptr %arrayidx17, align 16
  call void @llvm.quantum.rotxy(i16 %0, double 0x400921FB54442D18, double 0.000000e+00, i32 1)
  %arrayidx16 = getelementptr inbounds [9 x i16], ptr @q, i64 0, i64 0
  %1 = load i16, ptr %arrayidx16, align 16
  %2 = fmul double %conv, 5.000000e-01
  call void @llvm.quantum.rotxy(i16 %1, double 0x400921FB54442D18, double %2, i32 0)
  %arrayidx15 = getelementptr inbounds [9 x i16], ptr @q, i64 0, i64 1
  %3 = load i16, ptr %arrayidx15, align 16
  call void @llvm.quantum.rotxy(i16 %3, double 0x400921FB54442D18, double 0.000000e+00, i32 0)
  %arrayidx14 = getelementptr inbounds [9 x i16], ptr @q, i64 0, i64 1
  %4 = load i16, ptr %arrayidx14, align 16
  %5 = fmul double %conv2, 5.000000e-01
  call void @llvm.quantum.rotxy(i16 %4, double 0x400921FB54442D18, double %5, i32 0)
  %arrayidx13 = getelementptr inbounds [9 x i16], ptr @q, i64 0, i64 2
  %6 = load i16, ptr %arrayidx13, align 16
  call void @llvm.quantum.rotxy(i16 %6, double 0x400921FB54442D18, double 0.000000e+00, i32 0)
  %arrayidx12 = getelementptr inbounds [9 x i16], ptr @q, i64 0, i64 2
  %7 = load i16, ptr %arrayidx12, align 16
  %8 = fmul double %conv4, 5.000000e-01
  call void @llvm.quantum.rotxy(i16 %7, double 0x400921FB54442D18, double %8, i32 0)
  %arrayidx11 = getelementptr inbounds [9 x i16], ptr @q, i64 0, i64 3
  %9 = load i16, ptr %arrayidx11, align 16
  call void @llvm.quantum.rotxy(i16 %9, double 0x400921FB54442D18, double 0.000000e+00, i32 0)
  %arrayidx10 = getelementptr inbounds [9 x i16], ptr @q, i64 0, i64 3
  %10 = load i16, ptr %arrayidx10, align 16
  %11 = fmul double %conv6, 5.000000e-01
  call void @llvm.quantum.rotxy(i16 %10, double 0x400921FB54442D18, double %11, i32 0)
  %arrayidx9 = getelementptr inbounds [9 x i16], ptr @q, i64 0, i64 4
  %12 = load i16, ptr %arrayidx9, align 16
  call void @llvm.quantum.rotxy(i16 %12, double 0x400921FB54442D18, double 0.000000e+00, i32 0)
  %arrayidx8 = getelementptr inbounds [9 x i16], ptr @q, i64 0, i64 4
  %13 = load i16, ptr %arrayidx8, align 16
  %14 = fmul double %conv8, 5.000000e-01
  call void @llvm.quantum.rotxy(i16 %13, double 0x400921FB54442D18, double %14, i32 0)
  %arrayidx7 = getelementptr inbounds [9 x i16], ptr @q, i64 0, i64 5
  %15 = load i16, ptr %arrayidx7, align 16
  call void @llvm.quantum.rotxy(i16 %15, double 0x400921FB54442D18, double 0.000000e+00, i32 0)
  %arrayidx6 = getelementptr inbounds [9 x i16], ptr @q, i64 0, i64 5
  %16 = load i16, ptr %arrayidx6, align 16
  %17 = fmul double %conv10, 5.000000e-01
  call void @llvm.quantum.rotxy(i16 %16, double 0x400921FB54442D18, double %17, i32 2)
  br label %aqcc.meas.move.end

aqcc.meas.move.end:                               ; preds = %aqcc.quantum
  ret void
}

; Function Attrs: mustprogress noinline nounwind uwtable
define dso_local void @_Z13odds_and_endsjj.stub(i32 noundef %a, i32 noundef %b) local_unnamed_addr #3 section ".qbbs_text" {
entry:
  %rem = urem i32 %a, %b
  %conv = uitofp i32 %rem to double
  %shl = shl i32 %a, %b
  %conv1 = uitofp i32 %shl to double
  %shr = lshr i32 %a, %b
  %conv2 = uitofp i32 %shr to double
  %tobool = icmp ne i32 %a, 0
  %tobool3 = icmp ne i32 %b, 0
  %0 = and i1 %tobool, %tobool3
  %conv4 = uitofp i1 %0 to double
  %1 = or i32 %b, %a
  %2 = icmp ne i32 %1, 0
  %conv7 = uitofp i1 %2 to double
  %xor = xor i32 %b, %a
  %conv8 = uitofp i32 %xor to double
  %cmp = icmp ult i32 %a, %b
  %tobool11 = icmp ne i32 %xor, 0
  %3 = or i1 %cmp, %tobool11
  %conv13 = uitofp i1 %3 to double
  br label %aqcc.quantum

aqcc.quantum:                                     ; preds = %entry
  %arrayidx20 = getelementptr inbounds [9 x i16], ptr @q, i64 0, i64 0
  %4 = load i16, ptr %arrayidx20, align 16
  call void @llvm.quantum.rotxy(i16 %4, double 0x400921FB54442D18, double 0.000000e+00, i32 1)
  %arrayidx19 = getelementptr inbounds [9 x i16], ptr @q, i64 0, i64 0
  %5 = load i16, ptr %arrayidx19, align 16
  %6 = fmul double %conv, 5.000000e-01
  call void @llvm.quantum.rotxy(i16 %5, double 0x400921FB54442D18, double %6, i32 0)
  %arrayidx18 = getelementptr inbounds [9 x i16], ptr @q, i64 0, i64 1
  %7 = load i16, ptr %arrayidx18, align 16
  call void @llvm.quantum.rotxy(i16 %7, double 0x400921FB54442D18, double 0.000000e+00, i32 0)
  %arrayidx17 = getelementptr inbounds [9 x i16], ptr @q, i64 0, i64 1
  %8 = load i16, ptr %arrayidx17, align 16
  %9 = fmul double %conv1, 5.000000e-01
  call void @llvm.quantum.rotxy(i16 %8, double 0x400921FB54442D18, double %9, i32 0)
  %arrayidx16 = getelementptr inbounds [9 x i16], ptr @q, i64 0, i64 2
  %10 = load i16, ptr %arrayidx16, align 16
  call void @llvm.quantum.rotxy(i16 %10, double 0x400921FB54442D18, double 0.000000e+00, i32 0)
  %arrayidx15 = getelementptr inbounds [9 x i16], ptr @q, i64 0, i64 2
  %11 = load i16, ptr %arrayidx15, align 16
  %12 = fmul double %conv2, 5.000000e-01
  call void @llvm.quantum.rotxy(i16 %11, double 0x400921FB54442D18, double %12, i32 0)
  %arrayidx14 = getelementptr inbounds [9 x i16], ptr @q, i64 0, i64 3
  %13 = load i16, ptr %arrayidx14, align 16
  call void @llvm.quantum.rotxy(i16 %13, double 0x400921FB54442D18, double 0.000000e+00, i32 0)
  %arrayidx13 = getelementptr inbounds [9 x i16], ptr @q, i64 0, i64 3
  %14 = load i16, ptr %arrayidx13, align 16
  %15 = fmul double %conv4, 5.000000e-01
  call void @llvm.quantum.rotxy(i16 %14, double 0x400921FB54442D18, double %15, i32 0)
  %arrayidx12 = getelementptr inbounds [9 x i16], ptr @q, i64 0, i64 4
  %16 = load i16, ptr %arrayidx12, align 16
  call void @llvm.quantum.rotxy(i16 %16, double 0x400921FB54442D18, double 0.000000e+00, i32 0)
  %arrayidx11 = getelementptr inbounds [9 x i16], ptr @q, i64 0, i64 4
  %17 = load i16, ptr %arrayidx11, align 16
  %18 = fmul double %conv7, 5.000000e-01
  call void @llvm.quantum.rotxy(i16 %17, double 0x400921FB54442D18, double %18, i32 0)
  %arrayidx10 = getelementptr inbounds [9 x i16], ptr @q, i64 0, i64 5
  %19 = load i16, ptr %arrayidx10, align 16
  call void @llvm.quantum.rotxy(i16 %19, double 0x400921FB54442D18, double 0.000000e+00, i32 0)
  %arrayidx9 = getelementptr inbounds [9 x i16], ptr @q, i64 0, i64 5
  %20 = load i16, ptr %arrayidx9, align 16
  %21 = fmul double %conv8, 5.000000e-01
  call void @llvm.quantum.rotxy(i16 %20, double 0x400921FB54442D18, double %21, i32 0)
  %arrayidx8 = getelementptr inbounds [9 x i16], ptr @q, i64 0, i64 6
  %22 = load i16, ptr %arrayidx8, align 16
  call void @llvm.quantum.rotxy(i16 %22, double 0x400921FB54442D18, double 0.000000e+00, i32 0)
  %arrayidx7 = getelementptr inbounds [9 x i16], ptr @q, i64 0, i64 6
  %23 = load i16, ptr %arrayidx7, align 16
  %24 = fmul double %conv13, 5.000000e-01
  call void @llvm.quantum.rotxy(i16 %23, double 0x400921FB54442D18, double %24, i32 2)
  br label %aqcc.meas.move.end

aqcc.meas.move.end:                               ; preds = %aqcc.quantum
  ret void
}

; Function Attrs: mustprogress norecurse nounwind uwtable
define dso_local noundef i32 @main() local_unnamed_addr #6 {
entry:
  tail call void @_Z12const_anglesv.stub() #8
  ret i32 0
}

; Function Attrs: uwtable
define internal void @_GLOBAL__sub_I_parameter_printing.cpp() #7 section ".text.startup" {
entry:
  tail call void @_ZNSt8ios_base4InitC1Ev(ptr noundef nonnull align 1 dereferenceable(1) @_ZStL8__ioinit)
  %0 = tail call i32 @__cxa_atexit(ptr noundef nonnull @_ZNSt8ios_base4InitD1Ev, ptr noundef nonnull @_ZStL8__ioinit, ptr noundef nonnull @__dso_handle) #8
  store double 1.000000e-01, ptr @A, align 8, !tbaa !6
  ret void
}

declare i32 @_ZN4qrt021initializeQrtSystem64EPdPcS1_(ptr, ptr, ptr)

declare i1 @_ZN4qrt013isInitializedEv()

declare void @_ZN4qrt020updateMeasurementMapEPbmb(ptr, i64, i1)

declare i32 @_ZN4qrt014scheduleKernelEPFvvEj(ptr, i32)

declare i32 @_ZN4qrt018dismantleQrtSystemEv()

; Function Attrs: nounwind
declare void @llvm.quantum.rotz(i16, double, i32 immarg) #8

; Function Attrs: nounwind
declare void @llvm.quantum.rotxy(i16, double, double, i32 immarg) #8

; Function Attrs: nounwind
declare void @llvm.quantum.cphase(i16, i16, double, i32 immarg) #8

; Function Attrs: nounwind
declare void @llvm.quantum.swapalp(i16, i16, double, i32 immarg) #8

; Function Attrs: nounwind
declare void @llvm.quantum.measz(i16, ptr, i1, i32 immarg) #8

; Function Attrs: nounwind
declare void @llvm.quantum.prepz(i16, i32 immarg) #8

; Function Attrs: nounwind
declare void @llvm.quantum.rotz.imm(i16 immarg, double immarg, i32 immarg) #8

; Function Attrs: nounwind
declare void @llvm.quantum.rotxy.imm(i16 immarg, double immarg, double immarg, i32 immarg) #8

; Function Attrs: nounwind
declare void @llvm.quantum.cphase.imm(i16 immarg, i16 immarg, double immarg, i32 immarg) #8

; Function Attrs: nounwind
declare void @llvm.quantum.swapalp.imm(i16 immarg, i16 immarg, double immarg, i32 immarg) #8

; Function Attrs: nounwind
declare void @llvm.quantum.measz.imm(i16 immarg, i32 immarg) #8

; Function Attrs: nounwind
declare void @llvm.quantum.prepz.imm(i16 immarg, i32 immarg) #8

; Function Attrs: nounwind
declare void @llvm.quantum.rotz.imm.index(i16 immarg, i32 immarg, i32 immarg) #8

; Function Attrs: nounwind
declare void @llvm.quantum.rotxy.imm.index(i16 immarg, i32 immarg, i32 immarg, i32 immarg) #8

; Function Attrs: nounwind
declare void @llvm.quantum.cphase.imm.index(i16 immarg, i16 immarg, i32 immarg, i32 immarg) #8

; Function Attrs: nounwind
declare void @llvm.quantum.swapalp.imm.index(i16 immarg, i16 immarg, i32 immarg, i32 immarg) #8

; Function Attrs: nounwind
declare void @llvm.quantum.measz.imm.index(i16 immarg, i32 immarg, i32 immarg) #8

attributes #0 = { "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #1 = { nounwind "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #2 = { nofree nounwind }
attributes #3 = { mustprogress noinline nounwind uwtable "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #4 = { mustprogress noinline uwtable "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #5 = { nocallback nofree nosync nounwind speculatable willreturn memory(none) }
attributes #6 = { mustprogress norecurse nounwind uwtable "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #7 = { uwtable "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #8 = { nounwind }

!0 = !{!1, !1, i64 0}
!1 = !{!"int", !2, i64 0}
!2 = !{!"omnipotent char", !3, i64 0}
!3 = !{!"Simple C++ TBAA"}
!4 = !{!5, !5, i64 0}
!5 = !{!"double", !2, i64 0}
!6 = !{!7, !5, i64 0}
!7 = !{!"_ZTS3Foo", !5, i64 0}
; CHECK:  Circuit for QBB in quantum_kernel - 'const_angles'
; CHECK: |q_0> : - RXY(3.142, 0.) ---- RXY(3.142, 6.173e5) ----
; CHECK: |q_1> : - RXY(3.142, 0.) ---- RXY(3.142, 6173) -------
; CHECK: |q_2> : - RXY(3.142, 0.) ---- RXY(3.142, 61.73) ------
; CHECK: |q_3> : - RXY(3.142, 0.) ---- RXY(3.142, 0.6173) -----
; CHECK: |q_4> : - RXY(3.142, 0.) ---- RXY(3.142, 6.173e-3) ---
; CHECK: |q_5> : - RXY(3.142, 0.) ---- RXY(3.142, 6.173e-5) ---
; CHECK: |q_6> : - RXY(3.142, 0.) ---- RXY(3.142, 6.173e-7) ---
; CHECK: |q_7> : - RXY(3.142, 0.) ---- RXY(3.142, -0.6173) ----
; CHECK: |q_8> : - RXY(3.142, 0.) ---- RXY(3.142, 0.) ---------
; CHECK:  Circuit for QBB in quantum_kernel - 'function_angles'
; CHECK: |q_0> : - RXY(3.142, 0.) ---- RXY(3.142, getAngle() * 0.5) --------------------
; CHECK: |q_1> : - RXY(3.142, 0.) ---- RXY(3.142, getAngle(0.1) * 0.5) -----------------
; CHECK: |q_2> : - RXY(3.142, 0.) ---- RXY(3.142, getAngle(getAngle() + 0.1) * 0.5) ----
; CHECK: |q_3> : - RXY(3.142, 0.) ---- RXY(3.142, getAngle(0.1, 2) * 0.5) --------------
; CHECK: |q_4> : - RXY(3.142, 0.) ---- RXY(3.142, getAngle(getAngle(), i + 2) * 0.5) ---
; CHECK: |q_5> : - RXY(3.142, 0.) ---- RXY(3.142, Foo::get(A) * 0.5) -------------------
; CHECK:  Circuit for QBB in quantum_kernel - 'scalar_angles'
; CHECK: |q_0> : - RXY(3.142, 0.) ---- RXY(3.142, angle * 0.5) -----------
; CHECK: |q_1> : - RXY(3.142, 0.) ---- RXY(3.142, angles[2] * 0.5) -------
; CHECK: |q_2> : - RXY(3.142, 0.) ---- RXY(3.142, angles[i + 1] * 0.5) ---
; CHECK: |q_3> : - RXY(3.142, 0.) ---- RXY(3.142, a * 0.5) ---------------
; CHECK: |q_4> : - RXY(3.142, 0.) ---- RXY(3.142, b[2] * 0.5) ------------
; CHECK: |q_5> : - RXY(3.142, 0.) ---- RXY(3.142, b[i + 1] * 0.5) --------
; CHECK:  Circuit for QBB in quantum_kernel - 'binary_op_angles'
; CHECK: |q_0> : - RXY(3.142, 0.) ---- RXY(3.142, a * b * 0.5) -----------------------
; CHECK: |q_1> : - RXY(3.142, 0.) ---- RXY(3.142, a / b * 0.5) -----------------------
; CHECK: |q_2> : - RXY(3.142, 0.) ---- RXY(3.142, (a + b) * 0.5) ---------------------
; CHECK: |q_3> : - RXY(3.142, 0.) ---- RXY(3.142, (a - b) * 0.5) ---------------------
; CHECK: |q_4> : - RXY(3.142, 0.) ---- RXY(3.142, llvm.fmuladd.f64(a, b, c) * 0.5) ---
; CHECK: |q_5> : - RXY(3.142, 0.) ---- RXY(3.142, (b + c) * a * 0.5) -----------------
; CHECK: |q_6> : - RXY(3.142, 0.) ---- RXY(3.142, (a + b + c) / 2.5 * 0.5) -----------
; CHECK: |q_7> : - RXY(3.142, 0.) ---- RXY(3.142, -(a + b + c) * 0.5) ----------------
; CHECK:  Circuit for QBB in quantum_kernel - '_Z10meas_cbitsR'
; CHECK: |q_0> : - MeasZ(csig) -------
; CHECK: |q_1> : - MeasZ(c[1]) -------
; CHECK: |q_2> : - MeasZ(c[i + 2]) ---
; CHECK: |q_3> : - MeasZ(arg) --------
; CHECK:  Circuit for QBB in quantum_kernel - 'comparisons'
; CHECK: |q_0> : - RXY(3.142, 0.) ---- RXY(3.142, a == b * 0.5) ---
; CHECK: |q_1> : - RXY(3.142, 0.) ---- RXY(3.142, a != b * 0.5) ---
; CHECK: |q_2> : - RXY(3.142, 0.) ---- RXY(3.142, a < b * 0.5) ----
; CHECK: |q_3> : - RXY(3.142, 0.) ---- RXY(3.142, a <= b * 0.5) ---
; CHECK: |q_4> : - RXY(3.142, 0.) ---- RXY(3.142, a > b * 0.5) ----
; CHECK: |q_5> : - RXY(3.142, 0.) ---- RXY(3.142, a >= b * 0.5) ---
; CHECK:  Circuit for QBB in quantum_kernel - 'odds_and_ends'
; CHECK: |q_0> : - RXY(3.142, 0.) ---- RXY(3.142, a % b * 0.5) -----------------------
; CHECK: |q_1> : - RXY(3.142, 0.) ---- RXY(3.142, a << b * 0.5) ----------------------
; CHECK: |q_2> : - RXY(3.142, 0.) ---- RXY(3.142, a >> b * 0.5) ----------------------
; CHECK: |q_3> : - RXY(3.142, 0.) ---- RXY(3.142, (a != 0) && (b != 0) * 0.5) --------
; CHECK: |q_4> : - RXY(3.142, 0.) ---- RXY(3.142, (b || a) != 0 * 0.5) ---------------
; CHECK: |q_5> : - RXY(3.142, 0.) ---- RXY(3.142, b ^ a * 0.5) -----------------------
; CHECK: |q_6> : - RXY(3.142, 0.) ---- RXY(3.142, (a < b) || ((b ^ a) != 0) * 0.5) ---
