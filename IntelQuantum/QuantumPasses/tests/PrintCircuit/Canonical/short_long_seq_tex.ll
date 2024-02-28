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
@q = dso_local local_unnamed_addr global [3 x i16] zeroinitializer, align 2
@cat = dso_local global [5 x i16] zeroinitializer, align 2
@basil = dso_local global [5 x i16] zeroinitializer, align 2
@dog = dso_local global [35 x i16] zeroinitializer, align 16
@c = dso_local local_unnamed_addr global [3 x i8] zeroinitializer, align 1
@llvm.global.annotations = appending global [24 x { ptr, ptr, ptr, i32, ptr }] [{ ptr, ptr, ptr, i32, ptr } { ptr @_Z1HRt, ptr @.str, ptr @.str.1, i32 48, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr @_Z1XRt, ptr @.str.2, ptr @.str.1, i32 68, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr @_Z1YRt, ptr @.str.3, ptr @.str.1, i32 88, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr @_Z1ZRt, ptr @.str.4, ptr @.str.1, i32 109, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr @_Z1SRt, ptr @.str.5, ptr @.str.1, i32 129, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr @_Z4SdagRt, ptr @.str.6, ptr @.str.1, i32 149, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr @_Z1TRt, ptr @.str.7, ptr @.str.1, i32 169, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr @_Z4TdagRt, ptr @.str.8, ptr @.str.1, i32 189, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr @_Z2RXRtd, ptr @.str.9, ptr @.str.1, i32 209, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr @_Z2RYRtd, ptr @.str.10, ptr @.str.1, i32 230, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr @_Z2RZRtd, ptr @.str.11, ptr @.str.1, i32 251, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr @_Z2CZRtS_, ptr @.str.12, ptr @.str.1, i32 272, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr @_Z4CNOTRtS_, ptr @.str.13, ptr @.str.1, i32 293, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr @_Z4SWAPRtS_, ptr @.str.14, ptr @.str.1, i32 314, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr @_Z7ToffoliRtS_S_, ptr @.str.15, ptr @.str.1, i32 335, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr @_Z5PrepXRt, ptr @.str.16, ptr @.str.1, i32 359, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr @_Z5PrepYRt, ptr @.str.17, ptr @.str.1, i32 377, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr @_Z5PrepZRt, ptr @.str.18, ptr @.str.1, i32 395, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr @_Z5MeasXRtRbb, ptr @.str.19, ptr @.str.1, i32 413, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr @_Z5MeasYRtRbb, ptr @.str.20, ptr @.str.1, i32 433, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr @_Z5MeasZRtRbb, ptr @.str.21, ptr @.str.1, i32 453, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr @_Z6CPhaseRtS_d, ptr @.str.22, ptr @.str.1, i32 481, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr @_Z3RXYRtdd, ptr @.str.23, ptr @.str.1, i32 503, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr @_Z5SwapARtS_d, ptr @.str.24, ptr @.str.1, i32 525, ptr null }], section "llvm.metadata"

; Function Attrs: mustprogress noinline nounwind uwtable
define dso_local void @_Z1HRt(ptr noundef nonnull align 2 dereferenceable(2) "qubit_ref" %q) #0 {
entry:
  tail call void @llvm.quantum.qubit(ptr noundef nonnull align 2 dereferenceable(2) %q)
  ret void
}

; Function Attrs: mustprogress noinline nounwind uwtable
define dso_local void @_Z1XRt(ptr noundef nonnull align 2 dereferenceable(2) "qubit_ref" %q) #0 {
entry:
  tail call void @llvm.quantum.qubit(ptr noundef nonnull align 2 dereferenceable(2) %q)
  ret void
}

; Function Attrs: mustprogress noinline nounwind uwtable
define dso_local void @_Z1YRt(ptr noundef nonnull align 2 dereferenceable(2) "qubit_ref" %q) #0 {
entry:
  tail call void @llvm.quantum.qubit(ptr noundef nonnull align 2 dereferenceable(2) %q)
  ret void
}

; Function Attrs: mustprogress noinline nounwind uwtable
define dso_local void @_Z1ZRt(ptr noundef nonnull align 2 dereferenceable(2) "qubit_ref" %q) #0 {
entry:
  tail call void @llvm.quantum.qubit(ptr noundef nonnull align 2 dereferenceable(2) %q)
  ret void
}

; Function Attrs: mustprogress noinline nounwind uwtable
define dso_local void @_Z1SRt(ptr noundef nonnull align 2 dereferenceable(2) "qubit_ref" %q) #0 {
entry:
  tail call void @llvm.quantum.qubit(ptr noundef nonnull align 2 dereferenceable(2) %q)
  ret void
}

; Function Attrs: mustprogress noinline nounwind uwtable
define dso_local void @_Z4SdagRt(ptr noundef nonnull align 2 dereferenceable(2) "qubit_ref" %q) #0 {
entry:
  tail call void @llvm.quantum.qubit(ptr noundef nonnull align 2 dereferenceable(2) %q)
  ret void
}

; Function Attrs: mustprogress noinline nounwind uwtable
define dso_local void @_Z1TRt(ptr noundef nonnull align 2 dereferenceable(2) "qubit_ref" %q) #0 {
entry:
  tail call void @llvm.quantum.qubit(ptr noundef nonnull align 2 dereferenceable(2) %q)
  ret void
}

; Function Attrs: mustprogress noinline nounwind uwtable
define dso_local void @_Z4TdagRt(ptr noundef nonnull align 2 dereferenceable(2) "qubit_ref" %q) #0 {
entry:
  tail call void @llvm.quantum.qubit(ptr noundef nonnull align 2 dereferenceable(2) %q)
  ret void
}

; Function Attrs: mustprogress noinline nounwind uwtable
define dso_local void @_Z2RXRtd(ptr noundef nonnull align 2 dereferenceable(2) "qubit_ref" %q, double noundef %angle) #0 {
entry:
  tail call void @llvm.quantum.qubit(ptr noundef nonnull align 2 dereferenceable(2) %q)
  tail call void @llvm.quantum.param(double noundef %angle)
  ret void
}

