; NOTE: Assertions have been autogenerated by utils/update_test_checks.py UTC_ARGS: --version 2
; RUN: opt -S %loadintelquantumpasses -p="q-annotations-to-json,spin-native-annotate,no-op-module,function(instcombine),rpo-function-attrs,globalopt,globaldce,float2int,lower-constant-intrinsics,loop-simplify,lcssa,loop-rotate,loop-distribute,loop-vectorize,loop-simplify,loop-load-elim,instcombine,simplifycfg,instcombine,loop-simplify,lcssa,cgscc(function(loop(q-loop-unroll))),instcombine,loop-simplify,lcssa,function(loop-mssa(licm)),transform-warning,alignment-from-assumptions,strip-dead-prototypes,loop-simplify,lcssa,loop-sink,instsimplify,div-rem-pairs,simplifycfg,loop-simplify,lcssa,loop-rotate,lcssa,loop-idiom,loop-deletion,no-op-loop,cgscc(function(loop(q-loop-unroll))),sccp,instcombine,early-cse,mem2reg,simplifycfg,verify" -o - %s | FileCheck %s --dump-input fail
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
@q = dso_local global [5 x i16] zeroinitializer, align 2
@llvm.global.annotations = appending global [24 x { ptr, ptr, ptr, i32, ptr }] [{ ptr, ptr, ptr, i32, ptr } { ptr @_Z1HRt, ptr @.str, ptr @.str.1, i32 48, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr @_Z1XRt, ptr @.str.2, ptr @.str.1, i32 68, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr @_Z1YRt, ptr @.str.3, ptr @.str.1, i32 88, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr @_Z1ZRt, ptr @.str.4, ptr @.str.1, i32 109, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr @_Z1SRt, ptr @.str.5, ptr @.str.1, i32 129, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr @_Z4SdagRt, ptr @.str.6, ptr @.str.1, i32 149, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr @_Z1TRt, ptr @.str.7, ptr @.str.1, i32 169, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr @_Z4TdagRt, ptr @.str.8, ptr @.str.1, i32 189, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr @_Z2RXRtd, ptr @.str.9, ptr @.str.1, i32 209, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr @_Z2RYRtd, ptr @.str.10, ptr @.str.1, i32 230, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr @_Z2RZRtd, ptr @.str.11, ptr @.str.1, i32 251, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr @_Z2CZRtS_, ptr @.str.12, ptr @.str.1, i32 272, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr @_Z4CNOTRtS_, ptr @.str.13, ptr @.str.1, i32 293, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr @_Z4SWAPRtS_, ptr @.str.14, ptr @.str.1, i32 314, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr @_Z7ToffoliRtS_S_, ptr @.str.15, ptr @.str.1, i32 335, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr @_Z5PrepXRt, ptr @.str.16, ptr @.str.1, i32 359, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr @_Z5PrepYRt, ptr @.str.17, ptr @.str.1, i32 377, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr @_Z5PrepZRt, ptr @.str.18, ptr @.str.1, i32 395, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr @_Z5MeasXRtRbb, ptr @.str.19, ptr @.str.1, i32 413, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr @_Z5MeasYRtRbb, ptr @.str.20, ptr @.str.1, i32 433, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr @_Z5MeasZRtRbb, ptr @.str.21, ptr @.str.1, i32 453, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr @_Z6CPhaseRtS_d, ptr @.str.22, ptr @.str.1, i32 481, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr @_Z3RXYRtdd, ptr @.str.23, ptr @.str.1, i32 503, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr @_Z5SwapARtS_d, ptr @.str.24, ptr @.str.1, i32 525, ptr null }], section "llvm.metadata"

; Function Attrs: mustprogress noinline uwtable
define dso_local void @_Z1HRt(ptr noundef nonnull align 2 dereferenceable(2) %q) #0 {
; CHECK-LABEL: define dso_local void @_Z1HRt
; CHECK-SAME: (ptr noundef nonnull align 2 dereferenceable(2) [[Q:%.*]]) #[[ATTR0:[0-9]+]] {
; CHECK-NEXT:  entry:
; CHECK-NEXT:    tail call void @_Z20__test_quantum_qubitPt(ptr noundef nonnull [[Q]])
; CHECK-NEXT:    ret void
;
entry:
  tail call void @_Z20__test_quantum_qubitPt(ptr noundef nonnull %q)
  ret void
}

