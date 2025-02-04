//===- QuantumCircuitTest.h -----------------------------*- C++ -*-===//
// INTEL CONFIDENTIAL
//
// Copyright 2024 Intel Corporation.
//
// This software and the related documents are Intel copyrighted materials, and
// your use of them is governed by the express license under which they were
// provided to you ("License"). Unless the License provides otherwise, you may
// not use, modify, copy, publish, distribute, disclose or transmit this
// software or the related documents without Intel's prior written permission.
//
// This software and the related documents are provided as is, with no express
// or implied warranties, other than those that are expressly stated in the
// License.
//===----------------------------------------------------------------------===//
/// \file "QuantumCircuitTest.hpp"
///
/// This file provides the raw strings for Quantum Circuit unit tests.
///
//===----------------------------------------------------------------------===//

#ifndef QUANTUM_CIRCUIT_TEST_H
#define QUANTUM_CIRCUIT_TEST_H

static const char Code_Generated[] = R"(
; ModuleID = 'source-quantum-circuit.ll'
source_filename = "source-quantum-circuit.cpp"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

@.str = private unnamed_addr constant [325 x i8] c"{     \22matrix_real\22 : \221/sqrt(2)*[1, 1, 1, -1]\22,     \22matrix_imag\22 : [0, 0, 0, 0],     \22matrix_order\22 : \22rm\22,     \22is_hermitian\22 : true,     \22is_unitary\22 : true,     \22is_mutable\22 : true,     \22qubit_list\22 : [0],     \22parametric_list\22 : [],     \22control_qubit_list\22 : [],     \22local_basis_list\22 : [4],     \22identifier\22 : 3   }\00", section "llvm.metadata"
@.str.1 = private unnamed_addr constant [70 x i8] c"/srv/raid0/alitteke/aqcc16-build/include/clang/Quantum/quintrinsics.h\00", section "llvm.metadata"
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
@.str.25 = private unnamed_addr constant [392 x i8] c"{     \22matrix_real\22 : \22[cos((2*pi-theta)/2), 0, 0, cos((2*pi-theta)/2)]\22,     \22matrix_imag\22 : \22[-sin((2*pi-theta)/2), 0, 0, sin((2*pi-theta)/2)]\22,     \22matrix_order\22 : \22rm\22,     \22is_hermitian\22 : false,     \22is_unitary\22 : true,     \22is_mutable\22 : true,     \22qubit_list\22 : [0],     \22parametric_list\22 : [1],     \22control_qubit_list\22 : [],     \22local_basis_list\22 : [3],     \22identifier\22 : 55   }\00", section "llvm.metadata"
@.str.26 = private unnamed_addr constant [413 x i8] c"{     \22matrix_real\22 : \22[cos(theta/2), 0, 0, cos(theta/2)]\22,     \22matrix_imag\22 : \22[-sin(theta/2)/2, -sqrt(3)/2*sin(theta/2),  -sqrt(3)/2*sin(theta/2), sin(theta/2)/2]\22,     \22matrix_order\22 : \22rm\22,     \22is_hermitian\22 : false,     \22is_unitary\22 : true,     \22is_mutable\22 : true,     \22qubit_list\22 : [0],     \22parametric_list\22 : [1],     \22control_qubit_list\22 : [],     \22local_basis_list\22 : [0],     \22identifier\22 : 54   }\00", section "llvm.metadata"
@.str.27 = private unnamed_addr constant [399 x i8] c"{     \22matrix_real\22 : \22[0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]\22,     \22matrix_imag\22 : \22[0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]\22,     \22matrix_order\22 : \22rm\22,     \22is_hermitian\22 : false,     \22is_unitary\22 : true,     \22is_mutable\22 : true,     \22qubit_list\22 : [0,1],     \22parametric_list\22 : [2],     \22control_qubit_list\22 : [],     \22local_basis_list\22 : [0],     \22identifier\22 : 56   }\00", section "llvm.metadata"
@QubitReg = dso_local global [3 x i16] zeroinitializer, align 2
@CReg = dso_local local_unnamed_addr global [2 x i8] zeroinitializer, align 1
@llvm.global.annotations = appending global [27 x { ptr, ptr, ptr, i32, ptr }] [{ ptr, ptr, ptr, i32, ptr } { ptr @_Z1HRt, ptr @.str, ptr @.str.1, i32 47, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr @_Z1XRt, ptr @.str.2, ptr @.str.1, i32 66, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr @_Z1YRt, ptr @.str.3, ptr @.str.1, i32 85, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr @_Z1ZRt, ptr @.str.4, ptr @.str.1, i32 105, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr @_Z1SRt, ptr @.str.5, ptr @.str.1, i32 124, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr @_Z4SdagRt, ptr @.str.6, ptr @.str.1, i32 143, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr @_Z1TRt, ptr @.str.7, ptr @.str.1, i32 162, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr @_Z4TdagRt, ptr @.str.8, ptr @.str.1, i32 181, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr @_Z2RXRtd, ptr @.str.9, ptr @.str.1, i32 200, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr @_Z2RYRtd, ptr @.str.10, ptr @.str.1, i32 221, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr @_Z2RZRtd, ptr @.str.11, ptr @.str.1, i32 242, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr @_Z2CZRtS_, ptr @.str.12, ptr @.str.1, i32 263, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr @_Z4CNOTRtS_, ptr @.str.13, ptr @.str.1, i32 284, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr @_Z4SWAPRtS_, ptr @.str.14, ptr @.str.1, i32 305, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr @_Z7ToffoliRtS_S_, ptr @.str.15, ptr @.str.1, i32 326, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr @_Z5PrepXRt, ptr @.str.16, ptr @.str.1, i32 350, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr @_Z5PrepYRt, ptr @.str.17, ptr @.str.1, i32 367, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr @_Z5PrepZRt, ptr @.str.18, ptr @.str.1, i32 384, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr @_Z5MeasXRtRbb, ptr @.str.19, ptr @.str.1, i32 401, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr @_Z5MeasYRtRbb, ptr @.str.20, ptr @.str.1, i32 421, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr @_Z5MeasZRtRbb, ptr @.str.21, ptr @.str.1, i32 441, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr @_Z6CPhaseRtS_d, ptr @.str.22, ptr @.str.1, i32 469, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr @_Z3RXYRtdd, ptr @.str.23, ptr @.str.1, i32 491, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr @_Z5SwapARtS_d, ptr @.str.24, ptr @.str.1, i32 513, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr @_Z2JzRtd, ptr @.str.25, ptr @.str.1, i32 535, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr @_Z2JnRtd, ptr @.str.26, ptr @.str.1, i32 556, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr @_Z11PartialSwapRtS_d, ptr @.str.27, ptr @.str.1, i32 578, ptr null }], section "llvm.metadata"

