//===- QuantumQbitRefTest.hpp --------------------------------*- C++ -*-===//
//
//
// Copyright (C) 2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//
//===----------------------------------------------------------------------===//
/// \file "QuantumQbitRefTest.hpp"
///
/// This file provides the raw strings for Quantum Qbit Reference unit
/// test.
///
//===----------------------------------------------------------------------===//

#ifndef QUANTUM_QBIT_REF_TEST_HPP
#define QUANTUM_QBIT_REF_TEST_HPP

// Minimum working .ll file

static const char Code[] = R"(
; ModuleID = 'QbitRef_unittest.ll'
source_filename = "unit_tests/source-files/QbitRef_unittest.cpp"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

@.str = private unnamed_addr constant [325 x i8] c"{     \22matrix_real\22 : \221/sqrt(2)*[1, 1, 1, -1]\22,     \22matrix_imag\22 : [0, 0, 0, 0],     \22matrix_order\22 : \22rm\22,     \22is_hermitian\22 : true,     \22is_unitary\22 : true,     \22is_mutable\22 : true,     \22qubit_list\22 : [0],     \22parametric_list\22 : [],     \22control_qubit_list\22 : [],     \22local_basis_list\22 : [4],     \22identifier\22 : 1   }\00", section "llvm.metadata"
@.str.1 = private unnamed_addr constant [52  x i8] c"unit_tests/source-files/quintrinsics_QbitRef_test.h\00", section "llvm.metadata"
@q_glob1 = dso_local global i16 0, align 2
@q_glob2 = dso_local global [2 x i16] zeroinitializer, align 2
@llvm.global.annotations = appending global [1 x { ptr, ptr, ptr, i32, ptr }] [{ ptr, ptr, ptr, i32, ptr } { ptr @_Z5HGateRt, ptr @.str, ptr @.str.1, i32 43, ptr null }], section "llvm.metadata"

; Function Attrs: mustprogress noinline uwtable
define dso_local void @_Z5HGateRt(ptr noundef nonnull align 2 dereferenceable(2) "qubit_ref" %q) #0 {
entry:
  call void @llvm.quantum.qubit(ptr noundef nonnull %q)
  ret void
}

declare void @_Z15__quantum_qubitPt(ptr noundef "qubit_ref") local_unnamed_addr #1

; Function Attrs: mustprogress nofree noinline norecurse nosync nounwind willreturn memory(none) uwtable
define dso_local void @_Z14CtorSetterTestRtPtj.stub(ptr nocapture noundef nonnull readnone align 2 dereferenceable(2) %q0, ptr nocapture noundef readnone %q1, i32 noundef %idx_in) local_unnamed_addr #2 section ".qbbs_text" {
entry:
  ret void
}

; Function Attrs: nocallback nofree nosync nounwind willreturn memory(argmem: readwrite)
declare void @llvm.lifetime.start.p0(i64 immarg, ptr nocapture) #3

; Function Attrs: nocallback nofree nosync nounwind willreturn memory(argmem: readwrite)
declare void @llvm.lifetime.end.p0(i64 immarg, ptr nocapture) #3

