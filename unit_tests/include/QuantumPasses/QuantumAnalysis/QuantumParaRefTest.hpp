//===- QuantumParaRefTest.hpp --------------------------------*- C++ -*-===//
//
//
// Copyright (C) 2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//
//===----------------------------------------------------------------------===//
/// \file "QuantumParaRefTest.hpp"
///
/// This file provides the raw strings for Quantum Para Ref unit
/// test.
///
//===----------------------------------------------------------------------===//

#ifndef QUANTUM_PARAREF_TEST_HPP
#define QUANTUM_PARAREF_TEST_HPP

// Minimum working .ll file

static const char Code[] = R"(
; ModuleID = 'code_examples/ParaRef_unittest.cpp'
source_filename = "code_examples/ParaRef_unittest.cpp"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

@.str = private unnamed_addr constant [363 x i8] c"{     \22matrix_real\22 : \22[cos(angle/2), 0, 0, cos(angle/2)]\22,     \22matrix_imag\22 : \22[0, -sin(angle/2), sin(angle/2), 0]\22,     \22matrix_order\22 : \22rm\22,     \22is_hermitian\22 : false,     \22is_unitary\22 : true,     \22is_mutable\22 : true,     \22qubit_list\22 : [0],     \22parametric_list\22 : [1],     \22control_qubit_list\22 : [],     \22local_basis_list\22 : [1],     \22identifier\22 : 9   }\00", section "llvm.metadata"
@.str.1 = private unnamed_addr constant [42 x i8] c"code_examples/quintrinsics_ParaRef_test.h\00", section "llvm.metadata"
@__const._Z14CtorSetterTestiPidPd.para6 = private unnamed_addr constant [2 x i32] [i32 0, i32 1], align 4
@__const._Z14CtorSetterTestiPidPd.para8 = private unnamed_addr constant [2 x double] [double 1.000000e-02, double 2.000000e-02], align 16
@__const._Z14ResolutionTestiPidPd.para6 = private unnamed_addr constant [2 x i32] [i32 0, i32 1], align 4
@__const._Z14ResolutionTestiPidPd.para8 = private unnamed_addr constant [2 x double] [double 1.000000e-02, double 2.000000e-02], align 16
@__const._Z12EqualityTestiPidPd.para6 = private unnamed_addr constant [2 x i32] [i32 0, i32 1], align 4
@__const._Z12EqualityTestiPidPd.para8 = private unnamed_addr constant [2 x double] [double 1.000000e-02, double 2.000000e-02], align 16
@llvm.global.annotations = appending global [1 x { i8*, i8*, i8*, i32 }] [{ i8*, i8*, i8*, i32 } { i8* bitcast (void (i16, double)* @_Z2RXqd to i8*), i8* getelementptr inbounds ([363 x i8], [363 x i8]* @.str, i32 0, i32 0), i8* getelementptr inbounds ([42 x i8], [42 x i8]* @.str.1, i32 0, i32 0), i32 39 }], section "llvm.metadata"

; Function Attrs: nounwind uwtable
define dso_local void @_Z2RXqd(i16 %q, double %angle) #0 {
entry:
  %q.addr = alloca i16, align 2
  %angle.addr = alloca double, align 8
  store i16 %q, i16* %q.addr, align 2, !tbaa !2
  store double %angle, double* %angle.addr, align 8, !tbaa !6
  %0 = load i16, i16* %q.addr, align 2, !tbaa !2
  call void @llvm.quantum.qubit(i16 %0)
  %1 = load double, double* %angle.addr, align 8, !tbaa !6
  call void @llvm.quantum.param(double %1)
  ret void
}

; Function Attrs: nounwind
declare void @llvm.quantum.qubit(i16) #1

; Function Attrs: nounwind
declare void @llvm.quantum.param(double) #1