; Function Attrs: mustprogress noinline uwtable
define dso_local void @_Z1HRt(ptr noundef nonnull align 2 dereferenceable(2) "qubit_ref" %q) #0 {
entry:
  call void @llvm.quantum.qubit(ptr noundef nonnull %q)
  ret void
}

declare void @_Z15__quantum_qubitPt(ptr noundef) local_unnamed_addr #1

; Function Attrs: mustprogress noinline uwtable
define dso_local void @_Z1XRt(ptr noundef nonnull align 2 dereferenceable(2) "qubit_ref" %q) #0 {
entry:
  call void @llvm.quantum.qubit(ptr noundef nonnull %q)
  ret void
}

; Function Attrs: mustprogress noinline uwtable
define dso_local void @_Z1YRt(ptr noundef nonnull align 2 dereferenceable(2) "qubit_ref" %q) #0 {
entry:
  call void @llvm.quantum.qubit(ptr noundef nonnull %q)
  ret void
}

; Function Attrs: mustprogress noinline uwtable
define dso_local void @_Z1ZRt(ptr noundef nonnull align 2 dereferenceable(2) "qubit_ref" %q) #0 {
entry:
  call void @llvm.quantum.qubit(ptr noundef nonnull %q)
  ret void
}

; Function Attrs: mustprogress noinline uwtable
define dso_local void @_Z1SRt(ptr noundef nonnull align 2 dereferenceable(2) "qubit_ref" %q) #0 {
entry:
  call void @llvm.quantum.qubit(ptr noundef nonnull %q)
  ret void
}

; Function Attrs: mustprogress noinline uwtable
define dso_local void @_Z4SdagRt(ptr noundef nonnull align 2 dereferenceable(2) "qubit_ref" %q) #0 {
entry:
  call void @llvm.quantum.qubit(ptr noundef nonnull %q)
  ret void
}

; Function Attrs: mustprogress noinline uwtable
define dso_local void @_Z1TRt(ptr noundef nonnull align 2 dereferenceable(2) "qubit_ref" %q) #0 {
entry:
  call void @llvm.quantum.qubit(ptr noundef nonnull %q)
  ret void
}

; Function Attrs: mustprogress noinline uwtable
define dso_local void @_Z4TdagRt(ptr noundef nonnull align 2 dereferenceable(2) "qubit_ref" %q) #0 {
entry:
  call void @llvm.quantum.qubit(ptr noundef nonnull %q)
  ret void
}

