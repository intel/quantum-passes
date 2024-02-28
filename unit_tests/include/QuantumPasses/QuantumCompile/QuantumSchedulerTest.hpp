//===- QuantumScheduler.hpp --------------------------------*- C++ -*-===//
//
//
// Copyright (C) 2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//
//===----------------------------------------------------------------------===//
/// \file "QuantumScheduler.hpp"
///
/// This file provides the raw strings for Quantum Scheduler unit test.
///
//===----------------------------------------------------------------------===//

#ifndef QUANTUM_SCHEDULER_TEST_HPP
#define QUANTUM_SCHEDULER_TEST_HPP

/// Content of the .ll file describing the quantum program
/// used in unit tests for the QuantumScheduler pass.

// Code representing a single QBB with a few CNOT gates that cannot be scheduled
// without SWAPs on a linear connectivity. QBB is "hold" so initial and final
// qubit map should coincide to the desired ones from the QuantumModule.
static const char CodeHold[] = R"(
; ModuleID = 'CNOTs_beyond_linear_connectivity_conditioned_user.ll'
source_filename = "unit_tests/source-files/CNOTs_beyond_linear_connectivity.cpp"
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
@.str.24 = private unnamed_addr constant [503 x i8] c"{     \22matrix_real\22 : \22[1, 0, 0, 0, 0, (1+cos(angle))/2, (1-cos(angle))/2, 0, 0, (1-cos(angle))/2, (1+cos(angle))/2, 0, 0, 0, 0, 1]\22,     \22matrix_imag\22 : \22[0, 0, 0, 0, 0, sin(angle)/2, -sin(angle)/2, 0, 0, -sin(angle)/2, sin(angle)/2, 0, 0, 0, 0, 0]\22,     \22matrix_order\22 : \22rm\22,     \22is_hermitian\22 : false,     \22is_unitary\22 : true,     \22is_mutable\22 : true,     \22qubit_list\22 : [0, 1],     \22parametric_list\22 : [2],     \22control_qubit_list\22 : [],     \22local_basis_list\22 : [0, 0],     \22identifier\22 : 53   }\00", section "llvm.metadata"
@q = dso_local global [5 x i16] zeroinitializer, align 2
@c = dso_local local_unnamed_addr global [2 x i8] zeroinitializer, align 1
@llvm.global.annotations = appending global [24 x { ptr, ptr, ptr, i32, ptr }] [{ ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str, ptr @.str.1, i32 47, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.2, ptr @.str.1, i32 66, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.3, ptr @.str.1, i32 85, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.4, ptr @.str.1, i32 105, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.5, ptr @.str.1, i32 124, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.6, ptr @.str.1, i32 143, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.7, ptr @.str.1, i32 162, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.8, ptr @.str.1, i32 181, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.9, ptr @.str.1, i32 200, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.10, ptr @.str.1, i32 221, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.11, ptr @.str.1, i32 242, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.12, ptr @.str.1, i32 263, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.13, ptr @.str.1, i32 284, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.14, ptr @.str.1, i32 305, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.15, ptr @.str.1, i32 326, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.16, ptr @.str.1, i32 350, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.17, ptr @.str.1, i32 367, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.18, ptr @.str.1, i32 384, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.19, ptr @.str.1, i32 401, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.20, ptr @.str.1, i32 421, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.21, ptr @.str.1, i32 441, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.22, ptr @.str.1, i32 469, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.23, ptr @.str.1, i32 491, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.24, ptr @.str.1, i32 513, ptr null }], section "llvm.metadata"

; Function Attrs: mustprogress noinline nounwind uwtable
define dso_local void @_Z7circuitv.stub() local_unnamed_addr #0 section ".qbbs_text" {
  br label %aqcc.quantum

