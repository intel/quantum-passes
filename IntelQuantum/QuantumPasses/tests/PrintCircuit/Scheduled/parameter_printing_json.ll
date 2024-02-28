; RUN: opt -S %loadintelquantumpasses -p=q-annotations-to-json,spin-native-annotate,print-circuit-qbb --print-format json --quantum-debug=PrintJson -o /dev/null %s 2>&1 | FileCheck %s --dump-input fail
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
@q = dso_local global [9 x i16] [i16 0, i16 1, i16 2, i16 3, i16 4, i16 5, i16 6, i16 7, i16 8], align 16
@c = dso_local global [5 x i8] zeroinitializer, align 1
@csig = dso_local global i8 0, align 1
@i = dso_local local_unnamed_addr global i32 0, align 4
@A = dso_local global %class.Foo zeroinitializer, align 8
@angle = dso_local local_unnamed_addr global double 0.000000e+00, align 8
@angles = dso_local local_unnamed_addr global [4 x double] zeroinitializer, align 16
@llvm.global_ctors = appending global [1 x { i32, ptr, ptr }] [{ i32, ptr, ptr } { i32 65535, ptr @_GLOBAL__sub_I_parameter_printing.cpp, ptr null }]
@llvm.global.annotations = appending global [24 x { ptr, ptr, ptr, i32, ptr }] [{ ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str, ptr @.str.1, i32 48, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.2, ptr @.str.1, i32 68, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.3, ptr @.str.1, i32 88, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.4, ptr @.str.1, i32 109, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.5, ptr @.str.1, i32 129, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.6, ptr @.str.1, i32 149, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.7, ptr @.str.1, i32 169, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.8, ptr @.str.1, i32 189, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.9, ptr @.str.1, i32 209, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.10, ptr @.str.1, i32 230, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.11, ptr @.str.1, i32 251, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.12, ptr @.str.1, i32 272, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.13, ptr @.str.1, i32 293, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.14, ptr @.str.1, i32 314, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.15, ptr @.str.1, i32 335, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.16, ptr @.str.1, i32 359, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.17, ptr @.str.1, i32 377, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.18, ptr @.str.1, i32 395, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.19, ptr @.str.1, i32 413, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.20, ptr @.str.1, i32 433, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.21, ptr @.str.1, i32 453, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.22, ptr @.str.1, i32 481, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.23, ptr @.str.1, i32 503, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.24, ptr @.str.1, i32 525, ptr null }], section "llvm.metadata"
@aqcc.qsda = global [65536 x double] zeroinitializer #0

declare void @_ZNSt8ios_base4InitC1Ev(ptr noundef nonnull align 1 dereferenceable(1)) unnamed_addr #1

; Function Attrs: nounwind
declare void @_ZNSt8ios_base4InitD1Ev(ptr noundef nonnull align 1 dereferenceable(1)) unnamed_addr #2

; Function Attrs: nofree nounwind
declare i32 @__cxa_atexit(ptr, ptr, ptr) local_unnamed_addr #3

; Function Attrs: mustprogress noinline nounwind uwtable
define dso_local void @_Z12const_anglesv.stub() local_unnamed_addr #4 section ".qbbs_text" {
entry:
  br label %aqcc.quantum

aqcc.quantum:                                     ; preds = %entry
  call void @llvm.quantum.rotxy.imm(i16 0, double 0x400921FB54442D18, double 0.000000e+00, i32 1)
  call void @llvm.quantum.rotxy.imm(i16 0, double 0x400921FB54442D18, double 0x4013E22AE5140000, i32 0)
  call void @llvm.quantum.rotxy.imm(i16 1, double 0x400921FB54442D18, double 0.000000e+00, i32 0)
  call void @llvm.quantum.rotxy.imm(i16 1, double 0x400921FB54442D18, double 0x4006030714BB1000, i32 0)
  call void @llvm.quantum.rotxy.imm(i16 2, double 0x400921FB54442D18, double 0.000000e+00, i32 0)
  call void @llvm.quantum.rotxy.imm(i16 2, double 0x400921FB54442D18, double 0x4014B80A490AC280, i32 0)
  call void @llvm.quantum.rotxy.imm(i16 3, double 0x400921FB54442D18, double 0.000000e+00, i32 0)
  call void @llvm.quantum.rotxy.imm(i16 3, double 0x400921FB54442D18, double 0x3FE3C0CA4283DE1B, i32 0)
  call void @llvm.quantum.rotxy.imm(i16 4, double 0x400921FB54442D18, double 0.000000e+00, i32 0)
  call void @llvm.quantum.rotxy.imm(i16 4, double 0x400921FB54442D18, double 0x3F7948B0F8FAB5E6, i32 0)
  call void @llvm.quantum.rotxy.imm(i16 5, double 0x400921FB54442D18, double 0.000000e+00, i32 0)
  call void @llvm.quantum.rotxy.imm(i16 5, double 0x400921FB54442D18, double 0x3F102E85BE111841, i32 0)
  call void @llvm.quantum.rotxy.imm(i16 6, double 0x400921FB54442D18, double 0.000000e+00, i32 0)
  call void @llvm.quantum.rotxy.imm(i16 6, double 0x400921FB54442D18, double 0x3EA4B66DC015E19B, i32 0)
  call void @llvm.quantum.rotxy.imm(i16 7, double 0x400921FB54442D18, double 0.000000e+00, i32 0)
  call void @llvm.quantum.rotxy.imm(i16 7, double 0x400921FB54442D18, double 0x4016A9E20BF3B155, i32 0)
  call void @llvm.quantum.rotxy.imm(i16 8, double 0x400921FB54442D18, double 0.000000e+00, i32 0)
  call void @llvm.quantum.rotxy.imm(i16 8, double 0x400921FB54442D18, double 0.000000e+00, i32 2)
  br label %aqcc.meas.move.end