declare void @_Z20__test_quantum_qubitPt(ptr noundef) local_unnamed_addr #1

; Function Attrs: mustprogress noinline uwtable
define dso_local void @_Z1XRt(ptr noundef nonnull align 2 dereferenceable(2) %q) #0 {
; CHECK-LABEL: define dso_local void @_Z1XRt
; CHECK-SAME: (ptr noundef nonnull align 2 dereferenceable(2) [[Q:%.*]]) #[[ATTR0]] {
; CHECK-NEXT:  entry:
; CHECK-NEXT:    tail call void @_Z20__test_quantum_qubitPt(ptr noundef nonnull [[Q]])
; CHECK-NEXT:    ret void
;
entry:
  tail call void @_Z20__test_quantum_qubitPt(ptr noundef nonnull %q)
  ret void
}

; Function Attrs: mustprogress noinline uwtable
define dso_local void @_Z1YRt(ptr noundef nonnull align 2 dereferenceable(2) %q) #0 {
; CHECK-LABEL: define dso_local void @_Z1YRt
; CHECK-SAME: (ptr noundef nonnull align 2 dereferenceable(2) [[Q:%.*]]) #[[ATTR0]] {
; CHECK-NEXT:  entry:
; CHECK-NEXT:    tail call void @_Z20__test_quantum_qubitPt(ptr noundef nonnull [[Q]])
; CHECK-NEXT:    ret void
;
entry:
  tail call void @_Z20__test_quantum_qubitPt(ptr noundef nonnull %q)
  ret void
}

; Function Attrs: mustprogress noinline uwtable
define dso_local void @_Z1ZRt(ptr noundef nonnull align 2 dereferenceable(2) %q) #0 {
; CHECK-LABEL: define dso_local void @_Z1ZRt
; CHECK-SAME: (ptr noundef nonnull align 2 dereferenceable(2) [[Q:%.*]]) #[[ATTR0]] {
; CHECK-NEXT:  entry:
; CHECK-NEXT:    tail call void @_Z20__test_quantum_qubitPt(ptr noundef nonnull [[Q]])
; CHECK-NEXT:    ret void
;
entry:
  tail call void @_Z20__test_quantum_qubitPt(ptr noundef nonnull %q)
  ret void
}

; Function Attrs: mustprogress noinline uwtable
define dso_local void @_Z1SRt(ptr noundef nonnull align 2 dereferenceable(2) %q) #0 {
; CHECK-LABEL: define dso_local void @_Z1SRt
; CHECK-SAME: (ptr noundef nonnull align 2 dereferenceable(2) [[Q:%.*]]) #[[ATTR0]] {
; CHECK-NEXT:  entry:
; CHECK-NEXT:    tail call void @_Z20__test_quantum_qubitPt(ptr noundef nonnull [[Q]])
; CHECK-NEXT:    ret void
;
entry:
  tail call void @_Z20__test_quantum_qubitPt(ptr noundef nonnull %q)
  ret void
}

; Function Attrs: mustprogress noinline uwtable
define dso_local void @_Z4SdagRt(ptr noundef nonnull align 2 dereferenceable(2) %q) #0 {
; CHECK-LABEL: define dso_local void @_Z4SdagRt
; CHECK-SAME: (ptr noundef nonnull align 2 dereferenceable(2) [[Q:%.*]]) #[[ATTR0]] {
; CHECK-NEXT:  entry:
; CHECK-NEXT:    tail call void @_Z20__test_quantum_qubitPt(ptr noundef nonnull [[Q]])
; CHECK-NEXT:    ret void
;
entry:
  tail call void @_Z20__test_quantum_qubitPt(ptr noundef nonnull %q)
  ret void
}

; Function Attrs: mustprogress noinline uwtable
define dso_local void @_Z1TRt(ptr noundef nonnull align 2 dereferenceable(2) %q) #0 {
; CHECK-LABEL: define dso_local void @_Z1TRt
; CHECK-SAME: (ptr noundef nonnull align 2 dereferenceable(2) [[Q:%.*]]) #[[ATTR0]] {
; CHECK-NEXT:  entry:
; CHECK-NEXT:    tail call void @_Z20__test_quantum_qubitPt(ptr noundef nonnull [[Q]])
; CHECK-NEXT:    ret void
;
entry:
  tail call void @_Z20__test_quantum_qubitPt(ptr noundef nonnull %q)
  ret void
}

