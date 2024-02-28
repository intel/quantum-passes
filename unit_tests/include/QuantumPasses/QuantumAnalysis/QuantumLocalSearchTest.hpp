//===- QuantumLocalSearchTest.hpp --------------------------------*- C++
//-*-===//
//
//
// Copyright (C) 2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//
//===----------------------------------------------------------------------===//
/// \file "QuantumIteratorTest.hpp"
///
/// This file provides the raw strings for Quantum Local Search unit
/// test.
///
//===----------------------------------------------------------------------===//

#ifndef QUANTUM_LOCAL_SEARCH_TEST_HPP
#define QUANTUM_LOCAL_SEARCH_TEST_HPP

static const char local_3x3_grid_1[] = R"(
; ModuleID = 'local_3x3_grid_1_unrolled_user.ll'
source_filename = "test-samples/local_3x3_grid_1.cpp"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

%"class.std::ios_base::Init" = type { i8 }

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
@c = dso_local local_unnamed_addr global [7 x i8] zeroinitializer, align 1
@llvm.global_ctors = appending global [1 x { i32, ptr, ptr }] [{ i32, ptr, ptr } { i32 65535, ptr @_GLOBAL__sub_I_local_3x3_grid_1.cpp, ptr null }]
@llvm.global.annotations = appending global [24 x { ptr, ptr, ptr, i32, ptr }] [{ ptr, ptr, ptr, i32, ptr } { ptr @_Z1HRt, ptr @.str, ptr @.str.1, i32 48, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr @_Z1XRt, ptr @.str.2, ptr @.str.1, i32 68, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr @_Z1YRt, ptr @.str.3, ptr @.str.1, i32 88, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr @_Z1ZRt, ptr @.str.4, ptr @.str.1, i32 109, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr @_Z1SRt, ptr @.str.5, ptr @.str.1, i32 129, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr @_Z4SdagRt, ptr @.str.6, ptr @.str.1, i32 149, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr @_Z1TRt, ptr @.str.7, ptr @.str.1, i32 169, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr @_Z4TdagRt, ptr @.str.8, ptr @.str.1, i32 189, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr @_Z2RXRtd, ptr @.str.9, ptr @.str.1, i32 209, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr @_Z2RYRtd, ptr @.str.10, ptr @.str.1, i32 230, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr @_Z2RZRtd, ptr @.str.11, ptr @.str.1, i32 251, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr @_Z2CZRtS_, ptr @.str.12, ptr @.str.1, i32 272, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr @_Z4CNOTRtS_, ptr @.str.13, ptr @.str.1, i32 293, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr @_Z4SWAPRtS_, ptr @.str.14, ptr @.str.1, i32 314, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr @_Z7ToffoliRtS_S_, ptr @.str.15, ptr @.str.1, i32 335, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr @_Z5PrepXRt, ptr @.str.16, ptr @.str.1, i32 359, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr @_Z5PrepYRt, ptr @.str.17, ptr @.str.1, i32 377, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr @_Z5PrepZRt, ptr @.str.18, ptr @.str.1, i32 395, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr @_Z5MeasXRtRbb, ptr @.str.19, ptr @.str.1, i32 413, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr @_Z5MeasYRtRbb, ptr @.str.20, ptr @.str.1, i32 433, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr @_Z5MeasZRtRbb, ptr @.str.21, ptr @.str.1, i32 453, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr @_Z6CPhaseRtS_d, ptr @.str.22, ptr @.str.1, i32 481, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr @_Z3RXYRtdd, ptr @.str.23, ptr @.str.1, i32 503, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr @_Z5SwapARtS_d, ptr @.str.24, ptr @.str.1, i32 525, ptr null }], section "llvm.metadata"

; Function Attrs: mustprogress noinline nounwind uwtable
define dso_local void @_Z1HRt(ptr noundef nonnull align 2 dereferenceable(2) "qubit_ref" %0) #0 {
  tail call void @llvm.quantum.qubit(ptr noundef nonnull align 2 dereferenceable(2) %0)
  ret void
}

; Function Attrs: mustprogress noinline nounwind uwtable
define dso_local void @_Z1XRt(ptr noundef nonnull align 2 dereferenceable(2) "qubit_ref" %0) #0 {
  tail call void @llvm.quantum.qubit(ptr noundef nonnull align 2 dereferenceable(2) %0)
  ret void
}

; Function Attrs: mustprogress noinline nounwind uwtable
define dso_local void @_Z1YRt(ptr noundef nonnull align 2 dereferenceable(2) "qubit_ref" %0) #0 {
  tail call void @llvm.quantum.qubit(ptr noundef nonnull align 2 dereferenceable(2) %0)
  ret void
}

; Function Attrs: mustprogress noinline nounwind uwtable
define dso_local void @_Z1ZRt(ptr noundef nonnull align 2 dereferenceable(2) "qubit_ref" %0) #0 {
  tail call void @llvm.quantum.qubit(ptr noundef nonnull align 2 dereferenceable(2) %0)
  ret void
}

; Function Attrs: mustprogress noinline nounwind uwtable
define dso_local void @_Z1SRt(ptr noundef nonnull align 2 dereferenceable(2) "qubit_ref" %0) #0 {
  tail call void @llvm.quantum.qubit(ptr noundef nonnull align 2 dereferenceable(2) %0)
  ret void
}

; Function Attrs: mustprogress noinline nounwind uwtable
define dso_local void @_Z4SdagRt(ptr noundef nonnull align 2 dereferenceable(2) "qubit_ref" %0) #0 {
  tail call void @llvm.quantum.qubit(ptr noundef nonnull align 2 dereferenceable(2) %0)
  ret void
}

; Function Attrs: mustprogress noinline nounwind uwtable
define dso_local void @_Z1TRt(ptr noundef nonnull align 2 dereferenceable(2) "qubit_ref" %0) #0 {
  tail call void @llvm.quantum.qubit(ptr noundef nonnull align 2 dereferenceable(2) %0)
  ret void
}

; Function Attrs: mustprogress noinline nounwind uwtable
define dso_local void @_Z4TdagRt(ptr noundef nonnull align 2 dereferenceable(2) "qubit_ref" %0) #0 {
  tail call void @llvm.quantum.qubit(ptr noundef nonnull align 2 dereferenceable(2) %0)
  ret void
}

; Function Attrs: mustprogress noinline nounwind uwtable
define dso_local void @_Z2RXRtd(ptr noundef nonnull align 2 dereferenceable(2) "qubit_ref" %0, double noundef %1) #0 {
  tail call void @llvm.quantum.qubit(ptr noundef nonnull align 2 dereferenceable(2) %0)
  tail call void @llvm.quantum.param(double noundef %1)
  ret void
}

; Function Attrs: mustprogress noinline nounwind uwtable
define dso_local void @_Z2RYRtd(ptr noundef nonnull align 2 dereferenceable(2) "qubit_ref" %0, double noundef %1) #0 {
  tail call void @llvm.quantum.qubit(ptr noundef nonnull align 2 dereferenceable(2) %0)
  tail call void @llvm.quantum.param(double noundef %1)
  ret void
}

; Function Attrs: mustprogress noinline nounwind uwtable
define dso_local void @_Z2RZRtd(ptr noundef nonnull align 2 dereferenceable(2) "qubit_ref" %0, double noundef %1) #0 {
  tail call void @llvm.quantum.qubit(ptr noundef nonnull align 2 dereferenceable(2) %0)
  tail call void @llvm.quantum.param(double noundef %1)
  ret void
}

; Function Attrs: mustprogress noinline nounwind uwtable
define dso_local void @_Z2CZRtS_(ptr noundef nonnull align 2 dereferenceable(2) "qubit_ref" %0, ptr noundef nonnull align 2 dereferenceable(2) "qubit_ref" %1) #0 {
  tail call void @llvm.quantum.qubit(ptr noundef nonnull align 2 dereferenceable(2) %0)
  tail call void @llvm.quantum.qubit(ptr noundef nonnull %1)
  ret void
}

; Function Attrs: mustprogress noinline nounwind uwtable
define dso_local void @_Z4CNOTRtS_(ptr noundef nonnull align 2 dereferenceable(2) "qubit_ref" %0, ptr noundef nonnull align 2 dereferenceable(2) "qubit_ref" %1) #0 {
  tail call void @llvm.quantum.qubit(ptr noundef nonnull align 2 dereferenceable(2) %0)
  tail call void @llvm.quantum.qubit(ptr noundef nonnull %1)
  ret void
}

; Function Attrs: mustprogress noinline nounwind uwtable
define dso_local void @_Z4SWAPRtS_(ptr noundef nonnull align 2 dereferenceable(2) "qubit_ref" %0, ptr noundef nonnull align 2 dereferenceable(2) "qubit_ref" %1) #0 {
  tail call void @llvm.quantum.qubit(ptr noundef nonnull align 2 dereferenceable(2) %0)
  tail call void @llvm.quantum.qubit(ptr noundef nonnull %1)
  ret void
}

; Function Attrs: mustprogress noinline nounwind uwtable
define dso_local void @_Z7ToffoliRtS_S_(ptr noundef nonnull align 2 dereferenceable(2) "qubit_ref" %0, ptr noundef nonnull align 2 dereferenceable(2) "qubit_ref" %1, ptr noundef nonnull align 2 dereferenceable(2) "qubit_ref" %2) #0 {
  tail call void @llvm.quantum.qubit(ptr noundef nonnull align 2 dereferenceable(2) %0)
  tail call void @llvm.quantum.qubit(ptr noundef nonnull %1)
  tail call void @llvm.quantum.qubit(ptr noundef nonnull %2)
  ret void
}

; Function Attrs: mustprogress noinline nounwind uwtable
define dso_local void @_Z5PrepXRt(ptr noundef nonnull align 2 dereferenceable(2) "qubit_ref" %0) #0 {
  tail call void @llvm.quantum.qubit(ptr noundef nonnull align 2 dereferenceable(2) %0)
  ret void
}

; Function Attrs: mustprogress noinline nounwind uwtable
define dso_local void @_Z5PrepYRt(ptr noundef nonnull align 2 dereferenceable(2) "qubit_ref" %0) #0 {
  tail call void @llvm.quantum.qubit(ptr noundef nonnull align 2 dereferenceable(2) %0)
  ret void
}

; Function Attrs: mustprogress noinline nounwind uwtable
define dso_local void @_Z5PrepZRt(ptr noundef nonnull align 2 dereferenceable(2) "qubit_ref" %0) #0 {
  tail call void @llvm.quantum.qubit(ptr noundef nonnull align 2 dereferenceable(2) %0)
  ret void
}

; Function Attrs: mustprogress noinline nounwind uwtable
define dso_local void @_Z5MeasXRtRbb(ptr noundef nonnull align 2 dereferenceable(2) "qubit_ref" %0, ptr noundef nonnull align 1 dereferenceable(1) "cbit_ref" %1, i1 noundef zeroext %2) #0 {
  tail call void @llvm.quantum.qubit(ptr noundef nonnull align 2 dereferenceable(2) %0)
  tail call void @llvm.quantum.cbit(ptr noundef nonnull %1)
  %4 = uitofp i1 %2 to double
  tail call void @llvm.quantum.param(double noundef %4)
  ret void
}

; Function Attrs: mustprogress noinline nounwind uwtable
define dso_local void @_Z5MeasYRtRbb(ptr noundef nonnull align 2 dereferenceable(2) "qubit_ref" %0, ptr noundef nonnull align 1 dereferenceable(1) "cbit_ref" %1, i1 noundef zeroext %2) #0 {
  tail call void @llvm.quantum.qubit(ptr noundef nonnull align 2 dereferenceable(2) %0)
  tail call void @llvm.quantum.cbit(ptr noundef nonnull %1)
  %4 = uitofp i1 %2 to double
  tail call void @llvm.quantum.param(double noundef %4)
  ret void
}

; Function Attrs: mustprogress noinline nounwind uwtable
define dso_local void @_Z5MeasZRtRbb(ptr noundef nonnull align 2 dereferenceable(2) "qubit_ref" %0, ptr noundef nonnull align 1 dereferenceable(1) "cbit_ref" %1, i1 noundef zeroext %2) #0 {
  tail call void @llvm.quantum.qubit(ptr noundef nonnull align 2 dereferenceable(2) %0)
  tail call void @llvm.quantum.cbit(ptr noundef nonnull %1)
  %4 = uitofp i1 %2 to double
  tail call void @llvm.quantum.param(double noundef %4)
  ret void
}

; Function Attrs: mustprogress noinline nounwind uwtable
define dso_local void @_Z6CPhaseRtS_d(ptr noundef nonnull align 2 dereferenceable(2) "qubit_ref" %0, ptr noundef nonnull align 2 dereferenceable(2) "qubit_ref" %1, double noundef %2) #0 {
  tail call void @llvm.quantum.qubit(ptr noundef nonnull align 2 dereferenceable(2) %0)
  tail call void @llvm.quantum.qubit(ptr noundef nonnull %1)
  tail call void @llvm.quantum.param(double noundef %2)
  ret void
}

; Function Attrs: mustprogress noinline nounwind uwtable
define dso_local void @_Z3RXYRtdd(ptr noundef nonnull align 2 dereferenceable(2) "qubit_ref" %0, double noundef %1, double noundef %2) #0 {
  tail call void @llvm.quantum.qubit(ptr noundef nonnull align 2 dereferenceable(2) %0)
  tail call void @llvm.quantum.param(double noundef %1)
  tail call void @llvm.quantum.param(double noundef %2)
  ret void
}

; Function Attrs: mustprogress noinline nounwind uwtable
define dso_local void @_Z5SwapARtS_d(ptr noundef nonnull align 2 dereferenceable(2) "qubit_ref" %0, ptr noundef nonnull align 2 dereferenceable(2) "qubit_ref" %1, double noundef %2) #0 {
  tail call void @llvm.quantum.qubit(ptr noundef nonnull align 2 dereferenceable(2) %0)
  tail call void @llvm.quantum.qubit(ptr noundef nonnull %1)
  tail call void @llvm.quantum.param(double noundef %2)
  ret void
}

declare void @_ZNSt8ios_base4InitC1Ev(ptr noundef nonnull align 1 dereferenceable(1)) unnamed_addr #1

; Function Attrs: nounwind
declare void @_ZNSt8ios_base4InitD1Ev(ptr noundef nonnull align 1 dereferenceable(1)) unnamed_addr #2

; Function Attrs: nofree nounwind
declare i32 @__cxa_atexit(ptr, ptr, ptr) local_unnamed_addr #3

; Function Attrs: mustprogress noinline nounwind uwtable
define dso_local void @_Z13test_kernel_1v.stub() local_unnamed_addr #0 section ".qbbs_text" {
  %1 = alloca [7 x i16], align 2
  call void @llvm.lifetime.start.p0(i64 noundef 14, ptr noundef nonnull align 2 dereferenceable(14) %1) #7
  %2 = getelementptr inbounds [7 x i16], ptr %1, i64 0, i64 2
  %3 = getelementptr inbounds [7 x i16], ptr %1, i64 0, i64 4
  %4 = getelementptr inbounds [7 x i16], ptr %1, i64 0, i64 1
  %5 = getelementptr inbounds [7 x i16], ptr %1, i64 0, i64 3
  %6 = getelementptr inbounds [7 x i16], ptr %1, i64 0, i64 5
  %7 = getelementptr inbounds [7 x i16], ptr %1, i64 0, i64 6
  br label %aqcc.quantum

aqcc.quantum:                                     ; preds = %0
  %arrayidx10 = getelementptr inbounds [7 x i16], ptr %1, i64 0, i64 0
  %arrayidx11 = getelementptr inbounds [7 x i16], ptr %1, i64 0, i64 2
  call void @_Z4CNOTRtS_(ptr %arrayidx10, ptr %arrayidx11)
  %arrayidx8 = getelementptr inbounds [7 x i16], ptr %1, i64 0, i64 2
  %arrayidx9 = getelementptr inbounds [7 x i16], ptr %1, i64 0, i64 4
  call void @_Z4CNOTRtS_(ptr %arrayidx8, ptr %arrayidx9)
  %arrayidx6 = getelementptr inbounds [7 x i16], ptr %1, i64 0, i64 0
  %arrayidx7 = getelementptr inbounds [7 x i16], ptr %1, i64 0, i64 1
  call void @_Z4CNOTRtS_(ptr %arrayidx6, ptr %arrayidx7)
  %arrayidx4 = getelementptr inbounds [7 x i16], ptr %1, i64 0, i64 1
  %arrayidx5 = getelementptr inbounds [7 x i16], ptr %1, i64 0, i64 3
  call void @_Z4CNOTRtS_(ptr %arrayidx4, ptr %arrayidx5)
  %arrayidx2 = getelementptr inbounds [7 x i16], ptr %1, i64 0, i64 4
  %arrayidx3 = getelementptr inbounds [7 x i16], ptr %1, i64 0, i64 5
  call void @_Z4CNOTRtS_(ptr %arrayidx2, ptr %arrayidx3)
  %arrayidx = getelementptr inbounds [7 x i16], ptr %1, i64 0, i64 5
  %arrayidx1 = getelementptr inbounds [7 x i16], ptr %1, i64 0, i64 6
  call void @_Z4CNOTRtS_(ptr %arrayidx, ptr %arrayidx1)
  br label %aqcc.meas.move.end

aqcc.meas.move.end:                               ; preds = %aqcc.quantum
  call void @llvm.lifetime.end.p0(i64 noundef 14, ptr noundef nonnull %1) #7
  ret void
}

; Function Attrs: nocallback nofree nosync nounwind willreturn memory(argmem: readwrite)
declare void @llvm.lifetime.start.p0(i64 immarg, ptr nocapture) #4

; Function Attrs: nocallback nofree nosync nounwind willreturn memory(argmem: readwrite)
declare void @llvm.lifetime.end.p0(i64 immarg, ptr nocapture) #4

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(none) uwtable
define dso_local noundef i32 @main() local_unnamed_addr #5 {
  ret i32 0
}

; Function Attrs: uwtable
define internal void @_GLOBAL__sub_I_local_3x3_grid_1.cpp() #6 section ".text.startup" {
  tail call void @_ZNSt8ios_base4InitC1Ev(ptr noundef nonnull align 1 dereferenceable(1) @_ZStL8__ioinit)
  %1 = tail call i32 @__cxa_atexit(ptr noundef nonnull @_ZNSt8ios_base4InitD1Ev, ptr noundef nonnull @_ZStL8__ioinit, ptr noundef nonnull @__dso_handle) #7
  ret void
}

; Function Attrs: nounwind
declare void @llvm.quantum.qubit(ptr) #7

; Function Attrs: nounwind
declare void @llvm.quantum.param(double) #7

; Function Attrs: nounwind
declare void @llvm.quantum.cbit(ptr) #7

declare i32 @_ZN4qrt021initializeQrtSystem64EPdPcS1_(ptr, ptr, ptr)

declare i1 @_ZN4qrt013isInitializedEv()