; Function Attrs: nounwind uwtable
define dso_local void @_Z14CtorSetterTestiPidPd(i32 %para1, i32* %para2, double %para3, double* %para4) #0 {
entry:
  %para1.addr = alloca i32, align 4
  %para2.addr = alloca i32*, align 8
  %para3.addr = alloca double, align 8
  %para4.addr = alloca double*, align 8
  %para5 = alloca i32, align 4
  %para6 = alloca [2 x i32], align 4
  %para7 = alloca double, align 8
  %para8 = alloca [2 x double], align 16
  %q = alloca i16, align 2
  store i32 %para1, i32* %para1.addr, align 4, !tbaa !8
  store i32* %para2, i32** %para2.addr, align 8, !tbaa !10
  store double %para3, double* %para3.addr, align 8, !tbaa !6
  store double* %para4, double** %para4.addr, align 8, !tbaa !10
  %0 = bitcast i32* %para5 to i8*
  call void @llvm.lifetime.start.p0i8(i64 4, i8* %0) #1
  store i32 0, i32* %para5, align 4, !tbaa !8
  %1 = bitcast [2 x i32]* %para6 to i8*
  call void @llvm.lifetime.start.p0i8(i64 8, i8* %1) #1
  %2 = bitcast [2 x i32]* %para6 to i8*
  call void @llvm.memcpy.p0i8.p0i8.i64(i8* align 4 %2, i8* align 4 bitcast ([2 x i32]* @__const._Z14CtorSetterTestiPidPd.para6 to i8*), i64 8, i1 false)
  %3 = bitcast double* %para7 to i8*
  call void @llvm.lifetime.start.p0i8(i64 8, i8* %3) #1
  store double 1.000000e-02, double* %para7, align 8, !tbaa !6
  %4 = bitcast [2 x double]* %para8 to i8*
  call void @llvm.lifetime.start.p0i8(i64 16, i8* %4) #1
  %5 = bitcast [2 x double]* %para8 to i8*
  call void @llvm.memcpy.p0i8.p0i8.i64(i8* align 16 %5, i8* align 16 bitcast ([2 x double]* @__const._Z14CtorSetterTestiPidPd.para8 to i8*), i64 16, i1 false)
  %6 = bitcast i16* %q to i8*
  call void @llvm.lifetime.start.p0i8(i64 2, i8* %6) #1
  %7 = load i16, i16* %q, align 2, !tbaa !2
  %8 = load i32, i32* %para1.addr, align 4, !tbaa !8
  %conv = sitofp i32 %8 to double
  call void @_Z2RXqd(i16 %7, double %conv)
  %9 = load i16, i16* %q, align 2, !tbaa !2
  %10 = load i32*, i32** %para2.addr, align 8, !tbaa !10
  %arrayidx = getelementptr inbounds i32, i32* %10, i64 0
  %11 = load i32, i32* %arrayidx, align 4, !tbaa !8
  %conv1 = sitofp i32 %11 to double
  call void @_Z2RXqd(i16 %9, double %conv1)
  %12 = load i16, i16* %q, align 2, !tbaa !2
  %13 = load i32*, i32** %para2.addr, align 8, !tbaa !10
  %arrayidx2 = getelementptr inbounds i32, i32* %13, i64 1
  %14 = load i32, i32* %arrayidx2, align 4, !tbaa !8
  %conv3 = sitofp i32 %14 to double
  call void @_Z2RXqd(i16 %12, double %conv3)
  %15 = load i16, i16* %q, align 2, !tbaa !2
  %16 = load double, double* %para3.addr, align 8, !tbaa !6
  call void @_Z2RXqd(i16 %15, double %16)
  %17 = load i16, i16* %q, align 2, !tbaa !2
  %18 = load double*, double** %para4.addr, align 8, !tbaa !10
  %arrayidx4 = getelementptr inbounds double, double* %18, i64 0
  %19 = load double, double* %arrayidx4, align 8, !tbaa !6
  call void @_Z2RXqd(i16 %17, double %19)
  %20 = load i16, i16* %q, align 2, !tbaa !2
  %21 = load double*, double** %para4.addr, align 8, !tbaa !10
  %arrayidx5 = getelementptr inbounds double, double* %21, i64 1
  %22 = load double, double* %arrayidx5, align 8, !tbaa !6
  call void @_Z2RXqd(i16 %20, double %22)
  %23 = load i16, i16* %q, align 2, !tbaa !2
  %24 = load i32, i32* %para5, align 4, !tbaa !8
  %conv6 = sitofp i32 %24 to double
  call void @_Z2RXqd(i16 %23, double %conv6)
  %25 = load i16, i16* %q, align 2, !tbaa !2
  %arrayidx7 = getelementptr inbounds [2 x i32], [2 x i32]* %para6, i64 0, i64 0
  %26 = load i32, i32* %arrayidx7, align 4, !tbaa !8
  %conv8 = sitofp i32 %26 to double
  call void @_Z2RXqd(i16 %25, double %conv8)
  %27 = load i16, i16* %q, align 2, !tbaa !2
  %arrayidx9 = getelementptr inbounds [2 x i32], [2 x i32]* %para6, i64 0, i64 1
  %28 = load i32, i32* %arrayidx9, align 4, !tbaa !8
  %conv10 = sitofp i32 %28 to double
  call void @_Z2RXqd(i16 %27, double %conv10)
  %29 = load i16, i16* %q, align 2, !tbaa !2
  %30 = load double, double* %para7, align 8, !tbaa !6
  call void @_Z2RXqd(i16 %29, double %30)
  %31 = load i16, i16* %q, align 2, !tbaa !2
  %arrayidx11 = getelementptr inbounds [2 x double], [2 x double]* %para8, i64 0, i64 0
  %32 = load double, double* %arrayidx11, align 16, !tbaa !6
  call void @_Z2RXqd(i16 %31, double %32)
  %33 = load i16, i16* %q, align 2, !tbaa !2
  %arrayidx12 = getelementptr inbounds [2 x double], [2 x double]* %para8, i64 0, i64 1
  %34 = load double, double* %arrayidx12, align 8, !tbaa !6
  call void @_Z2RXqd(i16 %33, double %34)
  %35 = bitcast i16* %q to i8*
  call void @llvm.lifetime.end.p0i8(i64 2, i8* %35) #1
  %36 = bitcast [2 x double]* %para8 to i8*
  call void @llvm.lifetime.end.p0i8(i64 16, i8* %36) #1
  %37 = bitcast double* %para7 to i8*
  call void @llvm.lifetime.end.p0i8(i64 8, i8* %37) #1
  %38 = bitcast [2 x i32]* %para6 to i8*
  call void @llvm.lifetime.end.p0i8(i64 8, i8* %38) #1
  %39 = bitcast i32* %para5 to i8*
  call void @llvm.lifetime.end.p0i8(i64 4, i8* %39) #1
  ret void
}

; Function Attrs: argmemonly nounwind willreturn
declare void @llvm.lifetime.start.p0i8(i64 immarg, i8* nocapture) #2

; Function Attrs: argmemonly nounwind willreturn
declare void @llvm.memcpy.p0i8.p0i8.i64(i8* noalias nocapture writeonly, i8* noalias nocapture readonly, i64, i1 immarg) #2