aqcc.meas.move.end:                               ; preds = %aqcc.quantum
  store [9 x i16] [i16 0, i16 1, i16 2, i16 3, i16 4, i16 5, i16 6, i16 7, i16 8], ptr @q, align 2
  ret void
}

; Function Attrs: mustprogress noinline uwtable
define dso_local void @_Z15function_anglesv.stub() local_unnamed_addr #5 section ".qbbs_text" {
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
  %1 = fmul double %call, 5.000000e-01
  store double %1, ptr getelementptr inbounds ([65536 x double], ptr @aqcc.qsda, i64 0, i64 4), align 8
  %2 = fmul double %call1, 5.000000e-01
  store double %2, ptr getelementptr inbounds ([65536 x double], ptr @aqcc.qsda, i64 0, i64 5), align 8
  %3 = fmul double %call3, 5.000000e-01
  store double %3, ptr getelementptr inbounds ([65536 x double], ptr @aqcc.qsda, i64 0, i64 6), align 8
  %4 = fmul double %call4, 5.000000e-01
  store double %4, ptr getelementptr inbounds ([65536 x double], ptr @aqcc.qsda, i64 0, i64 7), align 8
  %5 = fmul double %call7, 5.000000e-01
  store double %5, ptr getelementptr inbounds ([65536 x double], ptr @aqcc.qsda, i64 0, i64 8), align 8
  %6 = fmul double %call8, 5.000000e-01
  store double %6, ptr getelementptr inbounds ([65536 x double], ptr @aqcc.qsda, i64 0, i64 9), align 8
  br label %aqcc.quantum

aqcc.quantum:                                     ; preds = %entry
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
  call void @llvm.quantum.rotxy.imm(i16 5, double 0x400921FB54442D18, double 0.000000e+00, i32 0)
  call void @llvm.quantum.rotxy.imm.index(i16 5, i32 2, i32 9, i32 2)
  br label %aqcc.meas.move.end

aqcc.meas.move.end:                               ; preds = %aqcc.quantum
  store [9 x i16] [i16 0, i16 1, i16 2, i16 3, i16 4, i16 5, i16 6, i16 7, i16 8], ptr @q, align 2
  ret void
}

declare noundef double @_Z8getAnglev() local_unnamed_addr #1

declare noundef double @_Z8getAngled(double noundef) local_unnamed_addr #1

declare noundef double @_Z8getAngledi(double noundef, i32 noundef) local_unnamed_addr #1

declare noundef double @_ZN3Foo3getEv(ptr noundef nonnull align 8 dereferenceable(8)) local_unnamed_addr #1

; Function Attrs: mustprogress noinline nounwind uwtable
define dso_local void @_Z13scalar_anglesdPd.stub(double noundef %a, ptr nocapture nofree noundef readonly %b) local_unnamed_addr #4 section ".qbbs_text" {
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
  %7 = fmul double %0, 5.000000e-01
  store double %7, ptr getelementptr inbounds ([65536 x double], ptr @aqcc.qsda, i64 0, i64 4), align 8
  %8 = fmul double %1, 5.000000e-01
  store double %8, ptr getelementptr inbounds ([65536 x double], ptr @aqcc.qsda, i64 0, i64 5), align 8
  %9 = fmul double %3, 5.000000e-01
  store double %9, ptr getelementptr inbounds ([65536 x double], ptr @aqcc.qsda, i64 0, i64 6), align 8
  %10 = fmul double %a, 5.000000e-01
  store double %10, ptr getelementptr inbounds ([65536 x double], ptr @aqcc.qsda, i64 0, i64 7), align 8
  %11 = fmul double %4, 5.000000e-01
  store double %11, ptr getelementptr inbounds ([65536 x double], ptr @aqcc.qsda, i64 0, i64 8), align 8
  %12 = fmul double %6, 5.000000e-01
  store double %12, ptr getelementptr inbounds ([65536 x double], ptr @aqcc.qsda, i64 0, i64 9), align 8
  br label %aqcc.quantum

aqcc.quantum:                                     ; preds = %entry
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
  call void @llvm.quantum.rotxy.imm(i16 5, double 0x400921FB54442D18, double 0.000000e+00, i32 0)
  call void @llvm.quantum.rotxy.imm.index(i16 5, i32 2, i32 9, i32 2)
  br label %aqcc.meas.move.end

aqcc.meas.move.end:                               ; preds = %aqcc.quantum
  store [9 x i16] [i16 0, i16 1, i16 2, i16 3, i16 4, i16 5, i16 6, i16 7, i16 8], ptr @q, align 2
  ret void
}