declare void @_ZN4qrt020updateMeasurementMapEPbmb(ptr, i64, i1)

declare i32 @_ZN4qrt014scheduleKernelEPFvvEj(ptr, i32)

declare i32 @_ZN4qrt018dismantleQrtSystemEv()

attributes #0 = { mustprogress noinline nounwind uwtable "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #1 = { "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #2 = { nounwind "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #3 = { nofree nounwind }
attributes #4 = { nocallback nofree nosync nounwind willreturn memory(argmem: readwrite) }
attributes #5 = { mustprogress nofree norecurse nosync nounwind willreturn memory(none) uwtable "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #6 = { uwtable "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #7 = { nounwind }

!llvm.module.flags = !{!0, !1, !2, !3}
!llvm.ident = !{!4}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{i32 8, !"PIC Level", i32 2}
!2 = !{i32 7, !"PIE Level", i32 2}
!3 = !{i32 7, !"uwtable", i32 2}
!4 = !{!"clang version 17.0.0 (https://github.com/intel-sandbox/applications.quantum.compiler-llvm.git 9dcb9e2143d5ccc873e3073261d3173a57f5498f)\"}

)"; // Must use Raw string to ensure proper parsing

static const char local_3x3_grid_2[] = R"(
; ModuleID = 'local_3x3_grid_2_unrolled_user.ll'
source_filename = "test-samples/local_3x3_grid_2.cpp"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

%"class.std::ios_base::Init" = type { i8 }

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
@c = dso_local local_unnamed_addr global [7 x i8] zeroinitializer, align 1
@llvm.global_ctors = appending global [1 x { i32, ptr, ptr }] [{ i32, ptr, ptr } { i32 65535, ptr @_GLOBAL__sub_I_local_3x3_grid_2.cpp, ptr null }]
@llvm.global.annotations = appending global [24 x { ptr, ptr, ptr, i32, ptr }] [{ ptr, ptr, ptr, i32, ptr } { ptr @_Z1HRt, ptr @.str, ptr @.str.1, i32 48, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr @_Z1XRt, ptr @.str.2, ptr @.str.1, i32 68, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr @_Z1YRt, ptr @.str.3, ptr @.str.1, i32 88, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr @_Z1ZRt, ptr @.str.4, ptr @.str.1, i32 109, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr @_Z1SRt, ptr @.str.5, ptr @.str.1, i32 129, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr @_Z4SdagRt, ptr @.str.6, ptr @.str.1, i32 149, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr @_Z1TRt, ptr @.str.7, ptr @.str.1, i32 169, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr @_Z4TdagRt, ptr @.str.8, ptr @.str.1, i32 189, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr @_Z2RXRtd, ptr @.str.9, ptr @.str.1, i32 209, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr @_Z2RYRtd, ptr @.str.10, ptr @.str.1, i32 230, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr @_Z2RZRtd, ptr @.str.11, ptr @.str.1, i32 251, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr @_Z2CZRtS_, ptr @.str.12, ptr @.str.1, i32 272, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr @_Z4CNOTRtS_, ptr @.str.13, ptr @.str.1, i32 293, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr @_Z4SWAPRtS_, ptr @.str.14, ptr @.str.1, i32 314, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr @_Z7ToffoliRtS_S_, ptr @.str.15, ptr @.str.1, i32 335, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr @_Z5PrepXRt, ptr @.str.16, ptr @.str.1, i32 359, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr @_Z5PrepYRt, ptr @.str.17, ptr @.str.1, i32 377, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr @_Z5PrepZRt, ptr @.str.18, ptr @.str.1, i32 395, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr @_Z5MeasXRtRbb, ptr @.str.19, ptr @.str.1, i32 413, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr @_Z5MeasYRtRbb, ptr @.str.20, ptr @.str.1, i32 433, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr @_Z5MeasZRtRbb, ptr @.str.21, ptr @.str.1, i32 453, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr @_Z6CPhaseRtS_d, ptr @.str.22, ptr @.str.1, i32 481, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr @_Z3RXYRtdd, ptr @.str.23, ptr @.str.1, i32 503, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr @_Z5SwapARtS_d, ptr @.str.24, ptr @.str.1, i32 525, ptr null }], section "llvm.metadata"

; Function Attrs: mustprogress noinline nounwind uwtable
define dso_local void @_Z1HRt(ptr noundef nonnull align 2 dereferenceable(2) "qubit_ref" %0) #0 {
  tail call void @llvm.quantum.qubit(ptr noundef nonnull align 2 dereferenceable(2) %0)
  ret void
}

; Function Attrs: mustprogress noinline nounwind uwtable
define dso_local void @_Z1XRt(ptr noundef nonnull align 2 dereferenceable(2) "qubit_ref" %0) #0 {
  tail call void @llvm.quantum.qubit(ptr noundef nonnull align 2 dereferenceable(2) %0)
  ret void
}

; Function Attrs: mustprogress noinline nounwind uwtable
define dso_local void @_Z1YRt(ptr noundef nonnull align 2 dereferenceable(2) "qubit_ref" %0) #0 {
  tail call void @llvm.quantum.qubit(ptr noundef nonnull align 2 dereferenceable(2) %0)
  ret void
}

; Function Attrs: mustprogress noinline nounwind uwtable
define dso_local void @_Z1ZRt(ptr noundef nonnull align 2 dereferenceable(2) "qubit_ref" %0) #0 {
  tail call void @llvm.quantum.qubit(ptr noundef nonnull align 2 dereferenceable(2) %0)
  ret void
}

; Function Attrs: mustprogress noinline nounwind uwtable
define dso_local void @_Z1SRt(ptr noundef nonnull align 2 dereferenceable(2) "qubit_ref" %0) #0 {
  tail call void @llvm.quantum.qubit(ptr noundef nonnull align 2 dereferenceable(2) %0)
  ret void
}

; Function Attrs: mustprogress noinline nounwind uwtable
define dso_local void @_Z4SdagRt(ptr noundef nonnull align 2 dereferenceable(2) "qubit_ref" %0) #0 {
  tail call void @llvm.quantum.qubit(ptr noundef nonnull align 2 dereferenceable(2) %0)
  ret void
}

; Function Attrs: mustprogress noinline nounwind uwtable
define dso_local void @_Z1TRt(ptr noundef nonnull align 2 dereferenceable(2) "qubit_ref" %0) #0 {
  tail call void @llvm.quantum.qubit(ptr noundef nonnull align 2 dereferenceable(2) %0)
  ret void
}

; Function Attrs: mustprogress noinline nounwind uwtable
define dso_local void @_Z4TdagRt(ptr noundef nonnull align 2 dereferenceable(2) "qubit_ref" %0) #0 {
  tail call void @llvm.quantum.qubit(ptr noundef nonnull align 2 dereferenceable(2) %0)
  ret void
}

; Function Attrs: mustprogress noinline nounwind uwtable
define dso_local void @_Z2RXRtd(ptr noundef nonnull align 2 dereferenceable(2) "qubit_ref" %0, double noundef %1) #0 {
  tail call void @llvm.quantum.qubit(ptr noundef nonnull align 2 dereferenceable(2) %0)
  tail call void @llvm.quantum.param(double noundef %1)
  ret void
}

; Function Attrs: mustprogress noinline nounwind uwtable
define dso_local void @_Z2RYRtd(ptr noundef nonnull align 2 dereferenceable(2) "qubit_ref" %0, double noundef %1) #0 {
  tail call void @llvm.quantum.qubit(ptr noundef nonnull align 2 dereferenceable(2) %0)
  tail call void @llvm.quantum.param(double noundef %1)
  ret void
}

; Function Attrs: mustprogress noinline nounwind uwtable
define dso_local void @_Z2RZRtd(ptr noundef nonnull align 2 dereferenceable(2) "qubit_ref" %0, double noundef %1) #0 {
  tail call void @llvm.quantum.qubit(ptr noundef nonnull align 2 dereferenceable(2) %0)
  tail call void @llvm.quantum.param(double noundef %1)
  ret void
}

; Function Attrs: mustprogress noinline nounwind uwtable
define dso_local void @_Z2CZRtS_(ptr noundef nonnull align 2 dereferenceable(2) "qubit_ref" %0, ptr noundef nonnull align 2 dereferenceable(2) "qubit_ref" %1) #0 {
  tail call void @llvm.quantum.qubit(ptr noundef nonnull align 2 dereferenceable(2) %0)
  tail call void @llvm.quantum.qubit(ptr noundef nonnull %1)
  ret void
}

; Function Attrs: mustprogress noinline nounwind uwtable
define dso_local void @_Z4CNOTRtS_(ptr noundef nonnull align 2 dereferenceable(2) "qubit_ref" %0, ptr noundef nonnull align 2 dereferenceable(2) "qubit_ref" %1) #0 {
  tail call void @llvm.quantum.qubit(ptr noundef nonnull align 2 dereferenceable(2) %0)
  tail call void @llvm.quantum.qubit(ptr noundef nonnull %1)
  ret void
}

; Function Attrs: mustprogress noinline nounwind uwtable
define dso_local void @_Z4SWAPRtS_(ptr noundef nonnull align 2 dereferenceable(2) "qubit_ref" %0, ptr noundef nonnull align 2 dereferenceable(2) "qubit_ref" %1) #0 {
  tail call void @llvm.quantum.qubit(ptr noundef nonnull align 2 dereferenceable(2) %0)
  tail call void @llvm.quantum.qubit(ptr noundef nonnull %1)
  ret void
}

; Function Attrs: mustprogress noinline nounwind uwtable
define dso_local void @_Z7ToffoliRtS_S_(ptr noundef nonnull align 2 dereferenceable(2) "qubit_ref" %0, ptr noundef nonnull align 2 dereferenceable(2) "qubit_ref" %1, ptr noundef nonnull align 2 dereferenceable(2) "qubit_ref" %2) #0 {
  tail call void @llvm.quantum.qubit(ptr noundef nonnull align 2 dereferenceable(2) %0)
  tail call void @llvm.quantum.qubit(ptr noundef nonnull %1)
  tail call void @llvm.quantum.qubit(ptr noundef nonnull %2)
  ret void
}

; Function Attrs: mustprogress noinline nounwind uwtable
define dso_local void @_Z5PrepXRt(ptr noundef nonnull align 2 dereferenceable(2) "qubit_ref" %0) #0 {
  tail call void @llvm.quantum.qubit(ptr noundef nonnull align 2 dereferenceable(2) %0)
  ret void
}

; Function Attrs: mustprogress noinline nounwind uwtable
define dso_local void @_Z5PrepYRt(ptr noundef nonnull align 2 dereferenceable(2) "qubit_ref" %0) #0 {
  tail call void @llvm.quantum.qubit(ptr noundef nonnull align 2 dereferenceable(2) %0)
  ret void
}

; Function Attrs: mustprogress noinline nounwind uwtable
define dso_local void @_Z5PrepZRt(ptr noundef nonnull align 2 dereferenceable(2) "qubit_ref" %0) #0 {
  tail call void @llvm.quantum.qubit(ptr noundef nonnull align 2 dereferenceable(2) %0)
  ret void
}

; Function Attrs: mustprogress noinline nounwind uwtable
define dso_local void @_Z5MeasXRtRbb(ptr noundef nonnull align 2 dereferenceable(2) "qubit_ref" %0, ptr noundef nonnull align 1 dereferenceable(1) "cbit_ref" %1, i1 noundef zeroext %2) #0 {
  tail call void @llvm.quantum.qubit(ptr noundef nonnull align 2 dereferenceable(2) %0)
  tail call void @llvm.quantum.cbit(ptr noundef nonnull %1)
  %4 = uitofp i1 %2 to double
  tail call void @llvm.quantum.param(double noundef %4)
  ret void
}

; Function Attrs: mustprogress noinline nounwind uwtable
define dso_local void @_Z5MeasYRtRbb(ptr noundef nonnull align 2 dereferenceable(2) "qubit_ref" %0, ptr noundef nonnull align 1 dereferenceable(1) "cbit_ref" %1, i1 noundef zeroext %2) #0 {
  tail call void @llvm.quantum.qubit(ptr noundef nonnull align 2 dereferenceable(2) %0)
  tail call void @llvm.quantum.cbit(ptr noundef nonnull %1)
  %4 = uitofp i1 %2 to double
  tail call void @llvm.quantum.param(double noundef %4)
  ret void
}

; Function Attrs: mustprogress noinline nounwind uwtable
define dso_local void @_Z5MeasZRtRbb(ptr noundef nonnull align 2 dereferenceable(2) "qubit_ref" %0, ptr noundef nonnull align 1 dereferenceable(1) "cbit_ref" %1, i1 noundef zeroext %2) #0 {
  tail call void @llvm.quantum.qubit(ptr noundef nonnull align 2 dereferenceable(2) %0)
  tail call void @llvm.quantum.cbit(ptr noundef nonnull %1)
  %4 = uitofp i1 %2 to double
  tail call void @llvm.quantum.param(double noundef %4)
  ret void
}

; Function Attrs: mustprogress noinline nounwind uwtable
define dso_local void @_Z6CPhaseRtS_d(ptr noundef nonnull align 2 dereferenceable(2) "qubit_ref" %0, ptr noundef nonnull align 2 dereferenceable(2) "qubit_ref" %1, double noundef %2) #0 {
  tail call void @llvm.quantum.qubit(ptr noundef nonnull align 2 dereferenceable(2) %0)
  tail call void @llvm.quantum.qubit(ptr noundef nonnull %1)
  tail call void @llvm.quantum.param(double noundef %2)
  ret void
}

; Function Attrs: mustprogress noinline nounwind uwtable
define dso_local void @_Z3RXYRtdd(ptr noundef nonnull align 2 dereferenceable(2) "qubit_ref" %0, double noundef %1, double noundef %2) #0 {
  tail call void @llvm.quantum.qubit(ptr noundef nonnull align 2 dereferenceable(2) %0)
  tail call void @llvm.quantum.param(double noundef %1)
  tail call void @llvm.quantum.param(double noundef %2)
  ret void
}

; Function Attrs: mustprogress noinline nounwind uwtable
define dso_local void @_Z5SwapARtS_d(ptr noundef nonnull align 2 dereferenceable(2) "qubit_ref" %0, ptr noundef nonnull align 2 dereferenceable(2) "qubit_ref" %1, double noundef %2) #0 {
  tail call void @llvm.quantum.qubit(ptr noundef nonnull align 2 dereferenceable(2) %0)
  tail call void @llvm.quantum.qubit(ptr noundef nonnull %1)
  tail call void @llvm.quantum.param(double noundef %2)
  ret void
}

declare void @_ZNSt8ios_base4InitC1Ev(ptr noundef nonnull align 1 dereferenceable(1)) unnamed_addr #1

; Function Attrs: nounwind
declare void @_ZNSt8ios_base4InitD1Ev(ptr noundef nonnull align 1 dereferenceable(1)) unnamed_addr #2

; Function Attrs: nofree nounwind
declare i32 @__cxa_atexit(ptr, ptr, ptr) local_unnamed_addr #3

; Function Attrs: mustprogress noinline nounwind uwtable
define dso_local void @_Z13test_kernel_2v.stub() local_unnamed_addr #0 section ".qbbs_text" {
  %1 = alloca [7 x i16], align 2
  call void @llvm.lifetime.start.p0(i64 noundef 14, ptr noundef nonnull align 2 dereferenceable(14) %1) #7
  %2 = getelementptr inbounds [7 x i16], ptr %1, i64 0, i64 1
  %3 = getelementptr inbounds [7 x i16], ptr %1, i64 0, i64 2
  %4 = getelementptr inbounds [7 x i16], ptr %1, i64 0, i64 6
  %5 = getelementptr inbounds [7 x i16], ptr %1, i64 0, i64 3
  %6 = getelementptr inbounds [7 x i16], ptr %1, i64 0, i64 4
  %7 = getelementptr inbounds [7 x i16], ptr %1, i64 0, i64 5
  br label %aqcc.quantum

aqcc.quantum:                                     ; preds = %0
  %arrayidx10 = getelementptr inbounds [7 x i16], ptr %1, i64 0, i64 0
  %arrayidx11 = getelementptr inbounds [7 x i16], ptr %1, i64 0, i64 1
  call void @_Z2CZRtS_(ptr %arrayidx10, ptr %arrayidx11)
  %arrayidx8 = getelementptr inbounds [7 x i16], ptr %1, i64 0, i64 1
  %arrayidx9 = getelementptr inbounds [7 x i16], ptr %1, i64 0, i64 2
  call void @_Z2CZRtS_(ptr %arrayidx8, ptr %arrayidx9)
  %arrayidx6 = getelementptr inbounds [7 x i16], ptr %1, i64 0, i64 2
  %arrayidx7 = getelementptr inbounds [7 x i16], ptr %1, i64 0, i64 6
  call void @_Z2CZRtS_(ptr %arrayidx6, ptr %arrayidx7)
  %arrayidx4 = getelementptr inbounds [7 x i16], ptr %1, i64 0, i64 2
  %arrayidx5 = getelementptr inbounds [7 x i16], ptr %1, i64 0, i64 3
  call void @_Z2CZRtS_(ptr %arrayidx4, ptr %arrayidx5)
  %arrayidx2 = getelementptr inbounds [7 x i16], ptr %1, i64 0, i64 4
  %arrayidx3 = getelementptr inbounds [7 x i16], ptr %1, i64 0, i64 1
  call void @_Z2CZRtS_(ptr %arrayidx2, ptr %arrayidx3)
  %arrayidx = getelementptr inbounds [7 x i16], ptr %1, i64 0, i64 5
  %arrayidx1 = getelementptr inbounds [7 x i16], ptr %1, i64 0, i64 4
  call void @_Z2CZRtS_(ptr %arrayidx, ptr %arrayidx1)
  br label %aqcc.meas.move.end

aqcc.meas.move.end:                               ; preds = %aqcc.quantum
  call void @llvm.lifetime.end.p0(i64 noundef 14, ptr noundef nonnull %1) #7
  ret void
}

; Function Attrs: nocallback nofree nosync nounwind willreturn memory(argmem: readwrite)
declare void @llvm.lifetime.start.p0(i64 immarg, ptr nocapture) #4

; Function Attrs: nocallback nofree nosync nounwind willreturn memory(argmem: readwrite)
declare void @llvm.lifetime.end.p0(i64 immarg, ptr nocapture) #4

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(none) uwtable
define dso_local noundef i32 @main() local_unnamed_addr #5 {
  ret i32 0
}

; Function Attrs: uwtable
define internal void @_GLOBAL__sub_I_local_3x3_grid_2.cpp() #6 section ".text.startup" {
  tail call void @_ZNSt8ios_base4InitC1Ev(ptr noundef nonnull align 1 dereferenceable(1) @_ZStL8__ioinit)
  %1 = tail call i32 @__cxa_atexit(ptr noundef nonnull @_ZNSt8ios_base4InitD1Ev, ptr noundef nonnull @_ZStL8__ioinit, ptr noundef nonnull @__dso_handle) #7
  ret void
}