; Function Attrs: mustprogress noinline nounwind uwtable
define dso_local void @_Z2RYRtd(ptr noundef nonnull align 2 dereferenceable(2) "qubit_ref" %q, double noundef %angle) #0 {
entry:
  tail call void @llvm.quantum.qubit(ptr noundef nonnull align 2 dereferenceable(2) %q)
  tail call void @llvm.quantum.param(double noundef %angle)
  ret void
}

; Function Attrs: mustprogress noinline nounwind uwtable
define dso_local void @_Z2RZRtd(ptr noundef nonnull align 2 dereferenceable(2) "qubit_ref" %q, double noundef %angle) #0 {
entry:
  tail call void @llvm.quantum.qubit(ptr noundef nonnull align 2 dereferenceable(2) %q)
  tail call void @llvm.quantum.param(double noundef %angle)
  ret void
}

; Function Attrs: mustprogress noinline nounwind uwtable
define dso_local void @_Z2CZRtS_(ptr noundef nonnull align 2 dereferenceable(2) "qubit_ref" %ctrl, ptr noundef nonnull align 2 dereferenceable(2) "qubit_ref" %target) #0 {
entry:
  tail call void @llvm.quantum.qubit(ptr noundef nonnull align 2 dereferenceable(2) %ctrl)
  tail call void @llvm.quantum.qubit(ptr noundef nonnull %target)
  ret void
}

; Function Attrs: mustprogress noinline nounwind uwtable
define dso_local void @_Z4CNOTRtS_(ptr noundef nonnull align 2 dereferenceable(2) "qubit_ref" %ctrl, ptr noundef nonnull align 2 dereferenceable(2) "qubit_ref" %target) #0 {
entry:
  tail call void @llvm.quantum.qubit(ptr noundef nonnull align 2 dereferenceable(2) %ctrl)
  tail call void @llvm.quantum.qubit(ptr noundef nonnull %target)
  ret void
}

; Function Attrs: mustprogress noinline nounwind uwtable
define dso_local void @_Z4SWAPRtS_(ptr noundef nonnull align 2 dereferenceable(2) "qubit_ref" %ctrl, ptr noundef nonnull align 2 dereferenceable(2) "qubit_ref" %target) #0 {
entry:
  tail call void @llvm.quantum.qubit(ptr noundef nonnull align 2 dereferenceable(2) %ctrl)
  tail call void @llvm.quantum.qubit(ptr noundef nonnull %target)
  ret void
}

; Function Attrs: mustprogress noinline nounwind uwtable
define dso_local void @_Z7ToffoliRtS_S_(ptr noundef nonnull align 2 dereferenceable(2) "qubit_ref" %ctrl0, ptr noundef nonnull align 2 dereferenceable(2) "qubit_ref" %ctrl1, ptr noundef nonnull align 2 dereferenceable(2) "qubit_ref" %target) #0 {
entry:
  tail call void @llvm.quantum.qubit(ptr noundef nonnull align 2 dereferenceable(2) %ctrl0)
  tail call void @llvm.quantum.qubit(ptr noundef nonnull %ctrl1)
  tail call void @llvm.quantum.qubit(ptr noundef nonnull %target)
  ret void
}

; Function Attrs: mustprogress noinline nounwind uwtable
define dso_local void @_Z5PrepXRt(ptr noundef nonnull align 2 dereferenceable(2) "qubit_ref" %q) #0 {
entry:
  tail call void @llvm.quantum.qubit(ptr noundef nonnull align 2 dereferenceable(2) %q)
  ret void
}

; Function Attrs: mustprogress noinline nounwind uwtable
define dso_local void @_Z5PrepYRt(ptr noundef nonnull align 2 dereferenceable(2) "qubit_ref" %q) #0 {
entry:
  tail call void @llvm.quantum.qubit(ptr noundef nonnull align 2 dereferenceable(2) %q)
  ret void
}

; Function Attrs: mustprogress noinline nounwind uwtable
define dso_local void @_Z5PrepZRt(ptr noundef nonnull align 2 dereferenceable(2) "qubit_ref" %q) #0 {
entry:
  tail call void @llvm.quantum.qubit(ptr noundef nonnull align 2 dereferenceable(2) %q)
  ret void
}

; Function Attrs: mustprogress noinline nounwind uwtable
define dso_local void @_Z5MeasXRtRbb(ptr noundef nonnull align 2 dereferenceable(2) "qubit_ref" %q, ptr noundef nonnull align 1 dereferenceable(1) "cbit_ref" %c, i1 noundef zeroext %flip) #0 {
entry:
  tail call void @llvm.quantum.qubit(ptr noundef nonnull align 2 dereferenceable(2) %q)
  tail call void @llvm.quantum.cbit(ptr noundef nonnull %c)
  %conv = uitofp i1 %flip to double
  tail call void @llvm.quantum.param(double noundef %conv)
  ret void
}

; Function Attrs: mustprogress noinline nounwind uwtable
define dso_local void @_Z5MeasYRtRbb(ptr noundef nonnull align 2 dereferenceable(2) "qubit_ref" %q, ptr noundef nonnull align 1 dereferenceable(1) "cbit_ref" %c, i1 noundef zeroext %flip) #0 {
entry:
  tail call void @llvm.quantum.qubit(ptr noundef nonnull align 2 dereferenceable(2) %q)
  tail call void @llvm.quantum.cbit(ptr noundef nonnull %c)
  %conv = uitofp i1 %flip to double
  tail call void @llvm.quantum.param(double noundef %conv)
  ret void
}

; Function Attrs: mustprogress noinline nounwind uwtable
define dso_local void @_Z5MeasZRtRbb(ptr noundef nonnull align 2 dereferenceable(2) "qubit_ref" %q, ptr noundef nonnull align 1 dereferenceable(1) "cbit_ref" %c, i1 noundef zeroext %flip) #0 {
entry:
  tail call void @llvm.quantum.qubit(ptr noundef nonnull align 2 dereferenceable(2) %q)
  tail call void @llvm.quantum.cbit(ptr noundef nonnull %c)
  %conv = uitofp i1 %flip to double
  tail call void @llvm.quantum.param(double noundef %conv)
  ret void
}