; Function Attrs: mustprogress noinline uwtable
define dso_local void @_Z2RXRtd(ptr noundef nonnull align 2 dereferenceable(2) "qubit_ref" %q, double noundef %angle) #0 {
entry:
  call void @llvm.quantum.qubit(ptr noundef nonnull %q)
  call void @llvm.quantum.param(double noundef %angle)
  ret void
}

declare void @_Z15__quantum_paramd(double noundef) local_unnamed_addr #1

; Function Attrs: mustprogress noinline uwtable
define dso_local void @_Z2RYRtd(ptr noundef nonnull align 2 dereferenceable(2) "qubit_ref" %q, double noundef %angle) #0 {
entry:
  call void @llvm.quantum.qubit(ptr noundef nonnull %q)
  call void @llvm.quantum.param(double noundef %angle)
  ret void
}

; Function Attrs: mustprogress noinline uwtable
define dso_local void @_Z2RZRtd(ptr noundef nonnull align 2 dereferenceable(2) "qubit_ref" %q, double noundef %angle) #0 {
entry:
  call void @llvm.quantum.qubit(ptr noundef nonnull %q)
  call void @llvm.quantum.param(double noundef %angle)
  ret void
}

; Function Attrs: mustprogress noinline uwtable
define dso_local void @_Z2CZRtS_(ptr noundef nonnull align 2 dereferenceable(2) "qubit_ref" %ctrl, ptr noundef nonnull align 2 dereferenceable(2) "qubit_ref" %target) #0 {
entry:
  call void @llvm.quantum.qubit(ptr noundef nonnull %ctrl)
  call void @llvm.quantum.qubit(ptr noundef nonnull %target)
  ret void
}

; Function Attrs: mustprogress noinline uwtable
define dso_local void @_Z4CNOTRtS_(ptr noundef nonnull align 2 dereferenceable(2) "qubit_ref" %ctrl, ptr noundef nonnull align 2 dereferenceable(2) "qubit_ref" %target) #0 {
entry:
  call void @llvm.quantum.qubit(ptr noundef nonnull %ctrl)
  call void @llvm.quantum.qubit(ptr noundef nonnull %target)
  ret void
}

; Function Attrs: mustprogress noinline uwtable
define dso_local void @_Z4SWAPRtS_(ptr noundef nonnull align 2 dereferenceable(2) "qubit_ref" %ctrl, ptr noundef nonnull align 2 dereferenceable(2) "qubit_ref" %target) #0 {
entry:
  call void @llvm.quantum.qubit(ptr noundef nonnull %ctrl)
  call void @llvm.quantum.qubit(ptr noundef nonnull %target)
  ret void
}

; Function Attrs: mustprogress noinline uwtable
define dso_local void @_Z7ToffoliRtS_S_(ptr noundef nonnull align 2 dereferenceable(2) "qubit_ref" %ctrl0, ptr noundef nonnull align 2 dereferenceable(2) "qubit_ref" %ctrl1, ptr noundef nonnull align 2 dereferenceable(2) "qubit_ref" %target) #0 {
entry:
  call void @llvm.quantum.qubit(ptr noundef nonnull %ctrl0)
  call void @llvm.quantum.qubit(ptr noundef nonnull %ctrl1)
  call void @llvm.quantum.qubit(ptr noundef nonnull %target)
  ret void
}

; Function Attrs: mustprogress noinline uwtable
define dso_local void @_Z5PrepXRt(ptr noundef nonnull align 2 dereferenceable(2) "qubit_ref" %q) #0 {
entry:
  call void @llvm.quantum.qubit(ptr noundef nonnull %q)
  ret void
}

; Function Attrs: mustprogress noinline uwtable
define dso_local void @_Z5PrepYRt(ptr noundef nonnull align 2 dereferenceable(2) "qubit_ref" %q) #0 {
entry:
  call void @llvm.quantum.qubit(ptr noundef nonnull %q)
  ret void
}

; Function Attrs: mustprogress noinline uwtable
define dso_local void @_Z5PrepZRt(ptr noundef nonnull align 2 dereferenceable(2) "qubit_ref" %q) #0 {
entry:
  call void @llvm.quantum.qubit(ptr noundef nonnull %q)
  ret void
}