; Function Attrs: nounwind
declare void @llvm.quantum.qubit(ptr) #7

; Function Attrs: nounwind
declare void @llvm.quantum.param(double) #7

; Function Attrs: nounwind
declare void @llvm.quantum.cbit(ptr) #7

declare i32 @_ZN4qrt021initializeQrtSystem64EPdPcS1_(ptr, ptr, ptr)

declare i1 @_ZN4qrt013isInitializedEv()

declare void @_ZN4qrt020updateMeasurementMapEPbmb(ptr, i64, i1)

declare i32 @_ZN4qrt014scheduleKernelEPFvvEj(ptr, i32)

declare i32 @_ZN4qrt018dismantleQrtSystemEv()

attributes #0 = { mustprogress noinline nounwind uwtable "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #1 = { "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #2 = { nounwind "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #3 = { nofree nounwind }
attributes #4 = { nocallback nofree nosync nounwind willreturn memory(argmem: readwrite) }
attributes #5 = { mustprogress nofree norecurse nosync nounwind willreturn memory(none) uwtable "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #6 = { uwtable "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #7 = { nounwind }

!llvm.module.flags = !{!0, !1, !2, !3}
!llvm.ident = !{!4}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{i32 8, !"PIC Level", i32 2}
!2 = !{i32 7, !"PIE Level", i32 2}
!3 = !{i32 7, !"uwtable", i32 2}
!4 = !{!"clang version 17.0.0 (https://github.com/intel-sandbox/applications.quantum.compiler-llvm.git 9dcb9e2143d5ccc873e3073261d3173a57f5498f)\"}

)"; // Must use Raw string to ensure proper parsing

const char tfd_example[] = R"(
; ModuleID = 'tfd_q4_hybrid_demo_unrolled_user.ll'
source_filename = "tfd_q4_hybrid_demo.cpp"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

%"class.std::ios_base::Init" = type { i8 }
%"struct.iqsdk::IqsCustomOp" = type { double, double, double, double, %"class.std::vector", %"class.std::__cxx11::basic_string", double, double, double, double }
%"class.std::vector" = type { %"struct.std::_Vector_base" }
%"struct.std::_Vector_base" = type { %"struct.std::_Vector_base<std::complex<double>, std::allocator<std::complex<double>>>::_Vector_impl" }
%"struct.std::_Vector_base<std::complex<double>, std::allocator<std::complex<double>>>::_Vector_impl" = type { %"struct.std::_Vector_base<std::complex<double>, std::allocator<std::complex<double>>>::_Vector_impl_data" }
%"struct.std::_Vector_base<std::complex<double>, std::allocator<std::complex<double>>>::_Vector_impl_data" = type { ptr, ptr, ptr }
%"class.std::__cxx11::basic_string" = type { %"struct.std::__cxx11::basic_string<char>::_Alloc_hider", i64, %union.anon }
%"struct.std::__cxx11::basic_string<char>::_Alloc_hider" = type { ptr }
%union.anon = type { i64, [8 x i8] }
%"class.std::basic_ostream" = type { ptr, %"class.std::basic_ios" }
%"class.std::basic_ios" = type { %"class.std::ios_base", ptr, i8, i8, ptr, ptr, ptr, ptr }
%"class.std::ios_base" = type { ptr, i64, i64, i32, i32, i32, ptr, %"struct.std::ios_base::_Words", [8 x %"struct.std::ios_base::_Words"], i32, ptr, %"class.std::locale" }
%"struct.std::ios_base::_Words" = type { ptr, i64 }
%"class.std::locale" = type { ptr }
%"struct.iqsdk::IqsConfig" = type { %"struct.iqsdk::DeviceConfig.base", i32, %"class.std::__cxx11::basic_string", double, i8, i64, %"class.std::function", %"class.std::function", %"class.std::function.3", %"class.std::function.5", %"class.std::function.7", %"class.std::function.7" }
%"struct.iqsdk::DeviceConfig.base" = type <{ ptr, %"class.std::__cxx11::basic_string", i8, i8 }>
%"class.std::function" = type { %"class.std::_Function_base", ptr }
%"class.std::_Function_base" = type { %"union.std::_Any_data", ptr }
%"union.std::_Any_data" = type { %"union.std::_Nocopy_types" }
%"union.std::_Nocopy_types" = type { { i64, i64 } }
%"class.std::function.3" = type { %"class.std::_Function_base", ptr }
%"class.std::function.5" = type { %"class.std::_Function_base", ptr }
%"class.std::function.7" = type { %"class.std::_Function_base", ptr }
%"class.iqsdk::FullStateSimulator" = type { %"class.iqsdk::SimulatorDevice" }
%"class.iqsdk::SimulatorDevice" = type { %"class.iqsdk::Device" }
%"class.iqsdk::Device" = type { ptr, i8, %"class.std::shared_ptr" }
%"class.std::shared_ptr" = type { %"class.std::__shared_ptr" }
%"class.std::__shared_ptr" = type { ptr, %"class.std::__shared_count" }
%"class.std::__shared_count" = type { ptr }
%"class.std::vector.9" = type { %"struct.std::_Vector_base.10" }
%"struct.std::_Vector_base.10" = type { %"struct.std::_Vector_base<double, std::allocator<double>>::_Vector_impl" }
%"struct.std::_Vector_base<double, std::allocator<double>>::_Vector_impl" = type { %"struct.std::_Vector_base<double, std::allocator<double>>::_Vector_impl_data" }
%"struct.std::_Vector_base<double, std::allocator<double>>::_Vector_impl_data" = type { ptr, ptr, ptr }
%"class.std::vector.14" = type { %"struct.std::_Vector_base.15" }
%"struct.std::_Vector_base.15" = type { %"struct.std::_Vector_base<std::reference_wrapper<unsigned short>, std::allocator<std::reference_wrapper<unsigned short>>>::_Vector_impl" }
%"struct.std::_Vector_base<std::reference_wrapper<unsigned short>, std::allocator<std::reference_wrapper<unsigned short>>>::_Vector_impl" = type { %"struct.std::_Vector_base<std::reference_wrapper<unsigned short>, std::allocator<std::reference_wrapper<unsigned short>>>::_Vector_impl_data" }
%"struct.std::_Vector_base<std::reference_wrapper<unsigned short>, std::allocator<std::reference_wrapper<unsigned short>>>::_Vector_impl_data" = type { ptr, ptr, ptr }
%"class.std::reference_wrapper" = type { ptr }
%"struct.iqsdk::DeviceConfig" = type <{ ptr, %"class.std::__cxx11::basic_string", i8, i8, [6 x i8] }>

$_ZN5iqsdk11IqsCustomOpD2Ev = comdat any

$_ZN5iqsdk9IqsConfigD2Ev = comdat any

$__clang_call_terminate = comdat any

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
@.str.24 = private unnamed_addr constant [503 x i8] c"{     \22matrix_real\22 : \22[1, 0, 0, 0, 0, (1+cos(angle))/2, (1-cos(angle))/2, 0, 0, (1-cos(angle))/2, (1+cos(angle))/2, 0, 0, 0, 0, 1]\22,     \22matrix_imag\22 : \22[0, 0, 0, 0, 0, sin(angle)/2, -sin(angle)/2, 0, 0, -sin(angle)/2, sin(angle)/2, 0, 0, 0, 0, 0]\22,     \22matrix_order\22 : \22rm\22,     \22is_hermitian\22 : false,     \22is_unitary\22 : true,     \22is_mutable\22 : true,     \22qubit_list\22 : [0, 1],     \22parametric_list\22 : [2],     \22control_qubit_list\22 : [],     \22local_basis_list\22 : [0, 0],     \22identifier\22 : 53   }\00", section "llvm.metadata"
@_ZStL8__ioinit = internal global %"class.std::ios_base::Init" zeroinitializer, align 1
@__dso_handle = external hidden global i8
@_ZN5iqsdkL14k_iqs_ideal_opE = internal global %"struct.iqsdk::IqsCustomOp" zeroinitializer, align 8
@.str.26 = private unnamed_addr constant [6 x i8] c"ideal\00", align 1
@QubitReg = dso_local global [4 x i16] zeroinitializer, align 2
@CReg = dso_local global [4 x i8] zeroinitializer, align 1
@QuantumVariableParams = dso_local local_unnamed_addr global [4 x double] zeroinitializer, align 16
@.str.27 = private unnamed_addr constant [10 x i8] c"noiseless\00", align 1
@_ZSt4cout = external global %"class.std::basic_ostream", align 8
@.str.28 = private unnamed_addr constant [26 x i8] c"+++++++++++++ Run count# \00", align 1
@.str.29 = private unnamed_addr constant [16 x i8] c" +++++++++++++\0A\00", align 1
@_ZTVN5iqsdk9IqsConfigE = external unnamed_addr constant { [5 x ptr] }, align 8
@_ZTVN5iqsdk12DeviceConfigE = external unnamed_addr constant { [5 x ptr] }, align 8
@.str.31 = private unnamed_addr constant [26 x i8] c"vector::_M_realloc_insert\00", align 1
@llvm.global_ctors = appending global [1 x { i32, ptr, ptr }] [{ i32, ptr, ptr } { i32 65535, ptr @_GLOBAL__sub_I_tfd_q4_hybrid_demo.cpp, ptr null }]
@llvm.global.annotations = appending global [24 x { ptr, ptr, ptr, i32, ptr }] [{ ptr, ptr, ptr, i32, ptr } { ptr @_Z1HRt, ptr @.str, ptr @.str.1, i32 47, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr @_Z1XRt, ptr @.str.2, ptr @.str.1, i32 66, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr @_Z1YRt, ptr @.str.3, ptr @.str.1, i32 85, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr @_Z1ZRt, ptr @.str.4, ptr @.str.1, i32 105, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr @_Z1SRt, ptr @.str.5, ptr @.str.1, i32 124, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr @_Z4SdagRt, ptr @.str.6, ptr @.str.1, i32 143, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr @_Z1TRt, ptr @.str.7, ptr @.str.1, i32 162, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr @_Z4TdagRt, ptr @.str.8, ptr @.str.1, i32 181, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr @_Z2RXRtd, ptr @.str.9, ptr @.str.1, i32 200, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr @_Z2RYRtd, ptr @.str.10, ptr @.str.1, i32 221, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr @_Z2RZRtd, ptr @.str.11, ptr @.str.1, i32 242, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr @_Z2CZRtS_, ptr @.str.12, ptr @.str.1, i32 263, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr @_Z4CNOTRtS_, ptr @.str.13, ptr @.str.1, i32 284, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr @_Z4SWAPRtS_, ptr @.str.14, ptr @.str.1, i32 305, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr @_Z7ToffoliRtS_S_, ptr @.str.15, ptr @.str.1, i32 326, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr @_Z5PrepXRt, ptr @.str.16, ptr @.str.1, i32 350, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr @_Z5PrepYRt, ptr @.str.17, ptr @.str.1, i32 367, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr @_Z5PrepZRt, ptr @.str.18, ptr @.str.1, i32 384, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr @_Z5MeasXRtRbb, ptr @.str.19, ptr @.str.1, i32 401, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr @_Z5MeasYRtRbb, ptr @.str.20, ptr @.str.1, i32 421, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr @_Z5MeasZRtRbb, ptr @.str.21, ptr @.str.1, i32 441, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr @_Z6CPhaseRtS_d, ptr @.str.22, ptr @.str.1, i32 469, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr @_Z3RXYRtdd, ptr @.str.23, ptr @.str.1, i32 491, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr @_Z5SwapARtS_d, ptr @.str.24, ptr @.str.1, i32 513, ptr null }], section "llvm.metadata"

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

declare void @_ZNSt8ios_base4InitC1Ev(ptr noundef nonnull align 1 dereferenceable(1)) unnamed_addr #1

; Function Attrs: nounwind
declare void @_ZNSt8ios_base4InitD1Ev(ptr noundef nonnull align 1 dereferenceable(1)) unnamed_addr #2

; Function Attrs: nofree nounwind
declare i32 @__cxa_atexit(ptr, ptr, ptr) local_unnamed_addr #3

; Function Attrs: nocallback nofree nounwind willreturn memory(argmem: write)
declare void @llvm.memset.p0.i64(ptr nocapture writeonly, i8, i64, i1 immarg) #4

; Function Attrs: nocallback nofree nosync nounwind willreturn memory(argmem: readwrite)
declare void @llvm.lifetime.start.p0(i64 immarg, ptr nocapture) #5

declare i32 @__gxx_personality_v0(...)

; Function Attrs: nocallback nofree nosync nounwind willreturn memory(argmem: readwrite)
declare void @llvm.lifetime.end.p0(i64 immarg, ptr nocapture) #5

; Function Attrs: inlinehint nounwind uwtable
define linkonce_odr dso_local void @_ZN5iqsdk11IqsCustomOpD2Ev(ptr noundef nonnull align 8 dereferenceable(120) %this) unnamed_addr #6 comdat align 2 personality ptr @__gxx_personality_v0 {
entry:
  %label = getelementptr inbounds %"struct.iqsdk::IqsCustomOp", ptr %this, i64 0, i32 5
  %0 = load ptr, ptr %label, align 8, !tbaa !5
  %1 = getelementptr inbounds %"struct.iqsdk::IqsCustomOp", ptr %this, i64 0, i32 5, i32 2
  %cmp.i.i.i = icmp eq ptr %0, %1
  br i1 %cmp.i.i.i, label %_ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEED2Ev.exit, label %if.then.i.i

if.then.i.i:                                      ; preds = %entry
  tail call void @_ZdlPv(ptr noundef %0) #16
  br label %_ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEED2Ev.exit

_ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEED2Ev.exit: ; preds = %if.then.i.i, %entry
  %process_matrix = getelementptr inbounds %"struct.iqsdk::IqsCustomOp", ptr %this, i64 0, i32 4
  %2 = load ptr, ptr %process_matrix, align 8, !tbaa !12
  %tobool.not.i.i.i = icmp eq ptr %2, null
  br i1 %tobool.not.i.i.i, label %_ZNSt6vectorISt7complexIdESaIS1_EED2Ev.exit, label %if.then.i.i.i

if.then.i.i.i:                                    ; preds = %_ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEED2Ev.exit
  tail call void @_ZdlPv(ptr noundef nonnull %2) #16
  br label %_ZNSt6vectorISt7complexIdESaIS1_EED2Ev.exit

_ZNSt6vectorISt7complexIdESaIS1_EED2Ev.exit:      ; preds = %if.then.i.i.i, %_ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEED2Ev.exit
  ret void
}

; Function Attrs: mustprogress noinline nounwind uwtable
define dso_local void @_Z10tfdQ4Setupv.stub() local_unnamed_addr #0 section ".qbbs_text" {
entry:
  %0 = load double, ptr getelementptr inbounds ([4 x double], ptr @QuantumVariableParams, i64 0, i64 2), align 16, !tbaa !14
  %1 = load double, ptr getelementptr inbounds ([4 x double], ptr @QuantumVariableParams, i64 0, i64 2), align 16, !tbaa !14
  %2 = load double, ptr getelementptr inbounds ([4 x double], ptr @QuantumVariableParams, i64 0, i64 2), align 16, !tbaa !14
  %3 = load double, ptr getelementptr inbounds ([4 x double], ptr @QuantumVariableParams, i64 0, i64 2), align 16, !tbaa !14
  %4 = load double, ptr getelementptr inbounds ([4 x double], ptr @QuantumVariableParams, i64 0, i64 3), align 8, !tbaa !14
  %5 = load double, ptr getelementptr inbounds ([4 x double], ptr @QuantumVariableParams, i64 0, i64 3), align 8, !tbaa !14
  %6 = load double, ptr @QuantumVariableParams, align 16, !tbaa !14
  %7 = load double, ptr @QuantumVariableParams, align 16, !tbaa !14
  %8 = load double, ptr getelementptr inbounds ([4 x double], ptr @QuantumVariableParams, i64 0, i64 1), align 8, !tbaa !14
  %9 = load double, ptr getelementptr inbounds ([4 x double], ptr @QuantumVariableParams, i64 0, i64 1), align 8, !tbaa !14
  br label %aqcc.quantum

