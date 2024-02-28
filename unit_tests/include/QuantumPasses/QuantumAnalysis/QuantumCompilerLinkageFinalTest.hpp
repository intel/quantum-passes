//===- QuantumCompilerLinkageFinalTest.hpp ----------------------*- C++ -*-===//
//
//
// Copyright (C) 2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//
//===----------------------------------------------------------------------===//
/// \file "QuantumCompilerLinkageFinalTest.hpp"
///
/// This file provides the raw strings for Quantum Compiler Linkage Final test
/// unit test.
///
//===----------------------------------------------------------------------===//

#ifndef QUANTUM_COMPILER_LINKAGE_FINAL_TEST_HPP
#define QUANTUM_COMPILER_LINKAGE_FINAL_TEST_HPP

// Minimum working .ll file

static const char Code[] = R"(
; ModuleID = '/hybrid_ex_separated_classical.ll'
source_filename = "unit_tests/source-files/hybrid_ex.cpp"
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
define dso_local void @_Z7PrepAllv.stub() local_unnamed_addr #1 {
entry:
  br label %aqcc.quantum

aqcc.quantum:                                     ; preds = %entry
  %QRTcall = call i32 @_ZN4qrt014scheduleKernelEPFvvEj(ptr @"_Z16PrepAll().QBB.0.v.stub", i32 9)
  br label %aqcc.meas.move.end

aqcc.meas.move.end:                               ; preds = %aqcc.quantum
  store [3 x i16] [i16 0, i16 1, i16 2], ptr @q, align 2
  ret void
}

; Function Attrs: mustprogress noinline nounwind uwtable
define dso_local void @_Z10VQE_anzatzPd.stub(ptr nocapture nofree noundef nonnull readonly align 8 dereferenceable(8) %angs) local_unnamed_addr #1 {
entry:
  %0 = load double, ptr %angs, align 8, !tbaa !5
  %arrayidx1 = getelementptr inbounds double, ptr %angs, i64 1
  %1 = load double, ptr %arrayidx1, align 8, !tbaa !5
  %arrayidx2 = getelementptr inbounds double, ptr %angs, i64 2
  %2 = load double, ptr %arrayidx2, align 8, !tbaa !5
  %arrayidx3 = getelementptr inbounds double, ptr %angs, i64 3
  %3 = load double, ptr %arrayidx3, align 8, !tbaa !5
  %arrayidx4 = getelementptr inbounds double, ptr %angs, i64 4
  %4 = load double, ptr %arrayidx4, align 8, !tbaa !5
  store double %0, ptr getelementptr inbounds ([65536 x double], ptr @aqcc.qsda, i64 0, i64 4), align 8
  store double %1, ptr getelementptr inbounds ([65536 x double], ptr @aqcc.qsda, i64 0, i64 5), align 8
  store double %2, ptr getelementptr inbounds ([65536 x double], ptr @aqcc.qsda, i64 0, i64 6), align 8
  store double %3, ptr getelementptr inbounds ([65536 x double], ptr @aqcc.qsda, i64 0, i64 7), align 8
  %5 = fmul double %4, 5.000000e-01
  store double %5, ptr getelementptr inbounds ([65536 x double], ptr @aqcc.qsda, i64 0, i64 8), align 8
  br label %aqcc.quantum

aqcc.quantum:                                     ; preds = %entry
  %QRTcall = call i32 @_ZN4qrt014scheduleKernelEPFvvEj(ptr @"_Z26VQE_anzatz(double*).QBB.1.v.stub", i32 18)
  br label %aqcc.meas.move.end