aqcc.quantum:                                     ; preds = %0
  %arrayidx38 = getelementptr inbounds [5 x i16], ptr @q, i64 0, i64 0
  %1 = load i16, ptr %arrayidx38, align 2
  call void @llvm.quantum.rotxy(i16 %1, double 0x3FF921FB54442D18, double 0x3FF921FB54442D18, i32 1)
  %arrayidx37 = getelementptr inbounds [5 x i16], ptr @q, i64 0, i64 0
  %2 = load i16, ptr %arrayidx37, align 2
  call void @llvm.quantum.rotxy(i16 %2, double 0x400921FB54442D18, double 0.000000e+00, i32 0)
  %arrayidx36 = getelementptr inbounds [5 x i16], ptr @q, i64 0, i64 1
  %3 = load i16, ptr %arrayidx36, align 2
  call void @llvm.quantum.rotxy(i16 %3, double 0x3FF921FB54442D18, double 0x4012D97C7F3321D2, i32 0)
  %arrayidx34 = getelementptr inbounds [5 x i16], ptr @q, i64 0, i64 0
  %4 = load i16, ptr %arrayidx34, align 2
  %arrayidx35 = getelementptr inbounds [5 x i16], ptr @q, i64 0, i64 1
  %5 = load i16, ptr %arrayidx35, align 2
  call void @llvm.quantum.cphase(i16 %4, i16 %5, double 0x400921FB54442D18, i32 0)
  %arrayidx33 = getelementptr inbounds [5 x i16], ptr @q, i64 0, i64 1
  %6 = load i16, ptr %arrayidx33, align 2
  call void @llvm.quantum.rotxy(i16 %6, double 0x3FF921FB54442D18, double 0x3FF921FB54442D18, i32 0)
  %arrayidx32 = getelementptr inbounds [5 x i16], ptr @q, i64 0, i64 2
  %7 = load i16, ptr %arrayidx32, align 2
  call void @llvm.quantum.rotxy(i16 %7, double 0x3FF921FB54442D18, double 0x4012D97C7F3321D2, i32 0)
  %arrayidx30 = getelementptr inbounds [5 x i16], ptr @q, i64 0, i64 1
  %8 = load i16, ptr %arrayidx30, align 2
  %arrayidx31 = getelementptr inbounds [5 x i16], ptr @q, i64 0, i64 2
  %9 = load i16, ptr %arrayidx31, align 2
  call void @llvm.quantum.cphase(i16 %8, i16 %9, double 0x400921FB54442D18, i32 0)
  %arrayidx29 = getelementptr inbounds [5 x i16], ptr @q, i64 0, i64 2
  %10 = load i16, ptr %arrayidx29, align 2
  call void @llvm.quantum.rotxy(i16 %10, double 0x3FF921FB54442D18, double 0x3FF921FB54442D18, i32 0)
  %arrayidx28 = getelementptr inbounds [5 x i16], ptr @q, i64 0, i64 0
  %11 = load i16, ptr %arrayidx28, align 2
  call void @llvm.quantum.rotxy(i16 %11, double 0x3FF921FB54442D18, double 0x4012D97C7F3321D2, i32 0)
  %arrayidx26 = getelementptr inbounds [5 x i16], ptr @q, i64 0, i64 3
  %12 = load i16, ptr %arrayidx26, align 2
  %arrayidx27 = getelementptr inbounds [5 x i16], ptr @q, i64 0, i64 0
  %13 = load i16, ptr %arrayidx27, align 2
  call void @llvm.quantum.cphase(i16 %12, i16 %13, double 0x400921FB54442D18, i32 0)
  %arrayidx25 = getelementptr inbounds [5 x i16], ptr @q, i64 0, i64 0
  %14 = load i16, ptr %arrayidx25, align 2
  call void @llvm.quantum.rotxy(i16 %14, double 0x3FF921FB54442D18, double 0x3FF921FB54442D18, i32 0)
  %arrayidx24 = getelementptr inbounds [5 x i16], ptr @q, i64 0, i64 4
  %15 = load i16, ptr %arrayidx24, align 2
  call void @llvm.quantum.rotxy(i16 %15, double 0x3FF921FB54442D18, double 0x4012D97C7F3321D2, i32 0)
  %arrayidx22 = getelementptr inbounds [5 x i16], ptr @q, i64 0, i64 1
  %16 = load i16, ptr %arrayidx22, align 2
  %arrayidx23 = getelementptr inbounds [5 x i16], ptr @q, i64 0, i64 4
  %17 = load i16, ptr %arrayidx23, align 2
  call void @llvm.quantum.cphase(i16 %16, i16 %17, double 0x400921FB54442D18, i32 0)
  %arrayidx21 = getelementptr inbounds [5 x i16], ptr @q, i64 0, i64 4
  %18 = load i16, ptr %arrayidx21, align 2
  call void @llvm.quantum.rotxy(i16 %18, double 0x3FF921FB54442D18, double 0x3FF921FB54442D18, i32 0)
  %arrayidx20 = getelementptr inbounds [5 x i16], ptr @q, i64 0, i64 2
  %19 = load i16, ptr %arrayidx20, align 2
  call void @llvm.quantum.rotxy(i16 %19, double 0x3FF921FB54442D18, double 0x4012D97C7F3321D2, i32 0)
  %arrayidx18 = getelementptr inbounds [5 x i16], ptr @q, i64 0, i64 4
  %20 = load i16, ptr %arrayidx18, align 2
  %arrayidx19 = getelementptr inbounds [5 x i16], ptr @q, i64 0, i64 2
  %21 = load i16, ptr %arrayidx19, align 2
  call void @llvm.quantum.cphase(i16 %20, i16 %21, double 0x400921FB54442D18, i32 0)
  %arrayidx17 = getelementptr inbounds [5 x i16], ptr @q, i64 0, i64 2
  %22 = load i16, ptr %arrayidx17, align 2
  call void @llvm.quantum.rotxy(i16 %22, double 0x3FF921FB54442D18, double 0x3FF921FB54442D18, i32 0)
  %arrayidx16 = getelementptr inbounds [5 x i16], ptr @q, i64 0, i64 3
  %23 = load i16, ptr %arrayidx16, align 2
  call void @llvm.quantum.rotxy(i16 %23, double 0x3FF921FB54442D18, double 0x4012D97C7F3321D2, i32 0)
  %arrayidx14 = getelementptr inbounds [5 x i16], ptr @q, i64 0, i64 2
  %24 = load i16, ptr %arrayidx14, align 2
  %arrayidx15 = getelementptr inbounds [5 x i16], ptr @q, i64 0, i64 3
  %25 = load i16, ptr %arrayidx15, align 2
  call void @llvm.quantum.cphase(i16 %24, i16 %25, double 0x400921FB54442D18, i32 0)
  %arrayidx13 = getelementptr inbounds [5 x i16], ptr @q, i64 0, i64 3
  %26 = load i16, ptr %arrayidx13, align 2
  call void @llvm.quantum.rotxy(i16 %26, double 0x3FF921FB54442D18, double 0x3FF921FB54442D18, i32 2)
  br label %aqcc.meas.move.end

aqcc.meas.move.end:                               ; preds = %aqcc.quantum
  ret void
}

; Function Attrs: mustprogress norecurse nounwind uwtable
define dso_local noundef i32 @main() local_unnamed_addr #1 {
  tail call void @_Z7circuitv.stub() #2
  ret i32 0
}

declare i32 @_ZN4qrt021initializeQrtSystem64EPdPcS1_(ptr, ptr, ptr)

declare i1 @_ZN4qrt013isInitializedEv()

declare void @_ZN4qrt020updateMeasurementMapEPbmb(ptr, i64, i1)

declare i32 @_ZN4qrt014scheduleKernelEPFvvEj(ptr, i32)

declare i32 @_ZN4qrt018dismantleQrtSystemEv()

declare void @_ZN4qrt04exitEPc(ptr)

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
attributes #1 = { mustprogress norecurse nounwind uwtable "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #2 = { nounwind }

!llvm.module.flags = !{!0, !1, !2, !3}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{i32 8, !"PIC Level", i32 2}
!2 = !{i32 7, !"PIE Level", i32 2}
!3 = !{i32 7, !"uwtable", i32 2}
)";

// Code representing a single QBB with a few CNOT gates that cannot be scheduled
// without SWAPs on a linear connectivity. QBB is "release" so the final qubit
// map should be updated in the QuantumModule.
static const char CodeRelease[] = R"(
; ModuleID = 'CNOTs_beyond_linear_connectivity_conditioned_user.ll'
source_filename = "unit_tests/source-files/CNOTs_beyond_linear_connectivity.cpp"
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
@.str.24 = private unnamed_addr constant [503 x i8] c"{     \22matrix_real\22 : \22[1, 0, 0, 0, 0, (1+cos(angle))/2, (1-cos(angle))/2, 0, 0, (1-cos(angle))/2, (1+cos(angle))/2, 0, 0, 0, 0, 1]\22,     \22matrix_imag\22 : \22[0, 0, 0, 0, 0, sin(angle)/2, -sin(angle)/2, 0, 0, -sin(angle)/2, sin(angle)/2, 0, 0, 0, 0, 0]\22,     \22matrix_order\22 : \22rm\22,     \22is_hermitian\22 : false,     \22is_unitary\22 : true,     \22is_mutable\22 : true,     \22qubit_list\22 : [0, 1],     \22parametric_list\22 : [2],     \22control_qubit_list\22 : [],     \22local_basis_list\22 : [0, 0],     \22identifier\22 : 53   }\00", section "llvm.metadata"
@q = dso_local global [5 x i16] zeroinitializer, align 2
@c = dso_local local_unnamed_addr global [2 x i8] zeroinitializer, align 1
@llvm.global.annotations = appending global [24 x { ptr, ptr, ptr, i32, ptr }] [{ ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str, ptr @.str.1, i32 47, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.2, ptr @.str.1, i32 66, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.3, ptr @.str.1, i32 85, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.4, ptr @.str.1, i32 105, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.5, ptr @.str.1, i32 124, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.6, ptr @.str.1, i32 143, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.7, ptr @.str.1, i32 162, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.8, ptr @.str.1, i32 181, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.9, ptr @.str.1, i32 200, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.10, ptr @.str.1, i32 221, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.11, ptr @.str.1, i32 242, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.12, ptr @.str.1, i32 263, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.13, ptr @.str.1, i32 284, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.14, ptr @.str.1, i32 305, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.15, ptr @.str.1, i32 326, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.16, ptr @.str.1, i32 350, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.17, ptr @.str.1, i32 367, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.18, ptr @.str.1, i32 384, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.19, ptr @.str.1, i32 401, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.20, ptr @.str.1, i32 421, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.21, ptr @.str.1, i32 441, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.22, ptr @.str.1, i32 469, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.23, ptr @.str.1, i32 491, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.24, ptr @.str.1, i32 513, ptr null }], section "llvm.metadata"