aqcc.quantum:                                     ; preds = %entry
  %arrayidx47 = getelementptr inbounds [4 x i16], ptr @QubitReg, i64 0, i64 0
  call void @_Z2RYRtd(ptr %arrayidx47, double 0x3FF921FB5443D6F4)
  %arrayidx46 = getelementptr inbounds [4 x i16], ptr @QubitReg, i64 0, i64 1
  call void @_Z2RYRtd(ptr %arrayidx46, double 0x3FF921FB5443D6F4)
  %arrayidx44 = getelementptr inbounds [4 x i16], ptr @QubitReg, i64 0, i64 0
  %arrayidx45 = getelementptr inbounds [4 x i16], ptr @QubitReg, i64 0, i64 2
  call void @_Z4CNOTRtS_(ptr %arrayidx44, ptr %arrayidx45)
  %arrayidx42 = getelementptr inbounds [4 x i16], ptr @QubitReg, i64 0, i64 1
  %arrayidx43 = getelementptr inbounds [4 x i16], ptr @QubitReg, i64 0, i64 3
  call void @_Z4CNOTRtS_(ptr %arrayidx42, ptr %arrayidx43)
  %arrayidx41 = getelementptr inbounds [4 x i16], ptr @QubitReg, i64 0, i64 0
  call void @_Z2RXRtd(ptr %arrayidx41, double %0)
  %arrayidx40 = getelementptr inbounds [4 x i16], ptr @QubitReg, i64 0, i64 1
  call void @_Z2RXRtd(ptr %arrayidx40, double %1)
  %arrayidx39 = getelementptr inbounds [4 x i16], ptr @QubitReg, i64 0, i64 2
  call void @_Z2RXRtd(ptr %arrayidx39, double %2)
  %arrayidx38 = getelementptr inbounds [4 x i16], ptr @QubitReg, i64 0, i64 3
  call void @_Z2RXRtd(ptr %arrayidx38, double %3)
  %arrayidx36 = getelementptr inbounds [4 x i16], ptr @QubitReg, i64 0, i64 1
  %arrayidx37 = getelementptr inbounds [4 x i16], ptr @QubitReg, i64 0, i64 0
  call void @_Z4CNOTRtS_(ptr %arrayidx36, ptr %arrayidx37)
  %arrayidx34 = getelementptr inbounds [4 x i16], ptr @QubitReg, i64 0, i64 3
  %arrayidx35 = getelementptr inbounds [4 x i16], ptr @QubitReg, i64 0, i64 2
  call void @_Z4CNOTRtS_(ptr %arrayidx34, ptr %arrayidx35)
  %arrayidx33 = getelementptr inbounds [4 x i16], ptr @QubitReg, i64 0, i64 0
  call void @_Z2RZRtd(ptr %arrayidx33, double %4)
  %arrayidx32 = getelementptr inbounds [4 x i16], ptr @QubitReg, i64 0, i64 2
  call void @_Z2RZRtd(ptr %arrayidx32, double %5)
  %arrayidx30 = getelementptr inbounds [4 x i16], ptr @QubitReg, i64 0, i64 1
  %arrayidx31 = getelementptr inbounds [4 x i16], ptr @QubitReg, i64 0, i64 0
  call void @_Z4CNOTRtS_(ptr %arrayidx30, ptr %arrayidx31)
  %arrayidx28 = getelementptr inbounds [4 x i16], ptr @QubitReg, i64 0, i64 3
  %arrayidx29 = getelementptr inbounds [4 x i16], ptr @QubitReg, i64 0, i64 2
  call void @_Z4CNOTRtS_(ptr %arrayidx28, ptr %arrayidx29)
  %arrayidx27 = getelementptr inbounds [4 x i16], ptr @QubitReg, i64 0, i64 0
  call void @_Z2RYRtd(ptr %arrayidx27, double 0xBFF921FB5443D6F4)
  %arrayidx26 = getelementptr inbounds [4 x i16], ptr @QubitReg, i64 0, i64 1
  call void @_Z2RYRtd(ptr %arrayidx26, double 0xBFF921FB5443D6F4)
  %arrayidx25 = getelementptr inbounds [4 x i16], ptr @QubitReg, i64 0, i64 2
  call void @_Z2RYRtd(ptr %arrayidx25, double 0xBFF921FB5443D6F4)
  %arrayidx24 = getelementptr inbounds [4 x i16], ptr @QubitReg, i64 0, i64 3
  call void @_Z2RYRtd(ptr %arrayidx24, double 0xBFF921FB5443D6F4)
  %arrayidx22 = getelementptr inbounds [4 x i16], ptr @QubitReg, i64 0, i64 2
  %arrayidx23 = getelementptr inbounds [4 x i16], ptr @QubitReg, i64 0, i64 0
  call void @_Z4CNOTRtS_(ptr %arrayidx22, ptr %arrayidx23)
  %arrayidx20 = getelementptr inbounds [4 x i16], ptr @QubitReg, i64 0, i64 3
  %arrayidx21 = getelementptr inbounds [4 x i16], ptr @QubitReg, i64 0, i64 1
  call void @_Z4CNOTRtS_(ptr %arrayidx20, ptr %arrayidx21)
  %arrayidx19 = getelementptr inbounds [4 x i16], ptr @QubitReg, i64 0, i64 0
  call void @_Z2RZRtd(ptr %arrayidx19, double %6)
  %arrayidx18 = getelementptr inbounds [4 x i16], ptr @QubitReg, i64 0, i64 1
  call void @_Z2RZRtd(ptr %arrayidx18, double %7)
  %arrayidx16 = getelementptr inbounds [4 x i16], ptr @QubitReg, i64 0, i64 2
  %arrayidx17 = getelementptr inbounds [4 x i16], ptr @QubitReg, i64 0, i64 0
  call void @_Z4CNOTRtS_(ptr %arrayidx16, ptr %arrayidx17)
  %arrayidx14 = getelementptr inbounds [4 x i16], ptr @QubitReg, i64 0, i64 3
  %arrayidx15 = getelementptr inbounds [4 x i16], ptr @QubitReg, i64 0, i64 1
  call void @_Z4CNOTRtS_(ptr %arrayidx14, ptr %arrayidx15)
  %arrayidx13 = getelementptr inbounds [4 x i16], ptr @QubitReg, i64 0, i64 0
  call void @_Z2RYRtd(ptr %arrayidx13, double 0x3FF921FB5443D6F4)
  %arrayidx12 = getelementptr inbounds [4 x i16], ptr @QubitReg, i64 0, i64 1
  call void @_Z2RYRtd(ptr %arrayidx12, double 0x3FF921FB5443D6F4)
  %arrayidx11 = getelementptr inbounds [4 x i16], ptr @QubitReg, i64 0, i64 2
  call void @_Z2RYRtd(ptr %arrayidx11, double 0x3FF921FB5443D6F4)
  %arrayidx10 = getelementptr inbounds [4 x i16], ptr @QubitReg, i64 0, i64 3
  call void @_Z2RYRtd(ptr %arrayidx10, double 0x3FF921FB5443D6F4)
  %arrayidx8 = getelementptr inbounds [4 x i16], ptr @QubitReg, i64 0, i64 0
  %arrayidx9 = getelementptr inbounds [4 x i16], ptr @QubitReg, i64 0, i64 2
  call void @_Z4CNOTRtS_(ptr %arrayidx8, ptr %arrayidx9)
  %arrayidx6 = getelementptr inbounds [4 x i16], ptr @QubitReg, i64 0, i64 1
  %arrayidx7 = getelementptr inbounds [4 x i16], ptr @QubitReg, i64 0, i64 3
  call void @_Z4CNOTRtS_(ptr %arrayidx6, ptr %arrayidx7)
  %arrayidx5 = getelementptr inbounds [4 x i16], ptr @QubitReg, i64 0, i64 2
  call void @_Z2RZRtd(ptr %arrayidx5, double %8)
  %arrayidx4 = getelementptr inbounds [4 x i16], ptr @QubitReg, i64 0, i64 3
  call void @_Z2RZRtd(ptr %arrayidx4, double %9)
  %arrayidx2 = getelementptr inbounds [4 x i16], ptr @QubitReg, i64 0, i64 0
  %arrayidx3 = getelementptr inbounds [4 x i16], ptr @QubitReg, i64 0, i64 2
  call void @_Z4CNOTRtS_(ptr %arrayidx2, ptr %arrayidx3)
  %arrayidx = getelementptr inbounds [4 x i16], ptr @QubitReg, i64 0, i64 1
  %arrayidx1 = getelementptr inbounds [4 x i16], ptr @QubitReg, i64 0, i64 3
  call void @_Z4CNOTRtS_(ptr %arrayidx, ptr %arrayidx1)
  br label %aqcc.meas.move.end

aqcc.meas.move.end:                               ; preds = %aqcc.quantum
  ret void
}

; Function Attrs: mustprogress noinline nounwind uwtable
define dso_local void @_Z8prepZAllv.stub() local_unnamed_addr #0 section ".qbbs_text" {
entry:
  br label %aqcc.quantum

aqcc.quantum:                                     ; preds = %entry
  %arrayidx3 = getelementptr inbounds [4 x i16], ptr @QubitReg, i64 0, i64 0
  call void @_Z5PrepZRt(ptr %arrayidx3)
  %arrayidx2 = getelementptr inbounds [4 x i16], ptr @QubitReg, i64 0, i64 1
  call void @_Z5PrepZRt(ptr %arrayidx2)
  %arrayidx1 = getelementptr inbounds [4 x i16], ptr @QubitReg, i64 0, i64 2
  call void @_Z5PrepZRt(ptr %arrayidx1)
  %arrayidx = getelementptr inbounds [4 x i16], ptr @QubitReg, i64 0, i64 3
  call void @_Z5PrepZRt(ptr %arrayidx)
  br label %aqcc.meas.move.end

aqcc.meas.move.end:                               ; preds = %aqcc.quantum
  ret void
}

; Function Attrs: mustprogress noinline nounwind uwtable
define dso_local void @_Z8measZAllv.stub() local_unnamed_addr #0 section ".qbbs_text" {
entry:
  br label %aqcc.quantum

aqcc.quantum:                                     ; preds = %entry
  %arrayidx3 = getelementptr inbounds [4 x i16], ptr @QubitReg, i64 0, i64 0
  call void @_Z5MeasZRtRbb(ptr %arrayidx3, ptr @CReg, i1 false)
  %arrayidx2 = getelementptr inbounds [4 x i16], ptr @QubitReg, i64 0, i64 1
  call void @_Z5MeasZRtRbb(ptr %arrayidx2, ptr getelementptr inbounds ([4 x i8], ptr @CReg, i64 0, i64 1), i1 false)
  %arrayidx1 = getelementptr inbounds [4 x i16], ptr @QubitReg, i64 0, i64 2
  call void @_Z5MeasZRtRbb(ptr %arrayidx1, ptr getelementptr inbounds ([4 x i8], ptr @CReg, i64 0, i64 2), i1 false)
  %arrayidx = getelementptr inbounds [4 x i16], ptr @QubitReg, i64 0, i64 3
  call void @_Z5MeasZRtRbb(ptr %arrayidx, ptr getelementptr inbounds ([4 x i8], ptr @CReg, i64 0, i64 3), i1 false)
  br label %aqcc.meas.move.end

aqcc.meas.move.end:                               ; preds = %aqcc.quantum
  ret void
}

; Function Attrs: norecurse uwtable
define dso_local noundef i32 @main() local_unnamed_addr #7 personality ptr @__gxx_personality_v0 {
entry:
  %iqs_config = alloca %"struct.iqsdk::IqsConfig", align 8
  %agg.tmp = alloca %"class.std::__cxx11::basic_string", align 8
  %iqs_device = alloca %"class.iqsdk::FullStateSimulator", align 8
  %ProbabilityRegister = alloca %"class.std::vector.9", align 8
  %qids = alloca %"class.std::vector.14", align 8
  %ref.tmp25 = alloca %"class.std::vector.9", align 8
  call void @llvm.lifetime.start.p0(i64 noundef 296, ptr nocapture noundef nonnull align 8 dereferenceable(296) %iqs_config) #16
  %0 = getelementptr inbounds %"class.std::__cxx11::basic_string", ptr %agg.tmp, i64 0, i32 2
  store ptr %0, ptr %agg.tmp, align 8, !tbaa !16
  call void @llvm.memcpy.p0.p0.i64(ptr noundef nonnull writeonly align 8 dereferenceable(9) %0, ptr noundef nonnull readonly align 1 dereferenceable(9) @.str.27, i64 noundef 9, i1 noundef false)
  %_M_string_length.i.i.i.i = getelementptr inbounds %"class.std::__cxx11::basic_string", ptr %agg.tmp, i64 0, i32 1
  store i64 9, ptr %_M_string_length.i.i.i.i, align 8, !tbaa !17
  %arrayidx.i.i.i = getelementptr inbounds %"class.std::__cxx11::basic_string", ptr %agg.tmp, i64 0, i32 2, i32 1, i64 1
  store i8 0, ptr %arrayidx.i.i.i, align 1, !tbaa !18
  %call = call i64 @time(ptr noundef null) #16
  invoke void @_ZN5iqsdk9IqsConfigC1EiNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEEbmbd(ptr noundef nonnull align 8 dereferenceable(296) %iqs_config, i32 noundef 1, ptr noundef nonnull %agg.tmp, i1 noundef zeroext false, i64 noundef %call, i1 noundef zeroext true, double noundef 1.000000e-02)
          to label %invoke.cont2 unwind label %lpad1

invoke.cont2:                                     ; preds = %entry
  %1 = load ptr, ptr %agg.tmp, align 8, !tbaa !5
  %cmp.i.i.i = icmp eq ptr %1, %0
  br i1 %cmp.i.i.i, label %_ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEED2Ev.exit, label %if.then.i.i

if.then.i.i:                                      ; preds = %invoke.cont2
  call void @_ZdlPv(ptr noundef %1) #16
  br label %_ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEED2Ev.exit

_ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEED2Ev.exit: ; preds = %if.then.i.i, %invoke.cont2
  %num_qubits = getelementptr inbounds %"struct.iqsdk::IqsConfig", ptr %iqs_config, i64 0, i32 1
  store i32 4, ptr %num_qubits, align 4, !tbaa !19
  call void @llvm.lifetime.start.p0(i64 noundef 32, ptr noundef nonnull %iqs_device) #16
  invoke void @_ZN5iqsdk18FullStateSimulatorC1ERNS_12DeviceConfigE(ptr noundef nonnull align 8 dereferenceable(32) %iqs_device, ptr noundef nonnull align 8 dereferenceable(296) %iqs_config)
          to label %invoke.cont5 unwind label %lpad4

invoke.cont5:                                     ; preds = %_ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEED2Ev.exit
  %call8 = invoke noundef i32 @_ZN5iqsdk6Device5readyEv(ptr noundef nonnull align 8 dereferenceable(32) %iqs_device)
          to label %invoke.cont7 unwind label %lpad6

invoke.cont7:                                     ; preds = %invoke.cont5
  store <2 x double> <double 0x3FBC71C71C708E5C, double 0x3FCC71C71C708E5C>, ptr @QuantumVariableParams, align 16, !tbaa !14
  store <2 x double> <double 0x3FD5555555546AC5, double 0x3FDC71C71C708E5C>, ptr getelementptr inbounds ([4 x double], ptr @QuantumVariableParams, i64 0, i64 2), align 16, !tbaa !14
  %_M_finish.i.i = getelementptr inbounds %"struct.std::_Vector_base<std::reference_wrapper<unsigned short>, std::allocator<std::reference_wrapper<unsigned short>>>::_Vector_impl_data", ptr %qids, i64 0, i32 1
  %_M_end_of_storage.i.i = getelementptr inbounds %"struct.std::_Vector_base<std::reference_wrapper<unsigned short>, std::allocator<std::reference_wrapper<unsigned short>>>::_Vector_impl_data", ptr %qids, i64 0, i32 2
  %_M_end_of_storage.i.i.i.i = getelementptr inbounds %"struct.std::_Vector_base<double, std::allocator<double>>::_Vector_impl_data", ptr %ProbabilityRegister, i64 0, i32 2
  %_M_end_of_storage.i5.i.i.i = getelementptr inbounds %"struct.std::_Vector_base<double, std::allocator<double>>::_Vector_impl_data", ptr %ref.tmp25, i64 0, i32 2
  br label %while.cond

while.cond:                                       ; preds = %_ZNSt6vectorIdSaIdEED2Ev.exit88, %invoke.cont7
  %RunCount.0 = phi i32 [ 0, %invoke.cont7 ], [ %inc, %_ZNSt6vectorIdSaIdEED2Ev.exit88 ]
  %Optimized.0 = phi i8 [ 0, %invoke.cont7 ], [ %Optimized.2, %_ZNSt6vectorIdSaIdEED2Ev.exit88 ]
  %2 = and i8 %Optimized.0, 1
  %tobool.not = icmp eq i8 %2, 0
  br i1 %tobool.not, label %while.body, label %while.end

while.body:                                       ; preds = %while.cond
  %call1.i76 = invoke noundef nonnull align 8 dereferenceable(8) ptr @_ZSt16__ostream_insertIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_PKS3_l(ptr noundef nonnull align 8 dereferenceable(8) @_ZSt4cout, ptr noundef nonnull @.str.28, i64 noundef 25)
          to label %invoke.cont10 unwind label %lpad9

invoke.cont10:                                    ; preds = %while.body
  %inc = add nuw nsw i32 %RunCount.0, 1
  %call13 = invoke noundef nonnull align 8 dereferenceable(8) ptr @_ZNSolsEi(ptr noundef nonnull align 8 dereferenceable(8) @_ZSt4cout, i32 noundef %RunCount.0)
          to label %invoke.cont12 unwind label %lpad9

invoke.cont12:                                    ; preds = %invoke.cont10
  %call1.i78 = invoke noundef nonnull align 8 dereferenceable(8) ptr @_ZSt16__ostream_insertIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_PKS3_l(ptr noundef nonnull align 8 dereferenceable(8) %call13, ptr noundef nonnull @.str.29, i64 noundef 15)
          to label %invoke.cont14 unwind label %lpad9

invoke.cont14:                                    ; preds = %invoke.cont12
  call void @_Z8prepZAllv.stub() #16
  call void @_Z10tfdQ4Setupv.stub() #16
  call void @llvm.lifetime.start.p0(i64 noundef 24, ptr noundef nonnull %ProbabilityRegister) #16
  call void @llvm.memset.p0.i64(ptr noundef nonnull align 8 dereferenceable(24) %ProbabilityRegister, i8 noundef 0, i64 noundef 24, i1 noundef false)
  call void @llvm.lifetime.start.p0(i64 noundef 24, ptr noundef nonnull %qids) #16
  call void @llvm.memset.p0.i64(ptr noundef nonnull align 8 dereferenceable(24) %qids, i8 noundef 0, i64 noundef 24, i1 noundef false)
  br label %for.body

for.cond.cleanup:                                 ; preds = %invoke.cont21
  call void @llvm.lifetime.start.p0(i64 noundef 24, ptr noundef nonnull %ref.tmp25) #16
  invoke void @_ZN5iqsdk15SimulatorDevice16getProbabilitiesERSt6vectorISt17reference_wrapperItESaIS3_EE(ptr noundef nonnull sret(%"class.std::vector.9") align 8 %ref.tmp25, ptr noundef nonnull align 8 dereferenceable(32) %iqs_device, ptr noundef nonnull align 8 dereferenceable(24) %qids)
          to label %invoke.cont27 unwind label %lpad26

lpad1:                                            ; preds = %entry
  %3 = landingpad { ptr, i32 }
          cleanup
  %4 = load ptr, ptr %agg.tmp, align 8, !tbaa !5
  %cmp.i.i.i80 = icmp eq ptr %4, %0
  br i1 %cmp.i.i.i80, label %ehcleanup62, label %if.then.i.i81

if.then.i.i81:                                    ; preds = %lpad1
  call void @_ZdlPv(ptr noundef %4) #16
  br label %ehcleanup62

lpad4:                                            ; preds = %_ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEED2Ev.exit
  %5 = landingpad { ptr, i32 }
          cleanup
  br label %ehcleanup58

lpad6:                                            ; preds = %invoke.cont5
  %6 = landingpad { ptr, i32 }
          cleanup
  br label %ehcleanup56

lpad9:                                            ; preds = %invoke.cont12, %invoke.cont10, %while.body
  %7 = landingpad { ptr, i32 }
          cleanup
  br label %ehcleanup56

for.body:                                         ; preds = %invoke.cont21, %invoke.cont14
  %8 = phi ptr [ null, %invoke.cont14 ], [ %24, %invoke.cont21 ]
  %indvars.iv = phi i64 [ 0, %invoke.cont14 ], [ %indvars.iv.next, %invoke.cont21 ]
  %arrayidx = getelementptr inbounds [4 x i16], ptr @QubitReg, i64 0, i64 %indvars.iv
  %9 = ptrtoint ptr %8 to i64
  %10 = load ptr, ptr %_M_end_of_storage.i.i, align 8, !tbaa !29
  %cmp.not.i.i = icmp eq ptr %8, %10
  br i1 %cmp.not.i.i, label %if.else.i.i, label %if.then.i.i83

if.then.i.i83:                                    ; preds = %for.body
  %11 = ptrtoint ptr %arrayidx to i64
  store i64 %11, ptr %8, align 8
  %12 = load ptr, ptr %_M_finish.i.i, align 8, !tbaa !31
  %incdec.ptr.i.i = getelementptr inbounds %"class.std::reference_wrapper", ptr %12, i64 1
  store ptr %incdec.ptr.i.i, ptr %_M_finish.i.i, align 8, !tbaa !31
  br label %invoke.cont21

