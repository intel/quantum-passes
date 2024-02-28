//===- QuantumCompilerLinkageTest.hpp --------------------------------*- C++
//-*-===//
//
//
// Copyright (C) 2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//
//===----------------------------------------------------------------------===//
/// \file "QuantumCompilerLinkageTest.hpp"
///
/// This file provides the raw strings for Quantum Compiler Linkage test
/// unit test.
///
//===----------------------------------------------------------------------===//

#ifndef QUANTUM_COMPILER_LINKAGE_TEST_HPP
#define QUANTUM_COMPILER_LINKAGE_TEST_HPP

// Minimum working .ll file

static const char Code[] = R"(
; ModuleID = 'QRT_declaration_mock_unrolled_user.ll'
source_filename = "unit_tests/source-files/QRT_declaration_mock.cpp"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

@_aqcc_dummy_bit = dso_local global i8 0, align 1
@_aqcc_dummy_para = dso_local global [1 x double] zeroinitializer, align 8
@_aqcc_dummy_config = dso_local global [2 x i8] c"a\00", align 1

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(none) uwtable
define dso_local void @_Z15_aqcc_dummy_QBBv() #0 {
entry:
  ret void
}

; Function Attrs: mustprogress uwtable
define dso_local void @_Z15_aqcc_dummy_foov() local_unnamed_addr #1 {
entry:
  %call = tail call noundef i32 @_ZN5_qrt024_initialize_qrt_system64EPdPc(ptr noundef nonnull @_aqcc_dummy_para, ptr noundef nonnull @_aqcc_dummy_config)
  %call1 = tail call noundef zeroext i1 @_ZN5_qrt015_is_initializedEv()
  tail call void @_ZN5_qrt022update_measurement_mapEPbmb(ptr noundef nonnull @_aqcc_dummy_bit, i64 noundef 0, i1 noundef zeroext false)
  %call2 = tail call noundef i32 @_ZN5_qrt015schedule_kernelEPFvvEj(ptr noundef nonnull @_Z15_aqcc_dummy_QBBv, i32 noundef 0)
  %call3 = tail call noundef i32 @_ZN5_qrt021_dismantle_qrt_systemEv()
  ret void
}

declare noundef i32 @_ZN5_qrt024_initialize_qrt_system64EPdPc(ptr noundef, ptr noundef) local_unnamed_addr #2

declare noundef zeroext i1 @_ZN5_qrt015_is_initializedEv() local_unnamed_addr #2

declare void @_ZN5_qrt022update_measurement_mapEPbmb(ptr noundef, i64 noundef, i1 noundef zeroext) local_unnamed_addr #2

declare noundef i32 @_ZN5_qrt015schedule_kernelEPFvvEj(ptr noundef, i32 noundef) local_unnamed_addr #2

declare noundef i32 @_ZN5_qrt021_dismantle_qrt_systemEv() local_unnamed_addr #2

declare i32 @_ZN4qrt021initializeQrtSystem64EPdPcS1_(ptr, ptr, ptr)

declare i1 @_ZN4qrt013isInitializedEv()

declare void @_ZN4qrt020updateMeasurementMapEPbmb(ptr, i64, i1)

declare i32 @_ZN4qrt014scheduleKernelEPFvvEj(ptr, i32)

declare i32 @_ZN4qrt018dismantleQrtSystemEv()

attributes #0 = { mustprogress nofree norecurse nosync nounwind willreturn memory(none) uwtable "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #1 = { mustprogress uwtable "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #2 = { "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }

!llvm.module.flags = !{!0, !1, !2, !3}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{i32 8, !"PIC Level", i32 2}
!2 = !{i32 7, !"PIE Level", i32 2}
!3 = !{i32 7, !"uwtable", i32 2}
)";

#endif