aqcc.meas.move.end:                               ; preds = %aqcc.quantum
  store [3 x i16] [i16 0, i16 1, i16 2], ptr @q, align 2
  ret void
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(none) uwtable
define dso_local noundef zeroext i1 @_Z13optimize_fakePdd(ptr nocapture nofree noundef readnone %angs, double noundef %cost) local_unnamed_addr #2 {
entry:
  %cmp = fcmp ogt double %cost, 0.000000e+00
  ret i1 %cmp
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(read, argmem: none, inaccessiblemem: none) uwtable
define dso_local noundef double @_Z6Energyv() local_unnamed_addr #3 {
entry:
  %0 = load i8, ptr @c, align 1, !tbaa !9, !range !11, !noundef !12
  %tobool = icmp ne i8 %0, 0
  %conv = uitofp i1 %tobool to double
  %1 = tail call double @llvm.fmuladd.f64(double noundef %conv, double noundef -2.000000e+00, double noundef 1.000000e+00)
  %2 = load i8, ptr getelementptr inbounds ([3 x i8], ptr @c, i64 0, i64 1), align 1, !tbaa !9, !range !11, !noundef !12
  %tobool1 = icmp ne i8 %2, 0
  %conv2 = uitofp i1 %tobool1 to double
  %3 = tail call double @llvm.fmuladd.f64(double noundef %conv2, double noundef -2.000000e+00, double noundef 1.000000e+00)
  %neg = fmul double %3, -1.340000e+00
  %4 = tail call double @llvm.fmuladd.f64(double noundef %1, double noundef 2.300000e-01, double noundef %neg)
  %5 = load i8, ptr getelementptr inbounds ([3 x i8], ptr @c, i64 0, i64 2), align 1, !tbaa !9, !range !11, !noundef !12
  %tobool5 = icmp ne i8 %5, 0
  %conv6 = uitofp i1 %tobool5 to double
  %6 = tail call double @llvm.fmuladd.f64(double noundef %conv6, double noundef -2.000000e+00, double noundef 1.000000e+00)
  %7 = tail call double @llvm.fmuladd.f64(double noundef %6, double noundef 2.140000e+00, double noundef %4)
  %mul = fmul double %6, 1.300000e-01
  %8 = tail call double @llvm.fmuladd.f64(double noundef %mul, double noundef %1, double noundef %7)
  ret double %8
}

; Function Attrs: nocallback nofree nosync nounwind speculatable willreturn memory(none)
declare double @llvm.fmuladd.f64(double, double, double) #4

; Function Attrs: mustprogress noinline nounwind uwtable
define dso_local void @_Z5MeasHv.stub() local_unnamed_addr #1 {
entry:
  br label %aqcc.quantum

aqcc.quantum:                                     ; preds = %entry
  %QRTcall = call i32 @_ZN4qrt014scheduleKernelEPFvvEj(ptr @"_Z14MeasH().QBB.2.v.stub", i32 7)
  br label %aqcc.meas.move.end

aqcc.meas.move.end:                               ; preds = %aqcc.quantum
  %QRTcast2 = bitcast ptr getelementptr inbounds ([3 x i8], ptr @c, i64 0, i64 2) to ptr
  call void @_ZN4qrt020updateMeasurementMapEPbmb(ptr %QRTcast2, i64 2, i1 false)
  %QRTcast1 = bitcast ptr getelementptr inbounds ([3 x i8], ptr @c, i64 0, i64 1) to ptr
  call void @_ZN4qrt020updateMeasurementMapEPbmb(ptr %QRTcast1, i64 1, i1 false)
  %QRTcast = bitcast ptr @c to ptr
  call void @_ZN4qrt020updateMeasurementMapEPbmb(ptr %QRTcast, i64 0, i1 false)
  store [3 x i16] [i16 0, i16 1, i16 2], ptr @q, align 2
  ret void
}

; Function Attrs: mustprogress norecurse nounwind uwtable
define dso_local noundef i32 @main() local_unnamed_addr #5 {
entry:
  %angs = alloca [5 x double], align 16
  call void @llvm.lifetime.start.p0(i64 noundef 40, ptr noundef nonnull align 16 dereferenceable(40) %angs) #7
  store <2 x double> <double 1.000000e+00, double 1.000000e+00>, ptr %angs, align 16
  %0 = getelementptr inbounds [5 x double], ptr %angs, i64 0, i64 2
  store <2 x double> <double 1.000000e+00, double 1.000000e+00>, ptr %0, align 16
  %1 = getelementptr inbounds [5 x double], ptr %angs, i64 0, i64 4
  store double 1.000000e+00, ptr %1, align 16
  br label %while.cond