; Function Attrs: mustprogress noinline uwtable
define dso_local void @_Z4TdagRt(ptr noundef nonnull align 2 dereferenceable(2) %q) #0 {
; CHECK-LABEL: define dso_local void @_Z4TdagRt
; CHECK-SAME: (ptr noundef nonnull align 2 dereferenceable(2) [[Q:%.*]]) #[[ATTR0]] {
; CHECK-NEXT:  entry:
; CHECK-NEXT:    tail call void @_Z20__test_quantum_qubitPt(ptr noundef nonnull [[Q]])
; CHECK-NEXT:    ret void
;
entry:
  tail call void @_Z20__test_quantum_qubitPt(ptr noundef nonnull %q)
  ret void
}

; Function Attrs: mustprogress noinline uwtable
define dso_local void @_Z2RXRtd(ptr noundef nonnull align 2 dereferenceable(2) %q, double noundef %angle) #0 {
; CHECK-LABEL: define dso_local void @_Z2RXRtd
; CHECK-SAME: (ptr noundef nonnull align 2 dereferenceable(2) [[Q:%.*]], double noundef [[ANGLE:%.*]]) #[[ATTR0]] {
; CHECK-NEXT:  entry:
; CHECK-NEXT:    tail call void @_Z20__test_quantum_qubitPt(ptr noundef nonnull [[Q]])
; CHECK-NEXT:    tail call void @_Z20__test_quantum_paramd(double noundef [[ANGLE]])
; CHECK-NEXT:    ret void
;
entry:
  tail call void @_Z20__test_quantum_qubitPt(ptr noundef nonnull %q)
  tail call void @_Z20__test_quantum_paramd(double noundef %angle)
  ret void
}

declare void @_Z20__test_quantum_paramd(double noundef) local_unnamed_addr #1

; Function Attrs: mustprogress noinline uwtable
define dso_local void @_Z2RYRtd(ptr noundef nonnull align 2 dereferenceable(2) %q, double noundef %angle) #0 {
; CHECK-LABEL: define dso_local void @_Z2RYRtd
; CHECK-SAME: (ptr noundef nonnull align 2 dereferenceable(2) [[Q:%.*]], double noundef [[ANGLE:%.*]]) #[[ATTR0]] {
; CHECK-NEXT:  entry:
; CHECK-NEXT:    tail call void @_Z20__test_quantum_qubitPt(ptr noundef nonnull [[Q]])
; CHECK-NEXT:    tail call void @_Z20__test_quantum_paramd(double noundef [[ANGLE]])
; CHECK-NEXT:    ret void
;
entry:
  tail call void @_Z20__test_quantum_qubitPt(ptr noundef nonnull %q)
  tail call void @_Z20__test_quantum_paramd(double noundef %angle)
  ret void
}

; Function Attrs: mustprogress noinline uwtable
define dso_local void @_Z2RZRtd(ptr noundef nonnull align 2 dereferenceable(2) %q, double noundef %angle) #0 {
; CHECK-LABEL: define dso_local void @_Z2RZRtd
; CHECK-SAME: (ptr noundef nonnull align 2 dereferenceable(2) [[Q:%.*]], double noundef [[ANGLE:%.*]]) #[[ATTR0]] {
; CHECK-NEXT:  entry:
; CHECK-NEXT:    tail call void @_Z20__test_quantum_qubitPt(ptr noundef nonnull [[Q]])
; CHECK-NEXT:    tail call void @_Z20__test_quantum_paramd(double noundef [[ANGLE]])
; CHECK-NEXT:    ret void
;
entry:
  tail call void @_Z20__test_quantum_qubitPt(ptr noundef nonnull %q)
  tail call void @_Z20__test_quantum_paramd(double noundef %angle)
  ret void
}