; Function Attrs: mustprogress noinline uwtable
define dso_local void @_Z5MeasXRtRbb(ptr noundef nonnull align 2 dereferenceable(2) "qubit_ref" %q, ptr noundef nonnull align 1 dereferenceable(1) "cbit_ref" %c, i1 noundef zeroext %flip) #0 {
entry:
  call void @llvm.quantum.qubit(ptr noundef nonnull %q)
  call void @llvm.quantum.cbit(ptr noundef nonnull %c)
  %conv = uitofp i1 %flip to double
  call void @llvm.quantum.param(double noundef %conv)
  ret void
}

declare void @_Z14__quantum_cbitPb(ptr noundef) local_unnamed_addr #1

; Function Attrs: mustprogress noinline uwtable
define dso_local void @_Z5MeasYRtRbb(ptr noundef nonnull align 2 dereferenceable(2) "qubit_ref" %q, ptr noundef nonnull align 1 dereferenceable(1) "cbit_ref" %c, i1 noundef zeroext %flip) #0 {
entry:
  call void @llvm.quantum.qubit(ptr noundef nonnull %q)
  call void @llvm.quantum.cbit(ptr noundef nonnull %c)
  %conv = uitofp i1 %flip to double
  call void @llvm.quantum.param(double noundef %conv)
  ret void
}

; Function Attrs: mustprogress noinline uwtable
define dso_local void @_Z5MeasZRtRbb(ptr noundef nonnull align 2 dereferenceable(2) "qubit_ref" %q, ptr noundef nonnull align 1 dereferenceable(1) "cbit_ref" %c, i1 noundef zeroext %flip) #0 {
entry:
  call void @llvm.quantum.qubit(ptr noundef nonnull %q)
  call void @llvm.quantum.cbit(ptr noundef nonnull %c)
  %conv = uitofp i1 %flip to double
  call void @llvm.quantum.param(double noundef %conv)
  ret void
}

; Function Attrs: mustprogress noinline uwtable
define dso_local void @_Z6CPhaseRtS_d(ptr noundef nonnull align 2 dereferenceable(2) "qubit_ref" %ctrl, ptr noundef nonnull align 2 dereferenceable(2) "qubit_ref" %target, double noundef %angle) #0 {
entry:
  call void @llvm.quantum.qubit(ptr noundef nonnull %ctrl)
  call void @llvm.quantum.qubit(ptr noundef nonnull %target)
  call void @llvm.quantum.param(double noundef %angle)
  ret void
}

; Function Attrs: mustprogress noinline uwtable
define dso_local void @_Z3RXYRtdd(ptr noundef nonnull align 2 dereferenceable(2) "qubit_ref" %q, double noundef %theta, double noundef %phi) #0 {
entry:
  call void @llvm.quantum.qubit(ptr noundef nonnull %q)
  call void @llvm.quantum.param(double noundef %theta)
  call void @llvm.quantum.param(double noundef %phi)
  ret void
}

; Function Attrs: mustprogress noinline uwtable
define dso_local void @_Z5SwapARtS_d(ptr noundef nonnull align 2 dereferenceable(2) "qubit_ref" %q1, ptr noundef nonnull align 2 dereferenceable(2) "qubit_ref" %q2, double noundef %angle) #0 {
entry:
  call void @llvm.quantum.qubit(ptr noundef nonnull %q1)
  call void @llvm.quantum.qubit(ptr noundef nonnull %q2)
  call void @llvm.quantum.param(double noundef %angle)
  ret void
}

; Function Attrs: mustprogress noinline uwtable
define dso_local void @_Z2JzRtd(ptr noundef nonnull align 2 dereferenceable(2) "qubit_ref" %q, double noundef %theta) #0 {
entry:
  call void @llvm.quantum.qubit(ptr noundef nonnull %q)
  call void @llvm.quantum.param(double noundef %theta)
  ret void
}

; Function Attrs: mustprogress noinline uwtable
define dso_local void @_Z2JnRtd(ptr noundef nonnull align 2 dereferenceable(2) "qubit_ref" %q, double noundef %theta) #0 {
entry:
  call void @llvm.quantum.qubit(ptr noundef nonnull %q)
  call void @llvm.quantum.param(double noundef %theta)
  ret void
}

; Function Attrs: mustprogress noinline uwtable
define dso_local void @_Z11PartialSwapRtS_d(ptr noundef nonnull align 2 dereferenceable(2) "qubit_ref" %q0, ptr noundef nonnull align 2 dereferenceable(2) "qubit_ref" %q1, double noundef %theta) #0 {
entry:
  call void @llvm.quantum.qubit(ptr noundef nonnull %q0)
  call void @llvm.quantum.qubit(ptr noundef nonnull %q1)
  call void @llvm.quantum.param(double noundef %theta)
  ret void
}