while.cond:                                       ; preds = %while.body, %entry
  %2 = load i8, ptr @c, align 1, !tbaa !9, !range !11, !noundef !12
  %tobool.i = icmp ne i8 %2, 0
  %conv.i = uitofp i1 %tobool.i to double
  %3 = tail call double @llvm.fmuladd.f64(double noundef %conv.i, double noundef -2.000000e+00, double noundef 1.000000e+00)
  %4 = load i8, ptr getelementptr inbounds ([3 x i8], ptr @c, i64 0, i64 1), align 1, !tbaa !9, !range !11, !noundef !12
  %tobool1.i = icmp ne i8 %4, 0
  %conv2.i = uitofp i1 %tobool1.i to double
  %5 = tail call double @llvm.fmuladd.f64(double noundef %conv2.i, double noundef -2.000000e+00, double noundef 1.000000e+00)
  %neg.i = fmul double %5, -1.340000e+00
  %6 = tail call double @llvm.fmuladd.f64(double noundef %3, double noundef 2.300000e-01, double noundef %neg.i)
  %7 = load i8, ptr getelementptr inbounds ([3 x i8], ptr @c, i64 0, i64 2), align 1, !tbaa !9, !range !11, !noundef !12
  %tobool5.i = icmp ne i8 %7, 0
  %conv6.i = uitofp i1 %tobool5.i to double
  %8 = tail call double @llvm.fmuladd.f64(double noundef %conv6.i, double noundef -2.000000e+00, double noundef 1.000000e+00)
  %9 = tail call double @llvm.fmuladd.f64(double noundef %8, double noundef 2.140000e+00, double noundef %6)
  %mul.i = fmul double %8, 1.300000e-01
  %10 = tail call double @llvm.fmuladd.f64(double noundef %mul.i, double noundef %3, double noundef %9)
  %cmp.i = fcmp ogt double %10, 0.000000e+00
  br i1 %cmp.i, label %while.body, label %while.end

while.body:                                       ; preds = %while.cond
  tail call void @_Z7PrepAllv.stub() #7
  call void @_Z10VQE_anzatzPd.stub(ptr noalias nocapture nofree noundef nonnull readonly align 16 dereferenceable(40) %angs) #7
  tail call void @_Z5MeasHv.stub() #7
  br label %while.cond, !llvm.loop !13

while.end:                                        ; preds = %while.cond
  call void @llvm.lifetime.end.p0(i64 noundef 40, ptr noundef nonnull align 16 dereferenceable(40) %angs) #7
  ret i32 0
}

; Function Attrs: nocallback nofree nosync nounwind willreturn memory(argmem: readwrite)
declare void @llvm.lifetime.start.p0(i64 immarg, ptr nocapture) #6

; Function Attrs: nocallback nofree nosync nounwind willreturn memory(argmem: readwrite)
declare void @llvm.lifetime.end.p0(i64 immarg, ptr nocapture) #6

declare i32 @_ZN4qrt021initializeQrtSystem64EPdPcS1_(ptr, ptr, ptr)

declare i1 @_ZN4qrt013isInitializedEv()

declare void @_ZN4qrt020updateMeasurementMapEPbmb(ptr, i64, i1)

declare i32 @_ZN4qrt014scheduleKernelEPFvvEj(ptr, i32)

declare i32 @_ZN4qrt018dismantleQrtSystemEv()

; Function Attrs: nounwind
declare void @llvm.quantum.rotz(i16, double, i32 immarg) #7

; Function Attrs: nounwind
declare void @llvm.quantum.rotxy(i16, double, double, i32 immarg) #7

; Function Attrs: nounwind
declare void @llvm.quantum.cphase(i16, i16, double, i32 immarg) #7

; Function Attrs: nounwind
declare void @llvm.quantum.swapalp(i16, i16, double, i32 immarg) #7

; Function Attrs: nounwind
declare void @llvm.quantum.measz(i16, ptr, i1, i32 immarg) #7