; Function Attrs: mustprogress noinline nounwind uwtable
define dso_local void @_Z6CPhaseRtS_d(ptr noundef nonnull align 2 dereferenceable(2) "qubit_ref" %ctrl, ptr noundef nonnull align 2 dereferenceable(2) "qubit_ref" %target, double noundef %angle) #0 {
entry:
  tail call void @llvm.quantum.qubit(ptr noundef nonnull align 2 dereferenceable(2) %ctrl)
  tail call void @llvm.quantum.qubit(ptr noundef nonnull %target)
  tail call void @llvm.quantum.param(double noundef %angle)
  ret void
}

; Function Attrs: mustprogress noinline nounwind uwtable
define dso_local void @_Z3RXYRtdd(ptr noundef nonnull align 2 dereferenceable(2) "qubit_ref" %q, double noundef %theta, double noundef %phi) #0 {
entry:
  tail call void @llvm.quantum.qubit(ptr noundef nonnull align 2 dereferenceable(2) %q)
  tail call void @llvm.quantum.param(double noundef %theta)
  tail call void @llvm.quantum.param(double noundef %phi)
  ret void
}

; Function Attrs: mustprogress noinline nounwind uwtable
define dso_local void @_Z5SwapARtS_d(ptr noundef nonnull align 2 dereferenceable(2) "qubit_ref" %q1, ptr noundef nonnull align 2 dereferenceable(2) "qubit_ref" %q2, double noundef %angle) #0 {
entry:
  tail call void @llvm.quantum.qubit(ptr noundef nonnull align 2 dereferenceable(2) %q1)
  tail call void @llvm.quantum.qubit(ptr noundef nonnull %q2)
  tail call void @llvm.quantum.param(double noundef %angle)
  ret void
}

; Function Attrs: mustprogress noinline nounwind uwtable
define dso_local void @_Z15multiQubitGatesv.stub() local_unnamed_addr #0 section ".qbbs_text" {
entry:
  br label %aqcc.quantum