; Function Attrs: argmemonly nounwind willreturn
declare void @llvm.lifetime.end.p0i8(i64 immarg, i8* nocapture) #2

; Function Attrs: nounwind uwtable
define dso_local void @_Z14ResolutionTestiPidPd(i32 %para1, i32* %para2, double %para3, double* %para4) #0 {
entry:
  %para1.addr = alloca i32, align 4
  %para2.addr = alloca i32*, align 8
  %para3.addr = alloca double, align 8
  %para4.addr = alloca double*, align 8
  %para5 = alloca i32, align 4
  %para6 = alloca [2 x i32], align 4
  %para7 = alloca double, align 8
  %para8 = alloca [2 x double], align 16
  %q = alloca i16, align 2
  store i32 %para1, i32* %para1.addr, align 4, !tbaa !8
  store i32* %para2, i32** %para2.addr, align 8, !tbaa !10
  store double %para3, double* %para3.addr, align 8, !tbaa !6
  store double* %para4, double** %para4.addr, align 8, !tbaa !10
  %0 = bitcast i32* %para5 to i8*
  call void @llvm.lifetime.start.p0i8(i64 4, i8* %0) #1
  store i32 0, i32* %para5, align 4, !tbaa !8
  %1 = bitcast [2 x i32]* %para6 to i8*
  call void @llvm.lifetime.start.p0i8(i64 8, i8* %1) #1
  %2 = bitcast [2 x i32]* %para6 to i8*
  call void @llvm.memcpy.p0i8.p0i8.i64(i8* align 4 %2, i8* align 4 bitcast ([2 x i32]* @__const._Z14ResolutionTestiPidPd.para6 to i8*), i64 8, i1 false)
  %3 = bitcast double* %para7 to i8*
  call void @llvm.lifetime.start.p0i8(i64 8, i8* %3) #1
  store double 1.000000e-02, double* %para7, align 8, !tbaa !6
  %4 = bitcast [2 x double]* %para8 to i8*
  call void @llvm.lifetime.start.p0i8(i64 16, i8* %4) #1
  %5 = bitcast [2 x double]* %para8 to i8*
  call void @llvm.memcpy.p0i8.p0i8.i64(i8* align 16 %5, i8* align 16 bitcast ([2 x double]* @__const._Z14ResolutionTestiPidPd.para8 to i8*), i64 16, i1 false)
  %6 = bitcast i16* %q to i8*
  call void @llvm.lifetime.start.p0i8(i64 2, i8* %6) #1
  %7 = load i16, i16* %q, align 2, !tbaa !2
  call void @_Z2RXqd(i16 %7, double 0.000000e+00)
  %8 = load i16, i16* %q, align 2, !tbaa !2
  call void @_Z2RXqd(i16 %8, double 1.000000e-02)
  %9 = load i16, i16* %q, align 2, !tbaa !2
  %10 = load i32, i32* %para5, align 4, !tbaa !8
  %conv = sitofp i32 %10 to double
  call void @_Z2RXqd(i16 %9, double %conv)
  %11 = load i16, i16* %q, align 2, !tbaa !2
  %arrayidx = getelementptr inbounds [2 x i32], [2 x i32]* %para6, i64 0, i64 0
  %12 = load i32, i32* %arrayidx, align 4, !tbaa !8
  %conv1 = sitofp i32 %12 to double
  call void @_Z2RXqd(i16 %11, double %conv1)
  %13 = load i16, i16* %q, align 2, !tbaa !2
  %arrayidx2 = getelementptr inbounds [2 x i32], [2 x i32]* %para6, i64 0, i64 1
  %14 = load i32, i32* %arrayidx2, align 4, !tbaa !8
  %conv3 = sitofp i32 %14 to double
  call void @_Z2RXqd(i16 %13, double %conv3)
  %15 = load i16, i16* %q, align 2, !tbaa !2
  %16 = load double, double* %para7, align 8, !tbaa !6
  call void @_Z2RXqd(i16 %15, double %16)
  %17 = load i16, i16* %q, align 2, !tbaa !2
  %arrayidx4 = getelementptr inbounds [2 x double], [2 x double]* %para8, i64 0, i64 0
  %18 = load double, double* %arrayidx4, align 16, !tbaa !6
  call void @_Z2RXqd(i16 %17, double %18)
  %19 = load i16, i16* %q, align 2, !tbaa !2
  %arrayidx5 = getelementptr inbounds [2 x double], [2 x double]* %para8, i64 0, i64 1
  %20 = load double, double* %arrayidx5, align 8, !tbaa !6
  call void @_Z2RXqd(i16 %19, double %20)
  %21 = load i32, i32* %para5, align 4, !tbaa !8
  %add = add nsw i32 %21, 1
  store i32 %add, i32* %para5, align 4, !tbaa !8
  %arrayidx6 = getelementptr inbounds [2 x i32], [2 x i32]* %para6, i64 0, i64 0
  %22 = load i32, i32* %arrayidx6, align 4, !tbaa !8
  %add7 = add nsw i32 %22, 1
  store i32 %add7, i32* %arrayidx6, align 4, !tbaa !8
  %arrayidx8 = getelementptr inbounds [2 x i32], [2 x i32]* %para6, i64 0, i64 1
  %23 = load i32, i32* %arrayidx8, align 4, !tbaa !8
  %add9 = add nsw i32 %23, 1
  store i32 %add9, i32* %arrayidx8, align 4, !tbaa !8
  %24 = load double, double* %para7, align 8, !tbaa !6
  %add10 = fadd double %24, 1.000000e+00
  store double %add10, double* %para7, align 8, !tbaa !6
  %arrayidx11 = getelementptr inbounds [2 x double], [2 x double]* %para8, i64 0, i64 0
  %25 = load double, double* %arrayidx11, align 16, !tbaa !6
  %add12 = fadd double %25, 1.000000e+00
  store double %add12, double* %arrayidx11, align 16, !tbaa !6
  %arrayidx13 = getelementptr inbounds [2 x double], [2 x double]* %para8, i64 0, i64 1
  %26 = load double, double* %arrayidx13, align 8, !tbaa !6
  %add14 = fadd double %26, 1.000000e+00
  store double %add14, double* %arrayidx13, align 8, !tbaa !6
  %27 = load i16, i16* %q, align 2, !tbaa !2
  %28 = load i32, i32* %para5, align 4, !tbaa !8
  %conv15 = sitofp i32 %28 to double
  call void @_Z2RXqd(i16 %27, double %conv15)
  %29 = load i16, i16* %q, align 2, !tbaa !2
  %arrayidx16 = getelementptr inbounds [2 x i32], [2 x i32]* %para6, i64 0, i64 0
  %30 = load i32, i32* %arrayidx16, align 4, !tbaa !8
  %conv17 = sitofp i32 %30 to double
  call void @_Z2RXqd(i16 %29, double %conv17)
  %31 = load i16, i16* %q, align 2, !tbaa !2
  %arrayidx18 = getelementptr inbounds [2 x i32], [2 x i32]* %para6, i64 0, i64 1
  %32 = load i32, i32* %arrayidx18, align 4, !tbaa !8
  %conv19 = sitofp i32 %32 to double
  call void @_Z2RXqd(i16 %31, double %conv19)
  %33 = load i16, i16* %q, align 2, !tbaa !2
  %34 = load double, double* %para7, align 8, !tbaa !6
  call void @_Z2RXqd(i16 %33, double %34)
  %35 = load i16, i16* %q, align 2, !tbaa !2
  %arrayidx20 = getelementptr inbounds [2 x double], [2 x double]* %para8, i64 0, i64 0
  %36 = load double, double* %arrayidx20, align 16, !tbaa !6
  call void @_Z2RXqd(i16 %35, double %36)
  %37 = load i16, i16* %q, align 2, !tbaa !2
  %arrayidx21 = getelementptr inbounds [2 x double], [2 x double]* %para8, i64 0, i64 1
  %38 = load double, double* %arrayidx21, align 8, !tbaa !6
  call void @_Z2RXqd(i16 %37, double %38)
  %39 = load i16, i16* %q, align 2, !tbaa !2
  %40 = load i32, i32* %para1.addr, align 4, !tbaa !8
  %conv22 = sitofp i32 %40 to double
  call void @_Z2RXqd(i16 %39, double %conv22)
  %41 = load i16, i16* %q, align 2, !tbaa !2
  %42 = load i32*, i32** %para2.addr, align 8, !tbaa !10
  %arrayidx23 = getelementptr inbounds i32, i32* %42, i64 0
  %43 = load i32, i32* %arrayidx23, align 4, !tbaa !8
  %conv24 = sitofp i32 %43 to double
  call void @_Z2RXqd(i16 %41, double %conv24)
  %44 = load i16, i16* %q, align 2, !tbaa !2
  %45 = load i32*, i32** %para2.addr, align 8, !tbaa !10
  %arrayidx25 = getelementptr inbounds i32, i32* %45, i64 1
  %46 = load i32, i32* %arrayidx25, align 4, !tbaa !8
  %conv26 = sitofp i32 %46 to double
  call void @_Z2RXqd(i16 %44, double %conv26)
  %47 = load i16, i16* %q, align 2, !tbaa !2
  %48 = load double, double* %para3.addr, align 8, !tbaa !6
  call void @_Z2RXqd(i16 %47, double %48)
  %49 = load i16, i16* %q, align 2, !tbaa !2
  %50 = load double*, double** %para4.addr, align 8, !tbaa !10
  %arrayidx27 = getelementptr inbounds double, double* %50, i64 0
  %51 = load double, double* %arrayidx27, align 8, !tbaa !6
  call void @_Z2RXqd(i16 %49, double %51)
  %52 = load i16, i16* %q, align 2, !tbaa !2
  %53 = load double*, double** %para4.addr, align 8, !tbaa !10
  %arrayidx28 = getelementptr inbounds double, double* %53, i64 1
  %54 = load double, double* %arrayidx28, align 8, !tbaa !6
  call void @_Z2RXqd(i16 %52, double %54)
  %55 = bitcast i16* %q to i8*
  call void @llvm.lifetime.end.p0i8(i64 2, i8* %55) #1
  %56 = bitcast [2 x double]* %para8 to i8*
  call void @llvm.lifetime.end.p0i8(i64 16, i8* %56) #1
  %57 = bitcast double* %para7 to i8*
  call void @llvm.lifetime.end.p0i8(i64 8, i8* %57) #1
  %58 = bitcast [2 x i32]* %para6 to i8*
  call void @llvm.lifetime.end.p0i8(i64 8, i8* %58) #1
  %59 = bitcast i32* %para5 to i8*
  call void @llvm.lifetime.end.p0i8(i64 4, i8* %59) #1
  ret void
}