; Function Attrs: mustprogress noinline nounwind uwtable
define dso_local void @_Z16binary_op_anglesddd.stub(double noundef %a, double noundef %b, double noundef %c) local_unnamed_addr #4 section ".qbbs_text" {
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
  %1 = fmul double %mul, 5.000000e-01
  store double %1, ptr getelementptr inbounds ([65536 x double], ptr @aqcc.qsda, i64 0, i64 4), align 8
  %2 = fmul double %div, 5.000000e-01
  store double %2, ptr getelementptr inbounds ([65536 x double], ptr @aqcc.qsda, i64 0, i64 5), align 8
  %3 = fmul double %add, 5.000000e-01
  store double %3, ptr getelementptr inbounds ([65536 x double], ptr @aqcc.qsda, i64 0, i64 6), align 8
  %4 = fmul double %sub, 5.000000e-01
  store double %4, ptr getelementptr inbounds ([65536 x double], ptr @aqcc.qsda, i64 0, i64 7), align 8
  %5 = fmul double %0, 5.000000e-01
  store double %5, ptr getelementptr inbounds ([65536 x double], ptr @aqcc.qsda, i64 0, i64 8), align 8
  %6 = fmul double %mul3, 5.000000e-01
  store double %6, ptr getelementptr inbounds ([65536 x double], ptr @aqcc.qsda, i64 0, i64 9), align 8
  %7 = fmul double %div6, 5.000000e-01
  store double %7, ptr getelementptr inbounds ([65536 x double], ptr @aqcc.qsda, i64 0, i64 10), align 8
  %8 = fmul double %fneg, 5.000000e-01
  store double %8, ptr getelementptr inbounds ([65536 x double], ptr @aqcc.qsda, i64 0, i64 11), align 8
  br label %aqcc.quantum

aqcc.quantum:                                     ; preds = %entry
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
  call void @llvm.quantum.rotxy.imm(i16 5, double 0x400921FB54442D18, double 0.000000e+00, i32 0)
  call void @llvm.quantum.rotxy.imm.index(i16 5, i32 2, i32 9, i32 0)
  call void @llvm.quantum.rotxy.imm(i16 6, double 0x400921FB54442D18, double 0.000000e+00, i32 0)
  call void @llvm.quantum.rotxy.imm.index(i16 6, i32 2, i32 10, i32 0)
  call void @llvm.quantum.rotxy.imm(i16 7, double 0x400921FB54442D18, double 0.000000e+00, i32 0)
  call void @llvm.quantum.rotxy.imm.index(i16 7, i32 2, i32 11, i32 2)
  br label %aqcc.meas.move.end

aqcc.meas.move.end:                               ; preds = %aqcc.quantum
  store [9 x i16] [i16 0, i16 1, i16 2, i16 3, i16 4, i16 5, i16 6, i16 7, i16 8], ptr @q, align 2
  ret void
}

; Function Attrs: nocallback nofree nosync nounwind speculatable willreturn memory(none)
declare double @llvm.fmuladd.f64(double, double, double) #6

; Function Attrs: mustprogress noinline nounwind uwtable
define dso_local void @_Z10meas_cbitsRb.stub(ptr noundef nonnull align 1 dereferenceable(1) "cbit_ref" %arg) local_unnamed_addr #4 section ".qbbs_text" {
entry:
  %0 = load i32, ptr @i, align 4, !tbaa !0
  %add = add nsw i32 %0, 2
  %idxprom = sext i32 %add to i64
  %arrayidx = getelementptr inbounds [5 x i8], ptr @c, i64 0, i64 %idxprom
  br label %aqcc.quantum

aqcc.quantum:                                     ; preds = %entry
  call void @llvm.quantum.measz.imm.index(i16 0, i32 0, i32 1)
  call void @llvm.quantum.measz.imm.index(i16 1, i32 1, i32 0)
  call void @llvm.quantum.measz.imm.index(i16 2, i32 2, i32 0)
  call void @llvm.quantum.measz.imm.index(i16 3, i32 3, i32 2)
  br label %aqcc.meas.move.end

aqcc.meas.move.end:                               ; preds = %aqcc.quantum
  %QRTcast3 = bitcast ptr %arg to ptr
  call void @_ZN4qrt020updateMeasurementMapEPbmb(ptr %QRTcast3, i64 3, i1 false)
  %QRTcast2 = bitcast ptr %arrayidx to ptr
  call void @_ZN4qrt020updateMeasurementMapEPbmb(ptr %QRTcast2, i64 2, i1 false)
  %QRTcast1 = bitcast ptr getelementptr inbounds ([5 x i8], ptr @c, i64 0, i64 1) to ptr
  call void @_ZN4qrt020updateMeasurementMapEPbmb(ptr %QRTcast1, i64 1, i1 false)
  %QRTcast = bitcast ptr @csig to ptr
  call void @_ZN4qrt020updateMeasurementMapEPbmb(ptr %QRTcast, i64 0, i1 false)
  store [9 x i16] [i16 0, i16 1, i16 2, i16 3, i16 4, i16 5, i16 6, i16 7, i16 8], ptr @q, align 2
  ret void
}

; Function Attrs: mustprogress noinline nounwind uwtable
define dso_local void @_Z11comparisonsjj.stub(i32 noundef %a, i32 noundef %b) local_unnamed_addr #4 section ".qbbs_text" {
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
  %0 = fmul double %conv, 5.000000e-01
  store double %0, ptr getelementptr inbounds ([65536 x double], ptr @aqcc.qsda, i64 0, i64 4), align 8
  %1 = fmul double %conv2, 5.000000e-01
  store double %1, ptr getelementptr inbounds ([65536 x double], ptr @aqcc.qsda, i64 0, i64 5), align 8
  %2 = fmul double %conv4, 5.000000e-01
  store double %2, ptr getelementptr inbounds ([65536 x double], ptr @aqcc.qsda, i64 0, i64 6), align 8
  %3 = fmul double %conv6, 5.000000e-01
  store double %3, ptr getelementptr inbounds ([65536 x double], ptr @aqcc.qsda, i64 0, i64 7), align 8
  %4 = fmul double %conv8, 5.000000e-01
  store double %4, ptr getelementptr inbounds ([65536 x double], ptr @aqcc.qsda, i64 0, i64 8), align 8
  %5 = fmul double %conv10, 5.000000e-01
  store double %5, ptr getelementptr inbounds ([65536 x double], ptr @aqcc.qsda, i64 0, i64 9), align 8
  br label %aqcc.quantum