; Function Attrs: mustprogress noinline uwtable
define dso_local void @_Z14ResolutionTestRtPtj.stub(ptr noundef nonnull align 2 dereferenceable(2) "qubit_ref" %q0, ptr noundef "qubit_ref" %q1, i32 noundef %idx_in) local_unnamed_addr #0 section ".qbbs_text" {
entry:
  %q3 = alloca i16, align 2
  %q4 = alloca [2 x i16], align 2
  call void @llvm.lifetime.start.p0(i64 2, ptr nonnull %q3) #4
  call void @llvm.lifetime.start.p0(i64 4, ptr nonnull %q4) #4
  tail call void @_Z5HGateRt(ptr noundef nonnull align 2 dereferenceable(2) %q0)
  tail call void @_Z5HGateRt(ptr noundef nonnull align 2 dereferenceable(2) %q1)
  %arrayidx1 = getelementptr inbounds i16, ptr %q1, i64 1
  tail call void @_Z5HGateRt(ptr noundef nonnull align 2 dereferenceable(2) %arrayidx1)
  call void @_Z5HGateRt(ptr noundef nonnull align 2 dereferenceable(2) %q3)
  call void @_Z5HGateRt(ptr noundef nonnull align 2 dereferenceable(2) %q4)
  %arrayidx3 = getelementptr inbounds [2 x i16], ptr %q4, i64 0, i64 1
  call void @_Z5HGateRt(ptr noundef nonnull align 2 dereferenceable(2) %arrayidx3)
  call void @_Z5HGateRt(ptr noundef nonnull align 2 dereferenceable(2) %q1)
  call void @_Z5HGateRt(ptr noundef nonnull align 2 dereferenceable(2) %q4)
  call void @_Z5HGateRt(ptr noundef nonnull align 2 dereferenceable(2) %arrayidx1)
  call void @_Z5HGateRt(ptr noundef nonnull align 2 dereferenceable(2) %arrayidx3)
  %idxprom11 = zext i32 %idx_in to i64
  %arrayidx12 = getelementptr inbounds i16, ptr %q1, i64 %idxprom11
  call void @_Z5HGateRt(ptr noundef nonnull align 2 dereferenceable(2) %arrayidx12)
  %arrayidx14 = getelementptr inbounds [2 x i16], ptr %q4, i64 0, i64 %idxprom11
  call void @_Z5HGateRt(ptr noundef nonnull align 2 dereferenceable(2) %arrayidx14)
  call void @llvm.lifetime.end.p0(i64 4, ptr nonnull %q4) #4
  call void @llvm.lifetime.end.p0(i64 2, ptr nonnull %q3) #4
  ret void
}