; Function Attrs: nounwind
declare void @llvm.quantum.prepz(i16, i32 immarg) #7

; Function Attrs: nounwind
declare void @llvm.quantum.rotz.imm(i16 immarg, double immarg, i32 immarg) #7

; Function Attrs: nounwind
declare void @llvm.quantum.rotxy.imm(i16 immarg, double immarg, double immarg, i32 immarg) #7

; Function Attrs: nounwind
declare void @llvm.quantum.cphase.imm(i16 immarg, i16 immarg, double immarg, i32 immarg) #7

; Function Attrs: nounwind
declare void @llvm.quantum.swapalp.imm(i16 immarg, i16 immarg, double immarg, i32 immarg) #7

; Function Attrs: nounwind
declare void @llvm.quantum.measz.imm(i16 immarg, i32 immarg) #7

; Function Attrs: nounwind
declare void @llvm.quantum.prepz.imm(i16 immarg, i32 immarg) #7

; Function Attrs: nounwind
declare void @llvm.quantum.rotz.imm.index(i16 immarg, i32 immarg, i32 immarg) #7

; Function Attrs: nounwind
declare void @llvm.quantum.rotxy.imm.index(i16 immarg, i32 immarg, i32 immarg, i32 immarg) #7

; Function Attrs: nounwind
declare void @llvm.quantum.cphase.imm.index(i16 immarg, i16 immarg, i32 immarg, i32 immarg) #7

; Function Attrs: nounwind
declare void @llvm.quantum.swapalp.imm.index(i16 immarg, i16 immarg, i32 immarg, i32 immarg) #7

; Function Attrs: nounwind
declare void @llvm.quantum.measz.imm.index(i16 immarg, i32 immarg, i32 immarg) #7

define void @"_Z16PrepAll().QBB.0.v.stub"() section ".qbbs_text" {
aqcc.quantum:
  call void @llvm.quantum.rotxy.imm(i16 0, double 0x3FF921FB54442D18, double 0x4012D97C7F3321D2, i32 1)
  call void @llvm.quantum.prepz.imm(i16 0, i32 0)
  call void @llvm.quantum.rotxy.imm(i16 0, double 0x3FF921FB54442D18, double 0x3FF921FB54442D18, i32 0)
  call void @llvm.quantum.rotxy.imm(i16 1, double 0x3FF921FB54442D18, double 0x4012D97C7F3321D2, i32 0)
  call void @llvm.quantum.prepz.imm(i16 1, i32 0)
  call void @llvm.quantum.rotxy.imm(i16 1, double 0x3FF921FB54442D18, double 0x3FF921FB54442D18, i32 0)
  call void @llvm.quantum.rotxy.imm(i16 2, double 0x3FF921FB54442D18, double 0x4012D97C7F3321D2, i32 0)
  call void @llvm.quantum.prepz.imm(i16 2, i32 0)
  call void @llvm.quantum.rotxy.imm(i16 2, double 0x3FF921FB54442D18, double 0x3FF921FB54442D18, i32 2)
  ret void
}