aqcc.quantum:                                     ; preds = %entry
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
  call void @llvm.quantum.rotxy.imm(i16 5, double 0x400921FB54442D18, double 0.000000e+00, i32 0)
  call void @llvm.quantum.rotxy.imm.index(i16 5, i32 2, i32 9, i32 2)
  br label %aqcc.meas.move.end

aqcc.meas.move.end:                               ; preds = %aqcc.quantum
  store [9 x i16] [i16 0, i16 1, i16 2, i16 3, i16 4, i16 5, i16 6, i16 7, i16 8], ptr @q, align 2
  ret void
}

; Function Attrs: mustprogress noinline nounwind uwtable
define dso_local void @_Z13odds_and_endsjj.stub(i32 noundef %a, i32 noundef %b) local_unnamed_addr #4 section ".qbbs_text" {
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
  %4 = fmul double %conv, 5.000000e-01
  store double %4, ptr getelementptr inbounds ([65536 x double], ptr @aqcc.qsda, i64 0, i64 4), align 8
  %5 = fmul double %conv1, 5.000000e-01
  store double %5, ptr getelementptr inbounds ([65536 x double], ptr @aqcc.qsda, i64 0, i64 5), align 8
  %6 = fmul double %conv2, 5.000000e-01
  store double %6, ptr getelementptr inbounds ([65536 x double], ptr @aqcc.qsda, i64 0, i64 6), align 8
  %7 = fmul double %conv4, 5.000000e-01
  store double %7, ptr getelementptr inbounds ([65536 x double], ptr @aqcc.qsda, i64 0, i64 7), align 8
  %8 = fmul double %conv7, 5.000000e-01
  store double %8, ptr getelementptr inbounds ([65536 x double], ptr @aqcc.qsda, i64 0, i64 8), align 8
  %9 = fmul double %conv8, 5.000000e-01
  store double %9, ptr getelementptr inbounds ([65536 x double], ptr @aqcc.qsda, i64 0, i64 9), align 8
  %10 = fmul double %conv13, 5.000000e-01
  store double %10, ptr getelementptr inbounds ([65536 x double], ptr @aqcc.qsda, i64 0, i64 10), align 8
  br label %aqcc.quantum

aqcc.quantum:                                     ; preds = %entry
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
  call void @llvm.quantum.rotxy.imm(i16 5, double 0x400921FB54442D18, double 0.000000e+00, i32 0)
  call void @llvm.quantum.rotxy.imm.index(i16 5, i32 2, i32 9, i32 0)
  call void @llvm.quantum.rotxy.imm(i16 6, double 0x400921FB54442D18, double 0.000000e+00, i32 0)
  call void @llvm.quantum.rotxy.imm.index(i16 6, i32 2, i32 10, i32 2)
  br label %aqcc.meas.move.end

aqcc.meas.move.end:                               ; preds = %aqcc.quantum
  store [9 x i16] [i16 0, i16 1, i16 2, i16 3, i16 4, i16 5, i16 6, i16 7, i16 8], ptr @q, align 2
  ret void
}

; Function Attrs: mustprogress norecurse nounwind uwtable
define dso_local noundef i32 @main() local_unnamed_addr #7 {
entry:
  tail call void @_Z12const_anglesv.stub() #9
  ret i32 0
}

; Function Attrs: uwtable
define internal void @_GLOBAL__sub_I_parameter_printing.cpp() #8 section ".text.startup" {
entry:
  tail call void @_ZNSt8ios_base4InitC1Ev(ptr noundef nonnull align 1 dereferenceable(1) @_ZStL8__ioinit)
  %0 = tail call i32 @__cxa_atexit(ptr noundef nonnull @_ZNSt8ios_base4InitD1Ev, ptr noundef nonnull @_ZStL8__ioinit, ptr noundef nonnull @__dso_handle) #9
  store double 1.000000e-01, ptr @A, align 8, !tbaa !6
  ret void
}

declare i32 @_ZN4qrt021initializeQrtSystem64EPdPcS1_(ptr, ptr, ptr)

declare i1 @_ZN4qrt013isInitializedEv()

declare void @_ZN4qrt020updateMeasurementMapEPbmb(ptr, i64, i1)

declare i32 @_ZN4qrt014scheduleKernelEPFvvEj(ptr, i32)

declare i32 @_ZN4qrt018dismantleQrtSystemEv()

; Function Attrs: nounwind
declare void @llvm.quantum.rotz(i16, double, i32 immarg) #9

; Function Attrs: nounwind
declare void @llvm.quantum.rotxy(i16, double, double, i32 immarg) #9

; Function Attrs: nounwind
declare void @llvm.quantum.cphase(i16, i16, double, i32 immarg) #9

; Function Attrs: nounwind
declare void @llvm.quantum.swapalp(i16, i16, double, i32 immarg) #9