; Function Attrs: mustprogress nofree noinline norecurse nosync nounwind willreturn memory(none) uwtable
define dso_local void @_Z12empty_kernelv.stub() local_unnamed_addr #2 section ".qbbs_text" {
entry:
  ret void
}

; Function Attrs: mustprogress noinline uwtable
define dso_local void @_Z22simple_canonical_blockv.stub() local_unnamed_addr #0 section ".qbbs_text" {
entry:
  tail call void @_Z5PrepZRt(ptr noundef nonnull align 2 dereferenceable(2) @QubitReg)
  ret void
}

declare void @llvm.quantum.prepz(i16, i32 immarg)

; Taken from scheduled stage, removed beginning and end blocks.
define dso_local void @_Z22simple_intrinsic_blockv.stub() local_unnamed_addr #1 section ".qbbs_text" {
entry:
  call void @llvm.quantum.prepz(i16 0, i32 1)
  ret void
}

; Function Attrs: mustprogress noinline uwtable
define dso_local void @_Z24simple_kernel_call_blockv.stub() local_unnamed_addr #0 section ".qbbs_text" {
entry:
  tail call void @_Z22simple_canonical_blockv.stub()
  ret void
}

; Function Attrs: mustprogress noinline uwtable
define dso_local void @_Z34simple_kernel_call_canonical_blockv.stub() local_unnamed_addr #0 section ".qbbs_text" {
entry:
  tail call void @_Z5PrepZRt(ptr noundef nonnull align 2 dereferenceable(2) @QubitReg)
  tail call void @_Z5PrepZRt(ptr noundef nonnull align 2 dereferenceable(2) getelementptr inbounds ([3 x i16], ptr @QubitReg, i64 0, i64 1))
  tail call void @_Z22simple_canonical_blockv.stub()
  tail call void @_Z1HRt(ptr noundef nonnull align 2 dereferenceable(2) @QubitReg)
  tail call void @_Z1HRt(ptr noundef nonnull align 2 dereferenceable(2) getelementptr inbounds ([3 x i16], ptr @QubitReg, i64 0, i64 1))
  tail call void @_Z1HRt(ptr noundef nonnull align 2 dereferenceable(2) getelementptr inbounds ([3 x i16], ptr @QubitReg, i64 0, i64 2))
  ret void
}

; Function Attrs: mustprogress noinline uwtable
define dso_local void @_Z18qbitref_dependencyv.stub() local_unnamed_addr #0 section ".qbbs_text" {
entry:
  tail call void @_Z4CNOTRtS_(ptr noundef nonnull align 2 dereferenceable(2) @QubitReg, ptr noundef nonnull align 2 dereferenceable(2) getelementptr inbounds ([3 x i16], ptr @QubitReg, i64 0, i64 1))
  tail call void @_Z1HRt(ptr noundef nonnull align 2 dereferenceable(2) @QubitReg)
  tail call void @_Z1HRt(ptr noundef nonnull align 2 dereferenceable(2) getelementptr inbounds ([3 x i16], ptr @QubitReg, i64 0, i64 1))
  tail call void @_Z4CNOTRtS_(ptr noundef nonnull align 2 dereferenceable(2) @QubitReg, ptr noundef nonnull align 2 dereferenceable(2) getelementptr inbounds ([3 x i16], ptr @QubitReg, i64 0, i64 1))
  ret void
}

; Function Attrs: nounwind
declare void @llvm.quantum.rotxy(i16, double, double, i32 immarg) #2

; Function Attrs: nounwind
declare void @llvm.quantum.cphase(i16, i16, double, i32 immarg) #2

; Taken from scheduled stage, removed beginning and end blocks.
; Function Attrs: mustprogress noinline nounwind uwtable
define dso_local void @_Z14qid_dependencyv.stub() local_unnamed_addr #1 section ".qbbs_text" {
entry:                                     ; preds = %entry
  call void @llvm.quantum.cphase(i16 0, i16 1, double 0.000000e+00, i32 1)
  call void @llvm.quantum.rotxy(i16 0, double 0.000000e+00, double 0.000000e+00, i32 0)
  call void @llvm.quantum.rotxy(i16 1, double 0.000000e+00, double 0.000000e+00, i32 0)
  call void @llvm.quantum.cphase(i16 0, i16 1, double 0.000000e+00, i32 2)
  ret void
}

; Beginning and end blocks added
; Function Attrs: mustprogress noinline uwtable
define dso_local void @_Z10basic_linev.stub() local_unnamed_addr #0 section ".qbbs_text" {
entry:
  br label %body