if.else.i.i:                                      ; preds = %for.body
  %13 = load ptr, ptr %qids, align 8, !tbaa !32
  %14 = ptrtoint ptr %13 to i64
  %sub.ptr.sub.i.i.i = sub i64 %9, %14
  %cmp.i.i = icmp eq i64 %sub.ptr.sub.i.i.i, 9223372036854775800
  br i1 %cmp.i.i, label %if.then.i.i97, label %_ZNKSt6vectorISt17reference_wrapperItESaIS1_EE12_M_check_lenEmPKc.exit.i

if.then.i.i97:                                    ; preds = %if.else.i.i
  invoke void @_ZSt20__throw_length_errorPKc(ptr noundef nonnull @.str.31) #17
          to label %.noexc98 unwind label %lpad20.loopexit.split-lp

.noexc98:                                         ; preds = %if.then.i.i97
  unreachable

_ZNKSt6vectorISt17reference_wrapperItESaIS1_EE12_M_check_lenEmPKc.exit.i: ; preds = %if.else.i.i
  %sub.ptr.div.i.i.i = ashr exact i64 %sub.ptr.sub.i.i.i, 3
  %.sroa.speculated.i.i = call i64 @llvm.umax.i64(i64 %sub.ptr.div.i.i.i, i64 noundef 1), !range !33
  %add.i.i = add i64 %.sroa.speculated.i.i, %sub.ptr.div.i.i.i
  %cmp7.i.i = icmp ult i64 %add.i.i, %sub.ptr.div.i.i.i
  %cmp9.i.i = icmp ugt i64 %add.i.i, 1152921504606846975
  %or.cond.i.i = or i1 %cmp7.i.i, %cmp9.i.i
  %cond.i.i = select i1 %or.cond.i.i, i64 1152921504606846975, i64 %add.i.i
  %cmp.not.i.i95 = icmp eq i64 %cond.i.i, 0
  br i1 %cmp.not.i.i95, label %_ZNSt12_Vector_baseISt17reference_wrapperItESaIS1_EE11_M_allocateEm.exit.i, label %_ZNSt16allocator_traitsISaISt17reference_wrapperItEEE8allocateERS2_m.exit.i.i

_ZNSt16allocator_traitsISaISt17reference_wrapperItEEE8allocateERS2_m.exit.i.i: ; preds = %_ZNKSt6vectorISt17reference_wrapperItESaIS1_EE12_M_check_lenEmPKc.exit.i
  %mul.i.i.i.i = shl i64 %cond.i.i, 3
  %call5.i.i.i.i99 = invoke noalias noundef nonnull ptr @_Znwm(i64 noundef %mul.i.i.i.i) #18
          to label %_ZNSt12_Vector_baseISt17reference_wrapperItESaIS1_EE11_M_allocateEm.exit.i unwind label %lpad20.loopexit

_ZNSt12_Vector_baseISt17reference_wrapperItESaIS1_EE11_M_allocateEm.exit.i: ; preds = %_ZNSt16allocator_traitsISaISt17reference_wrapperItEEE8allocateERS2_m.exit.i.i, %_ZNKSt6vectorISt17reference_wrapperItESaIS1_EE12_M_check_lenEmPKc.exit.i
  %cond.i31.i = phi ptr [ null, %_ZNKSt6vectorISt17reference_wrapperItESaIS1_EE12_M_check_lenEmPKc.exit.i ], [ %call5.i.i.i.i99, %_ZNSt16allocator_traitsISaISt17reference_wrapperItEEE8allocateERS2_m.exit.i.i ]
  %add.ptr.i96 = getelementptr inbounds %"class.std::reference_wrapper", ptr %cond.i31.i, i64 %sub.ptr.div.i.i.i
  %15 = ptrtoint ptr %arrayidx to i64
  store i64 %15, ptr %add.ptr.i96, align 8
  %cmp.not6.i.i.i.i.i = icmp eq ptr %13, %8
  br i1 %cmp.not6.i.i.i.i.i, label %_ZNSt6vectorISt17reference_wrapperItESaIS1_EE11_S_relocateEPS1_S4_S4_RS2_.exit.i, label %for.body.i.i.i.i.i.preheader

for.body.i.i.i.i.i.preheader:                     ; preds = %_ZNSt12_Vector_baseISt17reference_wrapperItESaIS1_EE11_M_allocateEm.exit.i
  %16 = add i64 %9, -8
  %17 = sub i64 %16, %14
  %18 = lshr i64 %17, 3
  %19 = add nuw nsw i64 %18, 1
  %min.iters.check = icmp ult i64 %17, 8
  br i1 %min.iters.check, label %scalar.ph, label %vector.ph

vector.ph:                                        ; preds = %for.body.i.i.i.i.i.preheader
  %n.vec = and i64 %19, 4611686018427387902
  %20 = shl i64 %n.vec, 3
  br label %vector.body

vector.body:                                      ; preds = %vector.body, %vector.ph
  %index = phi i64 [ 0, %vector.ph ], [ %index.next, %vector.body ]
  %21 = shl i64 %index, 3
  %next.gep = getelementptr i8, ptr %cond.i31.i, i64 %21
  %next.gep3 = getelementptr i8, ptr %13, i64 %21
  call void @llvm.experimental.noalias.scope.decl(metadata !34)
  call void @llvm.experimental.noalias.scope.decl(metadata !37)
  %wide.load = load <2 x i64>, ptr %next.gep3, align 8, !alias.scope !37, !noalias !34
  store <2 x i64> %wide.load, ptr %next.gep, align 8, !alias.scope !34, !noalias !37
  %index.next = add nuw i64 %index, 2
  %22 = icmp eq i64 %index.next, %n.vec
  br i1 %22, label %middle.block, label %vector.body, !llvm.loop !39

middle.block:                                     ; preds = %vector.body
  %ind.end = getelementptr i8, ptr %cond.i31.i, i64 %20
  %ind.end1 = getelementptr i8, ptr %13, i64 %20
  %cmp.n = icmp eq i64 %19, %n.vec
  br i1 %cmp.n, label %_ZNSt6vectorISt17reference_wrapperItESaIS1_EE11_S_relocateEPS1_S4_S4_RS2_.exit.i.thread, label %scalar.ph

_ZNSt6vectorISt17reference_wrapperItESaIS1_EE11_S_relocateEPS1_S4_S4_RS2_.exit.i.thread: ; preds = %middle.block
  %incdec.ptr.i11 = getelementptr %"class.std::reference_wrapper", ptr %ind.end, i64 1
  br label %if.then.i41.i

scalar.ph:                                        ; preds = %middle.block, %for.body.i.i.i.i.i.preheader
  %bc.resume.val = phi ptr [ %ind.end, %middle.block ], [ %cond.i31.i, %for.body.i.i.i.i.i.preheader ]
  %bc.resume.val2 = phi ptr [ %ind.end1, %middle.block ], [ %13, %for.body.i.i.i.i.i.preheader ]
  br label %for.body.i.i.i.i.i

for.body.i.i.i.i.i:                               ; preds = %for.body.i.i.i.i.i, %scalar.ph
  %__cur.08.i.i.i.i.i = phi ptr [ %incdec.ptr1.i.i.i.i.i, %for.body.i.i.i.i.i ], [ %bc.resume.val, %scalar.ph ]
  %__first.addr.07.i.i.i.i.i = phi ptr [ %incdec.ptr.i.i.i.i.i, %for.body.i.i.i.i.i ], [ %bc.resume.val2, %scalar.ph ]
  call void @llvm.experimental.noalias.scope.decl(metadata !34)
  call void @llvm.experimental.noalias.scope.decl(metadata !37)
  %23 = load i64, ptr %__first.addr.07.i.i.i.i.i, align 8, !alias.scope !37, !noalias !34
  store i64 %23, ptr %__cur.08.i.i.i.i.i, align 8, !alias.scope !34, !noalias !37
  %incdec.ptr.i.i.i.i.i = getelementptr inbounds %"class.std::reference_wrapper", ptr %__first.addr.07.i.i.i.i.i, i64 1
  %incdec.ptr1.i.i.i.i.i = getelementptr inbounds %"class.std::reference_wrapper", ptr %__cur.08.i.i.i.i.i, i64 1
  %cmp.not.i.i.i.i.i = icmp eq ptr %incdec.ptr.i.i.i.i.i, %8
  br i1 %cmp.not.i.i.i.i.i, label %_ZNSt6vectorISt17reference_wrapperItESaIS1_EE11_S_relocateEPS1_S4_S4_RS2_.exit.i, label %for.body.i.i.i.i.i, !llvm.loop !44

_ZNSt6vectorISt17reference_wrapperItESaIS1_EE11_S_relocateEPS1_S4_S4_RS2_.exit.i: ; preds = %for.body.i.i.i.i.i, %_ZNSt12_Vector_baseISt17reference_wrapperItESaIS1_EE11_M_allocateEm.exit.i
  %__cur.0.lcssa.i.i.i.i.i = phi ptr [ %cond.i31.i, %_ZNSt12_Vector_baseISt17reference_wrapperItESaIS1_EE11_M_allocateEm.exit.i ], [ %incdec.ptr1.i.i.i.i.i, %for.body.i.i.i.i.i ]
  %incdec.ptr.i = getelementptr %"class.std::reference_wrapper", ptr %__cur.0.lcssa.i.i.i.i.i, i64 1
  %tobool.not.i.i = icmp eq ptr %13, null
  br i1 %tobool.not.i.i, label %_ZNSt6vectorISt17reference_wrapperItESaIS1_EE17_M_realloc_insertIJS1_EEEvN9__gnu_cxx17__normal_iteratorIPS1_S3_EEDpOT_.exit, label %if.then.i41.i

if.then.i41.i:                                    ; preds = %_ZNSt6vectorISt17reference_wrapperItESaIS1_EE11_S_relocateEPS1_S4_S4_RS2_.exit.i, %_ZNSt6vectorISt17reference_wrapperItESaIS1_EE11_S_relocateEPS1_S4_S4_RS2_.exit.i.thread
  %incdec.ptr.i13 = phi ptr [ %incdec.ptr.i11, %_ZNSt6vectorISt17reference_wrapperItESaIS1_EE11_S_relocateEPS1_S4_S4_RS2_.exit.i.thread ], [ %incdec.ptr.i, %_ZNSt6vectorISt17reference_wrapperItESaIS1_EE11_S_relocateEPS1_S4_S4_RS2_.exit.i ]
  call void @_ZdlPv(ptr noundef nonnull %13) #16
  br label %_ZNSt6vectorISt17reference_wrapperItESaIS1_EE17_M_realloc_insertIJS1_EEEvN9__gnu_cxx17__normal_iteratorIPS1_S3_EEDpOT_.exit

_ZNSt6vectorISt17reference_wrapperItESaIS1_EE17_M_realloc_insertIJS1_EEEvN9__gnu_cxx17__normal_iteratorIPS1_S3_EEDpOT_.exit: ; preds = %if.then.i41.i, %_ZNSt6vectorISt17reference_wrapperItESaIS1_EE11_S_relocateEPS1_S4_S4_RS2_.exit.i
  %incdec.ptr.i14 = phi ptr [ %incdec.ptr.i13, %if.then.i41.i ], [ %incdec.ptr.i, %_ZNSt6vectorISt17reference_wrapperItESaIS1_EE11_S_relocateEPS1_S4_S4_RS2_.exit.i ]
  store ptr %cond.i31.i, ptr %qids, align 8, !tbaa !45
  store ptr %incdec.ptr.i14, ptr %_M_finish.i.i, align 8, !tbaa !31
  %add.ptr19.i = getelementptr inbounds %"class.std::reference_wrapper", ptr %cond.i31.i, i64 %cond.i.i
  store ptr %add.ptr19.i, ptr %_M_end_of_storage.i.i, align 8, !tbaa !29
  br label %invoke.cont21

invoke.cont21:                                    ; preds = %_ZNSt6vectorISt17reference_wrapperItESaIS1_EE17_M_realloc_insertIJS1_EEEvN9__gnu_cxx17__normal_iteratorIPS1_S3_EEDpOT_.exit, %if.then.i.i83
  %24 = phi ptr [ %incdec.ptr.i14, %_ZNSt6vectorISt17reference_wrapperItESaIS1_EE17_M_realloc_insertIJS1_EEEvN9__gnu_cxx17__normal_iteratorIPS1_S3_EEDpOT_.exit ], [ %incdec.ptr.i.i, %if.then.i.i83 ]
  %indvars.iv.next = add nuw nsw i64 %indvars.iv, 1
  %exitcond.not = icmp eq i64 %indvars.iv.next, 4
  br i1 %exitcond.not, label %for.cond.cleanup, label %for.body, !llvm.loop !46

lpad20.loopexit:                                  ; preds = %_ZNSt16allocator_traitsISaISt17reference_wrapperItEEE8allocateERS2_m.exit.i.i
  %lpad.loopexit = landingpad { ptr, i32 }
          cleanup
  br label %ehcleanup42

lpad20.loopexit.split-lp:                         ; preds = %if.then.i.i97
  %lpad.loopexit.split-lp = landingpad { ptr, i32 }
          cleanup
  br label %ehcleanup42

invoke.cont27:                                    ; preds = %for.cond.cleanup
  %25 = load ptr, ptr %ProbabilityRegister, align 8, !tbaa !47
  %26 = load <2 x ptr>, ptr %ref.tmp25, align 8, !tbaa !32
  store <2 x ptr> %26, ptr %ProbabilityRegister, align 8, !tbaa !32
  %27 = load ptr, ptr %_M_end_of_storage.i5.i.i.i, align 8, !tbaa !49
  store ptr %27, ptr %_M_end_of_storage.i.i.i.i, align 8, !tbaa !49
  %tobool.not.i.i.i.i.i = icmp eq ptr %25, null
  call void @llvm.memset.p0.i64(ptr noundef nonnull align 8 dereferenceable(24) %ref.tmp25, i8 noundef 0, i64 noundef 24, i1 noundef false)
  br i1 %tobool.not.i.i.i.i.i, label %_ZNSt6vectorIdSaIdEED2Ev.exit, label %_ZNSt6vectorIdSaIdEEaSEOS1_.exit

_ZNSt6vectorIdSaIdEEaSEOS1_.exit:                 ; preds = %invoke.cont27
  call void @_ZdlPv(ptr noundef nonnull %25) #16
  %.pr = load ptr, ptr %ref.tmp25, align 8, !tbaa !47
  %tobool.not.i.i.i = icmp eq ptr %.pr, null
  br i1 %tobool.not.i.i.i, label %_ZNSt6vectorIdSaIdEED2Ev.exit, label %if.then.i.i.i

if.then.i.i.i:                                    ; preds = %_ZNSt6vectorIdSaIdEEaSEOS1_.exit
  call void @_ZdlPv(ptr noundef nonnull %.pr) #16
  br label %_ZNSt6vectorIdSaIdEED2Ev.exit

_ZNSt6vectorIdSaIdEED2Ev.exit:                    ; preds = %if.then.i.i.i, %_ZNSt6vectorIdSaIdEEaSEOS1_.exit, %invoke.cont27
  call void @llvm.lifetime.end.p0(i64 noundef 24, ptr noundef nonnull %ref.tmp25) #16
  invoke void @_ZN5iqsdk15SimulatorDevice20displayProbabilitiesERSt6vectorIdSaIdEERS1_ISt17reference_wrapperItESaIS6_EE(ptr noundef nonnull align 8 dereferenceable(24) %ProbabilityRegister, ptr noundef nonnull align 8 dereferenceable(24) %qids)
          to label %invoke.cont31 unwind label %lpad30

invoke.cont31:                                    ; preds = %_ZNSt6vectorIdSaIdEED2Ev.exit
  call void @_Z8measZAllv.stub() #16
  %28 = load i8, ptr @CReg, align 1, !tbaa !50, !range !51, !noundef !52
  %tobool33.not = icmp eq i8 %28, 0
  %29 = load i8, ptr getelementptr inbounds ([4 x i8], ptr @CReg, i64 0, i64 1), align 1, !range !51
  %tobool34.not = icmp eq i8 %29, 0
  %or.cond = select i1 %tobool33.not, i1 true, i1 %tobool34.not
  br i1 %or.cond, label %if.else, label %cleanup

lpad26:                                           ; preds = %for.cond.cleanup
  %30 = landingpad { ptr, i32 }
          cleanup
  call void @llvm.lifetime.end.p0(i64 noundef 24, ptr noundef nonnull %ref.tmp25) #16
  br label %ehcleanup42

lpad30:                                           ; preds = %_ZNSt6vectorIdSaIdEED2Ev.exit
  %31 = landingpad { ptr, i32 }
          cleanup
  br label %ehcleanup42

if.else:                                          ; preds = %invoke.cont31
  %cmp35 = icmp eq i32 %inc, 4
  br i1 %cmp35, label %cleanup, label %if.else37

if.else37:                                        ; preds = %if.else
  %32 = load <2 x double>, ptr @QuantumVariableParams, align 16, !tbaa !14
  %33 = fmul <2 x double> %32, <double 2.000000e+00, double 2.000000e+00>
  store <2 x double> %33, ptr @QuantumVariableParams, align 16, !tbaa !14
  %34 = load <2 x double>, ptr getelementptr inbounds ([4 x double], ptr @QuantumVariableParams, i64 0, i64 2), align 16, !tbaa !14
  %35 = fmul <2 x double> %34, <double 2.000000e+00, double 2.000000e+00>
  store <2 x double> %35, ptr getelementptr inbounds ([4 x double], ptr @QuantumVariableParams, i64 0, i64 2), align 16, !tbaa !14
  br label %cleanup

cleanup:                                          ; preds = %if.else37, %if.else, %invoke.cont31
  %Optimized.2 = phi i8 [ %Optimized.0, %if.else ], [ %Optimized.0, %if.else37 ], [ 1, %invoke.cont31 ]
  %switch = phi i1 [ false, %if.else ], [ true, %if.else37 ], [ true, %invoke.cont31 ]
  %36 = load ptr, ptr %qids, align 8, !tbaa !45
  %tobool.not.i.i.i84 = icmp eq ptr %36, null
  br i1 %tobool.not.i.i.i84, label %_ZNSt6vectorISt17reference_wrapperItESaIS1_EED2Ev.exit, label %if.then.i.i.i85

if.then.i.i.i85:                                  ; preds = %cleanup
  call void @_ZdlPv(ptr noundef nonnull %36) #16
  br label %_ZNSt6vectorISt17reference_wrapperItESaIS1_EED2Ev.exit

_ZNSt6vectorISt17reference_wrapperItESaIS1_EED2Ev.exit: ; preds = %if.then.i.i.i85, %cleanup
  call void @llvm.lifetime.end.p0(i64 noundef 24, ptr noundef nonnull %qids) #16
  %37 = load ptr, ptr %ProbabilityRegister, align 8, !tbaa !47
  %tobool.not.i.i.i86 = icmp eq ptr %37, null
  br i1 %tobool.not.i.i.i86, label %_ZNSt6vectorIdSaIdEED2Ev.exit88, label %if.then.i.i.i87