aqcc.quantum:                                     ; preds = %entry
  %arrayidx89 = getelementptr inbounds [5 x i16], ptr @basil, i64 0, i64 0
  %arrayidx90 = getelementptr inbounds [5 x i16], ptr @cat, i64 0, i64 1
  call void @_Z4CNOTRtS_(ptr %arrayidx89, ptr %arrayidx90)
  %arrayidx87 = getelementptr inbounds [5 x i16], ptr @cat, i64 0, i64 1
  %arrayidx88 = getelementptr inbounds [5 x i16], ptr @cat, i64 0, i64 0
  call void @_Z2CZRtS_(ptr %arrayidx87, ptr %arrayidx88)
  %arrayidx84 = getelementptr inbounds [5 x i16], ptr @basil, i64 0, i64 0
  %arrayidx85 = getelementptr inbounds [5 x i16], ptr @cat, i64 0, i64 1
  %arrayidx86 = getelementptr inbounds [5 x i16], ptr @cat, i64 0, i64 2
  call void @_Z7ToffoliRtS_S_(ptr %arrayidx84, ptr %arrayidx85, ptr %arrayidx86)
  %arrayidx82 = getelementptr inbounds [5 x i16], ptr @basil, i64 0, i64 1
  %arrayidx83 = getelementptr inbounds [5 x i16], ptr @basil, i64 0, i64 2
  call void @_Z4SWAPRtS_(ptr %arrayidx82, ptr %arrayidx83)
  %arrayidx80 = getelementptr inbounds [5 x i16], ptr @cat, i64 0, i64 0
  %arrayidx81 = getelementptr inbounds [5 x i16], ptr @basil, i64 0, i64 1
  call void @_Z6CPhaseRtS_d(ptr %arrayidx80, ptr %arrayidx81, double 1.500000e+00)
  %arrayidx78 = getelementptr inbounds [5 x i16], ptr @cat, i64 0, i64 1
  %arrayidx79 = getelementptr inbounds [5 x i16], ptr @basil, i64 0, i64 0
  call void @_Z5SwapARtS_d(ptr %arrayidx78, ptr %arrayidx79, double 1.200000e+00)
  %arrayidx76 = getelementptr inbounds [5 x i16], ptr @basil, i64 0, i64 0
  %arrayidx77 = getelementptr inbounds [5 x i16], ptr @cat, i64 0, i64 1
  call void @_Z4CNOTRtS_(ptr %arrayidx76, ptr %arrayidx77)
  %arrayidx74 = getelementptr inbounds [5 x i16], ptr @cat, i64 0, i64 1
  %arrayidx75 = getelementptr inbounds [5 x i16], ptr @cat, i64 0, i64 0
  call void @_Z2CZRtS_(ptr %arrayidx74, ptr %arrayidx75)
  %arrayidx71 = getelementptr inbounds [5 x i16], ptr @basil, i64 0, i64 0
  %arrayidx72 = getelementptr inbounds [5 x i16], ptr @cat, i64 0, i64 1
  %arrayidx73 = getelementptr inbounds [5 x i16], ptr @cat, i64 0, i64 2
  call void @_Z7ToffoliRtS_S_(ptr %arrayidx71, ptr %arrayidx72, ptr %arrayidx73)
  %arrayidx69 = getelementptr inbounds [5 x i16], ptr @basil, i64 0, i64 1
  %arrayidx70 = getelementptr inbounds [5 x i16], ptr @basil, i64 0, i64 2
  call void @_Z4SWAPRtS_(ptr %arrayidx69, ptr %arrayidx70)
  %arrayidx67 = getelementptr inbounds [5 x i16], ptr @cat, i64 0, i64 0
  %arrayidx68 = getelementptr inbounds [5 x i16], ptr @basil, i64 0, i64 1
  call void @_Z6CPhaseRtS_d(ptr %arrayidx67, ptr %arrayidx68, double 1.500000e+00)
  %arrayidx65 = getelementptr inbounds [5 x i16], ptr @cat, i64 0, i64 1
  %arrayidx66 = getelementptr inbounds [5 x i16], ptr @basil, i64 0, i64 0
  call void @_Z5SwapARtS_d(ptr %arrayidx65, ptr %arrayidx66, double 1.200000e+00)
  %arrayidx63 = getelementptr inbounds [5 x i16], ptr @basil, i64 0, i64 0
  %arrayidx64 = getelementptr inbounds [5 x i16], ptr @cat, i64 0, i64 1
  call void @_Z4CNOTRtS_(ptr %arrayidx63, ptr %arrayidx64)
  %arrayidx61 = getelementptr inbounds [5 x i16], ptr @cat, i64 0, i64 1
  %arrayidx62 = getelementptr inbounds [5 x i16], ptr @cat, i64 0, i64 0
  call void @_Z2CZRtS_(ptr %arrayidx61, ptr %arrayidx62)
  %arrayidx58 = getelementptr inbounds [5 x i16], ptr @basil, i64 0, i64 0
  %arrayidx59 = getelementptr inbounds [5 x i16], ptr @cat, i64 0, i64 1
  %arrayidx60 = getelementptr inbounds [5 x i16], ptr @cat, i64 0, i64 2
  call void @_Z7ToffoliRtS_S_(ptr %arrayidx58, ptr %arrayidx59, ptr %arrayidx60)
  %arrayidx56 = getelementptr inbounds [5 x i16], ptr @basil, i64 0, i64 1
  %arrayidx57 = getelementptr inbounds [5 x i16], ptr @basil, i64 0, i64 2
  call void @_Z4SWAPRtS_(ptr %arrayidx56, ptr %arrayidx57)
  %arrayidx54 = getelementptr inbounds [5 x i16], ptr @cat, i64 0, i64 0
  %arrayidx55 = getelementptr inbounds [5 x i16], ptr @basil, i64 0, i64 1
  call void @_Z6CPhaseRtS_d(ptr %arrayidx54, ptr %arrayidx55, double 1.500000e+00)
  %arrayidx52 = getelementptr inbounds [5 x i16], ptr @cat, i64 0, i64 1
  %arrayidx53 = getelementptr inbounds [5 x i16], ptr @basil, i64 0, i64 0
  call void @_Z5SwapARtS_d(ptr %arrayidx52, ptr %arrayidx53, double 1.200000e+00)
  %arrayidx50 = getelementptr inbounds [5 x i16], ptr @basil, i64 0, i64 0
  %arrayidx51 = getelementptr inbounds [5 x i16], ptr @cat, i64 0, i64 1
  call void @_Z4CNOTRtS_(ptr %arrayidx50, ptr %arrayidx51)
  %arrayidx48 = getelementptr inbounds [5 x i16], ptr @cat, i64 0, i64 1
  %arrayidx49 = getelementptr inbounds [5 x i16], ptr @cat, i64 0, i64 0
  call void @_Z2CZRtS_(ptr %arrayidx48, ptr %arrayidx49)
  %arrayidx45 = getelementptr inbounds [5 x i16], ptr @basil, i64 0, i64 0
  %arrayidx46 = getelementptr inbounds [5 x i16], ptr @cat, i64 0, i64 1
  %arrayidx47 = getelementptr inbounds [5 x i16], ptr @cat, i64 0, i64 2
  call void @_Z7ToffoliRtS_S_(ptr %arrayidx45, ptr %arrayidx46, ptr %arrayidx47)
  %arrayidx43 = getelementptr inbounds [5 x i16], ptr @basil, i64 0, i64 1
  %arrayidx44 = getelementptr inbounds [5 x i16], ptr @basil, i64 0, i64 2
  call void @_Z4SWAPRtS_(ptr %arrayidx43, ptr %arrayidx44)
  %arrayidx41 = getelementptr inbounds [5 x i16], ptr @cat, i64 0, i64 0
  %arrayidx42 = getelementptr inbounds [5 x i16], ptr @basil, i64 0, i64 1
  call void @_Z6CPhaseRtS_d(ptr %arrayidx41, ptr %arrayidx42, double 1.500000e+00)
  %arrayidx39 = getelementptr inbounds [5 x i16], ptr @cat, i64 0, i64 1
  %arrayidx40 = getelementptr inbounds [5 x i16], ptr @basil, i64 0, i64 0
  call void @_Z5SwapARtS_d(ptr %arrayidx39, ptr %arrayidx40, double 1.200000e+00)
  %arrayidx37 = getelementptr inbounds [5 x i16], ptr @basil, i64 0, i64 0
  %arrayidx38 = getelementptr inbounds [5 x i16], ptr @cat, i64 0, i64 1
  call void @_Z4CNOTRtS_(ptr %arrayidx37, ptr %arrayidx38)
  %arrayidx35 = getelementptr inbounds [5 x i16], ptr @cat, i64 0, i64 1
  %arrayidx36 = getelementptr inbounds [5 x i16], ptr @cat, i64 0, i64 0
  call void @_Z2CZRtS_(ptr %arrayidx35, ptr %arrayidx36)
  %arrayidx32 = getelementptr inbounds [5 x i16], ptr @basil, i64 0, i64 0
  %arrayidx33 = getelementptr inbounds [5 x i16], ptr @cat, i64 0, i64 1
  %arrayidx34 = getelementptr inbounds [5 x i16], ptr @cat, i64 0, i64 2
  call void @_Z7ToffoliRtS_S_(ptr %arrayidx32, ptr %arrayidx33, ptr %arrayidx34)
  %arrayidx30 = getelementptr inbounds [5 x i16], ptr @basil, i64 0, i64 1
  %arrayidx31 = getelementptr inbounds [5 x i16], ptr @basil, i64 0, i64 2
  call void @_Z4SWAPRtS_(ptr %arrayidx30, ptr %arrayidx31)
  %arrayidx28 = getelementptr inbounds [5 x i16], ptr @cat, i64 0, i64 0
  %arrayidx29 = getelementptr inbounds [5 x i16], ptr @basil, i64 0, i64 1
  call void @_Z6CPhaseRtS_d(ptr %arrayidx28, ptr %arrayidx29, double 1.500000e+00)
  %arrayidx26 = getelementptr inbounds [5 x i16], ptr @cat, i64 0, i64 1
  %arrayidx27 = getelementptr inbounds [5 x i16], ptr @basil, i64 0, i64 0
  call void @_Z5SwapARtS_d(ptr %arrayidx26, ptr %arrayidx27, double 1.200000e+00)
  %arrayidx24 = getelementptr inbounds [5 x i16], ptr @basil, i64 0, i64 0
  %arrayidx25 = getelementptr inbounds [5 x i16], ptr @cat, i64 0, i64 1
  call void @_Z4CNOTRtS_(ptr %arrayidx24, ptr %arrayidx25)
  %arrayidx22 = getelementptr inbounds [5 x i16], ptr @cat, i64 0, i64 1
  %arrayidx23 = getelementptr inbounds [5 x i16], ptr @cat, i64 0, i64 0
  call void @_Z2CZRtS_(ptr %arrayidx22, ptr %arrayidx23)
  %arrayidx19 = getelementptr inbounds [5 x i16], ptr @basil, i64 0, i64 0
  %arrayidx20 = getelementptr inbounds [5 x i16], ptr @cat, i64 0, i64 1
  %arrayidx21 = getelementptr inbounds [5 x i16], ptr @cat, i64 0, i64 2
  call void @_Z7ToffoliRtS_S_(ptr %arrayidx19, ptr %arrayidx20, ptr %arrayidx21)
  %arrayidx17 = getelementptr inbounds [5 x i16], ptr @basil, i64 0, i64 1
  %arrayidx18 = getelementptr inbounds [5 x i16], ptr @basil, i64 0, i64 2
  call void @_Z4SWAPRtS_(ptr %arrayidx17, ptr %arrayidx18)
  %arrayidx15 = getelementptr inbounds [5 x i16], ptr @cat, i64 0, i64 0
  %arrayidx16 = getelementptr inbounds [5 x i16], ptr @basil, i64 0, i64 1
  call void @_Z6CPhaseRtS_d(ptr %arrayidx15, ptr %arrayidx16, double 1.500000e+00)
  %arrayidx13 = getelementptr inbounds [5 x i16], ptr @cat, i64 0, i64 1
  %arrayidx14 = getelementptr inbounds [5 x i16], ptr @basil, i64 0, i64 0
  call void @_Z5SwapARtS_d(ptr %arrayidx13, ptr %arrayidx14, double 1.200000e+00)
  %arrayidx11 = getelementptr inbounds [5 x i16], ptr @basil, i64 0, i64 0
  %arrayidx12 = getelementptr inbounds [5 x i16], ptr @cat, i64 0, i64 1
  call void @_Z4CNOTRtS_(ptr %arrayidx11, ptr %arrayidx12)
  %arrayidx9 = getelementptr inbounds [5 x i16], ptr @cat, i64 0, i64 1
  %arrayidx10 = getelementptr inbounds [5 x i16], ptr @cat, i64 0, i64 0
  call void @_Z2CZRtS_(ptr %arrayidx9, ptr %arrayidx10)
  %arrayidx6 = getelementptr inbounds [5 x i16], ptr @basil, i64 0, i64 0
  %arrayidx7 = getelementptr inbounds [5 x i16], ptr @cat, i64 0, i64 1
  %arrayidx8 = getelementptr inbounds [5 x i16], ptr @cat, i64 0, i64 2
  call void @_Z7ToffoliRtS_S_(ptr %arrayidx6, ptr %arrayidx7, ptr %arrayidx8)
  %arrayidx4 = getelementptr inbounds [5 x i16], ptr @basil, i64 0, i64 1
  %arrayidx5 = getelementptr inbounds [5 x i16], ptr @basil, i64 0, i64 2
  call void @_Z4SWAPRtS_(ptr %arrayidx4, ptr %arrayidx5)
  %arrayidx2 = getelementptr inbounds [5 x i16], ptr @cat, i64 0, i64 0
  %arrayidx3 = getelementptr inbounds [5 x i16], ptr @basil, i64 0, i64 1
  call void @_Z6CPhaseRtS_d(ptr %arrayidx2, ptr %arrayidx3, double 1.500000e+00)
  %arrayidx = getelementptr inbounds [5 x i16], ptr @cat, i64 0, i64 1
  %arrayidx1 = getelementptr inbounds [5 x i16], ptr @basil, i64 0, i64 0
  call void @_Z5SwapARtS_d(ptr %arrayidx, ptr %arrayidx1, double 1.200000e+00)
  br label %aqcc.meas.move.end