; Function Attrs: mustprogress noinline nounwind uwtable
define dso_local void @_Z7circuitv.stub() local_unnamed_addr #0 section ".qbbs_text" {
  br label %aqcc.quantum.release

aqcc.quantum.release:                             ; preds = %0
  %arrayidx38 = getelementptr inbounds [5 x i16], ptr @q, i64 0, i64 0
  %1 = load i16, ptr %arrayidx38, align 2
  call void @llvm.quantum.rotxy(i16 %1, double 0x3FF921FB54442D18, double 0x3FF921FB54442D18, i32 1)
  %arrayidx37 = getelementptr inbounds [5 x i16], ptr @q, i64 0, i64 0
  %2 = load i16, ptr %arrayidx37, align 2
  call void @llvm.quantum.rotxy(i16 %2, double 0x400921FB54442D18, double 0.000000e+00, i32 0)
  %arrayidx36 = getelementptr inbounds [5 x i16], ptr @q, i64 0, i64 1
  %3 = load i16, ptr %arrayidx36, align 2
  call void @llvm.quantum.rotxy(i16 %3, double 0x3FF921FB54442D18, double 0x4012D97C7F3321D2, i32 0)
  %arrayidx34 = getelementptr inbounds [5 x i16], ptr @q, i64 0, i64 0
  %4 = load i16, ptr %arrayidx34, align 2
  %arrayidx35 = getelementptr inbounds [5 x i16], ptr @q, i64 0, i64 1
  %5 = load i16, ptr %arrayidx35, align 2
  call void @llvm.quantum.cphase(i16 %4, i16 %5, double 0x400921FB54442D18, i32 0)
  %arrayidx33 = getelementptr inbounds [5 x i16], ptr @q, i64 0, i64 1
  %6 = load i16, ptr %arrayidx33, align 2
  call void @llvm.quantum.rotxy(i16 %6, double 0x3FF921FB54442D18, double 0x3FF921FB54442D18, i32 0)
  %arrayidx32 = getelementptr inbounds [5 x i16], ptr @q, i64 0, i64 2
  %7 = load i16, ptr %arrayidx32, align 2
  call void @llvm.quantum.rotxy(i16 %7, double 0x3FF921FB54442D18, double 0x4012D97C7F3321D2, i32 0)
  %arrayidx30 = getelementptr inbounds [5 x i16], ptr @q, i64 0, i64 1
  %8 = load i16, ptr %arrayidx30, align 2
  %arrayidx31 = getelementptr inbounds [5 x i16], ptr @q, i64 0, i64 2
  %9 = load i16, ptr %arrayidx31, align 2
  call void @llvm.quantum.cphase(i16 %8, i16 %9, double 0x400921FB54442D18, i32 0)
  %arrayidx29 = getelementptr inbounds [5 x i16], ptr @q, i64 0, i64 2
  %10 = load i16, ptr %arrayidx29, align 2
  call void @llvm.quantum.rotxy(i16 %10, double 0x3FF921FB54442D18, double 0x3FF921FB54442D18, i32 0)
  %arrayidx28 = getelementptr inbounds [5 x i16], ptr @q, i64 0, i64 0
  %11 = load i16, ptr %arrayidx28, align 2
  call void @llvm.quantum.rotxy(i16 %11, double 0x3FF921FB54442D18, double 0x4012D97C7F3321D2, i32 0)
  %arrayidx26 = getelementptr inbounds [5 x i16], ptr @q, i64 0, i64 3
  %12 = load i16, ptr %arrayidx26, align 2
  %arrayidx27 = getelementptr inbounds [5 x i16], ptr @q, i64 0, i64 0
  %13 = load i16, ptr %arrayidx27, align 2
  call void @llvm.quantum.cphase(i16 %12, i16 %13, double 0x400921FB54442D18, i32 0)
  %arrayidx25 = getelementptr inbounds [5 x i16], ptr @q, i64 0, i64 0
  %14 = load i16, ptr %arrayidx25, align 2
  call void @llvm.quantum.rotxy(i16 %14, double 0x3FF921FB54442D18, double 0x3FF921FB54442D18, i32 0)
  %arrayidx24 = getelementptr inbounds [5 x i16], ptr @q, i64 0, i64 4
  %15 = load i16, ptr %arrayidx24, align 2
  call void @llvm.quantum.rotxy(i16 %15, double 0x3FF921FB54442D18, double 0x4012D97C7F3321D2, i32 0)
  %arrayidx22 = getelementptr inbounds [5 x i16], ptr @q, i64 0, i64 1
  %16 = load i16, ptr %arrayidx22, align 2
  %arrayidx23 = getelementptr inbounds [5 x i16], ptr @q, i64 0, i64 4
  %17 = load i16, ptr %arrayidx23, align 2
  call void @llvm.quantum.cphase(i16 %16, i16 %17, double 0x400921FB54442D18, i32 0)
  %arrayidx21 = getelementptr inbounds [5 x i16], ptr @q, i64 0, i64 4
  %18 = load i16, ptr %arrayidx21, align 2
  call void @llvm.quantum.rotxy(i16 %18, double 0x3FF921FB54442D18, double 0x3FF921FB54442D18, i32 0)
  %arrayidx20 = getelementptr inbounds [5 x i16], ptr @q, i64 0, i64 2
  %19 = load i16, ptr %arrayidx20, align 2
  call void @llvm.quantum.rotxy(i16 %19, double 0x3FF921FB54442D18, double 0x4012D97C7F3321D2, i32 0)
  %arrayidx18 = getelementptr inbounds [5 x i16], ptr @q, i64 0, i64 4
  %20 = load i16, ptr %arrayidx18, align 2
  %arrayidx19 = getelementptr inbounds [5 x i16], ptr @q, i64 0, i64 2
  %21 = load i16, ptr %arrayidx19, align 2
  call void @llvm.quantum.cphase(i16 %20, i16 %21, double 0x400921FB54442D18, i32 0)
  %arrayidx17 = getelementptr inbounds [5 x i16], ptr @q, i64 0, i64 2
  %22 = load i16, ptr %arrayidx17, align 2
  call void @llvm.quantum.rotxy(i16 %22, double 0x3FF921FB54442D18, double 0x3FF921FB54442D18, i32 0)
  %arrayidx16 = getelementptr inbounds [5 x i16], ptr @q, i64 0, i64 3
  %23 = load i16, ptr %arrayidx16, align 2
  call void @llvm.quantum.rotxy(i16 %23, double 0x3FF921FB54442D18, double 0x4012D97C7F3321D2, i32 0)
  %arrayidx14 = getelementptr inbounds [5 x i16], ptr @q, i64 0, i64 2
  %24 = load i16, ptr %arrayidx14, align 2
  %arrayidx15 = getelementptr inbounds [5 x i16], ptr @q, i64 0, i64 3
  %25 = load i16, ptr %arrayidx15, align 2
  call void @llvm.quantum.cphase(i16 %24, i16 %25, double 0x400921FB54442D18, i32 0)
  %arrayidx13 = getelementptr inbounds [5 x i16], ptr @q, i64 0, i64 3
  %26 = load i16, ptr %arrayidx13, align 2
  call void @llvm.quantum.rotxy(i16 %26, double 0x3FF921FB54442D18, double 0x3FF921FB54442D18, i32 2)
  br label %aqcc.meas.move.end