; Function Attrs: mustprogress noinline uwtable
define dso_local void @_Z2CZRtS_(ptr noundef nonnull align 2 dereferenceable(2) %ctrl, ptr noundef nonnull align 2 dereferenceable(2) %target) #0 {
; CHECK-LABEL: define dso_local void @_Z2CZRtS_
; CHECK-SAME: (ptr noundef nonnull align 2 dereferenceable(2) [[CTRL:%.*]], ptr noundef nonnull align 2 dereferenceable(2) [[TARGET:%.*]]) #[[ATTR0]] {
; CHECK-NEXT:  entry:
; CHECK-NEXT:    tail call void @_Z20__test_quantum_qubitPt(ptr noundef nonnull [[CTRL]])
; CHECK-NEXT:    tail call void @_Z20__test_quantum_qubitPt(ptr noundef nonnull [[TARGET]])
; CHECK-NEXT:    ret void
;
entry:
  tail call void @_Z20__test_quantum_qubitPt(ptr noundef nonnull %ctrl)
  tail call void @_Z20__test_quantum_qubitPt(ptr noundef nonnull %target)
  ret void
}

; Function Attrs: mustprogress noinline uwtable
define dso_local void @_Z4CNOTRtS_(ptr noundef nonnull align 2 dereferenceable(2) %ctrl, ptr noundef nonnull align 2 dereferenceable(2) %target) #0 {
; CHECK-LABEL: define dso_local void @_Z4CNOTRtS_
; CHECK-SAME: (ptr noundef nonnull align 2 dereferenceable(2) [[CTRL:%.*]], ptr noundef nonnull align 2 dereferenceable(2) [[TARGET:%.*]]) #[[ATTR0]] {
; CHECK-NEXT:  entry:
; CHECK-NEXT:    tail call void @_Z20__test_quantum_qubitPt(ptr noundef nonnull [[CTRL]])
; CHECK-NEXT:    tail call void @_Z20__test_quantum_qubitPt(ptr noundef nonnull [[TARGET]])
; CHECK-NEXT:    ret void
;
entry:
  tail call void @_Z20__test_quantum_qubitPt(ptr noundef nonnull %ctrl)
  tail call void @_Z20__test_quantum_qubitPt(ptr noundef nonnull %target)
  ret void
}

; Function Attrs: mustprogress noinline uwtable
define dso_local void @_Z4SWAPRtS_(ptr noundef nonnull align 2 dereferenceable(2) %ctrl, ptr noundef nonnull align 2 dereferenceable(2) %target) #0 {
; CHECK-LABEL: define dso_local void @_Z4SWAPRtS_
; CHECK-SAME: (ptr noundef nonnull align 2 dereferenceable(2) [[CTRL:%.*]], ptr noundef nonnull align 2 dereferenceable(2) [[TARGET:%.*]]) #[[ATTR0]] {
; CHECK-NEXT:  entry:
; CHECK-NEXT:    tail call void @_Z20__test_quantum_qubitPt(ptr noundef nonnull [[CTRL]])
; CHECK-NEXT:    tail call void @_Z20__test_quantum_qubitPt(ptr noundef nonnull [[TARGET]])
; CHECK-NEXT:    ret void
;
entry:
  tail call void @_Z20__test_quantum_qubitPt(ptr noundef nonnull %ctrl)
  tail call void @_Z20__test_quantum_qubitPt(ptr noundef nonnull %target)
  ret void
}

; Function Attrs: mustprogress noinline uwtable
define dso_local void @_Z7ToffoliRtS_S_(ptr noundef nonnull align 2 dereferenceable(2) %ctrl0, ptr noundef nonnull align 2 dereferenceable(2) %ctrl1, ptr noundef nonnull align 2 dereferenceable(2) %target) #0 {
; CHECK-LABEL: define dso_local void @_Z7ToffoliRtS_S_
; CHECK-SAME: (ptr noundef nonnull align 2 dereferenceable(2) [[CTRL0:%.*]], ptr noundef nonnull align 2 dereferenceable(2) [[CTRL1:%.*]], ptr noundef nonnull align 2 dereferenceable(2) [[TARGET:%.*]]) #[[ATTR0]] {
; CHECK-NEXT:  entry:
; CHECK-NEXT:    tail call void @_Z20__test_quantum_qubitPt(ptr noundef nonnull [[CTRL0]])
; CHECK-NEXT:    tail call void @_Z20__test_quantum_qubitPt(ptr noundef nonnull [[CTRL1]])
; CHECK-NEXT:    tail call void @_Z20__test_quantum_qubitPt(ptr noundef nonnull [[TARGET]])
; CHECK-NEXT:    ret void
;
entry:
  tail call void @_Z20__test_quantum_qubitPt(ptr noundef nonnull %ctrl0)
  tail call void @_Z20__test_quantum_qubitPt(ptr noundef nonnull %ctrl1)
  tail call void @_Z20__test_quantum_qubitPt(ptr noundef nonnull %target)
  ret void
}