aqcc.meas.move.end:                               ; preds = %aqcc.quantum
  ret void
}

; Function Attrs: mustprogress noinline nounwind uwtable
define dso_local void @_Z16multiQubitGates2v.stub() local_unnamed_addr #0 section ".qbbs_text" {
entry:
  br label %aqcc.quantum

aqcc.quantum:                                     ; preds = %entry
  %arrayidx31 = getelementptr inbounds [5 x i16], ptr @basil, i64 0, i64 0
  %arrayidx32 = getelementptr inbounds [5 x i16], ptr @cat, i64 0, i64 1
  call void @_Z4CNOTRtS_(ptr %arrayidx31, ptr %arrayidx32)
  %arrayidx29 = getelementptr inbounds [5 x i16], ptr @cat, i64 0, i64 1
  %arrayidx30 = getelementptr inbounds [5 x i16], ptr @cat, i64 0, i64 0
  call void @_Z2CZRtS_(ptr %arrayidx29, ptr %arrayidx30)
  %arrayidx26 = getelementptr inbounds [5 x i16], ptr @basil, i64 0, i64 0
  %arrayidx27 = getelementptr inbounds [5 x i16], ptr @cat, i64 0, i64 1
  %arrayidx28 = getelementptr inbounds [5 x i16], ptr @cat, i64 0, i64 2
  call void @_Z7ToffoliRtS_S_(ptr %arrayidx26, ptr %arrayidx27, ptr %arrayidx28)
  %arrayidx24 = getelementptr inbounds [5 x i16], ptr @basil, i64 0, i64 1
  %arrayidx25 = getelementptr inbounds [5 x i16], ptr @basil, i64 0, i64 2
  call void @_Z4SWAPRtS_(ptr %arrayidx24, ptr %arrayidx25)
  %arrayidx22 = getelementptr inbounds [5 x i16], ptr @cat, i64 0, i64 0
  %arrayidx23 = getelementptr inbounds [5 x i16], ptr @basil, i64 0, i64 1
  call void @_Z6CPhaseRtS_d(ptr %arrayidx22, ptr %arrayidx23, double 1.500000e+00)
  %arrayidx20 = getelementptr inbounds [5 x i16], ptr @cat, i64 0, i64 1
  %arrayidx21 = getelementptr inbounds [5 x i16], ptr @basil, i64 0, i64 0
  call void @_Z5SwapARtS_d(ptr %arrayidx20, ptr %arrayidx21, double 1.200000e+00)
  %arrayidx18 = getelementptr inbounds [5 x i16], ptr @basil, i64 0, i64 0
  %arrayidx19 = getelementptr inbounds [5 x i16], ptr @cat, i64 0, i64 1
  call void @_Z4CNOTRtS_(ptr %arrayidx18, ptr %arrayidx19)
  %arrayidx16 = getelementptr inbounds [5 x i16], ptr @cat, i64 0, i64 1
  %arrayidx17 = getelementptr inbounds [5 x i16], ptr @cat, i64 0, i64 0
  call void @_Z2CZRtS_(ptr %arrayidx16, ptr %arrayidx17)
  %arrayidx13 = getelementptr inbounds [5 x i16], ptr @basil, i64 0, i64 0
  %arrayidx14 = getelementptr inbounds [5 x i16], ptr @cat, i64 0, i64 1
  %arrayidx15 = getelementptr inbounds [5 x i16], ptr @cat, i64 0, i64 2
  call void @_Z7ToffoliRtS_S_(ptr %arrayidx13, ptr %arrayidx14, ptr %arrayidx15)
  %arrayidx11 = getelementptr inbounds [5 x i16], ptr @basil, i64 0, i64 1
  %arrayidx12 = getelementptr inbounds [5 x i16], ptr @basil, i64 0, i64 2
  call void @_Z4SWAPRtS_(ptr %arrayidx11, ptr %arrayidx12)
  %arrayidx9 = getelementptr inbounds [5 x i16], ptr @cat, i64 0, i64 0
  %arrayidx10 = getelementptr inbounds [5 x i16], ptr @basil, i64 0, i64 1
  call void @_Z6CPhaseRtS_d(ptr %arrayidx9, ptr %arrayidx10, double 1.500000e+00)
  %arrayidx7 = getelementptr inbounds [5 x i16], ptr @cat, i64 0, i64 1
  %arrayidx8 = getelementptr inbounds [5 x i16], ptr @basil, i64 0, i64 0
  call void @_Z5SwapARtS_d(ptr %arrayidx7, ptr %arrayidx8, double 1.200000e+00)
  %arrayidx5 = getelementptr inbounds [5 x i16], ptr @basil, i64 0, i64 0
  %arrayidx6 = getelementptr inbounds [5 x i16], ptr @cat, i64 0, i64 1
  call void @_Z4CNOTRtS_(ptr %arrayidx5, ptr %arrayidx6)
  %arrayidx3 = getelementptr inbounds [5 x i16], ptr @cat, i64 0, i64 1
  %arrayidx4 = getelementptr inbounds [5 x i16], ptr @cat, i64 0, i64 0
  call void @_Z2CZRtS_(ptr %arrayidx3, ptr %arrayidx4)
  %arrayidx = getelementptr inbounds [5 x i16], ptr @basil, i64 0, i64 0
  %arrayidx1 = getelementptr inbounds [5 x i16], ptr @cat, i64 0, i64 1
  %arrayidx2 = getelementptr inbounds [5 x i16], ptr @cat, i64 0, i64 2
  call void @_Z7ToffoliRtS_S_(ptr %arrayidx, ptr %arrayidx1, ptr %arrayidx2)
  br label %aqcc.meas.move.end