body:
  tail call void @_Z5PrepZRt(ptr noundef nonnull align 2 dereferenceable(2) @QubitReg)
  tail call void @_Z1HRt(ptr noundef nonnull align 2 dereferenceable(2) @QubitReg)
  br label %end_block
end_block:
  ret void
}

; Beginning and end blocks added
; Function Attrs: mustprogress noinline uwtable
define dso_local void @_Z19basic_line_parallelv.stub() local_unnamed_addr #0 section ".qbbs_text" {
entry:
  br label %body
body:
  tail call void @_Z5PrepZRt(ptr noundef nonnull align 2 dereferenceable(2) @QubitReg)
  tail call void @_Z1HRt(ptr noundef nonnull align 2 dereferenceable(2) @QubitReg)
  tail call void @_Z5PrepZRt(ptr noundef nonnull align 2 dereferenceable(2) getelementptr inbounds ([3 x i16], ptr @QubitReg, i64 0, i64 1))
  br label %end_block
end_block:
  ret void
}

; Function Attrs: mustprogress noinline uwtable
define dso_local void @_Z10basic_loopv.stub() local_unnamed_addr #0 section ".qbbs_text" {
entry:
  br label %for.body

for.cond.cleanup:                                 ; preds = %for.body
  ret void

for.body:                                         ; preds = %for.body, %entry
  %indvars.iv = phi i64 [ 0, %entry ], [ %indvars.iv.next, %for.body ]
  %arrayidx = getelementptr inbounds [3 x i16], ptr @QubitReg, i64 0, i64 %indvars.iv
  tail call void @_Z5PrepZRt(ptr noundef nonnull align 2 dereferenceable(2) %arrayidx)
  tail call void @_Z1HRt(ptr noundef nonnull align 2 dereferenceable(2) %arrayidx)
  %indvars.iv.next = add nuw nsw i64 %indvars.iv, 1
  %cmp = icmp eq i64 %indvars.iv, 0
  br i1 %cmp, label %for.body, label %for.cond.cleanup, !llvm.loop !5
}

; Function Attrs: mustprogress noinline uwtable
define dso_local void @_Z19basic_loop_parallelv.stub() local_unnamed_addr #0 section ".qbbs_text" {
entry:
  br label %for.body

for.cond.cleanup:                                 ; preds = %for.body
  ret void

for.body:                                         ; preds = %for.body, %entry
  %indvars.iv = phi i64 [ 0, %entry ], [ %indvars.iv.next, %for.body ]
  %arrayidx = getelementptr inbounds [3 x i16], ptr @QubitReg, i64 0, i64 %indvars.iv
  tail call void @_Z5PrepZRt(ptr noundef nonnull align 2 dereferenceable(2) %arrayidx)
  tail call void @_Z1HRt(ptr noundef nonnull align 2 dereferenceable(2) %arrayidx)
  tail call void @_Z5PrepZRt(ptr noundef nonnull align 2 dereferenceable(2) getelementptr inbounds ([3 x i16], ptr @QubitReg, i64 0, i64 1))
  %indvars.iv.next = add nuw nsw i64 %indvars.iv, 1
  %cmp = icmp eq i64 %indvars.iv, 0
  br i1 %cmp, label %for.body, label %for.cond.cleanup, !llvm.loop !8
}

; Body1 inserted by hand
; Function Attrs: mustprogress noinline uwtable
define dso_local void @_Z16multi_block_loopv.stub() local_unnamed_addr #0 section ".qbbs_text" {
entry:
  br label %for.body

for.cond.cleanup:                                 ; preds = %for.body
  ret void

for.body:                                         ; preds = %for.body, %entry
  %indvars.iv = phi i64 [ 0, %entry ], [ %indvars.iv.next, %for.body ]
  %arrayidx = getelementptr inbounds [3 x i16], ptr @QubitReg, i64 0, i64 %indvars.iv
  tail call void @_Z5PrepZRt(ptr noundef nonnull align 2 dereferenceable(2) %arrayidx)
  tail call void @_Z1HRt(ptr noundef nonnull align 2 dereferenceable(2) %arrayidx)
  br label %body1

body1:
  %indvars.iv.next = add nuw nsw i64 %indvars.iv, 1
  %cmp = icmp eq i64 %indvars.iv, 0
  br i1 %cmp, label %for.body, label %for.cond.cleanup, !llvm.loop !9
}

; Body1 inserted by hand
; Function Attrs: mustprogress noinline uwtable
define dso_local void @_Z25multi_block_loop_parallelv.stub() local_unnamed_addr #0 section ".qbbs_text" {
entry:
  br label %for.body