; Function Attrs: nounwind uwtable
define dso_local void @_Z12EqualityTestiPidPd(i32 %para1, i32* %para2, double %para3, double* %para4) #0 {
entry:
  %para1.addr = alloca i32, align 4
  %para2.addr = alloca i32*, align 8
  %para3.addr = alloca double, align 8
  %para4.addr = alloca double*, align 8
  %para5 = alloca i32, align 4
  %para6 = alloca [2 x i32], align 4
  %para7 = alloca double, align 8
  %para8 = alloca [2 x double], align 16
  %q = alloca i16, align 2
  store i32 %para1, i32* %para1.addr, align 4, !tbaa !8
  store i32* %para2, i32** %para2.addr, align 8, !tbaa !10
  store double %para3, double* %para3.addr, align 8, !tbaa !6
  store double* %para4, double** %para4.addr, align 8, !tbaa !10
  %0 = bitcast i32* %para5 to i8*
  call void @llvm.lifetime.start.p0i8(i64 4, i8* %0) #1
  store i32 0, i32* %para5, align 4, !tbaa !8
  %1 = bitcast [2 x i32]* %para6 to i8*
  call void @llvm.lifetime.start.p0i8(i64 8, i8* %1) #1
  %2 = bitcast [2 x i32]* %para6 to i8*
  call void @llvm.memcpy.p0i8.p0i8.i64(i8* align 4 %2, i8* align 4 bitcast ([2 x i32]* @__const._Z12EqualityTestiPidPd.para6 to i8*), i64 8, i1 false)
  %3 = bitcast double* %para7 to i8*
  call void @llvm.lifetime.start.p0i8(i64 8, i8* %3) #1
  store double 1.000000e-02, double* %para7, align 8, !tbaa !6
  %4 = bitcast [2 x double]* %para8 to i8*
  call void @llvm.lifetime.start.p0i8(i64 16, i8* %4) #1
  %5 = bitcast [2 x double]* %para8 to i8*
  call void @llvm.memcpy.p0i8.p0i8.i64(i8* align 16 %5, i8* align 16 bitcast ([2 x double]* @__const._Z12EqualityTestiPidPd.para8 to i8*), i64 16, i1 false)
  %6 = bitcast i16* %q to i8*
  call void @llvm.lifetime.start.p0i8(i64 2, i8* %6) #1
  %7 = load i16, i16* %q, align 2, !tbaa !2
  call void @_Z2RXqd(i16 %7, double 1.000000e+00)
  %8 = load i16, i16* %q, align 2, !tbaa !2
  call void @_Z2RXqd(i16 %8, double 1.000000e+00)
  %9 = load i16, i16* %q, align 2, !tbaa !2
  call void @_Z2RXqd(i16 %9, double 1.000000e-01)
  %10 = load i16, i16* %q, align 2, !tbaa !2
  call void @_Z2RXqd(i16 %10, double 1.000000e-01)
  %11 = load i16, i16* %q, align 2, !tbaa !2
  %12 = load i32, i32* %para1.addr, align 4, !tbaa !8
  %conv = sitofp i32 %12 to double
  call void @_Z2RXqd(i16 %11, double %conv)
  %13 = load i16, i16* %q, align 2, !tbaa !2
  %14 = load i32, i32* %para1.addr, align 4, !tbaa !8
  %conv1 = sitofp i32 %14 to double
  call void @_Z2RXqd(i16 %13, double %conv1)
  %15 = load i16, i16* %q, align 2, !tbaa !2
  %16 = load i32*, i32** %para2.addr, align 8, !tbaa !10
  %arrayidx = getelementptr inbounds i32, i32* %16, i64 0
  %17 = load i32, i32* %arrayidx, align 4, !tbaa !8
  %conv2 = sitofp i32 %17 to double
  call void @_Z2RXqd(i16 %15, double %conv2)
  %18 = load i16, i16* %q, align 2, !tbaa !2
  %19 = load i32*, i32** %para2.addr, align 8, !tbaa !10
  %arrayidx3 = getelementptr inbounds i32, i32* %19, i64 0
  %20 = load i32, i32* %arrayidx3, align 4, !tbaa !8
  %conv4 = sitofp i32 %20 to double
  call void @_Z2RXqd(i16 %18, double %conv4)
  %21 = load i16, i16* %q, align 2, !tbaa !2
  %22 = load i32*, i32** %para2.addr, align 8, !tbaa !10
  %arrayidx5 = getelementptr inbounds i32, i32* %22, i64 1
  %23 = load i32, i32* %arrayidx5, align 4, !tbaa !8
  %conv6 = sitofp i32 %23 to double
  call void @_Z2RXqd(i16 %21, double %conv6)
  %24 = load i16, i16* %q, align 2, !tbaa !2
  %25 = load i32*, i32** %para2.addr, align 8, !tbaa !10
  %arrayidx7 = getelementptr inbounds i32, i32* %25, i64 1
  %26 = load i32, i32* %arrayidx7, align 4, !tbaa !8
  %conv8 = sitofp i32 %26 to double
  call void @_Z2RXqd(i16 %24, double %conv8)
  %27 = load i16, i16* %q, align 2, !tbaa !2
  %28 = load double, double* %para3.addr, align 8, !tbaa !6
  call void @_Z2RXqd(i16 %27, double %28)
  %29 = load i16, i16* %q, align 2, !tbaa !2
  %30 = load double, double* %para3.addr, align 8, !tbaa !6
  call void @_Z2RXqd(i16 %29, double %30)
  %31 = load i16, i16* %q, align 2, !tbaa !2
  %32 = load double*, double** %para4.addr, align 8, !tbaa !10
  %arrayidx9 = getelementptr inbounds double, double* %32, i64 0
  %33 = load double, double* %arrayidx9, align 8, !tbaa !6
  call void @_Z2RXqd(i16 %31, double %33)
  %34 = load i16, i16* %q, align 2, !tbaa !2
  %35 = load double*, double** %para4.addr, align 8, !tbaa !10
  %arrayidx10 = getelementptr inbounds double, double* %35, i64 0
  %36 = load double, double* %arrayidx10, align 8, !tbaa !6
  call void @_Z2RXqd(i16 %34, double %36)
  %37 = load i16, i16* %q, align 2, !tbaa !2
  %38 = load double*, double** %para4.addr, align 8, !tbaa !10
  %arrayidx11 = getelementptr inbounds double, double* %38, i64 1
  %39 = load double, double* %arrayidx11, align 8, !tbaa !6
  call void @_Z2RXqd(i16 %37, double %39)
  %40 = load i16, i16* %q, align 2, !tbaa !2
  %41 = load double*, double** %para4.addr, align 8, !tbaa !10
  %arrayidx12 = getelementptr inbounds double, double* %41, i64 1
  %42 = load double, double* %arrayidx12, align 8, !tbaa !6
  call void @_Z2RXqd(i16 %40, double %42)
  %43 = load i16, i16* %q, align 2, !tbaa !2
  %44 = load i32, i32* %para1.addr, align 4, !tbaa !8
  %conv13 = sitofp i32 %44 to double
  call void @_Z2RXqd(i16 %43, double %conv13)
  %45 = load i32, i32* %para1.addr, align 4, !tbaa !8
  %add = add nsw i32 %45, 1
  store i32 %add, i32* %para1.addr, align 4, !tbaa !8
  %46 = load i16, i16* %q, align 2, !tbaa !2
  %47 = load i32, i32* %para1.addr, align 4, !tbaa !8
  %conv14 = sitofp i32 %47 to double
  call void @_Z2RXqd(i16 %46, double %conv14)
  %48 = load i16, i16* %q, align 2, !tbaa !2
  %49 = load i32*, i32** %para2.addr, align 8, !tbaa !10
  %arrayidx15 = getelementptr inbounds i32, i32* %49, i64 0
  %50 = load i32, i32* %arrayidx15, align 4, !tbaa !8
  %conv16 = sitofp i32 %50 to double
  call void @_Z2RXqd(i16 %48, double %conv16)
  %51 = load i32*, i32** %para2.addr, align 8, !tbaa !10
  %arrayidx17 = getelementptr inbounds i32, i32* %51, i64 0
  %52 = load i32, i32* %arrayidx17, align 4, !tbaa !8
  %add18 = add nsw i32 %52, 1
  store i32 %add18, i32* %arrayidx17, align 4, !tbaa !8
  %53 = load i16, i16* %q, align 2, !tbaa !2
  %54 = load i32*, i32** %para2.addr, align 8, !tbaa !10
  %arrayidx19 = getelementptr inbounds i32, i32* %54, i64 0
  %55 = load i32, i32* %arrayidx19, align 4, !tbaa !8
  %conv20 = sitofp i32 %55 to double
  call void @_Z2RXqd(i16 %53, double %conv20)
  %56 = load i16, i16* %q, align 2, !tbaa !2
  %57 = load i32*, i32** %para2.addr, align 8, !tbaa !10
  %arrayidx21 = getelementptr inbounds i32, i32* %57, i64 1
  %58 = load i32, i32* %arrayidx21, align 4, !tbaa !8
  %conv22 = sitofp i32 %58 to double
  call void @_Z2RXqd(i16 %56, double %conv22)
  %59 = load i32*, i32** %para2.addr, align 8, !tbaa !10
  %arrayidx23 = getelementptr inbounds i32, i32* %59, i64 1
  %60 = load i32, i32* %arrayidx23, align 4, !tbaa !8
  %add24 = add nsw i32 %60, 1
  store i32 %add24, i32* %arrayidx23, align 4, !tbaa !8
  %61 = load i16, i16* %q, align 2, !tbaa !2
  %62 = load i32*, i32** %para2.addr, align 8, !tbaa !10
  %arrayidx25 = getelementptr inbounds i32, i32* %62, i64 1
  %63 = load i32, i32* %arrayidx25, align 4, !tbaa !8
  %conv26 = sitofp i32 %63 to double
  call void @_Z2RXqd(i16 %61, double %conv26)
  %64 = load i16, i16* %q, align 2, !tbaa !2
  %65 = load double, double* %para3.addr, align 8, !tbaa !6
  call void @_Z2RXqd(i16 %64, double %65)
  %66 = load double, double* %para3.addr, align 8, !tbaa !6
  %add27 = fadd double %66, 1.000000e+00
  store double %add27, double* %para3.addr, align 8, !tbaa !6
  %67 = load i16, i16* %q, align 2, !tbaa !2
  %68 = load double, double* %para3.addr, align 8, !tbaa !6
  call void @_Z2RXqd(i16 %67, double %68)
  %69 = load i16, i16* %q, align 2, !tbaa !2
  %70 = load double*, double** %para4.addr, align 8, !tbaa !10
  %arrayidx28 = getelementptr inbounds double, double* %70, i64 0
  %71 = load double, double* %arrayidx28, align 8, !tbaa !6
  call void @_Z2RXqd(i16 %69, double %71)
  %72 = load double*, double** %para4.addr, align 8, !tbaa !10
  %arrayidx29 = getelementptr inbounds double, double* %72, i64 0
  %73 = load double, double* %arrayidx29, align 8, !tbaa !6
  %add30 = fadd double %73, 1.000000e+00
  store double %add30, double* %arrayidx29, align 8, !tbaa !6
  %74 = load i16, i16* %q, align 2, !tbaa !2
  %75 = load double*, double** %para4.addr, align 8, !tbaa !10
  %arrayidx31 = getelementptr inbounds double, double* %75, i64 0
  %76 = load double, double* %arrayidx31, align 8, !tbaa !6
  call void @_Z2RXqd(i16 %74, double %76)
  %77 = load i16, i16* %q, align 2, !tbaa !2
  %78 = load double*, double** %para4.addr, align 8, !tbaa !10
  %arrayidx32 = getelementptr inbounds double, double* %78, i64 1
  %79 = load double, double* %arrayidx32, align 8, !tbaa !6
  call void @_Z2RXqd(i16 %77, double %79)
  %80 = load double*, double** %para4.addr, align 8, !tbaa !10
  %arrayidx33 = getelementptr inbounds double, double* %80, i64 1
  %81 = load double, double* %arrayidx33, align 8, !tbaa !6
  %add34 = fadd double %81, 1.000000e+00
  store double %add34, double* %arrayidx33, align 8, !tbaa !6
  %82 = load i16, i16* %q, align 2, !tbaa !2
  %83 = load double*, double** %para4.addr, align 8, !tbaa !10
  %arrayidx35 = getelementptr inbounds double, double* %83, i64 1
  %84 = load double, double* %arrayidx35, align 8, !tbaa !6
  call void @_Z2RXqd(i16 %82, double %84)
  %85 = load i16, i16* %q, align 2, !tbaa !2
  %86 = load i32, i32* %para1.addr, align 4, !tbaa !8
  %add36 = add nsw i32 %86, 1
  %conv37 = sitofp i32 %add36 to double
  call void @_Z2RXqd(i16 %85, double %conv37)
  %87 = load i16, i16* %q, align 2, !tbaa !2
  %88 = load i32, i32* %para1.addr, align 4, !tbaa !8
  %add38 = add nsw i32 %88, 1
  %conv39 = sitofp i32 %add38 to double
  call void @_Z2RXqd(i16 %87, double %conv39)
  %89 = load i16, i16* %q, align 2, !tbaa !2
  %90 = load i32*, i32** %para2.addr, align 8, !tbaa !10
  %arrayidx40 = getelementptr inbounds i32, i32* %90, i64 0
  %91 = load i32, i32* %arrayidx40, align 4, !tbaa !8
  %add41 = add nsw i32 %91, 1
  %conv42 = sitofp i32 %add41 to double
  call void @_Z2RXqd(i16 %89, double %conv42)
  %92 = load i16, i16* %q, align 2, !tbaa !2
  %93 = load i32*, i32** %para2.addr, align 8, !tbaa !10
  %arrayidx43 = getelementptr inbounds i32, i32* %93, i64 0
  %94 = load i32, i32* %arrayidx43, align 4, !tbaa !8
  %add44 = add nsw i32 %94, 1
  %conv45 = sitofp i32 %add44 to double
  call void @_Z2RXqd(i16 %92, double %conv45)
  %95 = load i16, i16* %q, align 2, !tbaa !2
  %96 = load i32*, i32** %para2.addr, align 8, !tbaa !10
  %arrayidx46 = getelementptr inbounds i32, i32* %96, i64 1
  %97 = load i32, i32* %arrayidx46, align 4, !tbaa !8
  %add47 = add nsw i32 %97, 1
  %conv48 = sitofp i32 %add47 to double
  call void @_Z2RXqd(i16 %95, double %conv48)
  %98 = load i16, i16* %q, align 2, !tbaa !2
  %99 = load i32*, i32** %para2.addr, align 8, !tbaa !10
  %arrayidx49 = getelementptr inbounds i32, i32* %99, i64 1
  %100 = load i32, i32* %arrayidx49, align 4, !tbaa !8
  %add50 = add nsw i32 %100, 1
  %conv51 = sitofp i32 %add50 to double
  call void @_Z2RXqd(i16 %98, double %conv51)
  %101 = load i16, i16* %q, align 2, !tbaa !2
  %102 = load double, double* %para3.addr, align 8, !tbaa !6
  %add52 = fadd double %102, 1.000000e+00
  call void @_Z2RXqd(i16 %101, double %add52)
  %103 = load i16, i16* %q, align 2, !tbaa !2
  %104 = load double, double* %para3.addr, align 8, !tbaa !6
  %add53 = fadd double %104, 1.000000e+00
  call void @_Z2RXqd(i16 %103, double %add53)
  %105 = load i16, i16* %q, align 2, !tbaa !2
  %106 = load double*, double** %para4.addr, align 8, !tbaa !10
  %arrayidx54 = getelementptr inbounds double, double* %106, i64 0
  %107 = load double, double* %arrayidx54, align 8, !tbaa !6
  %add55 = fadd double %107, 1.000000e+00
  call void @_Z2RXqd(i16 %105, double %add55)
  %108 = load i16, i16* %q, align 2, !tbaa !2
  %109 = load double*, double** %para4.addr, align 8, !tbaa !10
  %arrayidx56 = getelementptr inbounds double, double* %109, i64 0
  %110 = load double, double* %arrayidx56, align 8, !tbaa !6
  %add57 = fadd double %110, 1.000000e+00
  call void @_Z2RXqd(i16 %108, double %add57)
  %111 = load i16, i16* %q, align 2, !tbaa !2
  %112 = load double*, double** %para4.addr, align 8, !tbaa !10
  %arrayidx58 = getelementptr inbounds double, double* %112, i64 1
  %113 = load double, double* %arrayidx58, align 8, !tbaa !6
  %add59 = fadd double %113, 1.000000e+00
  call void @_Z2RXqd(i16 %111, double %add59)
  %114 = load i16, i16* %q, align 2, !tbaa !2
  %115 = load double*, double** %para4.addr, align 8, !tbaa !10
  %arrayidx60 = getelementptr inbounds double, double* %115, i64 1
  %116 = load double, double* %arrayidx60, align 8, !tbaa !6
  %add61 = fadd double %116, 1.000000e+00
  call void @_Z2RXqd(i16 %114, double %add61)
  %117 = load i16, i16* %q, align 2, !tbaa !2
  %118 = load i32, i32* %para1.addr, align 4, !tbaa !8
  %119 = load i32*, i32** %para2.addr, align 8, !tbaa !10
  %arrayidx62 = getelementptr inbounds i32, i32* %119, i64 0
  %120 = load i32, i32* %arrayidx62, align 4, !tbaa !8
  %add63 = add nsw i32 %118, %120
  %conv64 = sitofp i32 %add63 to double
  call void @_Z2RXqd(i16 %117, double %conv64)
  %121 = load i16, i16* %q, align 2, !tbaa !2
  %122 = load i32, i32* %para1.addr, align 4, !tbaa !8
  %123 = load i32*, i32** %para2.addr, align 8, !tbaa !10
  %arrayidx65 = getelementptr inbounds i32, i32* %123, i64 0
  %124 = load i32, i32* %arrayidx65, align 4, !tbaa !8
  %add66 = add nsw i32 %122, %124
  %conv67 = sitofp i32 %add66 to double
  call void @_Z2RXqd(i16 %121, double %conv67)
  %125 = load i16, i16* %q, align 2, !tbaa !2
  %126 = load double, double* %para3.addr, align 8, !tbaa !6
  %127 = load double*, double** %para4.addr, align 8, !tbaa !10
  %arrayidx68 = getelementptr inbounds double, double* %127, i64 0
  %128 = load double, double* %arrayidx68, align 8, !tbaa !6
  %add69 = fadd double %126, %128
  call void @_Z2RXqd(i16 %125, double %add69)
  %129 = load i16, i16* %q, align 2, !tbaa !2
  %130 = load double, double* %para3.addr, align 8, !tbaa !6
  %131 = load double*, double** %para4.addr, align 8, !tbaa !10
  %arrayidx70 = getelementptr inbounds double, double* %131, i64 0
  %132 = load double, double* %arrayidx70, align 8, !tbaa !6
  %add71 = fadd double %130, %132
  call void @_Z2RXqd(i16 %129, double %add71)
  %133 = bitcast i16* %q to i8*
  call void @llvm.lifetime.end.p0i8(i64 2, i8* %133) #1
  %134 = bitcast [2 x double]* %para8 to i8*
  call void @llvm.lifetime.end.p0i8(i64 16, i8* %134) #1
  %135 = bitcast double* %para7 to i8*
  call void @llvm.lifetime.end.p0i8(i64 8, i8* %135) #1
  %136 = bitcast [2 x i32]* %para6 to i8*
  call void @llvm.lifetime.end.p0i8(i64 8, i8* %136) #1
  %137 = bitcast i32* %para5 to i8*
  call void @llvm.lifetime.end.p0i8(i64 4, i8* %137) #1
  ret void
}