; Function Attrs: nounwind
declare void @llvm.quantum.measz(i16, ptr, i1, i32 immarg) #9

; Function Attrs: nounwind
declare void @llvm.quantum.prepz(i16, i32 immarg) #9

; Function Attrs: nounwind
declare void @llvm.quantum.rotz.imm(i16 immarg, double immarg, i32 immarg) #9

; Function Attrs: nounwind
declare void @llvm.quantum.rotxy.imm(i16 immarg, double immarg, double immarg, i32 immarg) #9

; Function Attrs: nounwind
declare void @llvm.quantum.cphase.imm(i16 immarg, i16 immarg, double immarg, i32 immarg) #9

; Function Attrs: nounwind
declare void @llvm.quantum.swapalp.imm(i16 immarg, i16 immarg, double immarg, i32 immarg) #9

; Function Attrs: nounwind
declare void @llvm.quantum.measz.imm(i16 immarg, i32 immarg) #9

; Function Attrs: nounwind
declare void @llvm.quantum.prepz.imm(i16 immarg, i32 immarg) #9

; Function Attrs: nounwind
declare void @llvm.quantum.rotz.imm.index(i16 immarg, i32 immarg, i32 immarg) #9

; Function Attrs: nounwind
declare void @llvm.quantum.rotxy.imm.index(i16 immarg, i32 immarg, i32 immarg, i32 immarg) #9

; Function Attrs: nounwind
declare void @llvm.quantum.cphase.imm.index(i16 immarg, i16 immarg, i32 immarg, i32 immarg) #9

; Function Attrs: nounwind
declare void @llvm.quantum.swapalp.imm.index(i16 immarg, i16 immarg, i32 immarg, i32 immarg) #9

; Function Attrs: nounwind
declare void @llvm.quantum.measz.imm.index(i16 immarg, i32 immarg, i32 immarg) #9

attributes #0 = { "quantum_shared_var" }
attributes #1 = { "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #2 = { nounwind "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #3 = { nofree nounwind }
attributes #4 = { mustprogress noinline nounwind uwtable "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #5 = { mustprogress noinline uwtable "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #6 = { nocallback nofree nosync nounwind speculatable willreturn memory(none) }
attributes #7 = { mustprogress norecurse nounwind uwtable "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #8 = { uwtable "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #9 = { nounwind }