; Function Attrs: mustprogress noinline uwtable
define dso_local void @_Z12EqualityTestRtPtjPj.stub(ptr noundef nonnull align 2 dereferenceable(2) "qubit_ref" %q0, ptr noundef "qubit_ref" %q1, i32 noundef %idx_in, ptr nocapture noundef %idx_in1) local_unnamed_addr #0 section ".qbbs_text" {
entry:
  %q3 = alloca i16, align 2
  %q4 = alloca [2 x i16], align 2
  call void @llvm.lifetime.start.p0(i64 2, ptr nonnull %q3) #4
  call void @llvm.lifetime.start.p0(i64 4, ptr nonnull %q4) #4
  tail call void @_Z5HGateRt(ptr noundef nonnull align 2 dereferenceable(2) %q0)
  tail call void @_Z5HGateRt(ptr noundef nonnull align 2 dereferenceable(2) %q0)
  tail call void @_Z5HGateRt(ptr noundef nonnull align 2 dereferenceable(2) %q1)
  tail call void @_Z5HGateRt(ptr noundef nonnull align 2 dereferenceable(2) %q1)
  %arrayidx2 = getelementptr inbounds i16, ptr %q1, i64 1
  tail call void @_Z5HGateRt(ptr noundef nonnull align 2 dereferenceable(2) %arrayidx2)
  tail call void @_Z5HGateRt(ptr noundef nonnull align 2 dereferenceable(2) %arrayidx2)
  call void @_Z5HGateRt(ptr noundef nonnull align 2 dereferenceable(2) %q3)
  call void @_Z5HGateRt(ptr noundef nonnull align 2 dereferenceable(2) %q3)
  call void @_Z5HGateRt(ptr noundef nonnull align 2 dereferenceable(2) %q4)
  call void @_Z5HGateRt(ptr noundef nonnull align 2 dereferenceable(2) %q4)
  %arrayidx6 = getelementptr inbounds [2 x i16], ptr %q4, i64 0, i64 1
  call void @_Z5HGateRt(ptr noundef nonnull align 2 dereferenceable(2) %arrayidx6)
  call void @_Z5HGateRt(ptr noundef nonnull align 2 dereferenceable(2) %arrayidx6)
  %idxprom = zext i32 %idx_in to i64
  %arrayidx8 = getelementptr inbounds i16, ptr %q1, i64 %idxprom
  call void @_Z5HGateRt(ptr noundef nonnull align 2 dereferenceable(2) %arrayidx8)
  call void @_Z5HGateRt(ptr noundef nonnull align 2 dereferenceable(2) %arrayidx8)
  %arrayidx12 = getelementptr inbounds [2 x i16], ptr %q4, i64 0, i64 %idxprom
  call void @_Z5HGateRt(ptr noundef nonnull align 2 dereferenceable(2) %arrayidx12)
  call void @_Z5HGateRt(ptr noundef nonnull align 2 dereferenceable(2) %arrayidx12)
  %0 = load i32, ptr %idx_in1, align 4, !tbaa !5
  %idxprom16 = zext i32 %0 to i64
  %arrayidx17 = getelementptr inbounds i16, ptr %q1, i64 %idxprom16
  call void @_Z5HGateRt(ptr noundef nonnull align 2 dereferenceable(2) %arrayidx17)
  %1 = load i32, ptr %idx_in1, align 4, !tbaa !5
  %idxprom19 = zext i32 %1 to i64
  %arrayidx20 = getelementptr inbounds i16, ptr %q1, i64 %idxprom19
  call void @_Z5HGateRt(ptr noundef nonnull align 2 dereferenceable(2) %arrayidx20)
  %arrayidx21 = getelementptr inbounds i32, ptr %idx_in1, i64 1
  %2 = load i32, ptr %arrayidx21, align 4, !tbaa !5
  %idxprom22 = zext i32 %2 to i64
  %arrayidx23 = getelementptr inbounds [2 x i16], ptr %q4, i64 0, i64 %idxprom22
  call void @_Z5HGateRt(ptr noundef nonnull align 2 dereferenceable(2) %arrayidx23)
  %3 = load i32, ptr %arrayidx21, align 4, !tbaa !5
  %idxprom25 = zext i32 %3 to i64
  %arrayidx26 = getelementptr inbounds [2 x i16], ptr %q4, i64 0, i64 %idxprom25
  call void @_Z5HGateRt(ptr noundef nonnull align 2 dereferenceable(2) %arrayidx26)
  call void @_Z5HGateRt(ptr noundef nonnull align 2 dereferenceable(2) %arrayidx8)
  %inc = add i32 %idx_in, 1
  %idxprom29 = zext i32 %inc to i64
  %arrayidx30 = getelementptr inbounds i16, ptr %q1, i64 %idxprom29
  call void @_Z5HGateRt(ptr noundef nonnull align 2 dereferenceable(2) %arrayidx30)
  %arrayidx32 = getelementptr inbounds [2 x i16], ptr %q4, i64 0, i64 %idxprom29
  call void @_Z5HGateRt(ptr noundef nonnull align 2 dereferenceable(2) %arrayidx32)
  %inc33 = add i32 %idx_in, 2
  %idxprom34 = zext i32 %inc33 to i64
  %arrayidx35 = getelementptr inbounds [2 x i16], ptr %q4, i64 0, i64 %idxprom34
  call void @_Z5HGateRt(ptr noundef nonnull align 2 dereferenceable(2) %arrayidx35)
  %4 = load i32, ptr %idx_in1, align 4, !tbaa !5
  %idxprom37 = zext i32 %4 to i64
  %arrayidx38 = getelementptr inbounds i16, ptr %q1, i64 %idxprom37
  call void @_Z5HGateRt(ptr noundef nonnull align 2 dereferenceable(2) %arrayidx38)
  %5 = load i32, ptr %idx_in1, align 4, !tbaa !5
  %inc40 = add i32 %5, 1
  store i32 %inc40, ptr %idx_in1, align 4, !tbaa !5
  %idxprom42 = zext i32 %inc40 to i64
  %arrayidx43 = getelementptr inbounds i16, ptr %q1, i64 %idxprom42
  call void @_Z5HGateRt(ptr noundef nonnull align 2 dereferenceable(2) %arrayidx43)
  %6 = load i32, ptr %arrayidx21, align 4, !tbaa !5
  %idxprom45 = zext i32 %6 to i64
  %arrayidx46 = getelementptr inbounds [2 x i16], ptr %q4, i64 0, i64 %idxprom45
  call void @_Z5HGateRt(ptr noundef nonnull align 2 dereferenceable(2) %arrayidx46)
  %7 = load i32, ptr %arrayidx21, align 4, !tbaa !5
  %inc48 = add i32 %7, 1
  store i32 %inc48, ptr %arrayidx21, align 4, !tbaa !5
  %idxprom50 = zext i32 %inc48 to i64
  %arrayidx51 = getelementptr inbounds [2 x i16], ptr %q4, i64 0, i64 %idxprom50
  call void @_Z5HGateRt(ptr noundef nonnull align 2 dereferenceable(2) %arrayidx51)
  %add = add i32 %idx_in, 3
  %idxprom52 = zext i32 %add to i64
  %arrayidx53 = getelementptr inbounds i16, ptr %q1, i64 %idxprom52
  call void @_Z5HGateRt(ptr noundef nonnull align 2 dereferenceable(2) %arrayidx53)
  call void @_Z5HGateRt(ptr noundef nonnull align 2 dereferenceable(2) %arrayidx53)
  %arrayidx59 = getelementptr inbounds [2 x i16], ptr %q4, i64 0, i64 %idxprom52
  call void @_Z5HGateRt(ptr noundef nonnull align 2 dereferenceable(2) %arrayidx59)
  call void @_Z5HGateRt(ptr noundef nonnull align 2 dereferenceable(2) %arrayidx59)
  %8 = load i32, ptr %idx_in1, align 4, !tbaa !5
  %add64 = add i32 %8, 1
  %idxprom65 = zext i32 %add64 to i64
  %arrayidx66 = getelementptr inbounds i16, ptr %q1, i64 %idxprom65
  call void @_Z5HGateRt(ptr noundef nonnull align 2 dereferenceable(2) %arrayidx66)
  %9 = load i32, ptr %idx_in1, align 4, !tbaa !5
  %add68 = add i32 %9, 1
  %idxprom69 = zext i32 %add68 to i64
  %arrayidx70 = getelementptr inbounds i16, ptr %q1, i64 %idxprom69
  call void @_Z5HGateRt(ptr noundef nonnull align 2 dereferenceable(2) %arrayidx70)
  %10 = load i32, ptr %arrayidx21, align 4, !tbaa !5
  %add72 = add i32 %10, 1
  %idxprom73 = zext i32 %add72 to i64
  %arrayidx74 = getelementptr inbounds [2 x i16], ptr %q4, i64 0, i64 %idxprom73
  call void @_Z5HGateRt(ptr noundef nonnull align 2 dereferenceable(2) %arrayidx74)
  %11 = load i32, ptr %arrayidx21, align 4, !tbaa !5
  %add76 = add i32 %11, 1
  %idxprom77 = zext i32 %add76 to i64
  %arrayidx78 = getelementptr inbounds [2 x i16], ptr %q4, i64 0, i64 %idxprom77
  call void @_Z5HGateRt(ptr noundef nonnull align 2 dereferenceable(2) %arrayidx78)
  %12 = load i32, ptr %idx_in1, align 4, !tbaa !5
  %add80 = add i32 %12, %inc33
  %idxprom81 = zext i32 %add80 to i64
  %arrayidx82 = getelementptr inbounds i16, ptr %q1, i64 %idxprom81
  call void @_Z5HGateRt(ptr noundef nonnull align 2 dereferenceable(2) %arrayidx82)
  %13 = load i32, ptr %idx_in1, align 4, !tbaa !5
  %add84 = add i32 %13, %inc33
  %idxprom85 = zext i32 %add84 to i64
  %arrayidx86 = getelementptr inbounds i16, ptr %q1, i64 %idxprom85
  call void @_Z5HGateRt(ptr noundef nonnull align 2 dereferenceable(2) %arrayidx86)
  %14 = load i32, ptr %arrayidx21, align 4, !tbaa !5
  %add88 = add i32 %14, %inc33
  %idxprom89 = zext i32 %add88 to i64
  %arrayidx90 = getelementptr inbounds [2 x i16], ptr %q4, i64 0, i64 %idxprom89
  call void @_Z5HGateRt(ptr noundef nonnull align 2 dereferenceable(2) %arrayidx90)
  %15 = load i32, ptr %arrayidx21, align 4, !tbaa !5
  %add92 = add i32 %15, %inc33
  %idxprom93 = zext i32 %add92 to i64
  %arrayidx94 = getelementptr inbounds [2 x i16], ptr %q4, i64 0, i64 %idxprom93
  call void @_Z5HGateRt(ptr noundef nonnull align 2 dereferenceable(2) %arrayidx94)
  call void @llvm.lifetime.end.p0(i64 4, ptr nonnull %q4) #4
  call void @llvm.lifetime.end.p0(i64 2, ptr nonnull %q3) #4
  ret void
}