for.cond.cleanup:                                 ; preds = %for.body
  ret void

for.body:                                         ; preds = %for.body, %entry
  %indvars.iv = phi i64 [ 0, %entry ], [ %indvars.iv.next, %for.body ]
  %arrayidx = getelementptr inbounds [3 x i16], ptr @QubitReg, i64 0, i64 %indvars.iv
  tail call void @_Z5PrepZRt(ptr noundef nonnull align 2 dereferenceable(2) %arrayidx)
  tail call void @_Z1HRt(ptr noundef nonnull align 2 dereferenceable(2) %arrayidx)
  tail call void @_Z5PrepZRt(ptr noundef nonnull align 2 dereferenceable(2) getelementptr inbounds ([3 x i16], ptr @QubitReg, i64 0, i64 1))
  br label %body1

body1:
  %indvars.iv.next = add nuw nsw i64 %indvars.iv, 1
  %cmp = icmp eq i64 %indvars.iv, 0
  br i1 %cmp, label %for.body, label %for.cond.cleanup, !llvm.loop !10
}

; Function Attrs: mustprogress noinline uwtable
define dso_local void @_Z12basic_branchb.stub(i1 noundef zeroext %test) local_unnamed_addr #0 section ".qbbs_text" {
entry:
  br label %body

body:
  tail call void @_Z5PrepZRt(ptr noundef nonnull align 2 dereferenceable(2) @QubitReg)
  tail call void @_Z1HRt(ptr noundef nonnull align 2 dereferenceable(2) @QubitReg)
  br i1 %test, label %if.then, label %if.else

if.then:                                          ; preds = %entry
  tail call void @_Z1ZRt(ptr noundef nonnull align 2 dereferenceable(2) @QubitReg)
  br label %if.end

if.else:                                          ; preds = %entry
  tail call void @_Z1XRt(ptr noundef nonnull align 2 dereferenceable(2) @QubitReg)
  br label %if.end

if.end:                                           ; preds = %if.else, %if.then
  ret void
}

; Function Attrs: mustprogress noinline uwtable
define dso_local void @_Z21basic_branch_parallelb.stub(i1 noundef zeroext %test) local_unnamed_addr #0 section ".qbbs_text" {
entry:
  br label %branch

branch:
  tail call void @_Z5PrepZRt(ptr noundef nonnull align 2 dereferenceable(2) @QubitReg)
  tail call void @_Z1HRt(ptr noundef nonnull align 2 dereferenceable(2) @QubitReg)
  tail call void @_Z5PrepZRt(ptr noundef nonnull align 2 dereferenceable(2) getelementptr inbounds ([3 x i16], ptr @QubitReg, i64 0, i64 1))
  br i1 %test, label %if.then, label %if.else

if.then:                                          ; preds = %entry
  tail call void @_Z1ZRt(ptr noundef nonnull align 2 dereferenceable(2) @QubitReg)
  tail call void @_Z1XRt(ptr noundef nonnull align 2 dereferenceable(2) @QubitReg)
  br label %if.end

if.else:                                          ; preds = %entry
  tail call void @_Z1XRt(ptr noundef nonnull align 2 dereferenceable(2) @QubitReg)
  tail call void @_Z1ZRt(ptr noundef nonnull align 2 dereferenceable(2) getelementptr inbounds ([3 x i16], ptr @QubitReg, i64 0, i64 1))
  br label %if.end

if.end:                                           ; preds = %if.else, %if.then
  ret void
}

; Function Attrs: mustprogress noinline uwtable
define dso_local void @_Z16basic_line_limitv.stub() local_unnamed_addr #0 section ".qbbs_text" {
entry:
  tail call void @_Z5PrepZRt(ptr noundef nonnull align 2 dereferenceable(2) @QubitReg)
  tail call void @_Z5PrepZRt(ptr noundef nonnull align 2 dereferenceable(2) getelementptr inbounds ([3 x i16], ptr @QubitReg, i64 0, i64 1))
  tail call void @_Z4CNOTRtS_(ptr noundef nonnull align 2 dereferenceable(2) @QubitReg, ptr noundef nonnull align 2 dereferenceable(2) getelementptr inbounds ([3 x i16], ptr @QubitReg, i64 0, i64 1))
  ret void
}

; Function Attrs: mustprogress noinline nounwind uwtable
define dso_local void @_Z18kernel_with_no_args.stub() local_unnamed_addr #0 section ".qbbs_text" {
entry:
  br label %aqcc.quantum