aqcc.meas.move.end:                               ; preds = %aqcc.quantum
  ret void
}

; Function Attrs: mustprogress noinline nounwind uwtable
define dso_local void @_Z4widev.stub() local_unnamed_addr #0 section ".qbbs_text" {
entry:
  br label %aqcc.quantum

aqcc.quantum:                                     ; preds = %entry
  %arrayidx28 = getelementptr inbounds [35 x i16], ptr @dog, i64 0, i64 0
  call void @_Z1XRt(ptr %arrayidx28)
  %arrayidx27 = getelementptr inbounds [35 x i16], ptr @dog, i64 0, i64 1
  call void @_Z1XRt(ptr %arrayidx27)
  %arrayidx26 = getelementptr inbounds [35 x i16], ptr @dog, i64 0, i64 2
  call void @_Z1XRt(ptr %arrayidx26)
  %arrayidx25 = getelementptr inbounds [35 x i16], ptr @dog, i64 0, i64 3
  call void @_Z1XRt(ptr %arrayidx25)
  %arrayidx24 = getelementptr inbounds [35 x i16], ptr @dog, i64 0, i64 4
  call void @_Z1XRt(ptr %arrayidx24)
  %arrayidx23 = getelementptr inbounds [35 x i16], ptr @dog, i64 0, i64 5
  call void @_Z1XRt(ptr %arrayidx23)
  %arrayidx22 = getelementptr inbounds [35 x i16], ptr @dog, i64 0, i64 6
  call void @_Z1XRt(ptr %arrayidx22)
  %arrayidx21 = getelementptr inbounds [35 x i16], ptr @dog, i64 0, i64 7
  call void @_Z1XRt(ptr %arrayidx21)
  %arrayidx20 = getelementptr inbounds [35 x i16], ptr @dog, i64 0, i64 8
  call void @_Z1XRt(ptr %arrayidx20)
  %arrayidx19 = getelementptr inbounds [35 x i16], ptr @dog, i64 0, i64 9
  call void @_Z1XRt(ptr %arrayidx19)
  %arrayidx18 = getelementptr inbounds [35 x i16], ptr @dog, i64 0, i64 10
  call void @_Z1XRt(ptr %arrayidx18)
  %arrayidx17 = getelementptr inbounds [35 x i16], ptr @dog, i64 0, i64 11
  call void @_Z1XRt(ptr %arrayidx17)
  %arrayidx16 = getelementptr inbounds [35 x i16], ptr @dog, i64 0, i64 12
  call void @_Z1XRt(ptr %arrayidx16)
  %arrayidx15 = getelementptr inbounds [35 x i16], ptr @dog, i64 0, i64 13
  call void @_Z1XRt(ptr %arrayidx15)
  %arrayidx14 = getelementptr inbounds [35 x i16], ptr @dog, i64 0, i64 14
  call void @_Z1XRt(ptr %arrayidx14)
  %arrayidx13 = getelementptr inbounds [35 x i16], ptr @dog, i64 0, i64 15
  call void @_Z1XRt(ptr %arrayidx13)
  %arrayidx12 = getelementptr inbounds [35 x i16], ptr @dog, i64 0, i64 16
  call void @_Z1XRt(ptr %arrayidx12)
  %arrayidx11 = getelementptr inbounds [35 x i16], ptr @dog, i64 0, i64 17
  call void @_Z1XRt(ptr %arrayidx11)
  %arrayidx10 = getelementptr inbounds [35 x i16], ptr @dog, i64 0, i64 18
  call void @_Z1XRt(ptr %arrayidx10)
  %arrayidx9 = getelementptr inbounds [35 x i16], ptr @dog, i64 0, i64 19
  call void @_Z1XRt(ptr %arrayidx9)
  %arrayidx8 = getelementptr inbounds [35 x i16], ptr @dog, i64 0, i64 20
  call void @_Z1XRt(ptr %arrayidx8)
  %arrayidx7 = getelementptr inbounds [35 x i16], ptr @dog, i64 0, i64 21
  call void @_Z1XRt(ptr %arrayidx7)
  %arrayidx6 = getelementptr inbounds [35 x i16], ptr @dog, i64 0, i64 22
  call void @_Z1XRt(ptr %arrayidx6)
  %arrayidx5 = getelementptr inbounds [35 x i16], ptr @dog, i64 0, i64 23
  call void @_Z1XRt(ptr %arrayidx5)
  %arrayidx4 = getelementptr inbounds [35 x i16], ptr @dog, i64 0, i64 24
  call void @_Z1XRt(ptr %arrayidx4)
  %arrayidx3 = getelementptr inbounds [35 x i16], ptr @dog, i64 0, i64 25
  call void @_Z1XRt(ptr %arrayidx3)
  %arrayidx2 = getelementptr inbounds [35 x i16], ptr @dog, i64 0, i64 26
  call void @_Z1XRt(ptr %arrayidx2)
  %arrayidx1 = getelementptr inbounds [35 x i16], ptr @dog, i64 0, i64 27
  call void @_Z1XRt(ptr %arrayidx1)
  %arrayidx = getelementptr inbounds [35 x i16], ptr @dog, i64 0, i64 28
  call void @_Z1XRt(ptr %arrayidx)
  br label %aqcc.meas.move.end