aqcc.meas.move.end:                               ; preds = %aqcc.quantum.release
  ret void
}

; Function Attrs: mustprogress norecurse nounwind uwtable
define dso_local noundef i32 @main() local_unnamed_addr #1 {
  tail call void @_Z7circuitv.stub() #2
  ret i32 0
}

declare i32 @_ZN4qrt021initializeQrtSystem64EPdPcS1_(ptr, ptr, ptr)

declare i1 @_ZN4qrt013isInitializedEv()

declare void @_ZN4qrt020updateMeasurementMapEPbmb(ptr, i64, i1)

declare i32 @_ZN4qrt014scheduleKernelEPFvvEj(ptr, i32)

declare i32 @_ZN4qrt018dismantleQrtSystemEv()

declare void @_ZN4qrt04exitEPc(ptr)

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
attributes #1 = { mustprogress norecurse nounwind uwtable "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #2 = { nounwind }

!llvm.module.flags = !{!0, !1, !2, !3}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{i32 8, !"PIC Level", i32 2}
!2 = !{i32 7, !"PIE Level", i32 2}
!3 = !{i32 7, !"uwtable", i32 2}
)";

// Code representing a single QBB with several CNOT gates.
// QBB is "hold" so initial and final qubit map should coincide.
static const char CodeCnots[] = R"(
; ModuleID = 'random_CNOTs_10q_lowered.ll'
source_filename = "unit_tests/source-files/random_CNOTs_10q.cpp"
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
@.str.24 = private unnamed_addr constant [503 x i8] c"{     \22matrix_real\22 : \22[1, 0, 0, 0, 0, (1+cos(angle))/2, (1-cos(angle))/2, 0, 0, (1-cos(angle))/2, (1+cos(angle))/2, 0, 0, 0, 0, 1]\22,     \22matrix_imag\22 : \22[0, 0, 0, 0, 0, sin(angle)/2, -sin(angle)/2, 0, 0, -sin(angle)/2, sin(angle)/2, 0, 0, 0, 0, 0]\22,     \22matrix_order\22 : \22rm\22,     \22is_hermitian\22 : false,     \22is_unitary\22 : true,     \22is_mutable\22 : true,     \22qubit_list\22 : [0, 1],     \22parametric_list\22 : [2],     \22control_qubit_list\22 : [],     \22local_basis_list\22 : [0, 0],     \22identifier\22 : 53   }\00", section "llvm.metadata"
@q = dso_local global [10 x i16] zeroinitializer, align 16
@c = dso_local local_unnamed_addr global [1 x i8] zeroinitializer, align 1
@llvm.global.annotations = appending global [24 x { ptr, ptr, ptr, i32, ptr }] [{ ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str, ptr @.str.1, i32 47, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.2, ptr @.str.1, i32 66, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.3, ptr @.str.1, i32 85, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.4, ptr @.str.1, i32 105, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.5, ptr @.str.1, i32 124, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.6, ptr @.str.1, i32 143, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.7, ptr @.str.1, i32 162, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.8, ptr @.str.1, i32 181, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.9, ptr @.str.1, i32 200, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.10, ptr @.str.1, i32 221, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.11, ptr @.str.1, i32 242, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.12, ptr @.str.1, i32 263, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.13, ptr @.str.1, i32 284, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.14, ptr @.str.1, i32 305, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.15, ptr @.str.1, i32 326, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.16, ptr @.str.1, i32 350, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.17, ptr @.str.1, i32 367, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.18, ptr @.str.1, i32 384, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.19, ptr @.str.1, i32 401, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.20, ptr @.str.1, i32 421, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.21, ptr @.str.1, i32 441, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.22, ptr @.str.1, i32 469, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.23, ptr @.str.1, i32 491, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.24, ptr @.str.1, i32 513, ptr null }], section "llvm.metadata"

; Function Attrs: mustprogress noinline nounwind uwtable
define dso_local void @_Z7circuitv.stub() local_unnamed_addr #0 section ".qbbs_text" {
  br label %aqcc.quantum