; Function Attrs: mustprogress noinline uwtable
define dso_local void @_Z5PrepXRt(ptr noundef nonnull align 2 dereferenceable(2) %q) #0 {
; CHECK-LABEL: define dso_local void @_Z5PrepXRt
; CHECK-SAME: (ptr noundef nonnull align 2 dereferenceable(2) [[Q:%.*]]) #[[ATTR0]] {
; CHECK-NEXT:  entry:
; CHECK-NEXT:    tail call void @_Z20__test_quantum_qubitPt(ptr noundef nonnull [[Q]])
; CHECK-NEXT:    ret void
;
entry:
  tail call void @_Z20__test_quantum_qubitPt(ptr noundef nonnull %q)
  ret void
}

; Function Attrs: mustprogress noinline uwtable
define dso_local void @_Z5PrepYRt(ptr noundef nonnull align 2 dereferenceable(2) %q) #0 {
; CHECK-LABEL: define dso_local void @_Z5PrepYRt
; CHECK-SAME: (ptr noundef nonnull align 2 dereferenceable(2) [[Q:%.*]]) #[[ATTR0]] {
; CHECK-NEXT:  entry:
; CHECK-NEXT:    tail call void @_Z20__test_quantum_qubitPt(ptr noundef nonnull [[Q]])
; CHECK-NEXT:    ret void
;
entry:
  tail call void @_Z20__test_quantum_qubitPt(ptr noundef nonnull %q)
  ret void
}

; Function Attrs: mustprogress noinline uwtable
define dso_local void @_Z5PrepZRt(ptr noundef nonnull align 2 dereferenceable(2) %q) #0 {
; CHECK-LABEL: define dso_local void @_Z5PrepZRt
; CHECK-SAME: (ptr noundef nonnull align 2 dereferenceable(2) [[Q:%.*]]) #[[ATTR0]] {
; CHECK-NEXT:  entry:
; CHECK-NEXT:    tail call void @_Z20__test_quantum_qubitPt(ptr noundef nonnull [[Q]])
; CHECK-NEXT:    ret void
;
entry:
  tail call void @_Z20__test_quantum_qubitPt(ptr noundef nonnull %q)
  ret void
}

; Function Attrs: mustprogress noinline uwtable
define dso_local void @_Z5MeasXRtRbb(ptr noundef nonnull align 2 dereferenceable(2) %q, ptr noundef nonnull align 1 dereferenceable(1) %c, i1 noundef zeroext %flip) #0 {
; CHECK-LABEL: define dso_local void @_Z5MeasXRtRbb
; CHECK-SAME: (ptr noundef nonnull align 2 dereferenceable(2) [[Q:%.*]], ptr noundef nonnull align 1 dereferenceable(1) [[C:%.*]], i1 noundef zeroext [[FLIP:%.*]]) #[[ATTR0]] {
; CHECK-NEXT:  entry:
; CHECK-NEXT:    tail call void @_Z20__test_quantum_qubitPt(ptr noundef nonnull [[Q]])
; CHECK-NEXT:    tail call void @_Z19__test_quantum_cbitPb(ptr noundef nonnull [[C]])
; CHECK-NEXT:    [[CONV:%.*]] = uitofp i1 [[FLIP]] to double
; CHECK-NEXT:    tail call void @_Z20__test_quantum_paramd(double noundef [[CONV]])
; CHECK-NEXT:    ret void
;
entry:
  tail call void @_Z20__test_quantum_qubitPt(ptr noundef nonnull %q)
  tail call void @_Z19__test_quantum_cbitPb(ptr noundef nonnull %c)
  %conv = uitofp i1 %flip to double
  tail call void @_Z20__test_quantum_paramd(double noundef %conv)
  ret void
}

declare void @_Z19__test_quantum_cbitPb(ptr noundef) local_unnamed_addr #1