if.then.i.i.i87:                                  ; preds = %_ZNSt6vectorISt17reference_wrapperItESaIS1_EED2Ev.exit
  call void @_ZdlPv(ptr noundef nonnull %37) #16
  br label %_ZNSt6vectorIdSaIdEED2Ev.exit88

_ZNSt6vectorIdSaIdEED2Ev.exit88:                  ; preds = %if.then.i.i.i87, %_ZNSt6vectorISt17reference_wrapperItESaIS1_EED2Ev.exit
  call void @llvm.lifetime.end.p0(i64 noundef 24, ptr noundef nonnull %ProbabilityRegister) #16
  br i1 %switch, label %while.cond, label %while.end

ehcleanup42:                                      ; preds = %lpad30, %lpad26, %lpad20.loopexit.split-lp, %lpad20.loopexit
  %.pn68 = phi { ptr, i32 } [ %31, %lpad30 ], [ %30, %lpad26 ], [ %lpad.loopexit, %lpad20.loopexit ], [ %lpad.loopexit.split-lp, %lpad20.loopexit.split-lp ]
  %38 = load ptr, ptr %qids, align 8, !tbaa !45
  %tobool.not.i.i.i89 = icmp eq ptr %38, null
  br i1 %tobool.not.i.i.i89, label %_ZNSt6vectorISt17reference_wrapperItESaIS1_EED2Ev.exit91, label %if.then.i.i.i90

if.then.i.i.i90:                                  ; preds = %ehcleanup42
  call void @_ZdlPv(ptr noundef nonnull %38) #16
  br label %_ZNSt6vectorISt17reference_wrapperItESaIS1_EED2Ev.exit91

_ZNSt6vectorISt17reference_wrapperItESaIS1_EED2Ev.exit91: ; preds = %if.then.i.i.i90, %ehcleanup42
  call void @llvm.lifetime.end.p0(i64 noundef 24, ptr noundef nonnull %qids) #16
  %39 = load ptr, ptr %ProbabilityRegister, align 8, !tbaa !47
  %tobool.not.i.i.i92 = icmp eq ptr %39, null
  br i1 %tobool.not.i.i.i92, label %_ZNSt6vectorIdSaIdEED2Ev.exit94, label %if.then.i.i.i93

if.then.i.i.i93:                                  ; preds = %_ZNSt6vectorISt17reference_wrapperItESaIS1_EED2Ev.exit91
  call void @_ZdlPv(ptr noundef nonnull %39) #16
  br label %_ZNSt6vectorIdSaIdEED2Ev.exit94

_ZNSt6vectorIdSaIdEED2Ev.exit94:                  ; preds = %if.then.i.i.i93, %_ZNSt6vectorISt17reference_wrapperItESaIS1_EED2Ev.exit91
  call void @llvm.lifetime.end.p0(i64 noundef 24, ptr noundef nonnull %ProbabilityRegister) #16
  br label %ehcleanup56

while.end:                                        ; preds = %_ZNSt6vectorIdSaIdEED2Ev.exit88, %while.cond
  call void @_ZN5iqsdk6DeviceD2Ev(ptr noundef nonnull align 8 dereferenceable(32) %iqs_device) #16
  call void @llvm.lifetime.end.p0(i64 noundef 32, ptr noundef nonnull %iqs_device) #16
  call void @_ZN5iqsdk9IqsConfigD2Ev(ptr noundef nonnull align 8 dereferenceable(296) %iqs_config) #16
  call void @llvm.lifetime.end.p0(i64 noundef 296, ptr noundef nonnull %iqs_config) #16
  ret i32 0

ehcleanup56:                                      ; preds = %_ZNSt6vectorIdSaIdEED2Ev.exit94, %lpad9, %lpad6
  %.pn68.pn.pn = phi { ptr, i32 } [ %6, %lpad6 ], [ %.pn68, %_ZNSt6vectorIdSaIdEED2Ev.exit94 ], [ %7, %lpad9 ]
  call void @_ZN5iqsdk6DeviceD2Ev(ptr noundef nonnull align 8 dereferenceable(32) %iqs_device) #16
  br label %ehcleanup58

ehcleanup58:                                      ; preds = %ehcleanup56, %lpad4
  %.pn68.pn.pn.pn = phi { ptr, i32 } [ %.pn68.pn.pn, %ehcleanup56 ], [ %5, %lpad4 ]
  call void @llvm.lifetime.end.p0(i64 noundef 32, ptr noundef nonnull %iqs_device) #16
  call void @_ZN5iqsdk9IqsConfigD2Ev(ptr noundef nonnull align 8 dereferenceable(296) %iqs_config) #16
  br label %ehcleanup62

ehcleanup62:                                      ; preds = %ehcleanup58, %if.then.i.i81, %lpad1
  %.pn68.pn.pn.pn.pn = phi { ptr, i32 } [ %.pn68.pn.pn.pn, %ehcleanup58 ], [ %3, %lpad1 ], [ %3, %if.then.i.i81 ]
  call void @llvm.lifetime.end.p0(i64 noundef 296, ptr noundef nonnull %iqs_config) #16
  resume { ptr, i32 } %.pn68.pn.pn.pn.pn
}

; Function Attrs: nounwind
declare i64 @time(ptr noundef) local_unnamed_addr #2

declare void @_ZN5iqsdk9IqsConfigC1EiNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEEbmbd(ptr noundef nonnull align 8 dereferenceable(296), i32 noundef, ptr noundef, i1 noundef zeroext, i64 noundef, i1 noundef zeroext, double noundef) unnamed_addr #1

declare void @_ZN5iqsdk18FullStateSimulatorC1ERNS_12DeviceConfigE(ptr noundef nonnull align 8 dereferenceable(32), ptr noundef nonnull align 8 dereferenceable(42)) unnamed_addr #1

declare noundef i32 @_ZN5iqsdk6Device5readyEv(ptr noundef nonnull align 8 dereferenceable(32)) local_unnamed_addr #1

declare noundef nonnull align 8 dereferenceable(8) ptr @_ZNSolsEi(ptr noundef nonnull align 8 dereferenceable(8), i32 noundef) local_unnamed_addr #1

declare void @_ZN5iqsdk15SimulatorDevice16getProbabilitiesERSt6vectorISt17reference_wrapperItESaIS3_EE(ptr sret(%"class.std::vector.9") align 8, ptr noundef nonnull align 8 dereferenceable(32), ptr noundef nonnull align 8 dereferenceable(24)) local_unnamed_addr #1

declare void @_ZN5iqsdk15SimulatorDevice20displayProbabilitiesERSt6vectorIdSaIdEERS1_ISt17reference_wrapperItESaIS6_EE(ptr noundef nonnull align 8 dereferenceable(24), ptr noundef nonnull align 8 dereferenceable(24)) local_unnamed_addr #1

; Function Attrs: nounwind
declare void @_ZN5iqsdk6DeviceD2Ev(ptr noundef nonnull align 8 dereferenceable(32)) unnamed_addr #2

; Function Attrs: inlinehint nounwind uwtable
define linkonce_odr dso_local void @_ZN5iqsdk9IqsConfigD2Ev(ptr noundef nonnull align 8 dereferenceable(296) %this) unnamed_addr #6 comdat align 2 personality ptr @__gxx_personality_v0 {
entry:
  store ptr getelementptr inbounds ({ [5 x ptr] }, ptr @_ZTVN5iqsdk9IqsConfigE, i64 0, inrange i32 0, i64 2), ptr %this, align 8, !tbaa !53
  %_M_manager.i = getelementptr inbounds %"struct.iqsdk::IqsConfig", ptr %this, i64 0, i32 11, i32 0, i32 1
  %0 = load ptr, ptr %_M_manager.i, align 8, !tbaa !55
  %tobool.not.i = icmp eq ptr %0, null
  br i1 %tobool.not.i, label %_ZNSt14_Function_baseD2Ev.exit, label %if.then.i

if.then.i:                                        ; preds = %entry
  %CPhaseRotation = getelementptr inbounds %"struct.iqsdk::IqsConfig", ptr %this, i64 0, i32 11
  %call.i = invoke noundef zeroext i1 %0(ptr noundef nonnull align 8 dereferenceable(16) %CPhaseRotation, ptr noundef nonnull align 8 dereferenceable(16) %CPhaseRotation, i32 noundef 3)
          to label %_ZNSt14_Function_baseD2Ev.exit unwind label %terminate.lpad.i

terminate.lpad.i:                                 ; preds = %if.then.i
  %1 = landingpad { ptr, i32 }
          catch ptr null
  %2 = extractvalue { ptr, i32 } %1, 0
  tail call void @__clang_call_terminate(ptr noundef %2) #19
  unreachable

_ZNSt14_Function_baseD2Ev.exit:                   ; preds = %if.then.i, %entry
  %_M_manager.i2 = getelementptr inbounds %"struct.iqsdk::IqsConfig", ptr %this, i64 0, i32 10, i32 0, i32 1
  %3 = load ptr, ptr %_M_manager.i2, align 8, !tbaa !55
  %tobool.not.i3 = icmp eq ptr %3, null
  br i1 %tobool.not.i3, label %_ZNSt14_Function_baseD2Ev.exit7, label %if.then.i4

if.then.i4:                                       ; preds = %_ZNSt14_Function_baseD2Ev.exit
  %ISwapRotation = getelementptr inbounds %"struct.iqsdk::IqsConfig", ptr %this, i64 0, i32 10
  %call.i5 = invoke noundef zeroext i1 %3(ptr noundef nonnull align 8 dereferenceable(16) %ISwapRotation, ptr noundef nonnull align 8 dereferenceable(16) %ISwapRotation, i32 noundef 3)
          to label %_ZNSt14_Function_baseD2Ev.exit7 unwind label %terminate.lpad.i6

terminate.lpad.i6:                                ; preds = %if.then.i4
  %4 = landingpad { ptr, i32 }
          catch ptr null
  %5 = extractvalue { ptr, i32 } %4, 0
  tail call void @__clang_call_terminate(ptr noundef %5) #19
  unreachable

_ZNSt14_Function_baseD2Ev.exit7:                  ; preds = %if.then.i4, %_ZNSt14_Function_baseD2Ev.exit
  %_M_manager.i8 = getelementptr inbounds %"struct.iqsdk::IqsConfig", ptr %this, i64 0, i32 9, i32 0, i32 1
  %6 = load ptr, ptr %_M_manager.i8, align 8, !tbaa !55
  %tobool.not.i9 = icmp eq ptr %6, null
  br i1 %tobool.not.i9, label %_ZNSt14_Function_baseD2Ev.exit13, label %if.then.i10

if.then.i10:                                      ; preds = %_ZNSt14_Function_baseD2Ev.exit7
  %RotationZ = getelementptr inbounds %"struct.iqsdk::IqsConfig", ptr %this, i64 0, i32 9
  %call.i11 = invoke noundef zeroext i1 %6(ptr noundef nonnull align 8 dereferenceable(16) %RotationZ, ptr noundef nonnull align 8 dereferenceable(16) %RotationZ, i32 noundef 3)
          to label %_ZNSt14_Function_baseD2Ev.exit13 unwind label %terminate.lpad.i12

terminate.lpad.i12:                               ; preds = %if.then.i10
  %7 = landingpad { ptr, i32 }
          catch ptr null
  %8 = extractvalue { ptr, i32 } %7, 0
  tail call void @__clang_call_terminate(ptr noundef %8) #19
  unreachable

_ZNSt14_Function_baseD2Ev.exit13:                 ; preds = %if.then.i10, %_ZNSt14_Function_baseD2Ev.exit7
  %_M_manager.i14 = getelementptr inbounds %"struct.iqsdk::IqsConfig", ptr %this, i64 0, i32 8, i32 0, i32 1
  %9 = load ptr, ptr %_M_manager.i14, align 8, !tbaa !55
  %tobool.not.i15 = icmp eq ptr %9, null
  br i1 %tobool.not.i15, label %_ZNSt14_Function_baseD2Ev.exit19, label %if.then.i16

if.then.i16:                                      ; preds = %_ZNSt14_Function_baseD2Ev.exit13
  %RotationXY = getelementptr inbounds %"struct.iqsdk::IqsConfig", ptr %this, i64 0, i32 8
  %call.i17 = invoke noundef zeroext i1 %9(ptr noundef nonnull align 8 dereferenceable(16) %RotationXY, ptr noundef nonnull align 8 dereferenceable(16) %RotationXY, i32 noundef 3)
          to label %_ZNSt14_Function_baseD2Ev.exit19 unwind label %terminate.lpad.i18

terminate.lpad.i18:                               ; preds = %if.then.i16
  %10 = landingpad { ptr, i32 }
          catch ptr null
  %11 = extractvalue { ptr, i32 } %10, 0
  tail call void @__clang_call_terminate(ptr noundef %11) #19
  unreachable

_ZNSt14_Function_baseD2Ev.exit19:                 ; preds = %if.then.i16, %_ZNSt14_Function_baseD2Ev.exit13
  %_M_manager.i20 = getelementptr inbounds %"struct.iqsdk::IqsConfig", ptr %this, i64 0, i32 7, i32 0, i32 1
  %12 = load ptr, ptr %_M_manager.i20, align 8, !tbaa !55
  %tobool.not.i21 = icmp eq ptr %12, null
  br i1 %tobool.not.i21, label %_ZNSt14_Function_baseD2Ev.exit25, label %if.then.i22

if.then.i22:                                      ; preds = %_ZNSt14_Function_baseD2Ev.exit19
  %PrepZ = getelementptr inbounds %"struct.iqsdk::IqsConfig", ptr %this, i64 0, i32 7
  %call.i23 = invoke noundef zeroext i1 %12(ptr noundef nonnull align 8 dereferenceable(16) %PrepZ, ptr noundef nonnull align 8 dereferenceable(16) %PrepZ, i32 noundef 3)
          to label %_ZNSt14_Function_baseD2Ev.exit25 unwind label %terminate.lpad.i24

terminate.lpad.i24:                               ; preds = %if.then.i22
  %13 = landingpad { ptr, i32 }
          catch ptr null
  %14 = extractvalue { ptr, i32 } %13, 0
  tail call void @__clang_call_terminate(ptr noundef %14) #19
  unreachable

_ZNSt14_Function_baseD2Ev.exit25:                 ; preds = %if.then.i22, %_ZNSt14_Function_baseD2Ev.exit19
  %_M_manager.i26 = getelementptr inbounds %"struct.iqsdk::IqsConfig", ptr %this, i64 0, i32 6, i32 0, i32 1
  %15 = load ptr, ptr %_M_manager.i26, align 8, !tbaa !55
  %tobool.not.i27 = icmp eq ptr %15, null
  br i1 %tobool.not.i27, label %_ZNSt14_Function_baseD2Ev.exit31, label %if.then.i28

if.then.i28:                                      ; preds = %_ZNSt14_Function_baseD2Ev.exit25
  %MeasZ = getelementptr inbounds %"struct.iqsdk::IqsConfig", ptr %this, i64 0, i32 6
  %call.i29 = invoke noundef zeroext i1 %15(ptr noundef nonnull align 8 dereferenceable(16) %MeasZ, ptr noundef nonnull align 8 dereferenceable(16) %MeasZ, i32 noundef 3)
          to label %_ZNSt14_Function_baseD2Ev.exit31 unwind label %terminate.lpad.i30

terminate.lpad.i30:                               ; preds = %if.then.i28
  %16 = landingpad { ptr, i32 }
          catch ptr null
  %17 = extractvalue { ptr, i32 } %16, 0
  tail call void @__clang_call_terminate(ptr noundef %17) #19
  unreachable

_ZNSt14_Function_baseD2Ev.exit31:                 ; preds = %if.then.i28, %_ZNSt14_Function_baseD2Ev.exit25
  %simulation_type = getelementptr inbounds %"struct.iqsdk::IqsConfig", ptr %this, i64 0, i32 2
  %18 = load ptr, ptr %simulation_type, align 8, !tbaa !5
  %19 = getelementptr inbounds %"struct.iqsdk::IqsConfig", ptr %this, i64 0, i32 2, i32 2
  %cmp.i.i.i = icmp eq ptr %18, %19
  br i1 %cmp.i.i.i, label %_ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEED2Ev.exit, label %if.then.i.i

if.then.i.i:                                      ; preds = %_ZNSt14_Function_baseD2Ev.exit31
  tail call void @_ZdlPv(ptr noundef %18) #16
  br label %_ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEED2Ev.exit

_ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEED2Ev.exit: ; preds = %if.then.i.i, %_ZNSt14_Function_baseD2Ev.exit31
  store ptr getelementptr inbounds ({ [5 x ptr] }, ptr @_ZTVN5iqsdk12DeviceConfigE, i64 0, inrange i32 0, i64 2), ptr %this, align 8, !tbaa !53
  %backend.i = getelementptr inbounds %"struct.iqsdk::DeviceConfig", ptr %this, i64 0, i32 1
  %20 = load ptr, ptr %backend.i, align 8, !tbaa !5
  %21 = getelementptr inbounds %"struct.iqsdk::DeviceConfig", ptr %this, i64 0, i32 1, i32 2
  %cmp.i.i.i.i = icmp eq ptr %20, %21
  br i1 %cmp.i.i.i.i, label %_ZN5iqsdk12DeviceConfigD2Ev.exit, label %if.then.i.i.i

if.then.i.i.i:                                    ; preds = %_ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEED2Ev.exit
  tail call void @_ZdlPv(ptr noundef %20) #16
  br label %_ZN5iqsdk12DeviceConfigD2Ev.exit

_ZN5iqsdk12DeviceConfigD2Ev.exit:                 ; preds = %if.then.i.i.i, %_ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEED2Ev.exit
  ret void
}

; Function Attrs: noinline noreturn nounwind uwtable
define linkonce_odr hidden void @__clang_call_terminate(ptr noundef %0) local_unnamed_addr #8 comdat {
  %2 = tail call ptr @__cxa_begin_catch(ptr %0) #16
  tail call void @_ZSt9terminatev() #19
  unreachable
}

declare ptr @__cxa_begin_catch(ptr) local_unnamed_addr

declare void @_ZSt9terminatev() local_unnamed_addr

; Function Attrs: nobuiltin nounwind
declare void @_ZdlPv(ptr noundef) local_unnamed_addr #9

; Function Attrs: nocallback nofree nounwind willreturn memory(argmem: readwrite)
declare void @llvm.memcpy.p0.p0.i64(ptr noalias nocapture writeonly, ptr noalias nocapture readonly, i64, i1 immarg) #10

declare noundef nonnull align 8 dereferenceable(8) ptr @_ZSt16__ostream_insertIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_PKS3_l(ptr noundef nonnull align 8 dereferenceable(8), ptr noundef, i64 noundef) local_unnamed_addr #1

; Function Attrs: noreturn
declare void @_ZSt20__throw_length_errorPKc(ptr noundef) local_unnamed_addr #11