aqcc.meas.move.end:                               ; preds = %aqcc.quantum
  ret void
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(none) uwtable
define dso_local noundef i32 @main() local_unnamed_addr #1 {
entry:
  ret i32 0
}

; Function Attrs: nounwind
declare void @llvm.quantum.qubit(ptr) #2

; Function Attrs: nounwind
declare void @llvm.quantum.param(double) #2

; Function Attrs: nounwind
declare void @llvm.quantum.cbit(ptr) #2

declare i32 @_ZN4qrt021initializeQrtSystem64EPdPcS1_(ptr, ptr, ptr)

declare i1 @_ZN4qrt013isInitializedEv()

declare void @_ZN4qrt020updateMeasurementMapEPbmb(ptr, i64, i1)

declare i32 @_ZN4qrt014scheduleKernelEPFvvEj(ptr, i32)

declare i32 @_ZN4qrt018dismantleQrtSystemEv()

attributes #0 = { mustprogress noinline nounwind uwtable "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #1 = { mustprogress nofree norecurse nosync nounwind willreturn memory(none) uwtable "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #2 = { nounwind }
; CHECK: LaTex instructions: 
; CHECK: \lstick{\ket{basil\_0}} & \qw & \ctrl{4} & \qw & \qw & \ctrl{4} & \qw & \gate{SwapA(1.2)} & \ctrl{4} & \qw & \qw & \ctrl{4} & \qw & \gate{SwapA(1.2)} & \ctrl{4} & \qw & \qw & \ctrl{4} & \qw & \gate{SwapA(1.2)} & \ctrl{4} & \qw & \qw & \ctrl{4} & \qw & \gate{SwapA(1.2)} & \ctrl{4} & \qw & \qw & \ctrl{4} & \qw & \gate{SwapA(1.2)} & \ctrl{4} & \qw & \qw & \ctrl{4} & \qw & \gate{SwapA(1.2)} & \ctrl{4} & \qw & \qw & \ctrl{4} & \gate{SwapA(1.2)} & \qw \\ 
; CHECK: \lstick{\ket{basil\_1}} & \qswap & \qw & \qw & \gate{Phase(1.5)} & \qw & \qswap & \qw & \qw & \qw & \gate{Phase(1.5)} & \qw & \qswap & \qw & \qw & \qw & \gate{Phase(1.5)} & \qw & \qswap & \qw & \qw & \qw & \gate{Phase(1.5)} & \qw & \qswap & \qw & \qw & \qw & \gate{Phase(1.5)} & \qw & \qswap & \qw & \qw & \qw & \gate{Phase(1.5)} & \qw & \qswap & \qw & \qw & \qw & \gate{Phase(1.5)} & \qw & \qw & \qw \\ 
; CHECK: \lstick{\ket{basil\_2}} & \qswap \qwx[-1] & \qw & \qw & \qw & \qw & \qswap \qwx[-1] & \qw & \qw & \qw & \qw & \qw & \qswap \qwx[-1] & \qw & \qw & \qw & \qw & \qw & \qswap \qwx[-1] & \qw & \qw & \qw & \qw & \qw & \qswap \qwx[-1] & \qw & \qw & \qw & \qw & \qw & \qswap \qwx[-1] & \qw & \qw & \qw & \qw & \qw & \qswap \qwx[-1] & \qw & \qw & \qw & \qw & \qw & \qw & \qw \\ 
; CHECK: \lstick{\ket{cat\_0}} & \qw & \qw & \gate{Z} & \ctrl{-2} & \qw & \qw & \qw & \qw & \gate{Z} & \ctrl{-2} & \qw & \qw & \qw & \qw & \gate{Z} & \ctrl{-2} & \qw & \qw & \qw & \qw & \gate{Z} & \ctrl{-2} & \qw & \qw & \qw & \qw & \gate{Z} & \ctrl{-2} & \qw & \qw & \qw & \qw & \gate{Z} & \ctrl{-2} & \qw & \qw & \qw & \qw & \gate{Z} & \ctrl{-2} & \qw & \qw & \qw \\ 
; CHECK: \lstick{\ket{cat\_1}} & \qw & \targ & \ctrl{-1} & \qw & \ctrl{1} & \qw & \gate{SwapA(1.2)} \qwx[-4] & \targ & \ctrl{-1} & \qw & \ctrl{1} & \qw & \gate{SwapA(1.2)} \qwx[-4] & \targ & \ctrl{-1} & \qw & \ctrl{1} & \qw & \gate{SwapA(1.2)} \qwx[-4] & \targ & \ctrl{-1} & \qw & \ctrl{1} & \qw & \gate{SwapA(1.2)} \qwx[-4] & \targ & \ctrl{-1} & \qw & \ctrl{1} & \qw & \gate{SwapA(1.2)} \qwx[-4] & \targ & \ctrl{-1} & \qw & \ctrl{1} & \qw & \gate{SwapA(1.2)} \qwx[-4] & \targ & \ctrl{-1} & \qw & \ctrl{1} & \gate{SwapA(1.2)} \qwx[-4] & \qw \\ 
; CHECK: \lstick{\ket{cat\_2}} & \qw & \qw & \qw & \qw & \targ & \qw & \qw & \qw & \qw & \qw & \targ & \qw & \qw & \qw & \qw & \qw & \targ & \qw & \qw & \qw & \qw & \qw & \targ & \qw & \qw & \qw & \qw & \qw & \targ & \qw & \qw & \qw & \qw & \qw & \targ & \qw & \qw & \qw & \qw & \qw & \targ & \qw & \qw 
; CHECK: LaTex instructions: 
; CHECK: \lstick{\ket{basil\_0}} & \qw & \ctrl{4} & \qw & \qw & \ctrl{4} & \qw & \gate{SwapA(1.2)} & \ctrl{4} & \qw & \qw & \ctrl{4} & \gate{SwapA(1.2)} & \ctrl{4} & \qw & \ctrl{4} & \qw \\ 
; CHECK: \lstick{\ket{basil\_1}} & \qswap & \qw & \qw & \gate{Phase(1.5)} & \qw & \qswap & \qw & \qw & \qw & \gate{Phase(1.5)} & \qw & \qw & \qw & \qw & \qw & \qw \\ 
; CHECK: \lstick{\ket{basil\_2}} & \qswap \qwx[-1] & \qw & \qw & \qw & \qw & \qswap \qwx[-1] & \qw & \qw & \qw & \qw & \qw & \qw & \qw & \qw & \qw & \qw \\ 
; CHECK: \lstick{\ket{cat\_0}} & \qw & \qw & \gate{Z} & \ctrl{-2} & \qw & \qw & \qw & \qw & \gate{Z} & \ctrl{-2} & \qw & \qw & \qw & \gate{Z} & \qw & \qw \\ 
; CHECK: \lstick{\ket{cat\_1}} & \qw & \targ & \ctrl{-1} & \qw & \ctrl{1} & \qw & \gate{SwapA(1.2)} \qwx[-4] & \targ & \ctrl{-1} & \qw & \ctrl{1} & \gate{SwapA(1.2)} \qwx[-4] & \targ & \ctrl{-1} & \ctrl{1} & \qw \\ 
; CHECK: \lstick{\ket{cat\_2}} & \qw & \qw & \qw & \qw & \targ & \qw & \qw & \qw & \qw & \qw & \targ & \qw & \qw & \qw & \targ & \qw 
; CHECK: LaTex instructions: 
; CHECK: \lstick{\ket{dog\_0}} & \gate{X} & \qw \\ 
; CHECK: \lstick{\ket{dog\_1}} & \gate{X} & \qw \\ 
; CHECK: \lstick{\ket{dog\_2}} & \gate{X} & \qw \\ 
; CHECK: \lstick{\ket{dog\_3}} & \gate{X} & \qw \\ 
; CHECK: \lstick{\ket{dog\_4}} & \gate{X} & \qw \\ 
; CHECK: \lstick{\ket{dog\_5}} & \gate{X} & \qw \\ 
; CHECK: \lstick{\ket{dog\_6}} & \gate{X} & \qw \\ 
; CHECK: \lstick{\ket{dog\_7}} & \gate{X} & \qw \\ 
; CHECK: \lstick{\ket{dog\_8}} & \gate{X} & \qw \\ 
; CHECK: \lstick{\ket{dog\_9}} & \gate{X} & \qw \\ 
; CHECK: \lstick{\ket{dog\_10}} & \gate{X} & \qw \\ 
; CHECK: \lstick{\ket{dog\_11}} & \gate{X} & \qw \\ 
; CHECK: \lstick{\ket{dog\_12}} & \gate{X} & \qw \\ 
; CHECK: \lstick{\ket{dog\_13}} & \gate{X} & \qw \\ 
; CHECK: \lstick{\ket{dog\_14}} & \gate{X} & \qw \\ 
; CHECK: \lstick{\ket{dog\_15}} & \gate{X} & \qw \\ 
; CHECK: \lstick{\ket{dog\_16}} & \gate{X} & \qw \\ 
; CHECK: \lstick{\ket{dog\_17}} & \gate{X} & \qw \\ 
; CHECK: \lstick{\ket{dog\_18}} & \gate{X} & \qw \\ 
; CHECK: \lstick{\ket{dog\_19}} & \gate{X} & \qw \\ 
; CHECK: \lstick{\ket{dog\_20}} & \gate{X} & \qw \\ 
; CHECK: \lstick{\ket{dog\_21}} & \gate{X} & \qw \\ 
; CHECK: \lstick{\ket{dog\_22}} & \gate{X} & \qw \\ 
; CHECK: \lstick{\ket{dog\_23}} & \gate{X} & \qw \\ 
; CHECK: \lstick{\ket{dog\_24}} & \gate{X} & \qw \\ 
; CHECK: \lstick{\ket{dog\_25}} & \gate{X} & \qw \\ 
; CHECK: \lstick{\ket{dog\_26}} & \gate{X} & \qw \\ 
; CHECK: \lstick{\ket{dog\_27}} & \gate{X} & \qw \\ 
; CHECK: \lstick{\ket{dog\_28}} & \gate{X} & \qw 