; Function Attrs: mustprogress noinline uwtable
define dso_local void @_Z5MeasYRtRbb(ptr noundef nonnull align 2 dereferenceable(2) %q, ptr noundef nonnull align 1 dereferenceable(1) %c, i1 noundef zeroext %flip) #0 {
; CHECK-LABEL: define dso_local void @_Z5MeasYRtRbb
; CHECK-SAME: (ptr noundef nonnull align 2 dereferenceable(2) [[Q:%.*]], ptr noundef nonnull align 1 dereferenceable(1) [[C:%.*]], i1 noundef zeroext [[FLIP:%.*]]) #[[ATTR0]] {
; CHECK-NEXT:  entry:
; CHECK-NEXT:    tail call void @_Z20__test_quantum_qubitPt(ptr noundef nonnull [[Q]])
; CHECK-NEXT:    tail call void @_Z19__test_quantum_cbitPb(ptr noundef nonnull [[C]])
; CHECK-NEXT:    [[CONV:%.*]] = uitofp i1 [[FLIP]] to double
; CHECK-NEXT:    tail call void @_Z20__test_quantum_paramd(double noundef [[CONV]])
; CHECK-NEXT:    ret void
;
entry:
  tail call void @_Z20__test_quantum_qubitPt(ptr noundef nonnull %q)
  tail call void @_Z19__test_quantum_cbitPb(ptr noundef nonnull %c)
  %conv = uitofp i1 %flip to double
  tail call void @_Z20__test_quantum_paramd(double noundef %conv)
  ret void
}

; Function Attrs: mustprogress noinline uwtable
define dso_local void @_Z5MeasZRtRbb(ptr noundef nonnull align 2 dereferenceable(2) %q, ptr noundef nonnull align 1 dereferenceable(1) %c, i1 noundef zeroext %flip) #0 {
; CHECK-LABEL: define dso_local void @_Z5MeasZRtRbb
; CHECK-SAME: (ptr noundef nonnull align 2 dereferenceable(2) [[Q:%.*]], ptr noundef nonnull align 1 dereferenceable(1) [[C:%.*]], i1 noundef zeroext [[FLIP:%.*]]) #[[ATTR0]] {
; CHECK-NEXT:  entry:
; CHECK-NEXT:    tail call void @_Z20__test_quantum_qubitPt(ptr noundef nonnull [[Q]])
; CHECK-NEXT:    tail call void @_Z19__test_quantum_cbitPb(ptr noundef nonnull [[C]])
; CHECK-NEXT:    [[CONV:%.*]] = uitofp i1 [[FLIP]] to double
; CHECK-NEXT:    tail call void @_Z20__test_quantum_paramd(double noundef [[CONV]])
; CHECK-NEXT:    ret void
;
entry:
  tail call void @_Z20__test_quantum_qubitPt(ptr noundef nonnull %q)
  tail call void @_Z19__test_quantum_cbitPb(ptr noundef nonnull %c)
  %conv = uitofp i1 %flip to double
  tail call void @_Z20__test_quantum_paramd(double noundef %conv)
  ret void
}

; Function Attrs: mustprogress noinline uwtable
define dso_local void @_Z6CPhaseRtS_d(ptr noundef nonnull align 2 dereferenceable(2) %ctrl, ptr noundef nonnull align 2 dereferenceable(2) %target, double noundef %angle) #0 {
; CHECK-LABEL: define dso_local void @_Z6CPhaseRtS_d
; CHECK-SAME: (ptr noundef nonnull align 2 dereferenceable(2) [[CTRL:%.*]], ptr noundef nonnull align 2 dereferenceable(2) [[TARGET:%.*]], double noundef [[ANGLE:%.*]]) #[[ATTR0]] {
; CHECK-NEXT:  entry:
; CHECK-NEXT:    tail call void @_Z20__test_quantum_qubitPt(ptr noundef nonnull [[CTRL]])
; CHECK-NEXT:    tail call void @_Z20__test_quantum_qubitPt(ptr noundef nonnull [[TARGET]])
; CHECK-NEXT:    tail call void @_Z20__test_quantum_paramd(double noundef [[ANGLE]])
; CHECK-NEXT:    ret void
;
entry:
  tail call void @_Z20__test_quantum_qubitPt(ptr noundef nonnull %ctrl)
  tail call void @_Z20__test_quantum_qubitPt(ptr noundef nonnull %target)
  tail call void @_Z20__test_quantum_paramd(double noundef %angle)
  ret void
}