!0 = !{!1, !1, i64 0}
!1 = !{!"int", !2, i64 0}
!2 = !{!"omnipotent char", !3, i64 0}
!3 = !{!"Simple C++ TBAA"}
!4 = !{!5, !5, i64 0}
!5 = !{!"double", !2, i64 0}
!6 = !{!7, !5, i64 0}
!7 = !{!"_ZTS3Foo", !5, i64 0}
; CHECK: Json instructions: 
; CHECK: {
; CHECK:     "IntelQuantumID": "Circuit-v1",
; CHECK:     "title": "const_angles",
; CHECK:     "numQbits": 9,
; CHECK:     "qbitNames": [
; CHECK:         "qid_0",
; CHECK:         "qid_1",
; CHECK:         "qid_2",
; CHECK:         "qid_3",
; CHECK:         "qid_4",
; CHECK:         "qid_5",
; CHECK:         "qid_6",
; CHECK:         "qid_7",
; CHECK:         "qid_8"
; CHECK:     ],
; CHECK:     "gates": [
; CHECK:         {
; CHECK:             "name": "RXY",
; CHECK:             "qubits": [
; CHECK:                 0
; CHECK:             ]
; CHECK:         },
; CHECK:         {
; CHECK:             "name": "RXY",
; CHECK:             "qubits": [
; CHECK:                 1
; CHECK:             ]
; CHECK:         },
; CHECK:         {
; CHECK:             "name": "RXY",
; CHECK:             "qubits": [
; CHECK:                 2
; CHECK:             ]
; CHECK:         },
; CHECK:         {
; CHECK:             "name": "RXY",
; CHECK:             "qubits": [
; CHECK:                 3
; CHECK:             ]
; CHECK:         },
; CHECK:         {
; CHECK:             "name": "RXY",
; CHECK:             "qubits": [
; CHECK:                 4
; CHECK:             ]
; CHECK:         },
; CHECK:         {
; CHECK:             "name": "RXY",
; CHECK:             "qubits": [
; CHECK:                 5
; CHECK:             ]
; CHECK:         },
; CHECK:         {
; CHECK:             "name": "RXY",
; CHECK:             "qubits": [
; CHECK:                 6
; CHECK:             ]
; CHECK:         },
; CHECK:         {
; CHECK:             "name": "RXY",
; CHECK:             "qubits": [
; CHECK:                 7
; CHECK:             ]
; CHECK:         },
; CHECK:         {
; CHECK:             "name": "RXY",
; CHECK:             "qubits": [
; CHECK:                 8
; CHECK:             ]
; CHECK:         },
; CHECK:         {
; CHECK:             "name": "RXY",
; CHECK:             "qubits": [
; CHECK:                 0
; CHECK:             ]
; CHECK:         },
; CHECK:         {
; CHECK:             "name": "RXY",
; CHECK:             "qubits": [
; CHECK:                 1
; CHECK:             ]
; CHECK:         },
; CHECK:         {
; CHECK:             "name": "RXY",
; CHECK:             "qubits": [
; CHECK:                 2
; CHECK:             ]
; CHECK:         },
; CHECK:         {
; CHECK:             "name": "RXY",
; CHECK:             "qubits": [
; CHECK:                 3
; CHECK:             ]
; CHECK:         },
; CHECK:         {
; CHECK:             "name": "RXY",
; CHECK:             "qubits": [
; CHECK:                 4
; CHECK:             ]
; CHECK:         },
; CHECK:         {
; CHECK:             "name": "RXY",
; CHECK:             "qubits": [
; CHECK:                 5
; CHECK:             ]
; CHECK:         },
; CHECK:         {
; CHECK:             "name": "RXY",
; CHECK:             "qubits": [
; CHECK:                 6
; CHECK:             ]
; CHECK:         },
; CHECK:         {
; CHECK:             "name": "RXY",
; CHECK:             "qubits": [
; CHECK:                 7
; CHECK:             ]
; CHECK:         },
; CHECK:         {
; CHECK:             "name": "RXY",
; CHECK:             "qubits": [
; CHECK:                 8
; CHECK:             ]
; CHECK:         }
; CHECK:     ]
; CHECK: }
; CHECK: Json instructions: 
; CHECK: {
; CHECK:     "IntelQuantumID": "Circuit-v1",
; CHECK:     "title": "function_angles",
; CHECK:     "numQbits": 6,
; CHECK:     "qbitNames": [
; CHECK:         "qid_0",
; CHECK:         "qid_1",
; CHECK:         "qid_2",
; CHECK:         "qid_3",
; CHECK:         "qid_4",
; CHECK:         "qid_5"
; CHECK:     ],
; CHECK:     "gates": [
; CHECK:         {
; CHECK:             "name": "RXY",
; CHECK:             "qubits": [
; CHECK:                 0
; CHECK:             ]
; CHECK:         },
; CHECK:         {
; CHECK:             "name": "RXY",
; CHECK:             "qubits": [
; CHECK:                 1
; CHECK:             ]
; CHECK:         },
; CHECK:         {
; CHECK:             "name": "RXY",
; CHECK:             "qubits": [
; CHECK:                 2
; CHECK:             ]
; CHECK:         },
; CHECK:         {
; CHECK:             "name": "RXY",
; CHECK:             "qubits": [
; CHECK:                 3
; CHECK:             ]
; CHECK:         },
; CHECK:         {
; CHECK:             "name": "RXY",
; CHECK:             "qubits": [
; CHECK:                 4
; CHECK:             ]
; CHECK:         },
; CHECK:         {
; CHECK:             "name": "RXY",
; CHECK:             "qubits": [
; CHECK:                 5
; CHECK:             ]
; CHECK:         },
; CHECK:         {
; CHECK:             "name": "RXY",
; CHECK:             "qubits": [
; CHECK:                 0
; CHECK:             ]
; CHECK:         },
; CHECK:         {
; CHECK:             "name": "RXY",
; CHECK:             "qubits": [
; CHECK:                 1
; CHECK:             ]
; CHECK:         },
; CHECK:         {
; CHECK:             "name": "RXY",
; CHECK:             "qubits": [
; CHECK:                 2
; CHECK:             ]
; CHECK:         },
; CHECK:         {
; CHECK:             "name": "RXY",
; CHECK:             "qubits": [
; CHECK:                 3
; CHECK:             ]
; CHECK:         },
; CHECK:         {
; CHECK:             "name": "RXY",
; CHECK:             "qubits": [
; CHECK:                 4
; CHECK:             ]
; CHECK:         },
; CHECK:         {
; CHECK:             "name": "RXY",
; CHECK:             "qubits": [
; CHECK:                 5
; CHECK:             ]
; CHECK:         }
; CHECK:     ]
; CHECK: }
; CHECK: Json instructions: 
; CHECK: {
; CHECK:     "IntelQuantumID": "Circuit-v1",
; CHECK:     "title": "scalar_angles",
; CHECK:     "numQbits": 6,
; CHECK:     "qbitNames": [
; CHECK:         "qid_0",
; CHECK:         "qid_1",
; CHECK:         "qid_2",
; CHECK:         "qid_3",
; CHECK:         "qid_4",
; CHECK:         "qid_5"
; CHECK:     ],
; CHECK:     "gates": [
; CHECK:         {
; CHECK:             "name": "RXY",
; CHECK:             "qubits": [
; CHECK:                 0
; CHECK:             ]
; CHECK:         },
; CHECK:         {
; CHECK:             "name": "RXY",
; CHECK:             "qubits": [
; CHECK:                 1
; CHECK:             ]
; CHECK:         },
; CHECK:         {
; CHECK:             "name": "RXY",
; CHECK:             "qubits": [
; CHECK:                 2
; CHECK:             ]
; CHECK:         },
; CHECK:         {
; CHECK:             "name": "RXY",
; CHECK:             "qubits": [
; CHECK:                 3
; CHECK:             ]
; CHECK:         },
; CHECK:         {
; CHECK:             "name": "RXY",
; CHECK:             "qubits": [
; CHECK:                 4
; CHECK:             ]
; CHECK:         },
; CHECK:         {
; CHECK:             "name": "RXY",
; CHECK:             "qubits": [
; CHECK:                 5
; CHECK:             ]
; CHECK:         },
; CHECK:         {
; CHECK:             "name": "RXY",
; CHECK:             "qubits": [
; CHECK:                 0
; CHECK:             ]
; CHECK:         },
; CHECK:         {
; CHECK:             "name": "RXY",
; CHECK:             "qubits": [
; CHECK:                 1
; CHECK:             ]
; CHECK:         },
; CHECK:         {
; CHECK:             "name": "RXY",
; CHECK:             "qubits": [
; CHECK:                 2
; CHECK:             ]
; CHECK:         },
; CHECK:         {
; CHECK:             "name": "RXY",
; CHECK:             "qubits": [
; CHECK:                 3
; CHECK:             ]
; CHECK:         },
; CHECK:         {
; CHECK:             "name": "RXY",
; CHECK:             "qubits": [
; CHECK:                 4
; CHECK:             ]
; CHECK:         },
; CHECK:         {
; CHECK:             "name": "RXY",
; CHECK:             "qubits": [
; CHECK:                 5
; CHECK:             ]
; CHECK:         }
; CHECK:     ]
; CHECK: }
; CHECK: Json instructions: 
; CHECK: {
; CHECK:     "IntelQuantumID": "Circuit-v1",
; CHECK:     "title": "binary_op_angles",
; CHECK:     "numQbits": 8,
; CHECK:     "qbitNames": [
; CHECK:         "qid_0",
; CHECK:         "qid_1",
; CHECK:         "qid_2",
; CHECK:         "qid_3",
; CHECK:         "qid_4",
; CHECK:         "qid_5",
; CHECK:         "qid_6",
; CHECK:         "qid_7"
; CHECK:     ],
; CHECK:     "gates": [
; CHECK:         {
; CHECK:             "name": "RXY",
; CHECK:             "qubits": [
; CHECK:                 0
; CHECK:             ]
; CHECK:         },
; CHECK:         {
; CHECK:             "name": "RXY",
; CHECK:             "qubits": [
; CHECK:                 1
; CHECK:             ]
; CHECK:         },
; CHECK:         {
; CHECK:             "name": "RXY",
; CHECK:             "qubits": [
; CHECK:                 2
; CHECK:             ]
; CHECK:         },
; CHECK:         {
; CHECK:             "name": "RXY",
; CHECK:             "qubits": [
; CHECK:                 3
; CHECK:             ]
; CHECK:         },
; CHECK:         {
; CHECK:             "name": "RXY",
; CHECK:             "qubits": [
; CHECK:                 4
; CHECK:             ]
; CHECK:         },
; CHECK:         {
; CHECK:             "name": "RXY",
; CHECK:             "qubits": [
; CHECK:                 5
; CHECK:             ]
; CHECK:         },
; CHECK:         {
; CHECK:             "name": "RXY",
; CHECK:             "qubits": [
; CHECK:                 6
; CHECK:             ]
; CHECK:         },
; CHECK:         {
; CHECK:             "name": "RXY",
; CHECK:             "qubits": [
; CHECK:                 7
; CHECK:             ]
; CHECK:         },
; CHECK:         {
; CHECK:             "name": "RXY",
; CHECK:             "qubits": [
; CHECK:                 0
; CHECK:             ]
; CHECK:         },
; CHECK:         {
; CHECK:             "name": "RXY",
; CHECK:             "qubits": [
; CHECK:                 1
; CHECK:             ]
; CHECK:         },
; CHECK:         {
; CHECK:             "name": "RXY",
; CHECK:             "qubits": [
; CHECK:                 2
; CHECK:             ]
; CHECK:         },
; CHECK:         {
; CHECK:             "name": "RXY",
; CHECK:             "qubits": [
; CHECK:                 3
; CHECK:             ]
; CHECK:         },
; CHECK:         {
; CHECK:             "name": "RXY",
; CHECK:             "qubits": [
; CHECK:                 4
; CHECK:             ]
; CHECK:         },
; CHECK:         {
; CHECK:             "name": "RXY",
; CHECK:             "qubits": [
; CHECK:                 5
; CHECK:             ]
; CHECK:         },
; CHECK:         {
; CHECK:             "name": "RXY",
; CHECK:             "qubits": [
; CHECK:                 6
; CHECK:             ]
; CHECK:         },
; CHECK:         {
; CHECK:             "name": "RXY",
; CHECK:             "qubits": [
; CHECK:                 7
; CHECK:             ]
; CHECK:         }
; CHECK:     ]
; CHECK: }
; CHECK: Json instructions: 
; CHECK: {
; CHECK:     "IntelQuantumID": "Circuit-v1",
; CHECK:     "title": "_Z10meas_cbitsR",
; CHECK:     "numQbits": 4,
; CHECK:     "qbitNames": [
; CHECK:         "qid_0",
; CHECK:         "qid_1",
; CHECK:         "qid_2",
; CHECK:         "qid_3"
; CHECK:     ],
; CHECK:     "gates": [
; CHECK:         {
; CHECK:             "name": "MeasZ",
; CHECK:             "qubits": [
; CHECK:                 0
; CHECK:             ]
; CHECK:         },
; CHECK:         {
; CHECK:             "name": "MeasZ",
; CHECK:             "qubits": [
; CHECK:                 1
; CHECK:             ]
; CHECK:         },
; CHECK:         {
; CHECK:             "name": "MeasZ",
; CHECK:             "qubits": [
; CHECK:                 2
; CHECK:             ]
; CHECK:         },
; CHECK:         {
; CHECK:             "name": "MeasZ",
; CHECK:             "qubits": [
; CHECK:                 3
; CHECK:             ]
; CHECK:         }
; CHECK:     ]
; CHECK: }
; CHECK: Json instructions: 
; CHECK: {
; CHECK:     "IntelQuantumID": "Circuit-v1",
; CHECK:     "title": "comparisons",
; CHECK:     "numQbits": 6,
; CHECK:     "qbitNames": [
; CHECK:         "qid_0",
; CHECK:         "qid_1",
; CHECK:         "qid_2",
; CHECK:         "qid_3",
; CHECK:         "qid_4",
; CHECK:         "qid_5"
; CHECK:     ],
; CHECK:     "gates": [
; CHECK:         {
; CHECK:             "name": "RXY",
; CHECK:             "qubits": [
; CHECK:                 0
; CHECK:             ]
; CHECK:         },
; CHECK:         {
; CHECK:             "name": "RXY",
; CHECK:             "qubits": [
; CHECK:                 1
; CHECK:             ]
; CHECK:         },
; CHECK:         {
; CHECK:             "name": "RXY",
; CHECK:             "qubits": [
; CHECK:                 2
; CHECK:             ]
; CHECK:         },
; CHECK:         {
; CHECK:             "name": "RXY",
; CHECK:             "qubits": [
; CHECK:                 3
; CHECK:             ]
; CHECK:         },
; CHECK:         {
; CHECK:             "name": "RXY",
; CHECK:             "qubits": [
; CHECK:                 4
; CHECK:             ]
; CHECK:         },
; CHECK:         {
; CHECK:             "name": "RXY",
; CHECK:             "qubits": [
; CHECK:                 5
; CHECK:             ]
; CHECK:         },
; CHECK:         {
; CHECK:             "name": "RXY",
; CHECK:             "qubits": [
; CHECK:                 0
; CHECK:             ]
; CHECK:         },
; CHECK:         {
; CHECK:             "name": "RXY",
; CHECK:             "qubits": [
; CHECK:                 1
; CHECK:             ]
; CHECK:         },
; CHECK:         {
; CHECK:             "name": "RXY",
; CHECK:             "qubits": [
; CHECK:                 2
; CHECK:             ]
; CHECK:         },
; CHECK:         {
; CHECK:             "name": "RXY",
; CHECK:             "qubits": [
; CHECK:                 3
; CHECK:             ]
; CHECK:         },
; CHECK:         {
; CHECK:             "name": "RXY",
; CHECK:             "qubits": [
; CHECK:                 4
; CHECK:             ]
; CHECK:         },
; CHECK:         {
; CHECK:             "name": "RXY",
; CHECK:             "qubits": [
; CHECK:                 5
; CHECK:             ]
; CHECK:         }
; CHECK:     ]
; CHECK: }
; CHECK: Json instructions: 
; CHECK: {
; CHECK:     "IntelQuantumID": "Circuit-v1",
; CHECK:     "title": "odds_and_ends",
; CHECK:     "numQbits": 7,
; CHECK:     "qbitNames": [
; CHECK:         "qid_0",
; CHECK:         "qid_1",
; CHECK:         "qid_2",
; CHECK:         "qid_3",
; CHECK:         "qid_4",
; CHECK:         "qid_5",
; CHECK:         "qid_6"
; CHECK:     ],
; CHECK:     "gates": [
; CHECK:         {
; CHECK:             "name": "RXY",
; CHECK:             "qubits": [
; CHECK:                 0
; CHECK:             ]
; CHECK:         },
; CHECK:         {
; CHECK:             "name": "RXY",
; CHECK:             "qubits": [
; CHECK:                 1
; CHECK:             ]
; CHECK:         },
; CHECK:         {
; CHECK:             "name": "RXY",
; CHECK:             "qubits": [
; CHECK:                 2
; CHECK:             ]
; CHECK:         },
; CHECK:         {
; CHECK:             "name": "RXY",
; CHECK:             "qubits": [
; CHECK:                 3
; CHECK:             ]
; CHECK:         },
; CHECK:         {
; CHECK:             "name": "RXY",
; CHECK:             "qubits": [
; CHECK:                 4
; CHECK:             ]
; CHECK:         },
; CHECK:         {
; CHECK:             "name": "RXY",
; CHECK:             "qubits": [
; CHECK:                 5
; CHECK:             ]
; CHECK:         },
; CHECK:         {
; CHECK:             "name": "RXY",
; CHECK:             "qubits": [
; CHECK:                 6
; CHECK:             ]
; CHECK:         },
; CHECK:         {
; CHECK:             "name": "RXY",
; CHECK:             "qubits": [
; CHECK:                 0
; CHECK:             ]
; CHECK:         },
; CHECK:         {
; CHECK:             "name": "RXY",
; CHECK:             "qubits": [
; CHECK:                 1
; CHECK:             ]
; CHECK:         },
; CHECK:         {
; CHECK:             "name": "RXY",
; CHECK:             "qubits": [
; CHECK:                 2
; CHECK:             ]
; CHECK:         },
; CHECK:         {
; CHECK:             "name": "RXY",
; CHECK:             "qubits": [
; CHECK:                 3
; CHECK:             ]
; CHECK:         },
; CHECK:         {
; CHECK:             "name": "RXY",
; CHECK:             "qubits": [
; CHECK:                 4
; CHECK:             ]
; CHECK:         },
; CHECK:         {
; CHECK:             "name": "RXY",
; CHECK:             "qubits": [
; CHECK:                 5
; CHECK:             ]
; CHECK:         },
; CHECK:         {
; CHECK:             "name": "RXY",
; CHECK:             "qubits": [
; CHECK:                 6
; CHECK:             ]
; CHECK:         }
; CHECK:     ]
; CHECK: }