define void @"_Z26VQE_anzatz(double*).QBB.1.v.stub"() section ".qbbs_text" {
aqcc.quantum:
  call void @llvm.quantum.rotxy.imm.index(i16 0, i32 4, i32 0, i32 1)
  call void @llvm.quantum.rotxy.imm.index(i16 1, i32 5, i32 0, i32 0)
  call void @llvm.quantum.rotxy.imm.index(i16 2, i32 6, i32 0, i32 0)
  call void @llvm.quantum.rotxy.imm(i16 1, double 0x3FF921FB54442D18, double 0x4012D97C7F3321D2, i32 0)
  call void @llvm.quantum.cphase.imm(i16 0, i16 1, double 0x400921FB54442D18, i32 0)
  call void @llvm.quantum.rotxy.imm(i16 1, double 0x3FF921FB54442D18, double 0x3FF921FB54442D18, i32 0)
  call void @llvm.quantum.rotxy.imm.index(i16 1, i32 7, i32 1, i32 0)
  call void @llvm.quantum.rotxy.imm(i16 2, double 0x3FF921FB54442D18, double 0x4012D97C7F3321D2, i32 0)
  call void @llvm.quantum.cphase.imm(i16 1, i16 2, double 0x400921FB54442D18, i32 0)
  call void @llvm.quantum.rotxy.imm(i16 2, double 0x3FF921FB54442D18, double 0x3FF921FB54442D18, i32 0)
  call void @llvm.quantum.rotxy.imm(i16 2, double 0x400921FB54442D18, double 0.000000e+00, i32 0)
  call void @llvm.quantum.rotxy.imm.index(i16 2, i32 2, i32 8, i32 0)
  call void @llvm.quantum.rotxy.imm(i16 2, double 0x3FF921FB54442D18, double 0x4012D97C7F3321D2, i32 0)
  call void @llvm.quantum.cphase.imm(i16 1, i16 2, double 0x400921FB54442D18, i32 0)
  call void @llvm.quantum.rotxy.imm(i16 2, double 0x3FF921FB54442D18, double 0x3FF921FB54442D18, i32 0)
  call void @llvm.quantum.rotxy.imm(i16 1, double 0x3FF921FB54442D18, double 0x4012D97C7F3321D2, i32 0)
  call void @llvm.quantum.cphase.imm(i16 0, i16 1, double 0x400921FB54442D18, i32 0)
  call void @llvm.quantum.rotxy.imm(i16 1, double 0x3FF921FB54442D18, double 0x3FF921FB54442D18, i32 2)
  ret void
}

define void @"_Z14MeasH().QBB.2.v.stub"() section ".qbbs_text" {
aqcc.quantum:
  call void @llvm.quantum.measz.imm.index(i16 0, i32 0, i32 1)
  call void @llvm.quantum.rotxy.imm(i16 1, double 0x3FF921FB54442D18, double 0x4012D97C7F3321D2, i32 0)
  call void @llvm.quantum.measz.imm.index(i16 1, i32 1, i32 0)
  call void @llvm.quantum.rotxy.imm(i16 1, double 0x3FF921FB54442D18, double 0x3FF921FB54442D18, i32 0)
  call void @llvm.quantum.rotxy.imm(i16 2, double 0x3FF921FB54442D18, double 0.000000e+00, i32 0)
  call void @llvm.quantum.measz.imm.index(i16 2, i32 2, i32 0)
  call void @llvm.quantum.rotxy.imm(i16 2, double 0x3FF921FB54442D18, double 0x400921FB54442D18, i32 2)
  ret void
}

attributes #0 = { "quantum_shared_var" }
attributes #1 = { mustprogress noinline nounwind uwtable "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #2 = { mustprogress nofree norecurse nosync nounwind willreturn memory(none) uwtable "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #3 = { mustprogress nofree norecurse nosync nounwind willreturn memory(read, argmem: none, inaccessiblemem: none) uwtable "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #4 = { nocallback nofree nosync nounwind speculatable willreturn memory(none) }
attributes #5 = { mustprogress norecurse nounwind uwtable "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #6 = { nocallback nofree nosync nounwind willreturn memory(argmem: readwrite) }
attributes #7 = { nounwind }

!llvm.module.flags = !{!0, !1, !2, !3}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{i32 8, !"PIC Level", i32 2}
!2 = !{i32 7, !"PIE Level", i32 2}
!3 = !{i32 7, !"uwtable", i32 2}
!5 = !{!6, !6, i64 0}
!6 = !{!"double", !7, i64 0}
!7 = !{!"omnipotent char", !8, i64 0}
!8 = !{!"Simple C++ TBAA"}
!9 = !{!10, !10, i64 0}
!10 = !{!"bool", !7, i64 0}
!11 = !{i8 0, i8 2}
!12 = !{}
!13 = distinct !{!13, !14, !15}
!14 = !{!"llvm.loop.mustprogress"}
!15 = !{!"llvm.loop.unroll.disable"}

)";

#endif