; Function Attrs: mustprogress noinline uwtable
define dso_local void @_Z3RXYRtdd(ptr noundef nonnull align 2 dereferenceable(2) %q, double noundef %theta, double noundef %phi) #0 {
; CHECK-LABEL: define dso_local void @_Z3RXYRtdd
; CHECK-SAME: (ptr noundef nonnull align 2 dereferenceable(2) [[Q:%.*]], double noundef [[THETA:%.*]], double noundef [[PHI:%.*]]) #[[ATTR0]] {
; CHECK-NEXT:  entry:
; CHECK-NEXT:    tail call void @_Z20__test_quantum_qubitPt(ptr noundef nonnull [[Q]])
; CHECK-NEXT:    tail call void @_Z20__test_quantum_paramd(double noundef [[THETA]])
; CHECK-NEXT:    tail call void @_Z20__test_quantum_paramd(double noundef [[PHI]])
; CHECK-NEXT:    ret void
;
entry:
  tail call void @_Z20__test_quantum_qubitPt(ptr noundef nonnull %q)
  tail call void @_Z20__test_quantum_paramd(double noundef %theta)
  tail call void @_Z20__test_quantum_paramd(double noundef %phi)
  ret void
}

; Function Attrs: mustprogress noinline uwtable
define dso_local void @_Z5SwapARtS_d(ptr noundef nonnull align 2 dereferenceable(2) %q1, ptr noundef nonnull align 2 dereferenceable(2) %q2, double noundef %angle) #0 {
; CHECK-LABEL: define dso_local void @_Z5SwapARtS_d
; CHECK-SAME: (ptr noundef nonnull align 2 dereferenceable(2) [[Q1:%.*]], ptr noundef nonnull align 2 dereferenceable(2) [[Q2:%.*]], double noundef [[ANGLE:%.*]]) #[[ATTR0]] {
; CHECK-NEXT:  entry:
; CHECK-NEXT:    tail call void @_Z20__test_quantum_qubitPt(ptr noundef nonnull [[Q1]])
; CHECK-NEXT:    tail call void @_Z20__test_quantum_qubitPt(ptr noundef nonnull [[Q2]])
; CHECK-NEXT:    tail call void @_Z20__test_quantum_paramd(double noundef [[ANGLE]])
; CHECK-NEXT:    ret void
;
entry:
  tail call void @_Z20__test_quantum_qubitPt(ptr noundef nonnull %q1)
  tail call void @_Z20__test_quantum_qubitPt(ptr noundef nonnull %q2)
  tail call void @_Z20__test_quantum_paramd(double noundef %angle)
  ret void
}

