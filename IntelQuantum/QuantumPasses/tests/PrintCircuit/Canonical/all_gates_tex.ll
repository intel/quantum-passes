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
@q = dso_local global [3 x i16] zeroinitializer, align 2
@c = dso_local global [3 x i8] zeroinitializer, align 1
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
define dso_local void @_Z3allv.stub() local_unnamed_addr #0 section ".qbbs_text" {
entry:
  br label %aqcc.quantum

aqcc.quantum:                                     ; preds = %entry
  %arrayidx28 = getelementptr inbounds [3 x i16], ptr @q, i64 0, i64 0
  call void @_Z5PrepXRt(ptr %arrayidx28)
  %arrayidx27 = getelementptr inbounds [3 x i16], ptr @q, i64 0, i64 1
  call void @_Z5PrepZRt(ptr %arrayidx27)
  %arrayidx26 = getelementptr inbounds [3 x i16], ptr @q, i64 0, i64 2
  call void @_Z5PrepYRt(ptr %arrayidx26)
  %arrayidx25 = getelementptr inbounds [3 x i16], ptr @q, i64 0, i64 0
  call void @_Z1XRt(ptr %arrayidx25)
  %arrayidx24 = getelementptr inbounds [3 x i16], ptr @q, i64 0, i64 0
  call void @_Z1HRt(ptr %arrayidx24)
  %arrayidx23 = getelementptr inbounds [3 x i16], ptr @q, i64 0, i64 1
  call void @_Z1YRt(ptr %arrayidx23)
  %arrayidx22 = getelementptr inbounds [3 x i16], ptr @q, i64 0, i64 0
  call void @_Z1ZRt(ptr %arrayidx22)
  %arrayidx21 = getelementptr inbounds [3 x i16], ptr @q, i64 0, i64 2
  call void @_Z1SRt(ptr %arrayidx21)
  %arrayidx20 = getelementptr inbounds [3 x i16], ptr @q, i64 0, i64 0
  call void @_Z4SdagRt(ptr %arrayidx20)
  %arrayidx19 = getelementptr inbounds [3 x i16], ptr @q, i64 0, i64 1
  call void @_Z1TRt(ptr %arrayidx19)
  %arrayidx18 = getelementptr inbounds [3 x i16], ptr @q, i64 0, i64 2
  call void @_Z4TdagRt(ptr %arrayidx18)
  %arrayidx17 = getelementptr inbounds [3 x i16], ptr @q, i64 0, i64 0
  call void @_Z2RXRtd(ptr %arrayidx17, double 1.000000e-01)
  %arrayidx16 = getelementptr inbounds [3 x i16], ptr @q, i64 0, i64 1
  call void @_Z2RZRtd(ptr %arrayidx16, double 3.000000e-01)
  %arrayidx15 = getelementptr inbounds [3 x i16], ptr @q, i64 0, i64 2
  call void @_Z2RYRtd(ptr %arrayidx15, double 8.000000e-01)
  %arrayidx14 = getelementptr inbounds [3 x i16], ptr @q, i64 0, i64 2
  call void @_Z3RXYRtdd(ptr %arrayidx14, double 5.000000e-01, double 6.000000e-01)
  %arrayidx12 = getelementptr inbounds [3 x i16], ptr @q, i64 0, i64 0
  %arrayidx13 = getelementptr inbounds [3 x i16], ptr @q, i64 0, i64 1
  call void @_Z2CZRtS_(ptr %arrayidx12, ptr %arrayidx13)
  %arrayidx10 = getelementptr inbounds [3 x i16], ptr @q, i64 0, i64 2
  %arrayidx11 = getelementptr inbounds [3 x i16], ptr @q, i64 0, i64 1
  call void @_Z4CNOTRtS_(ptr %arrayidx10, ptr %arrayidx11)
  %arrayidx8 = getelementptr inbounds [3 x i16], ptr @q, i64 0, i64 0
  %arrayidx9 = getelementptr inbounds [3 x i16], ptr @q, i64 0, i64 1
  call void @_Z5SwapARtS_d(ptr %arrayidx8, ptr %arrayidx9, double 3.000000e-01)
  %arrayidx5 = getelementptr inbounds [3 x i16], ptr @q, i64 0, i64 0
  %arrayidx6 = getelementptr inbounds [3 x i16], ptr @q, i64 0, i64 1
  %arrayidx7 = getelementptr inbounds [3 x i16], ptr @q, i64 0, i64 2
  call void @_Z7ToffoliRtS_S_(ptr %arrayidx5, ptr %arrayidx6, ptr %arrayidx7)
  %arrayidx3 = getelementptr inbounds [3 x i16], ptr @q, i64 0, i64 0
  %arrayidx4 = getelementptr inbounds [3 x i16], ptr @q, i64 0, i64 2
  call void @_Z4SWAPRtS_(ptr %arrayidx3, ptr %arrayidx4)
  %arrayidx2 = getelementptr inbounds [3 x i16], ptr @q, i64 0, i64 1
  call void @_Z5MeasXRtRbb(ptr %arrayidx2, ptr getelementptr inbounds ([3 x i8], ptr @c, i64 0, i64 1), i1 false)
  %arrayidx1 = getelementptr inbounds [3 x i16], ptr @q, i64 0, i64 2
  call void @_Z5MeasZRtRbb(ptr %arrayidx1, ptr getelementptr inbounds ([3 x i8], ptr @c, i64 0, i64 2), i1 false)
  %arrayidx = getelementptr inbounds [3 x i16], ptr @q, i64 0, i64 0
  call void @_Z5MeasYRtRbb(ptr %arrayidx, ptr @c, i1 false)
  br label %aqcc.meas.move.end

aqcc.meas.move.end:                               ; preds = %aqcc.quantum
  ret void
}

; Function Attrs: mustprogress norecurse nounwind uwtable
define dso_local noundef i32 @main() local_unnamed_addr #1 {
entry:
  tail call void @_Z3allv.stub() #2
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
attributes #1 = { mustprogress norecurse nounwind uwtable "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #2 = { nounwind }
; CHECK: LaTex instructions: 
; CHECK: \lstick{\ket{q\_0}} & \gate{PrepX} & \gate{X} & \gate{H} & \gate{Z} & \gate{S^\dag} & \gate{RX(0.1)} & \ctrl{1} & \qw & \gate{SwapA(0.3)} & \ctrl{1} & \qw & \qswap & \measure{\mbox{Y}} & \qw \\ 
; CHECK: \lstick{\ket{q\_1}} & \gate{PrepZ} & \gate{Y} & \gate{T} & \gate{RZ(0.3)} & \qw & \qw & \gate{Z} & \targ & \gate{SwapA(0.3)} \qwx[-1] & \ctrl{1} & \measure{\mbox{X}} & \qw & \qw & \qw \\ 
; CHECK: \lstick{\ket{q\_2}} & \gate{PrepY} & \gate{S} & \gate{T^\dag} & \gate{RY(0.8)} & \gate{RXY(0.5, 0.6)} & \qw & \qw & \ctrl{-1} & \qw & \targ & \qw & \qswap \qwx[-2] & \measure{\mbox{Z}} & \qw 