; Function Attrs: mustprogress noinline uwtable
define dso_local void @_Z10GlobalTestii.stub(i32 noundef %idx1, i32 noundef %idx2) local_unnamed_addr #0 section ".qbbs_text" {
entry:
  tail call void @_Z5HGateRt(ptr noundef nonnull align 2 dereferenceable(2) @q_glob1)
  tail call void @_Z5HGateRt(ptr noundef nonnull align 2 dereferenceable(2) @q_glob1)
  tail call void @_Z5HGateRt(ptr noundef nonnull align 2 dereferenceable(2) @q_glob2)
  tail call void @_Z5HGateRt(ptr noundef nonnull align 2 dereferenceable(2) @q_glob2)
  tail call void @_Z5HGateRt(ptr noundef nonnull align 2 dereferenceable(2) getelementptr inbounds ([2 x i16], ptr @q_glob2, i64 0, i64 1))
  tail call void @_Z5HGateRt(ptr noundef nonnull align 2 dereferenceable(2) getelementptr inbounds ([2 x i16], ptr @q_glob2, i64 0, i64 1))
  %idxprom = sext i32 %idx1 to i64
  %arrayidx = getelementptr inbounds [2 x i16], ptr @q_glob2, i64 0, i64 %idxprom
  tail call void @_Z5HGateRt(ptr noundef nonnull align 2 dereferenceable(2) %arrayidx)
  tail call void @_Z5HGateRt(ptr noundef nonnull align 2 dereferenceable(2) %arrayidx)
  %add = add nsw i32 %idx1, 1
  %idxprom3 = sext i32 %add to i64
  %arrayidx4 = getelementptr inbounds [2 x i16], ptr @q_glob2, i64 0, i64 %idxprom3
  tail call void @_Z5HGateRt(ptr noundef nonnull align 2 dereferenceable(2) %arrayidx4)
  tail call void @_Z5HGateRt(ptr noundef nonnull align 2 dereferenceable(2) %arrayidx4)
  %add8 = add nsw i32 %idx2, %idx1
  %idxprom9 = sext i32 %add8 to i64
  %arrayidx10 = getelementptr inbounds [2 x i16], ptr @q_glob2, i64 0, i64 %idxprom9
  tail call void @_Z5HGateRt(ptr noundef nonnull align 2 dereferenceable(2) %arrayidx10)
  tail call void @_Z5HGateRt(ptr noundef nonnull align 2 dereferenceable(2) %arrayidx10)
  ret void
}

; Function Attrs: nounwind
declare void @llvm.quantum.qubit(ptr) #4

attributes #0 = { mustprogress noinline uwtable "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #1 = { "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #2 = { mustprogress nofree noinline norecurse nosync nounwind willreturn memory(none) uwtable "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #3 = { nocallback nofree nosync nounwind willreturn memory(argmem: readwrite) }
attributes #4 = { nounwind }

!llvm.module.flags = !{!0, !1, !2, !3}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{i32 8, !"PIC Level", i32 2}
!2 = !{i32 7, !"PIE Level", i32 2}
!3 = !{i32 7, !"uwtable", i32 2}
!5 = !{!6, !6, i64 0}
!6 = !{!"int", !7, i64 0}
!7 = !{!"omnipotent char", !8, i64 0}
!8 = !{!"Simple C++ TBAA"}

)"; // Must use Raw string to ensure proper parsing

#endif