; Function Attrs: nobuiltin allocsize(0)
declare noundef nonnull ptr @_Znwm(i64 noundef) local_unnamed_addr #12

; Function Attrs: uwtable
define internal void @_GLOBAL__sub_I_tfd_q4_hybrid_demo.cpp() #13 section ".text.startup" personality ptr @__gxx_personality_v0 {
entry:
  tail call void @_ZNSt8ios_base4InitC1Ev(ptr noundef nonnull align 1 dereferenceable(1) @_ZStL8__ioinit)
  %0 = tail call i32 @__cxa_atexit(ptr noundef nonnull @_ZNSt8ios_base4InitD1Ev, ptr noundef nonnull @_ZStL8__ioinit, ptr noundef nonnull @__dso_handle) #16
  tail call void @llvm.memset.p0.i64(ptr noundef nonnull align 8 dereferenceable(56) @_ZN5iqsdkL14k_iqs_ideal_opE, i8 noundef 0, i64 noundef 56, i1 noundef false)
  store ptr getelementptr inbounds (%"struct.iqsdk::IqsCustomOp", ptr @_ZN5iqsdkL14k_iqs_ideal_opE, i64 0, i32 5, i32 2), ptr getelementptr inbounds (%"struct.iqsdk::IqsCustomOp", ptr @_ZN5iqsdkL14k_iqs_ideal_opE, i64 0, i32 5), align 8, !tbaa !16
  tail call void @llvm.memcpy.p0.p0.i64(ptr noundef nonnull align 8 dereferenceable(5) getelementptr inbounds (%"struct.iqsdk::IqsCustomOp", ptr @_ZN5iqsdkL14k_iqs_ideal_opE, i64 0, i32 5, i32 2), ptr noundef nonnull align 1 dereferenceable(5) @.str.26, i64 noundef 5, i1 noundef false)
  store i64 5, ptr getelementptr inbounds (%"struct.iqsdk::IqsCustomOp", ptr @_ZN5iqsdkL14k_iqs_ideal_opE, i64 0, i32 5, i32 1), align 8, !tbaa !17
  store i8 0, ptr getelementptr inbounds (i8, ptr getelementptr inbounds (%"struct.iqsdk::IqsCustomOp", ptr @_ZN5iqsdkL14k_iqs_ideal_opE, i64 0, i32 5, i32 2), i64 5), align 1, !tbaa !18
  tail call void @llvm.memset.p0.i64(ptr noundef nonnull align 8 dereferenceable(32) getelementptr inbounds (%"struct.iqsdk::IqsCustomOp", ptr @_ZN5iqsdkL14k_iqs_ideal_opE, i64 0, i32 6), i8 noundef 0, i64 noundef 32, i1 noundef false)
  %1 = tail call i32 @__cxa_atexit(ptr noundef nonnull @_ZN5iqsdk11IqsCustomOpD2Ev, ptr noundef nonnull @_ZN5iqsdkL14k_iqs_ideal_opE, ptr noundef nonnull @__dso_handle) #16
  ret void
}

; Function Attrs: nocallback nofree nosync nounwind speculatable willreturn memory(none)
declare i64 @llvm.umax.i64(i64, i64) #14

; Function Attrs: nocallback nofree nosync nounwind willreturn memory(inaccessiblemem: readwrite)
declare void @llvm.experimental.noalias.scope.decl(metadata) #15

; Function Attrs: nounwind
declare void @llvm.quantum.qubit(ptr) #16

; Function Attrs: nounwind
declare void @llvm.quantum.param(double) #16

; Function Attrs: nounwind
declare void @llvm.quantum.cbit(ptr) #16

declare i32 @_ZN4qrt021initializeQrtSystem64EPdPcS1_(ptr, ptr, ptr)

declare i1 @_ZN4qrt013isInitializedEv()

declare void @_ZN4qrt020updateMeasurementMapEPbmb(ptr, i64, i1)

declare i32 @_ZN4qrt014scheduleKernelEPFvvEj(ptr, i32)

declare i32 @_ZN4qrt018dismantleQrtSystemEv()

declare void @_ZN4qrt04exitEPc(ptr)

attributes #0 = { mustprogress noinline nounwind uwtable "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #1 = { "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #2 = { nounwind "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #3 = { nofree nounwind }
attributes #4 = { nocallback nofree nounwind willreturn memory(argmem: write) }
attributes #5 = { nocallback nofree nosync nounwind willreturn memory(argmem: readwrite) }
attributes #6 = { inlinehint nounwind uwtable "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #7 = { norecurse uwtable "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #8 = { noinline noreturn nounwind uwtable "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #9 = { nobuiltin nounwind "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #10 = { nocallback nofree nounwind willreturn memory(argmem: readwrite) }
attributes #11 = { noreturn "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #12 = { nobuiltin allocsize(0) "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #13 = { uwtable "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #14 = { nocallback nofree nosync nounwind speculatable willreturn memory(none) }
attributes #15 = { nocallback nofree nosync nounwind willreturn memory(inaccessiblemem: readwrite) }
attributes #16 = { nounwind }
attributes #17 = { noreturn }
attributes #18 = { allocsize(0) }
attributes #19 = { noreturn nounwind }

!llvm.module.flags = !{!0, !1, !2, !3}
!llvm.ident = !{!4}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{i32 8, !"PIC Level", i32 2}
!2 = !{i32 7, !"PIE Level", i32 2}
!3 = !{i32 7, !"uwtable", i32 2}
!4 = !{!"LLVM.org clang version 18.0.0 (https://github.com/intel-sandbox/applications.quantum.compiler-llvm.git cb7433d6f6b33800d710fbc37e8dc16cf90ced56)\"}
!5 = !{!6, !8, i64 0}
!6 = !{!"_ZTSNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEE", !7, i64 0, !11, i64 8, !9, i64 16}
!7 = !{!"_ZTSNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE12_Alloc_hiderE", !8, i64 0}
!8 = !{!"any pointer", !9, i64 0}
!9 = !{!"omnipotent char", !10, i64 0}
!10 = !{!"Simple C++ TBAA"}
!11 = !{!"long", !9, i64 0}
!12 = !{!13, !8, i64 0}
!13 = !{!"_ZTSNSt12_Vector_baseISt7complexIdESaIS1_EE17_Vector_impl_dataE", !8, i64 0, !8, i64 8, !8, i64 16}
!14 = !{!15, !15, i64 0}
!15 = !{!"double", !9, i64 0}
!16 = !{!7, !8, i64 0}
!17 = !{!6, !11, i64 8}
!18 = !{!9, !9, i64 0}
!19 = !{!20, !23, i64 44}
!20 = !{!"_ZTSN5iqsdk9IqsConfigE", !21, i64 0, !23, i64 44, !6, i64 48, !15, i64 80, !22, i64 88, !11, i64 96, !24, i64 104, !24, i64 136, !26, i64 168, !27, i64 200, !28, i64 232, !28, i64 264}
!21 = !{!"_ZTSN5iqsdk12DeviceConfigE", !6, i64 8, !22, i64 40, !22, i64 41}
!22 = !{!"bool", !9, i64 0}
!23 = !{!"int", !9, i64 0}
!24 = !{!"_ZTSSt8functionIFN5iqsdk11IqsCustomOpEjEE", !25, i64 0, !8, i64 24}
!25 = !{!"_ZTSSt14_Function_base", !9, i64 0, !8, i64 16}
!26 = !{!"_ZTSSt8functionIFN5iqsdk11IqsCustomOpEjddEE", !25, i64 0, !8, i64 24}
!27 = !{!"_ZTSSt8functionIFN5iqsdk11IqsCustomOpEjdEE", !25, i64 0, !8, i64 24}
!28 = !{!"_ZTSSt8functionIFN5iqsdk11IqsCustomOpEjjdEE", !25, i64 0, !8, i64 24}
!29 = !{!30, !8, i64 16}
!30 = !{!"_ZTSNSt12_Vector_baseISt17reference_wrapperItESaIS1_EE17_Vector_impl_dataE", !8, i64 0, !8, i64 8, !8, i64 16}
!31 = !{!30, !8, i64 8}
!32 = !{!8, !8, i64 0}
!33 = !{i64 1, i64 0}
!34 = !{!35}
!35 = distinct !{!35, !36, !"_ZSt19__relocate_object_aISt17reference_wrapperItES1_SaIS1_EEvPT_PT0_RT1_: %__dest"}
!36 = distinct !{!36, !"_ZSt19__relocate_object_aISt17reference_wrapperItES1_SaIS1_EEvPT_PT0_RT1_"}
!37 = !{!38}
!38 = distinct !{!38, !36, !"_ZSt19__relocate_object_aISt17reference_wrapperItES1_SaIS1_EEvPT_PT0_RT1_: %__orig"}
!39 = distinct !{!39, !40, !41, !42, !43}
!40 = !{!"llvm.loop.mustprogress"}
!41 = !{!"llvm.loop.unroll.disable"}
!42 = !{!"llvm.loop.isvectorized", i32 1}
!43 = !{!"llvm.loop.unroll.runtime.disable"}
!44 = distinct !{!44, !40, !41, !42}
!45 = !{!30, !8, i64 0}
!46 = distinct !{!46, !40, !41}
!47 = !{!48, !8, i64 0}
!48 = !{!"_ZTSNSt12_Vector_baseIdSaIdEE17_Vector_impl_dataE", !8, i64 0, !8, i64 8, !8, i64 16}
!49 = !{!48, !8, i64 16}
!50 = !{!22, !22, i64 0}
!51 = !{i8 0, i8 2}
!52 = !{}
!53 = !{!54, !54, i64 0}
!54 = !{!"vtable pointer", !10, i64 0}
!55 = !{!25, !8, i64 16}
)";

const char local_5x5_grid[] = R"(
; ModuleID = 'local_5x5_grid_unrolled_user.ll'
source_filename = "test-samples/local_5x5_grid.cpp"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

%"class.std::ios_base::Init" = type { i8 }

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
@q = dso_local global [25 x i16] zeroinitializer, align 16
@c = dso_local local_unnamed_addr global [25 x i8] zeroinitializer, align 16
@llvm.global_ctors = appending global [1 x { i32, ptr, ptr }] [{ i32, ptr, ptr } { i32 65535, ptr @_GLOBAL__sub_I_local_5x5_grid.cpp, ptr null }]
@llvm.global.annotations = appending global [24 x { ptr, ptr, ptr, i32, ptr }] [{ ptr, ptr, ptr, i32, ptr } { ptr @_Z1HRt, ptr @.str, ptr @.str.1, i32 48, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr @_Z1XRt, ptr @.str.2, ptr @.str.1, i32 68, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr @_Z1YRt, ptr @.str.3, ptr @.str.1, i32 88, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr @_Z1ZRt, ptr @.str.4, ptr @.str.1, i32 109, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr @_Z1SRt, ptr @.str.5, ptr @.str.1, i32 129, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr @_Z4SdagRt, ptr @.str.6, ptr @.str.1, i32 149, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr @_Z1TRt, ptr @.str.7, ptr @.str.1, i32 169, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr @_Z4TdagRt, ptr @.str.8, ptr @.str.1, i32 189, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr @_Z2RXRtd, ptr @.str.9, ptr @.str.1, i32 209, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr @_Z2RYRtd, ptr @.str.10, ptr @.str.1, i32 230, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr @_Z2RZRtd, ptr @.str.11, ptr @.str.1, i32 251, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr @_Z2CZRtS_, ptr @.str.12, ptr @.str.1, i32 272, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr @_Z4CNOTRtS_, ptr @.str.13, ptr @.str.1, i32 293, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr @_Z4SWAPRtS_, ptr @.str.14, ptr @.str.1, i32 314, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr @_Z7ToffoliRtS_S_, ptr @.str.15, ptr @.str.1, i32 335, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr @_Z5PrepXRt, ptr @.str.16, ptr @.str.1, i32 359, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr @_Z5PrepYRt, ptr @.str.17, ptr @.str.1, i32 377, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr @_Z5PrepZRt, ptr @.str.18, ptr @.str.1, i32 395, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr @_Z5MeasXRtRbb, ptr @.str.19, ptr @.str.1, i32 413, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr @_Z5MeasYRtRbb, ptr @.str.20, ptr @.str.1, i32 433, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr @_Z5MeasZRtRbb, ptr @.str.21, ptr @.str.1, i32 453, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr @_Z6CPhaseRtS_d, ptr @.str.22, ptr @.str.1, i32 481, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr @_Z3RXYRtdd, ptr @.str.23, ptr @.str.1, i32 503, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr @_Z5SwapARtS_d, ptr @.str.24, ptr @.str.1, i32 525, ptr null }], section "llvm.metadata"

; Function Attrs: mustprogress noinline nounwind uwtable
define dso_local void @_Z1HRt(ptr noundef nonnull align 2 dereferenceable(2) "qubit_ref" %0) #0 {
  tail call void @llvm.quantum.qubit(ptr noundef nonnull align 2 dereferenceable(2) %0)
  ret void
}

; Function Attrs: mustprogress noinline nounwind uwtable
define dso_local void @_Z1XRt(ptr noundef nonnull align 2 dereferenceable(2) "qubit_ref" %0) #0 {
  tail call void @llvm.quantum.qubit(ptr noundef nonnull align 2 dereferenceable(2) %0)
  ret void
}

; Function Attrs: mustprogress noinline nounwind uwtable
define dso_local void @_Z1YRt(ptr noundef nonnull align 2 dereferenceable(2) "qubit_ref" %0) #0 {
  tail call void @llvm.quantum.qubit(ptr noundef nonnull align 2 dereferenceable(2) %0)
  ret void
}

; Function Attrs: mustprogress noinline nounwind uwtable
define dso_local void @_Z1ZRt(ptr noundef nonnull align 2 dereferenceable(2) "qubit_ref" %0) #0 {
  tail call void @llvm.quantum.qubit(ptr noundef nonnull align 2 dereferenceable(2) %0)
  ret void
}

; Function Attrs: mustprogress noinline nounwind uwtable
define dso_local void @_Z1SRt(ptr noundef nonnull align 2 dereferenceable(2) "qubit_ref" %0) #0 {
  tail call void @llvm.quantum.qubit(ptr noundef nonnull align 2 dereferenceable(2) %0)
  ret void
}

; Function Attrs: mustprogress noinline nounwind uwtable
define dso_local void @_Z4SdagRt(ptr noundef nonnull align 2 dereferenceable(2) "qubit_ref" %0) #0 {
  tail call void @llvm.quantum.qubit(ptr noundef nonnull align 2 dereferenceable(2) %0)
  ret void
}

; Function Attrs: mustprogress noinline nounwind uwtable
define dso_local void @_Z1TRt(ptr noundef nonnull align 2 dereferenceable(2) "qubit_ref" %0) #0 {
  tail call void @llvm.quantum.qubit(ptr noundef nonnull align 2 dereferenceable(2) %0)
  ret void
}

; Function Attrs: mustprogress noinline nounwind uwtable
define dso_local void @_Z4TdagRt(ptr noundef nonnull align 2 dereferenceable(2) "qubit_ref" %0) #0 {
  tail call void @llvm.quantum.qubit(ptr noundef nonnull align 2 dereferenceable(2) %0)
  ret void
}

; Function Attrs: mustprogress noinline nounwind uwtable
define dso_local void @_Z2RXRtd(ptr noundef nonnull align 2 dereferenceable(2) "qubit_ref" %0, double noundef %1) #0 {
  tail call void @llvm.quantum.qubit(ptr noundef nonnull align 2 dereferenceable(2) %0)
  tail call void @llvm.quantum.param(double noundef %1)
  ret void
}

; Function Attrs: mustprogress noinline nounwind uwtable
define dso_local void @_Z2RYRtd(ptr noundef nonnull align 2 dereferenceable(2) "qubit_ref" %0, double noundef %1) #0 {
  tail call void @llvm.quantum.qubit(ptr noundef nonnull align 2 dereferenceable(2) %0)
  tail call void @llvm.quantum.param(double noundef %1)
  ret void
}

; Function Attrs: mustprogress noinline nounwind uwtable
define dso_local void @_Z2RZRtd(ptr noundef nonnull align 2 dereferenceable(2) "qubit_ref" %0, double noundef %1) #0 {
  tail call void @llvm.quantum.qubit(ptr noundef nonnull align 2 dereferenceable(2) %0)
  tail call void @llvm.quantum.param(double noundef %1)
  ret void
}

; Function Attrs: mustprogress noinline nounwind uwtable
define dso_local void @_Z2CZRtS_(ptr noundef nonnull align 2 dereferenceable(2) "qubit_ref" %0, ptr noundef nonnull align 2 dereferenceable(2) "qubit_ref" %1) #0 {
  tail call void @llvm.quantum.qubit(ptr noundef nonnull align 2 dereferenceable(2) %0)
  tail call void @llvm.quantum.qubit(ptr noundef nonnull %1)
  ret void
}

; Function Attrs: mustprogress noinline nounwind uwtable
define dso_local void @_Z4CNOTRtS_(ptr noundef nonnull align 2 dereferenceable(2) "qubit_ref" %0, ptr noundef nonnull align 2 dereferenceable(2) "qubit_ref" %1) #0 {
  tail call void @llvm.quantum.qubit(ptr noundef nonnull align 2 dereferenceable(2) %0)
  tail call void @llvm.quantum.qubit(ptr noundef nonnull %1)
  ret void
}

; Function Attrs: mustprogress noinline nounwind uwtable
define dso_local void @_Z4SWAPRtS_(ptr noundef nonnull align 2 dereferenceable(2) "qubit_ref" %0, ptr noundef nonnull align 2 dereferenceable(2) "qubit_ref" %1) #0 {
  tail call void @llvm.quantum.qubit(ptr noundef nonnull align 2 dereferenceable(2) %0)
  tail call void @llvm.quantum.qubit(ptr noundef nonnull %1)
  ret void
}

; Function Attrs: mustprogress noinline nounwind uwtable
define dso_local void @_Z7ToffoliRtS_S_(ptr noundef nonnull align 2 dereferenceable(2) "qubit_ref" %0, ptr noundef nonnull align 2 dereferenceable(2) "qubit_ref" %1, ptr noundef nonnull align 2 dereferenceable(2) "qubit_ref" %2) #0 {
  tail call void @llvm.quantum.qubit(ptr noundef nonnull align 2 dereferenceable(2) %0)
  tail call void @llvm.quantum.qubit(ptr noundef nonnull %1)
  tail call void @llvm.quantum.qubit(ptr noundef nonnull %2)
  ret void
}

; Function Attrs: mustprogress noinline nounwind uwtable
define dso_local void @_Z5PrepXRt(ptr noundef nonnull align 2 dereferenceable(2) "qubit_ref" %0) #0 {
  tail call void @llvm.quantum.qubit(ptr noundef nonnull align 2 dereferenceable(2) %0)
  ret void
}