; Function Attrs: mustprogress noinline uwtable
define dso_local void @_Z10nestedLoopv() local_unnamed_addr #0 section ".qbbs_text" {
; CHECK-LABEL: define dso_local void @_Z10nestedLoopv
; CHECK-SAME: () local_unnamed_addr #[[ATTR0]] section ".qbbs_text" {
; CHECK-NEXT:  entry:
; CHECK-NEXT:    br label [[FOR_BODY:%.*]]
; CHECK:       for.cond.loopexit:
; CHECK-NEXT:    [[INDVARS_IV_NEXT:%.*]] = add nuw nsw i64 [[INDVARS_IV:%.*]], 1
; CHECK-NEXT:    [[EXITCOND24_NOT:%.*]] = icmp eq i64 [[INDVARS_IV_NEXT22:%.*]], 5
; CHECK-NEXT:    br i1 [[EXITCOND24_NOT]], label [[FOR_COND_CLEANUP:%.*]], label [[FOR_BODY]], !llvm.loop [[LOOP0:![0-9]+]]
; CHECK:       for.cond.cleanup:
; CHECK-NEXT:    ret void
; CHECK:       for.body:
; CHECK-NEXT:    [[INDVARS_IV21:%.*]] = phi i64 [ 0, [[ENTRY:%.*]] ], [ [[INDVARS_IV_NEXT22]], [[FOR_COND_LOOPEXIT:%.*]] ]
; CHECK-NEXT:    [[INDVARS_IV]] = phi i64 [ 1, [[ENTRY]] ], [ [[INDVARS_IV_NEXT]], [[FOR_COND_LOOPEXIT]] ]
; CHECK-NEXT:    [[INDVARS_IV_NEXT22]] = add nuw nsw i64 [[INDVARS_IV21]], 1
; CHECK-NEXT:    [[CMP215:%.*]] = icmp ult i64 [[INDVARS_IV21]], 4
; CHECK-NEXT:    br i1 [[CMP215]], label [[FOR_BODY4_LR_PH:%.*]], label [[FOR_COND_LOOPEXIT]]
; CHECK:       for.body4.lr.ph:
; CHECK-NEXT:    [[ARRAYIDX:%.*]] = getelementptr inbounds [5 x i16], ptr @q, i64 0, i64 [[INDVARS_IV21]]
; CHECK-NEXT:    br label [[FOR_BODY4:%.*]]
; CHECK:       for.body4:
; CHECK-NEXT:    [[INDVARS_IV18:%.*]] = phi i64 [ [[INDVARS_IV]], [[FOR_BODY4_LR_PH]] ], [ [[INDVARS_IV_NEXT19:%.*]], [[FOR_BODY4]] ]
; CHECK-NEXT:    [[ARRAYIDX6:%.*]] = getelementptr inbounds [5 x i16], ptr @q, i64 0, i64 [[INDVARS_IV18]]
; CHECK-NEXT:    tail call void @_Z4CNOTRtS_(ptr noundef nonnull align 2 dereferenceable(2) [[ARRAYIDX]], ptr noundef nonnull align 2 dereferenceable(2) [[ARRAYIDX6]])
; CHECK-NEXT:    [[INDVARS_IV_NEXT19]] = add nuw nsw i64 [[INDVARS_IV18]], 1
; CHECK-NEXT:    [[EXITCOND_NOT:%.*]] = icmp eq i64 [[INDVARS_IV_NEXT19]], 5
; CHECK-NEXT:    br i1 [[EXITCOND_NOT]], label [[FOR_COND_LOOPEXIT]], label [[FOR_BODY4]], !llvm.loop [[LOOP3:![0-9]+]]
;
entry:
  br label %for.body

for.cond.loopexit:                                ; preds = %for.body4, %for.body
  %indvars.iv.next = add nuw nsw i64 %indvars.iv, 1
  %exitcond24.not = icmp eq i64 %indvars.iv.next22, 5
  br i1 %exitcond24.not, label %for.cond.cleanup, label %for.body, !llvm.loop !0

for.cond.cleanup:                                 ; preds = %for.cond.loopexit
  ret void

for.body:                                         ; preds = %for.cond.loopexit, %entry
  %indvars.iv21 = phi i64 [ 0, %entry ], [ %indvars.iv.next22, %for.cond.loopexit ]
  %indvars.iv = phi i64 [ 1, %entry ], [ %indvars.iv.next, %for.cond.loopexit ]
  %indvars.iv.next22 = add nuw nsw i64 %indvars.iv21, 1
  %cmp215 = icmp ult i64 %indvars.iv21, 4
  br i1 %cmp215, label %for.body4.lr.ph, label %for.cond.loopexit

for.body4.lr.ph:                                  ; preds = %for.body
  %arrayidx = getelementptr inbounds [5 x i16], ptr @q, i64 0, i64 %indvars.iv21
  br label %for.body4

for.body4:                                        ; preds = %for.body4, %for.body4.lr.ph
  %indvars.iv18 = phi i64 [ %indvars.iv, %for.body4.lr.ph ], [ %indvars.iv.next19, %for.body4 ]
  %arrayidx6 = getelementptr inbounds [5 x i16], ptr @q, i64 0, i64 %indvars.iv18
  tail call void @_Z4CNOTRtS_(ptr noundef nonnull align 2 dereferenceable(2) %arrayidx, ptr noundef nonnull align 2 dereferenceable(2) %arrayidx6)
  %indvars.iv.next19 = add nuw nsw i64 %indvars.iv18, 1
  %exitcond.not = icmp eq i64 %indvars.iv.next19, 5
  br i1 %exitcond.not, label %for.cond.loopexit, label %for.body4, !llvm.loop !3
}

attributes #0 = { mustprogress noinline uwtable "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #1 = { "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }

!0 = distinct !{!0, !1, !2}
!1 = !{!"llvm.loop.mustprogress"}
!2 = !{!"llvm.loop.unroll.disable"}
!3 = distinct !{!3, !1, !2}