aqcc.quantum:                                     ; preds = %0
  %arrayidx158 = getelementptr inbounds [10 x i16], ptr @q, i64 0, i64 0
  %1 = load i16, ptr %arrayidx158, align 16
  call void @llvm.quantum.rotxy(i16 %1, double 0x3FF921FB54442D18, double 0x3FF921FB54442D18, i32 1)
  %arrayidx157 = getelementptr inbounds [10 x i16], ptr @q, i64 0, i64 0
  %2 = load i16, ptr %arrayidx157, align 16
  call void @llvm.quantum.rotxy(i16 %2, double 0x400921FB54442D18, double 0.000000e+00, i32 0)
  %arrayidx156 = getelementptr inbounds [10 x i16], ptr @q, i64 0, i64 1
  %3 = load i16, ptr %arrayidx156, align 16
  call void @llvm.quantum.rotxy(i16 %3, double 0x3FF921FB54442D18, double 0x4012D97C7F3321D2, i32 0)
  %arrayidx154 = getelementptr inbounds [10 x i16], ptr @q, i64 0, i64 0
  %4 = load i16, ptr %arrayidx154, align 16
  %arrayidx155 = getelementptr inbounds [10 x i16], ptr @q, i64 0, i64 1
  %5 = load i16, ptr %arrayidx155, align 16
  call void @llvm.quantum.cphase(i16 %4, i16 %5, double 0x400921FB54442D18, i32 0)
  %arrayidx153 = getelementptr inbounds [10 x i16], ptr @q, i64 0, i64 1
  %6 = load i16, ptr %arrayidx153, align 16
  call void @llvm.quantum.rotxy(i16 %6, double 0x3FF921FB54442D18, double 0x3FF921FB54442D18, i32 0)
  %arrayidx152 = getelementptr inbounds [10 x i16], ptr @q, i64 0, i64 2
  %7 = load i16, ptr %arrayidx152, align 16
  call void @llvm.quantum.rotxy(i16 %7, double 0x3FF921FB54442D18, double 0x4012D97C7F3321D2, i32 0)
  %arrayidx150 = getelementptr inbounds [10 x i16], ptr @q, i64 0, i64 1
  %8 = load i16, ptr %arrayidx150, align 16
  %arrayidx151 = getelementptr inbounds [10 x i16], ptr @q, i64 0, i64 2
  %9 = load i16, ptr %arrayidx151, align 16
  call void @llvm.quantum.cphase(i16 %8, i16 %9, double 0x400921FB54442D18, i32 0)
  %arrayidx149 = getelementptr inbounds [10 x i16], ptr @q, i64 0, i64 2
  %10 = load i16, ptr %arrayidx149, align 16
  call void @llvm.quantum.rotxy(i16 %10, double 0x3FF921FB54442D18, double 0x3FF921FB54442D18, i32 0)
  %arrayidx148 = getelementptr inbounds [10 x i16], ptr @q, i64 0, i64 3
  %11 = load i16, ptr %arrayidx148, align 16
  call void @llvm.quantum.rotxy(i16 %11, double 0x3FF921FB54442D18, double 0x3FF921FB54442D18, i32 0)
  %arrayidx147 = getelementptr inbounds [10 x i16], ptr @q, i64 0, i64 3
  %12 = load i16, ptr %arrayidx147, align 16
  call void @llvm.quantum.rotxy(i16 %12, double 0x400921FB54442D18, double 0.000000e+00, i32 0)
  %arrayidx146 = getelementptr inbounds [10 x i16], ptr @q, i64 0, i64 1
  %13 = load i16, ptr %arrayidx146, align 16
  call void @llvm.quantum.rotxy(i16 %13, double 0x3FF921FB54442D18, double 0x4012D97C7F3321D2, i32 0)
  %arrayidx144 = getelementptr inbounds [10 x i16], ptr @q, i64 0, i64 3
  %14 = load i16, ptr %arrayidx144, align 16
  %arrayidx145 = getelementptr inbounds [10 x i16], ptr @q, i64 0, i64 1
  %15 = load i16, ptr %arrayidx145, align 16
  call void @llvm.quantum.cphase(i16 %14, i16 %15, double 0x400921FB54442D18, i32 0)
  %arrayidx143 = getelementptr inbounds [10 x i16], ptr @q, i64 0, i64 1
  %16 = load i16, ptr %arrayidx143, align 16
  call void @llvm.quantum.rotxy(i16 %16, double 0x3FF921FB54442D18, double 0x3FF921FB54442D18, i32 0)
  %arrayidx142 = getelementptr inbounds [10 x i16], ptr @q, i64 0, i64 6
  %17 = load i16, ptr %arrayidx142, align 16
  call void @llvm.quantum.rotxy(i16 %17, double 0x3FF921FB54442D18, double 0x4012D97C7F3321D2, i32 0)
  %arrayidx140 = getelementptr inbounds [10 x i16], ptr @q, i64 0, i64 3
  %18 = load i16, ptr %arrayidx140, align 16
  %arrayidx141 = getelementptr inbounds [10 x i16], ptr @q, i64 0, i64 6
  %19 = load i16, ptr %arrayidx141, align 16
  call void @llvm.quantum.cphase(i16 %18, i16 %19, double 0x400921FB54442D18, i32 0)
  %arrayidx139 = getelementptr inbounds [10 x i16], ptr @q, i64 0, i64 6
  %20 = load i16, ptr %arrayidx139, align 16
  call void @llvm.quantum.rotxy(i16 %20, double 0x3FF921FB54442D18, double 0x3FF921FB54442D18, i32 0)
  %arrayidx138 = getelementptr inbounds [10 x i16], ptr @q, i64 0, i64 5
  %21 = load i16, ptr %arrayidx138, align 16
  call void @llvm.quantum.rotxy(i16 %21, double 0x3FF921FB54442D18, double 0x3FF921FB54442D18, i32 0)
  %arrayidx137 = getelementptr inbounds [10 x i16], ptr @q, i64 0, i64 5
  %22 = load i16, ptr %arrayidx137, align 16
  call void @llvm.quantum.rotxy(i16 %22, double 0x400921FB54442D18, double 0.000000e+00, i32 0)
  %arrayidx136 = getelementptr inbounds [10 x i16], ptr @q, i64 0, i64 8
  %23 = load i16, ptr %arrayidx136, align 16
  call void @llvm.quantum.rotxy(i16 %23, double 0x3FF921FB54442D18, double 0x4012D97C7F3321D2, i32 0)
  %arrayidx134 = getelementptr inbounds [10 x i16], ptr @q, i64 0, i64 5
  %24 = load i16, ptr %arrayidx134, align 16
  %arrayidx135 = getelementptr inbounds [10 x i16], ptr @q, i64 0, i64 8
  %25 = load i16, ptr %arrayidx135, align 16
  call void @llvm.quantum.cphase(i16 %24, i16 %25, double 0x400921FB54442D18, i32 0)
  %arrayidx133 = getelementptr inbounds [10 x i16], ptr @q, i64 0, i64 8
  %26 = load i16, ptr %arrayidx133, align 16
  call void @llvm.quantum.rotxy(i16 %26, double 0x3FF921FB54442D18, double 0x3FF921FB54442D18, i32 0)
  %arrayidx132 = getelementptr inbounds [10 x i16], ptr @q, i64 0, i64 5
  %27 = load i16, ptr %arrayidx132, align 16
  call void @llvm.quantum.rotxy(i16 %27, double 0x3FF921FB54442D18, double 0x4012D97C7F3321D2, i32 0)
  %arrayidx130 = getelementptr inbounds [10 x i16], ptr @q, i64 0, i64 1
  %28 = load i16, ptr %arrayidx130, align 16
  %arrayidx131 = getelementptr inbounds [10 x i16], ptr @q, i64 0, i64 5
  %29 = load i16, ptr %arrayidx131, align 16
  call void @llvm.quantum.cphase(i16 %28, i16 %29, double 0x400921FB54442D18, i32 0)
  %arrayidx129 = getelementptr inbounds [10 x i16], ptr @q, i64 0, i64 5
  %30 = load i16, ptr %arrayidx129, align 16
  call void @llvm.quantum.rotxy(i16 %30, double 0x3FF921FB54442D18, double 0x3FF921FB54442D18, i32 0)
  %arrayidx128 = getelementptr inbounds [10 x i16], ptr @q, i64 0, i64 6
  %31 = load i16, ptr %arrayidx128, align 16
  call void @llvm.quantum.rotxy(i16 %31, double 0x3FF921FB54442D18, double 0x3FF921FB54442D18, i32 0)
  %arrayidx127 = getelementptr inbounds [10 x i16], ptr @q, i64 0, i64 6
  %32 = load i16, ptr %arrayidx127, align 16
  call void @llvm.quantum.rotxy(i16 %32, double 0x400921FB54442D18, double 0.000000e+00, i32 0)
  %arrayidx126 = getelementptr inbounds [10 x i16], ptr @q, i64 0, i64 2
  %33 = load i16, ptr %arrayidx126, align 16
  call void @llvm.quantum.rotxy(i16 %33, double 0x3FF921FB54442D18, double 0x4012D97C7F3321D2, i32 0)
  %arrayidx124 = getelementptr inbounds [10 x i16], ptr @q, i64 0, i64 6
  %34 = load i16, ptr %arrayidx124, align 16
  %arrayidx125 = getelementptr inbounds [10 x i16], ptr @q, i64 0, i64 2
  %35 = load i16, ptr %arrayidx125, align 16
  call void @llvm.quantum.cphase(i16 %34, i16 %35, double 0x400921FB54442D18, i32 0)
  %arrayidx123 = getelementptr inbounds [10 x i16], ptr @q, i64 0, i64 2
  %36 = load i16, ptr %arrayidx123, align 16
  call void @llvm.quantum.rotxy(i16 %36, double 0x3FF921FB54442D18, double 0x3FF921FB54442D18, i32 0)
  %arrayidx122 = getelementptr inbounds [10 x i16], ptr @q, i64 0, i64 9
  %37 = load i16, ptr %arrayidx122, align 16
  call void @llvm.quantum.rotxy(i16 %37, double 0x3FF921FB54442D18, double 0x4012D97C7F3321D2, i32 0)
  %arrayidx120 = getelementptr inbounds [10 x i16], ptr @q, i64 0, i64 6
  %38 = load i16, ptr %arrayidx120, align 16
  %arrayidx121 = getelementptr inbounds [10 x i16], ptr @q, i64 0, i64 9
  %39 = load i16, ptr %arrayidx121, align 16
  call void @llvm.quantum.cphase(i16 %38, i16 %39, double 0x400921FB54442D18, i32 0)
  %arrayidx119 = getelementptr inbounds [10 x i16], ptr @q, i64 0, i64 9
  %40 = load i16, ptr %arrayidx119, align 16
  call void @llvm.quantum.rotxy(i16 %40, double 0x3FF921FB54442D18, double 0x3FF921FB54442D18, i32 0)
  %arrayidx118 = getelementptr inbounds [10 x i16], ptr @q, i64 0, i64 9
  %41 = load i16, ptr %arrayidx118, align 16
  call void @llvm.quantum.rotxy(i16 %41, double 0x3FF921FB54442D18, double 0x3FF921FB54442D18, i32 0)
  %arrayidx117 = getelementptr inbounds [10 x i16], ptr @q, i64 0, i64 9
  %42 = load i16, ptr %arrayidx117, align 16
  call void @llvm.quantum.rotxy(i16 %42, double 0x400921FB54442D18, double 0.000000e+00, i32 0)
  %arrayidx116 = getelementptr inbounds [10 x i16], ptr @q, i64 0, i64 0
  %43 = load i16, ptr %arrayidx116, align 16
  call void @llvm.quantum.rotxy(i16 %43, double 0x3FF921FB54442D18, double 0x4012D97C7F3321D2, i32 0)
  %arrayidx114 = getelementptr inbounds [10 x i16], ptr @q, i64 0, i64 9
  %44 = load i16, ptr %arrayidx114, align 16
  %arrayidx115 = getelementptr inbounds [10 x i16], ptr @q, i64 0, i64 0
  %45 = load i16, ptr %arrayidx115, align 16
  call void @llvm.quantum.cphase(i16 %44, i16 %45, double 0x400921FB54442D18, i32 0)
  %arrayidx113 = getelementptr inbounds [10 x i16], ptr @q, i64 0, i64 0
  %46 = load i16, ptr %arrayidx113, align 16
  call void @llvm.quantum.rotxy(i16 %46, double 0x3FF921FB54442D18, double 0x3FF921FB54442D18, i32 0)
  %arrayidx112 = getelementptr inbounds [10 x i16], ptr @q, i64 0, i64 9
  %47 = load i16, ptr %arrayidx112, align 16
  call void @llvm.quantum.rotxy(i16 %47, double 0x3FF921FB54442D18, double 0x4012D97C7F3321D2, i32 0)
  %arrayidx110 = getelementptr inbounds [10 x i16], ptr @q, i64 0, i64 8
  %48 = load i16, ptr %arrayidx110, align 16
  %arrayidx111 = getelementptr inbounds [10 x i16], ptr @q, i64 0, i64 9
  %49 = load i16, ptr %arrayidx111, align 16
  call void @llvm.quantum.cphase(i16 %48, i16 %49, double 0x400921FB54442D18, i32 0)
  %arrayidx109 = getelementptr inbounds [10 x i16], ptr @q, i64 0, i64 9
  %50 = load i16, ptr %arrayidx109, align 16
  call void @llvm.quantum.rotxy(i16 %50, double 0x3FF921FB54442D18, double 0x3FF921FB54442D18, i32 0)
  %arrayidx108 = getelementptr inbounds [10 x i16], ptr @q, i64 0, i64 7
  %51 = load i16, ptr %arrayidx108, align 16
  call void @llvm.quantum.rotxy(i16 %51, double 0x3FF921FB54442D18, double 0x4012D97C7F3321D2, i32 0)
  %arrayidx106 = getelementptr inbounds [10 x i16], ptr @q, i64 0, i64 4
  %52 = load i16, ptr %arrayidx106, align 16
  %arrayidx107 = getelementptr inbounds [10 x i16], ptr @q, i64 0, i64 7
  %53 = load i16, ptr %arrayidx107, align 16
  call void @llvm.quantum.cphase(i16 %52, i16 %53, double 0x400921FB54442D18, i32 0)
  %arrayidx105 = getelementptr inbounds [10 x i16], ptr @q, i64 0, i64 7
  %54 = load i16, ptr %arrayidx105, align 16
  call void @llvm.quantum.rotxy(i16 %54, double 0x3FF921FB54442D18, double 0x3FF921FB54442D18, i32 0)
  %arrayidx104 = getelementptr inbounds [10 x i16], ptr @q, i64 0, i64 3
  %55 = load i16, ptr %arrayidx104, align 16
  call void @llvm.quantum.rotxy(i16 %55, double 0x3FF921FB54442D18, double 0x4012D97C7F3321D2, i32 0)
  %arrayidx102 = getelementptr inbounds [10 x i16], ptr @q, i64 0, i64 5
  %56 = load i16, ptr %arrayidx102, align 16
  %arrayidx103 = getelementptr inbounds [10 x i16], ptr @q, i64 0, i64 3
  %57 = load i16, ptr %arrayidx103, align 16
  call void @llvm.quantum.cphase(i16 %56, i16 %57, double 0x400921FB54442D18, i32 0)
  %arrayidx101 = getelementptr inbounds [10 x i16], ptr @q, i64 0, i64 3
  %58 = load i16, ptr %arrayidx101, align 16
  call void @llvm.quantum.rotxy(i16 %58, double 0x3FF921FB54442D18, double 0x3FF921FB54442D18, i32 0)
  %arrayidx100 = getelementptr inbounds [10 x i16], ptr @q, i64 0, i64 2
  %59 = load i16, ptr %arrayidx100, align 16
  call void @llvm.quantum.rotxy(i16 %59, double 0x3FF921FB54442D18, double 0x3FF921FB54442D18, i32 0)
  %arrayidx99 = getelementptr inbounds [10 x i16], ptr @q, i64 0, i64 2
  %60 = load i16, ptr %arrayidx99, align 16
  call void @llvm.quantum.rotxy(i16 %60, double 0x400921FB54442D18, double 0.000000e+00, i32 0)
  %arrayidx98 = getelementptr inbounds [10 x i16], ptr @q, i64 0, i64 3
  %61 = load i16, ptr %arrayidx98, align 16
  call void @llvm.quantum.rotxy(i16 %61, double 0x3FF921FB54442D18, double 0x4012D97C7F3321D2, i32 0)
  %arrayidx96 = getelementptr inbounds [10 x i16], ptr @q, i64 0, i64 2
  %62 = load i16, ptr %arrayidx96, align 16
  %arrayidx97 = getelementptr inbounds [10 x i16], ptr @q, i64 0, i64 3
  %63 = load i16, ptr %arrayidx97, align 16
  call void @llvm.quantum.cphase(i16 %62, i16 %63, double 0x400921FB54442D18, i32 0)
  %arrayidx95 = getelementptr inbounds [10 x i16], ptr @q, i64 0, i64 3
  %64 = load i16, ptr %arrayidx95, align 16
  call void @llvm.quantum.rotxy(i16 %64, double 0x3FF921FB54442D18, double 0x3FF921FB54442D18, i32 0)
  %arrayidx94 = getelementptr inbounds [10 x i16], ptr @q, i64 0, i64 6
  %65 = load i16, ptr %arrayidx94, align 16
  call void @llvm.quantum.rotxy(i16 %65, double 0x3FF921FB54442D18, double 0x4012D97C7F3321D2, i32 0)
  %arrayidx92 = getelementptr inbounds [10 x i16], ptr @q, i64 0, i64 1
  %66 = load i16, ptr %arrayidx92, align 16
  %arrayidx93 = getelementptr inbounds [10 x i16], ptr @q, i64 0, i64 6
  %67 = load i16, ptr %arrayidx93, align 16
  call void @llvm.quantum.cphase(i16 %66, i16 %67, double 0x400921FB54442D18, i32 0)
  %arrayidx91 = getelementptr inbounds [10 x i16], ptr @q, i64 0, i64 6
  %68 = load i16, ptr %arrayidx91, align 16
  call void @llvm.quantum.rotxy(i16 %68, double 0x3FF921FB54442D18, double 0x3FF921FB54442D18, i32 0)
  %arrayidx90 = getelementptr inbounds [10 x i16], ptr @q, i64 0, i64 1
  %69 = load i16, ptr %arrayidx90, align 16
  call void @llvm.quantum.rotxy(i16 %69, double 0x3FF921FB54442D18, double 0x3FF921FB54442D18, i32 0)
  %arrayidx89 = getelementptr inbounds [10 x i16], ptr @q, i64 0, i64 1
  %70 = load i16, ptr %arrayidx89, align 16
  call void @llvm.quantum.rotxy(i16 %70, double 0x400921FB54442D18, double 0.000000e+00, i32 0)
  %arrayidx88 = getelementptr inbounds [10 x i16], ptr @q, i64 0, i64 0
  %71 = load i16, ptr %arrayidx88, align 16
  call void @llvm.quantum.rotxy(i16 %71, double 0x3FF921FB54442D18, double 0x4012D97C7F3321D2, i32 0)
  %arrayidx86 = getelementptr inbounds [10 x i16], ptr @q, i64 0, i64 1
  %72 = load i16, ptr %arrayidx86, align 16
  %arrayidx87 = getelementptr inbounds [10 x i16], ptr @q, i64 0, i64 0
  %73 = load i16, ptr %arrayidx87, align 16
  call void @llvm.quantum.cphase(i16 %72, i16 %73, double 0x400921FB54442D18, i32 0)
  %arrayidx85 = getelementptr inbounds [10 x i16], ptr @q, i64 0, i64 0
  %74 = load i16, ptr %arrayidx85, align 16
  call void @llvm.quantum.rotxy(i16 %74, double 0x3FF921FB54442D18, double 0x3FF921FB54442D18, i32 0)
  %arrayidx84 = getelementptr inbounds [10 x i16], ptr @q, i64 0, i64 7
  %75 = load i16, ptr %arrayidx84, align 16
  call void @llvm.quantum.rotxy(i16 %75, double 0x3FF921FB54442D18, double 0x4012D97C7F3321D2, i32 0)
  %arrayidx82 = getelementptr inbounds [10 x i16], ptr @q, i64 0, i64 9
  %76 = load i16, ptr %arrayidx82, align 16
  %arrayidx83 = getelementptr inbounds [10 x i16], ptr @q, i64 0, i64 7
  %77 = load i16, ptr %arrayidx83, align 16
  call void @llvm.quantum.cphase(i16 %76, i16 %77, double 0x400921FB54442D18, i32 0)
  %arrayidx81 = getelementptr inbounds [10 x i16], ptr @q, i64 0, i64 7
  %78 = load i16, ptr %arrayidx81, align 16
  call void @llvm.quantum.rotxy(i16 %78, double 0x3FF921FB54442D18, double 0x3FF921FB54442D18, i32 0)
  %arrayidx80 = getelementptr inbounds [10 x i16], ptr @q, i64 0, i64 3
  %79 = load i16, ptr %arrayidx80, align 16
  call void @llvm.quantum.rotxy(i16 %79, double 0x3FF921FB54442D18, double 0x4012D97C7F3321D2, i32 0)
  %arrayidx78 = getelementptr inbounds [10 x i16], ptr @q, i64 0, i64 4
  %80 = load i16, ptr %arrayidx78, align 16
  %arrayidx79 = getelementptr inbounds [10 x i16], ptr @q, i64 0, i64 3
  %81 = load i16, ptr %arrayidx79, align 16
  call void @llvm.quantum.cphase(i16 %80, i16 %81, double 0x400921FB54442D18, i32 0)
  %arrayidx77 = getelementptr inbounds [10 x i16], ptr @q, i64 0, i64 3
  %82 = load i16, ptr %arrayidx77, align 16
  call void @llvm.quantum.rotxy(i16 %82, double 0x3FF921FB54442D18, double 0x3FF921FB54442D18, i32 0)
  %arrayidx76 = getelementptr inbounds [10 x i16], ptr @q, i64 0, i64 7
  %83 = load i16, ptr %arrayidx76, align 16
  call void @llvm.quantum.rotxy(i16 %83, double 0x3FF921FB54442D18, double 0x4012D97C7F3321D2, i32 0)
  %arrayidx74 = getelementptr inbounds [10 x i16], ptr @q, i64 0, i64 5
  %84 = load i16, ptr %arrayidx74, align 16
  %arrayidx75 = getelementptr inbounds [10 x i16], ptr @q, i64 0, i64 7
  %85 = load i16, ptr %arrayidx75, align 16
  call void @llvm.quantum.cphase(i16 %84, i16 %85, double 0x400921FB54442D18, i32 0)
  %arrayidx73 = getelementptr inbounds [10 x i16], ptr @q, i64 0, i64 7
  %86 = load i16, ptr %arrayidx73, align 16
  call void @llvm.quantum.rotxy(i16 %86, double 0x3FF921FB54442D18, double 0x3FF921FB54442D18, i32 0)
  %arrayidx72 = getelementptr inbounds [10 x i16], ptr @q, i64 0, i64 4
  %87 = load i16, ptr %arrayidx72, align 16
  call void @llvm.quantum.rotxy(i16 %87, double 0x3FF921FB54442D18, double 0x3FF921FB54442D18, i32 0)
  %arrayidx71 = getelementptr inbounds [10 x i16], ptr @q, i64 0, i64 4
  %88 = load i16, ptr %arrayidx71, align 16
  call void @llvm.quantum.rotxy(i16 %88, double 0x400921FB54442D18, double 0.000000e+00, i32 0)
  %arrayidx70 = getelementptr inbounds [10 x i16], ptr @q, i64 0, i64 7
  %89 = load i16, ptr %arrayidx70, align 16
  call void @llvm.quantum.rotxy(i16 %89, double 0x3FF921FB54442D18, double 0x4012D97C7F3321D2, i32 0)
  %arrayidx68 = getelementptr inbounds [10 x i16], ptr @q, i64 0, i64 4
  %90 = load i16, ptr %arrayidx68, align 16
  %arrayidx69 = getelementptr inbounds [10 x i16], ptr @q, i64 0, i64 7
  %91 = load i16, ptr %arrayidx69, align 16
  call void @llvm.quantum.cphase(i16 %90, i16 %91, double 0x400921FB54442D18, i32 0)
  %arrayidx67 = getelementptr inbounds [10 x i16], ptr @q, i64 0, i64 7
  %92 = load i16, ptr %arrayidx67, align 16
  call void @llvm.quantum.rotxy(i16 %92, double 0x3FF921FB54442D18, double 0x3FF921FB54442D18, i32 0)
  %arrayidx66 = getelementptr inbounds [10 x i16], ptr @q, i64 0, i64 0
  %93 = load i16, ptr %arrayidx66, align 16
  call void @llvm.quantum.rotxy(i16 %93, double 0x3FF921FB54442D18, double 0x4012D97C7F3321D2, i32 0)
  %arrayidx64 = getelementptr inbounds [10 x i16], ptr @q, i64 0, i64 1
  %94 = load i16, ptr %arrayidx64, align 16
  %arrayidx65 = getelementptr inbounds [10 x i16], ptr @q, i64 0, i64 0
  %95 = load i16, ptr %arrayidx65, align 16
  call void @llvm.quantum.cphase(i16 %94, i16 %95, double 0x400921FB54442D18, i32 0)
  %arrayidx63 = getelementptr inbounds [10 x i16], ptr @q, i64 0, i64 0
  %96 = load i16, ptr %arrayidx63, align 16
  call void @llvm.quantum.rotxy(i16 %96, double 0x3FF921FB54442D18, double 0x3FF921FB54442D18, i32 0)
  %arrayidx62 = getelementptr inbounds [10 x i16], ptr @q, i64 0, i64 4
  %97 = load i16, ptr %arrayidx62, align 16
  call void @llvm.quantum.rotxy(i16 %97, double 0x3FF921FB54442D18, double 0x3FF921FB54442D18, i32 0)
  %arrayidx61 = getelementptr inbounds [10 x i16], ptr @q, i64 0, i64 4
  %98 = load i16, ptr %arrayidx61, align 16
  call void @llvm.quantum.rotxy(i16 %98, double 0x400921FB54442D18, double 0.000000e+00, i32 0)
  %arrayidx60 = getelementptr inbounds [10 x i16], ptr @q, i64 0, i64 2
  %99 = load i16, ptr %arrayidx60, align 16
  call void @llvm.quantum.rotxy(i16 %99, double 0x3FF921FB54442D18, double 0x4012D97C7F3321D2, i32 0)
  %arrayidx58 = getelementptr inbounds [10 x i16], ptr @q, i64 0, i64 3
  %100 = load i16, ptr %arrayidx58, align 16
  %arrayidx59 = getelementptr inbounds [10 x i16], ptr @q, i64 0, i64 2
  %101 = load i16, ptr %arrayidx59, align 16
  call void @llvm.quantum.cphase(i16 %100, i16 %101, double 0x400921FB54442D18, i32 0)
  %arrayidx57 = getelementptr inbounds [10 x i16], ptr @q, i64 0, i64 2
  %102 = load i16, ptr %arrayidx57, align 16
  call void @llvm.quantum.rotxy(i16 %102, double 0x3FF921FB54442D18, double 0x3FF921FB54442D18, i32 0)
  %arrayidx56 = getelementptr inbounds [10 x i16], ptr @q, i64 0, i64 4
  %103 = load i16, ptr %arrayidx56, align 16
  call void @llvm.quantum.rotxy(i16 %103, double 0x3FF921FB54442D18, double 0x4012D97C7F3321D2, i32 0)
  %arrayidx54 = getelementptr inbounds [10 x i16], ptr @q, i64 0, i64 5
  %104 = load i16, ptr %arrayidx54, align 16
  %arrayidx55 = getelementptr inbounds [10 x i16], ptr @q, i64 0, i64 4
  %105 = load i16, ptr %arrayidx55, align 16
  call void @llvm.quantum.cphase(i16 %104, i16 %105, double 0x400921FB54442D18, i32 0)
  %arrayidx53 = getelementptr inbounds [10 x i16], ptr @q, i64 0, i64 4
  %106 = load i16, ptr %arrayidx53, align 16
  call void @llvm.quantum.rotxy(i16 %106, double 0x3FF921FB54442D18, double 0x3FF921FB54442D18, i32 2)
  br label %aqcc.meas.move.end

aqcc.meas.move.end:                               ; preds = %aqcc.quantum
  ret void
}

; Function Attrs: mustprogress norecurse nounwind uwtable
define dso_local noundef i32 @main() local_unnamed_addr #1 {
  tail call void @_Z7circuitv.stub() #2
  ret i32 0
}

declare i32 @_ZN4qrt021initializeQrtSystem64EPdPcS1_(ptr, ptr, ptr)

declare i1 @_ZN4qrt013isInitializedEv()

declare void @_ZN4qrt020updateMeasurementMapEPbmb(ptr, i64, i1)

declare i32 @_ZN4qrt014scheduleKernelEPFvvEj(ptr, i32)

declare i32 @_ZN4qrt018dismantleQrtSystemEv()

declare void @_ZN4qrt04exitEPc(ptr)

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
attributes #1 = { mustprogress norecurse nounwind uwtable "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #2 = { nounwind }

!llvm.module.flags = !{!0, !1, !2, !3}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{i32 8, !"PIC Level", i32 2}
!2 = !{i32 7, !"PIE Level", i32 2}
!3 = !{i32 7, !"uwtable", i32 2}
)";

#endif