aqcc.quantum:                                     ; preds = %entry
  %arrayidx = getelementptr inbounds [3 x i16], ptr @QubitReg, i64 0, i64 0
  call void @_Z2RZRtd(ptr %arrayidx, double 2.0)
  br label %aqcc.meas.move.end

aqcc.meas.move.end:                               ; preds = %aqcc.quantum
  ret void
}

; Function Attrs: mustprogress noinline nounwind uwtable
define dso_local void @_Z18kernel_with_doubled.stub(double noundef %test) local_unnamed_addr #0 section ".qbbs_text" {
entry:
  br label %aqcc.quantum

aqcc.quantum:                                     ; preds = %entry
  %arrayidx = getelementptr inbounds [3 x i16], ptr @QubitReg, i64 0, i64 0
  call void @_Z2RZRtd(ptr %arrayidx, double %test)
  br label %aqcc.meas.move.end

aqcc.meas.move.end:                               ; preds = %aqcc.quantum
  ret void
}

; Function Attrs: mustprogress noinline nounwind uwtable
define dso_local void @_Z26kernel_with_int_and_doubleid.stub(i32 noundef %test1, double noundef %test2) local_unnamed_addr #0 section ".qbbs_text" {
entry:
  br label %aqcc.quantum

aqcc.quantum:                                     ; preds = %entry
  %arrayidx = getelementptr inbounds [3 x i16], ptr @QubitReg, i64 0, i64 0
  call void @_Z2RZRtd(ptr %arrayidx, double %test2)
  br label %aqcc.meas.move.end

aqcc.meas.move.end:                               ; preds = %aqcc.quantum
  ret void
}

; Function Attrs: mustprogress noinline uwtable
define dso_local void @_Z23kernel_with_local_qubitd.stub(double noundef %test2) local_unnamed_addr #1 section ".qbbs_text" {
entry:
  %q_local = alloca [2 x i16], align 2
  br label %aqcc.quantum

aqcc.quantum:                                     ; preds = %entry
  %arrayidx2 = getelementptr inbounds [2 x i16], ptr %q_local, i64 0, i64 0
  %0 = load i16, ptr %arrayidx2, align 2
  call void @llvm.quantum.rotxy(i16 %0, double 0x400921FB54442D18, double 0.000000e+00, i32 1)
  %arrayidx1 = getelementptr inbounds [2 x i16], ptr %q_local, i64 0, i64 0
  %1 = load i16, ptr %arrayidx1, align 2
  call void @llvm.quantum.rotxy(i16 %1, double 0x400921FB54442D18, double 1.570000e+00, i32 2)
  br label %aqcc.meas.move.end

aqcc.meas.move.end:                               ; preds = %aqcc.quantum
  ret void
}

; Function Attrs: mustprogress noinline uwtable
define dso_local void @_Z23kernel_with_qubit_arg_1Pt.stub(ptr noundef nonnull "qubit_ref" %q_arg) local_unnamed_addr #0 section ".qbbs_text" {
entry:
  tail call void @_Z2RZRtd(ptr noundef nonnull align 2 dereferenceable(2) %q_arg, double noundef 3.140000e+00)
  ret void
}

; Function Attrs: mustprogress noinline uwtable
define dso_local void @_Z23kernel_with_qubit_arg_2Pt.stub(ptr noundef nonnull "qubit_ref" %q_arg) local_unnamed_addr #0 section ".qbbs_text" {
entry:
  tail call void @_Z2RXRtd(ptr noundef nonnull align 2 dereferenceable(2) %q_arg, double noundef 3.140000e+00)
  ret void
}

; Function Attrs: nounwind
declare void @llvm.quantum.qubit(ptr "qubit_ref") #3

; Function Attrs: nounwind
declare void @llvm.quantum.param(double) #3

; Function Attrs: nounwind
declare void @llvm.quantum.cbit(ptr "cbit_ref") #3

attributes #0 = { mustprogress noinline uwtable "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #1 = { "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #2 = { mustprogress nofree noinline norecurse nosync nounwind willreturn memory(none) uwtable "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #3 = { nounwind }

!llvm.module.flags = !{!0, !1, !2, !3}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{i32 8, !"PIC Level", i32 2}
!2 = !{i32 7, !"PIE Level", i32 2}
!3 = !{i32 7, !"uwtable", i32 2}
!5 = distinct !{!5, !6, !7}
!6 = !{!"llvm.loop.mustprogress"}
!7 = !{!"llvm.loop.unroll.disable"}
!8 = distinct !{!8, !6, !7}
!9 = distinct !{!9, !6, !7}
!10 = distinct !{!10, !6, !7}
)";

#endif