; Function Attrs: nounwind uwtable
define dso_local void @_Z14ArithmeticTestddd(double %para1, double %para2, double %cnst) #0 {
entry:
  %para1.addr = alloca double, align 8
  %para2.addr = alloca double, align 8
  %cnst.addr = alloca double, align 8
  %q = alloca i16, align 2
  store double %para1, double* %para1.addr, align 8, !tbaa !6
  store double %para2, double* %para2.addr, align 8, !tbaa !6
  store double %cnst, double* %cnst.addr, align 8, !tbaa !6
  %0 = bitcast i16* %q to i8*
  call void @llvm.lifetime.start.p0i8(i64 2, i8* %0) #1
  %1 = load i16, i16* %q, align 2, !tbaa !2
  call void @_Z2RXqd(i16 %1, double 1.200000e+00)
  %2 = load i16, i16* %q, align 2, !tbaa !2
  call void @_Z2RXqd(i16 %2, double 1.800000e+00)
  %3 = load i16, i16* %q, align 2, !tbaa !2
  %4 = load double, double* %para1.addr, align 8, !tbaa !6
  call void @_Z2RXqd(i16 %3, double %4)
  %5 = load i16, i16* %q, align 2, !tbaa !2
  %6 = load double, double* %para2.addr, align 8, !tbaa !6
  call void @_Z2RXqd(i16 %5, double %6)
  %7 = bitcast i16* %q to i8*
  call void @llvm.lifetime.end.p0i8(i64 2, i8* %7) #1
  ret void
}

attributes #0 = { nounwind uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="none" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { nounwind }
attributes #2 = { argmemonly nounwind willreturn }

!llvm.module.flags = !{!0}
!llvm.ident = !{!1}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{!"clang version 10.0.0"}
!2 = !{!3, !3, i64 0}
!3 = !{!"qbit", !4, i64 0}
!4 = !{!"omnipotent char", !5, i64 0}
!5 = !{!"Simple C++ TBAA"}
!6 = !{!7, !7, i64 0}
!7 = !{!"double", !4, i64 0}
!8 = !{!9, !9, i64 0}
!9 = !{!"int", !4, i64 0}
!10 = !{!11, !11, i64 0}
!11 = !{!"any pointer", !4, i64 0}
)"; // Must use Raw string to ensure proper parsing

#endif