; Function Attrs: mustprogress noinline nounwind uwtable
define dso_local void @_Z5PrepYRt(ptr noundef nonnull align 2 dereferenceable(2) "qubit_ref" %0) #0 {
  tail call void @llvm.quantum.qubit(ptr noundef nonnull align 2 dereferenceable(2) %0)
  ret void
}

; Function Attrs: mustprogress noinline nounwind uwtable
define dso_local void @_Z5PrepZRt(ptr noundef nonnull align 2 dereferenceable(2) "qubit_ref" %0) #0 {
  tail call void @llvm.quantum.qubit(ptr noundef nonnull align 2 dereferenceable(2) %0)
  ret void
}

; Function Attrs: mustprogress noinline nounwind uwtable
define dso_local void @_Z5MeasXRtRbb(ptr noundef nonnull align 2 dereferenceable(2) "qubit_ref" %0, ptr noundef nonnull align 1 dereferenceable(1) "cbit_ref" %1, i1 noundef zeroext %2) #0 {
  tail call void @llvm.quantum.qubit(ptr noundef nonnull align 2 dereferenceable(2) %0)
  tail call void @llvm.quantum.cbit(ptr noundef nonnull %1)
  %4 = uitofp i1 %2 to double
  tail call void @llvm.quantum.param(double noundef %4)
  ret void
}

; Function Attrs: mustprogress noinline nounwind uwtable
define dso_local void @_Z5MeasYRtRbb(ptr noundef nonnull align 2 dereferenceable(2) "qubit_ref" %0, ptr noundef nonnull align 1 dereferenceable(1) "cbit_ref" %1, i1 noundef zeroext %2) #0 {
  tail call void @llvm.quantum.qubit(ptr noundef nonnull align 2 dereferenceable(2) %0)
  tail call void @llvm.quantum.cbit(ptr noundef nonnull %1)
  %4 = uitofp i1 %2 to double
  tail call void @llvm.quantum.param(double noundef %4)
  ret void
}

; Function Attrs: mustprogress noinline nounwind uwtable
define dso_local void @_Z5MeasZRtRbb(ptr noundef nonnull align 2 dereferenceable(2) "qubit_ref" %0, ptr noundef nonnull align 1 dereferenceable(1) "cbit_ref" %1, i1 noundef zeroext %2) #0 {
  tail call void @llvm.quantum.qubit(ptr noundef nonnull align 2 dereferenceable(2) %0)
  tail call void @llvm.quantum.cbit(ptr noundef nonnull %1)
  %4 = uitofp i1 %2 to double
  tail call void @llvm.quantum.param(double noundef %4)
  ret void
}

; Function Attrs: mustprogress noinline nounwind uwtable
define dso_local void @_Z6CPhaseRtS_d(ptr noundef nonnull align 2 dereferenceable(2) "qubit_ref" %0, ptr noundef nonnull align 2 dereferenceable(2) "qubit_ref" %1, double noundef %2) #0 {
  tail call void @llvm.quantum.qubit(ptr noundef nonnull align 2 dereferenceable(2) %0)
  tail call void @llvm.quantum.qubit(ptr noundef nonnull %1)
  tail call void @llvm.quantum.param(double noundef %2)
  ret void
}

; Function Attrs: mustprogress noinline nounwind uwtable
define dso_local void @_Z3RXYRtdd(ptr noundef nonnull align 2 dereferenceable(2) "qubit_ref" %0, double noundef %1, double noundef %2) #0 {
  tail call void @llvm.quantum.qubit(ptr noundef nonnull align 2 dereferenceable(2) %0)
  tail call void @llvm.quantum.param(double noundef %1)
  tail call void @llvm.quantum.param(double noundef %2)
  ret void
}

; Function Attrs: mustprogress noinline nounwind uwtable
define dso_local void @_Z5SwapARtS_d(ptr noundef nonnull align 2 dereferenceable(2) "qubit_ref" %0, ptr noundef nonnull align 2 dereferenceable(2) "qubit_ref" %1, double noundef %2) #0 {
  tail call void @llvm.quantum.qubit(ptr noundef nonnull align 2 dereferenceable(2) %0)
  tail call void @llvm.quantum.qubit(ptr noundef nonnull %1)
  tail call void @llvm.quantum.param(double noundef %2)
  ret void
}

declare void @_ZNSt8ios_base4InitC1Ev(ptr noundef nonnull align 1 dereferenceable(1)) unnamed_addr #1

; Function Attrs: nounwind
declare void @_ZNSt8ios_base4InitD1Ev(ptr noundef nonnull align 1 dereferenceable(1)) unnamed_addr #2

; Function Attrs: nofree nounwind
declare i32 @__cxa_atexit(ptr, ptr, ptr) local_unnamed_addr #3

; Function Attrs: mustprogress noinline nounwind uwtable
define dso_local void @_Z10mbl_q3_1tsv.stub() local_unnamed_addr #0 section ".qbbs_text" {
  br label %aqcc.quantum

aqcc.quantum:                                     ; preds = %0
  %arrayidx78 = getelementptr inbounds [25 x i16], ptr @q, i64 0, i64 0
  %arrayidx79 = getelementptr inbounds [25 x i16], ptr @q, i64 0, i64 1
  call void @_Z2CZRtS_(ptr %arrayidx78, ptr %arrayidx79)
  %arrayidx76 = getelementptr inbounds [25 x i16], ptr @q, i64 0, i64 0
  %arrayidx77 = getelementptr inbounds [25 x i16], ptr @q, i64 0, i64 2
  call void @_Z2CZRtS_(ptr %arrayidx76, ptr %arrayidx77)
  %arrayidx74 = getelementptr inbounds [25 x i16], ptr @q, i64 0, i64 1
  %arrayidx75 = getelementptr inbounds [25 x i16], ptr @q, i64 0, i64 16
  call void @_Z2CZRtS_(ptr %arrayidx74, ptr %arrayidx75)
  %arrayidx72 = getelementptr inbounds [25 x i16], ptr @q, i64 0, i64 1
  %arrayidx73 = getelementptr inbounds [25 x i16], ptr @q, i64 0, i64 19
  call void @_Z2CZRtS_(ptr %arrayidx72, ptr %arrayidx73)
  %arrayidx70 = getelementptr inbounds [25 x i16], ptr @q, i64 0, i64 16
  %arrayidx71 = getelementptr inbounds [25 x i16], ptr @q, i64 0, i64 17
  call void @_Z2CZRtS_(ptr %arrayidx70, ptr %arrayidx71)
  %arrayidx68 = getelementptr inbounds [25 x i16], ptr @q, i64 0, i64 16
  %arrayidx69 = getelementptr inbounds [25 x i16], ptr @q, i64 0, i64 15
  call void @_Z2CZRtS_(ptr %arrayidx68, ptr %arrayidx69)
  %arrayidx66 = getelementptr inbounds [25 x i16], ptr @q, i64 0, i64 17
  %arrayidx67 = getelementptr inbounds [25 x i16], ptr @q, i64 0, i64 18
  call void @_Z2CZRtS_(ptr %arrayidx66, ptr %arrayidx67)
  %arrayidx64 = getelementptr inbounds [25 x i16], ptr @q, i64 0, i64 17
  %arrayidx65 = getelementptr inbounds [25 x i16], ptr @q, i64 0, i64 20
  call void @_Z2CZRtS_(ptr %arrayidx64, ptr %arrayidx65)
  %arrayidx62 = getelementptr inbounds [25 x i16], ptr @q, i64 0, i64 18
  %arrayidx63 = getelementptr inbounds [25 x i16], ptr @q, i64 0, i64 21
  call void @_Z2CZRtS_(ptr %arrayidx62, ptr %arrayidx63)
  %arrayidx60 = getelementptr inbounds [25 x i16], ptr @q, i64 0, i64 2
  %arrayidx61 = getelementptr inbounds [25 x i16], ptr @q, i64 0, i64 19
  call void @_Z2CZRtS_(ptr %arrayidx60, ptr %arrayidx61)
  %arrayidx58 = getelementptr inbounds [25 x i16], ptr @q, i64 0, i64 2
  %arrayidx59 = getelementptr inbounds [25 x i16], ptr @q, i64 0, i64 12
  call void @_Z2CZRtS_(ptr %arrayidx58, ptr %arrayidx59)
  %arrayidx56 = getelementptr inbounds [25 x i16], ptr @q, i64 0, i64 19
  %arrayidx57 = getelementptr inbounds [25 x i16], ptr @q, i64 0, i64 15
  call void @_Z2CZRtS_(ptr %arrayidx56, ptr %arrayidx57)
  %arrayidx54 = getelementptr inbounds [25 x i16], ptr @q, i64 0, i64 19
  %arrayidx55 = getelementptr inbounds [25 x i16], ptr @q, i64 0, i64 13
  call void @_Z2CZRtS_(ptr %arrayidx54, ptr %arrayidx55)
  %arrayidx52 = getelementptr inbounds [25 x i16], ptr @q, i64 0, i64 15
  %arrayidx53 = getelementptr inbounds [25 x i16], ptr @q, i64 0, i64 20
  call void @_Z2CZRtS_(ptr %arrayidx52, ptr %arrayidx53)
  %arrayidx50 = getelementptr inbounds [25 x i16], ptr @q, i64 0, i64 15
  %arrayidx51 = getelementptr inbounds [25 x i16], ptr @q, i64 0, i64 14
  call void @_Z2CZRtS_(ptr %arrayidx50, ptr %arrayidx51)
  %arrayidx48 = getelementptr inbounds [25 x i16], ptr @q, i64 0, i64 20
  %arrayidx49 = getelementptr inbounds [25 x i16], ptr @q, i64 0, i64 21
  call void @_Z2CZRtS_(ptr %arrayidx48, ptr %arrayidx49)
  %arrayidx46 = getelementptr inbounds [25 x i16], ptr @q, i64 0, i64 20
  %arrayidx47 = getelementptr inbounds [25 x i16], ptr @q, i64 0, i64 22
  call void @_Z2CZRtS_(ptr %arrayidx46, ptr %arrayidx47)
  %arrayidx44 = getelementptr inbounds [25 x i16], ptr @q, i64 0, i64 21
  %arrayidx45 = getelementptr inbounds [25 x i16], ptr @q, i64 0, i64 6
  call void @_Z2CZRtS_(ptr %arrayidx44, ptr %arrayidx45)
  %arrayidx42 = getelementptr inbounds [25 x i16], ptr @q, i64 0, i64 12
  %arrayidx43 = getelementptr inbounds [25 x i16], ptr @q, i64 0, i64 13
  call void @_Z2CZRtS_(ptr %arrayidx42, ptr %arrayidx43)
  %arrayidx40 = getelementptr inbounds [25 x i16], ptr @q, i64 0, i64 12
  %arrayidx41 = getelementptr inbounds [25 x i16], ptr @q, i64 0, i64 7
  call void @_Z2CZRtS_(ptr %arrayidx40, ptr %arrayidx41)
  %arrayidx38 = getelementptr inbounds [25 x i16], ptr @q, i64 0, i64 13
  %arrayidx39 = getelementptr inbounds [25 x i16], ptr @q, i64 0, i64 14
  call void @_Z2CZRtS_(ptr %arrayidx38, ptr %arrayidx39)
  %arrayidx36 = getelementptr inbounds [25 x i16], ptr @q, i64 0, i64 13
  %arrayidx37 = getelementptr inbounds [25 x i16], ptr @q, i64 0, i64 24
  call void @_Z2CZRtS_(ptr %arrayidx36, ptr %arrayidx37)
  %arrayidx34 = getelementptr inbounds [25 x i16], ptr @q, i64 0, i64 14
  %arrayidx35 = getelementptr inbounds [25 x i16], ptr @q, i64 0, i64 22
  call void @_Z2CZRtS_(ptr %arrayidx34, ptr %arrayidx35)
  %arrayidx32 = getelementptr inbounds [25 x i16], ptr @q, i64 0, i64 14
  %arrayidx33 = getelementptr inbounds [25 x i16], ptr @q, i64 0, i64 8
  call void @_Z2CZRtS_(ptr %arrayidx32, ptr %arrayidx33)
  %arrayidx30 = getelementptr inbounds [25 x i16], ptr @q, i64 0, i64 22
  %arrayidx31 = getelementptr inbounds [25 x i16], ptr @q, i64 0, i64 6
  call void @_Z2CZRtS_(ptr %arrayidx30, ptr %arrayidx31)
  %arrayidx28 = getelementptr inbounds [25 x i16], ptr @q, i64 0, i64 22
  %arrayidx29 = getelementptr inbounds [25 x i16], ptr @q, i64 0, i64 23
  call void @_Z2CZRtS_(ptr %arrayidx28, ptr %arrayidx29)
  %arrayidx26 = getelementptr inbounds [25 x i16], ptr @q, i64 0, i64 6
  %arrayidx27 = getelementptr inbounds [25 x i16], ptr @q, i64 0, i64 5
  call void @_Z2CZRtS_(ptr %arrayidx26, ptr %arrayidx27)
  %arrayidx24 = getelementptr inbounds [25 x i16], ptr @q, i64 0, i64 7
  %arrayidx25 = getelementptr inbounds [25 x i16], ptr @q, i64 0, i64 24
  call void @_Z2CZRtS_(ptr %arrayidx24, ptr %arrayidx25)
  %arrayidx22 = getelementptr inbounds [25 x i16], ptr @q, i64 0, i64 7
  %arrayidx23 = getelementptr inbounds [25 x i16], ptr @q, i64 0, i64 11
  call void @_Z2CZRtS_(ptr %arrayidx22, ptr %arrayidx23)
  %arrayidx20 = getelementptr inbounds [25 x i16], ptr @q, i64 0, i64 24
  %arrayidx21 = getelementptr inbounds [25 x i16], ptr @q, i64 0, i64 8
  call void @_Z2CZRtS_(ptr %arrayidx20, ptr %arrayidx21)
  %arrayidx18 = getelementptr inbounds [25 x i16], ptr @q, i64 0, i64 24
  %arrayidx19 = getelementptr inbounds [25 x i16], ptr @q, i64 0, i64 10
  call void @_Z2CZRtS_(ptr %arrayidx18, ptr %arrayidx19)
  %arrayidx16 = getelementptr inbounds [25 x i16], ptr @q, i64 0, i64 8
  %arrayidx17 = getelementptr inbounds [25 x i16], ptr @q, i64 0, i64 23
  call void @_Z2CZRtS_(ptr %arrayidx16, ptr %arrayidx17)
  %arrayidx14 = getelementptr inbounds [25 x i16], ptr @q, i64 0, i64 8
  %arrayidx15 = getelementptr inbounds [25 x i16], ptr @q, i64 0, i64 9
  call void @_Z2CZRtS_(ptr %arrayidx14, ptr %arrayidx15)
  %arrayidx12 = getelementptr inbounds [25 x i16], ptr @q, i64 0, i64 23
  %arrayidx13 = getelementptr inbounds [25 x i16], ptr @q, i64 0, i64 5
  call void @_Z2CZRtS_(ptr %arrayidx12, ptr %arrayidx13)
  %arrayidx10 = getelementptr inbounds [25 x i16], ptr @q, i64 0, i64 23
  %arrayidx11 = getelementptr inbounds [25 x i16], ptr @q, i64 0, i64 4
  call void @_Z2CZRtS_(ptr %arrayidx10, ptr %arrayidx11)
  %arrayidx8 = getelementptr inbounds [25 x i16], ptr @q, i64 0, i64 5
  %arrayidx9 = getelementptr inbounds [25 x i16], ptr @q, i64 0, i64 3
  call void @_Z2CZRtS_(ptr %arrayidx8, ptr %arrayidx9)
  %arrayidx6 = getelementptr inbounds [25 x i16], ptr @q, i64 0, i64 11
  %arrayidx7 = getelementptr inbounds [25 x i16], ptr @q, i64 0, i64 10
  call void @_Z2CZRtS_(ptr %arrayidx6, ptr %arrayidx7)
  %arrayidx4 = getelementptr inbounds [25 x i16], ptr @q, i64 0, i64 10
  %arrayidx5 = getelementptr inbounds [25 x i16], ptr @q, i64 0, i64 9
  call void @_Z2CZRtS_(ptr %arrayidx4, ptr %arrayidx5)
  %arrayidx2 = getelementptr inbounds [25 x i16], ptr @q, i64 0, i64 9
  %arrayidx3 = getelementptr inbounds [25 x i16], ptr @q, i64 0, i64 4
  call void @_Z2CZRtS_(ptr %arrayidx2, ptr %arrayidx3)
  %arrayidx = getelementptr inbounds [25 x i16], ptr @q, i64 0, i64 4
  %arrayidx1 = getelementptr inbounds [25 x i16], ptr @q, i64 0, i64 3
  call void @_Z2CZRtS_(ptr %arrayidx, ptr %arrayidx1)
  br label %aqcc.meas.move.end

aqcc.meas.move.end:                               ; preds = %aqcc.quantum
  ret void
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(none) uwtable
define dso_local noundef i32 @main() local_unnamed_addr #4 {
  ret i32 0
}

; Function Attrs: uwtable
define internal void @_GLOBAL__sub_I_local_5x5_grid.cpp() #5 section ".text.startup" {
  tail call void @_ZNSt8ios_base4InitC1Ev(ptr noundef nonnull align 1 dereferenceable(1) @_ZStL8__ioinit)
  %1 = tail call i32 @__cxa_atexit(ptr noundef nonnull @_ZNSt8ios_base4InitD1Ev, ptr noundef nonnull @_ZStL8__ioinit, ptr noundef nonnull @__dso_handle) #6
  ret void
}

; Function Attrs: nounwind
declare void @llvm.quantum.qubit(ptr) #6

; Function Attrs: nounwind
declare void @llvm.quantum.param(double) #6

; Function Attrs: nounwind
declare void @llvm.quantum.cbit(ptr) #6

declare i32 @_ZN4qrt021initializeQrtSystem64EPdPcS1_(ptr, ptr, ptr)

declare i1 @_ZN4qrt013isInitializedEv()

declare void @_ZN4qrt020updateMeasurementMapEPbmb(ptr, i64, i1)

declare i32 @_ZN4qrt014scheduleKernelEPFvvEj(ptr, i32)

declare i32 @_ZN4qrt018dismantleQrtSystemEv()

attributes #0 = { mustprogress noinline nounwind uwtable "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #1 = { "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #2 = { nounwind "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #3 = { nofree nounwind }
attributes #4 = { mustprogress nofree norecurse nosync nounwind willreturn memory(none) uwtable "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #5 = { uwtable "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #6 = { nounwind }

!llvm.module.flags = !{!0, !1, !2, !3}
!llvm.ident = !{!4}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{i32 8, !"PIC Level", i32 2}
!2 = !{i32 7, !"PIE Level", i32 2}
!3 = !{i32 7, !"uwtable", i32 2}
!4 = !{!"clang version 17.0.0 (https://github.com/intel-sandbox/applications.quantum.compiler-llvm.git 9dcb9e2143d5ccc873e3073261d3173a57f5498f)\"}

)"; // Must use Raw string to ensure proper parsing

#endif
