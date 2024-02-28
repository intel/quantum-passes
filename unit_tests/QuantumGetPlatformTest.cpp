//===- QuantumGetPlatformTest.cpp -----------------------------*- C++ -*-===//
//
//===----------------------------------------------------------------------===//
//
// Copyright (C) 2022 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//
//===----------------------------------------------------------------------===//
/// \file "QuantumGetPlatformTest.cpp"
///
/// This file implements unit test for the QuantumGetPlatformPass
///
//===----------------------------------------------------------------------===//

#include "IntelQuantumPasses/QuantumAnalysis/QuantumCompilerLinkage.h"

#include "IntelQuantumPasses/QuantumAnalysis/QuantumModule.h"
#include "llvm/AsmParser/Parser.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRPrintingPasses.h"
#include "llvm/IR/InstIterator.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/PassManager.h"
#include "llvm/IRReader/IRReader.h"
#include "llvm/InitializePasses.h"
#include "llvm/Pass.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/SourceMgr.h"
#include "llvm/Support/raw_ostream.h"

#include "gtest/gtest.h"

#include <map>

using namespace llvm;

static std::unique_ptr<Module> parseIR(LLVMContext &C, const char *IR) {
  SMDiagnostic Err;
  std::unique_ptr<Module> Mod = parseAssemblyString(IR, Err, C);
  if (!Mod)
    Err.print("QuantumGetPlatformTest", errs());
  return Mod;
}

// Minimum working .ll file for quantum annotations

const char Code[] = R"(
; ModuleID = 'QRT_declaration_mock_unrolled_user.ll'
source_filename = "unit_tests/source-files/QRT_declaration_mock.cpp"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

!llvm.module.flags = !{!0, !1, !2, !3}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{i32 8, !"PIC Level", i32 2}
!2 = !{i32 7, !"PIE Level", i32 2}
!3 = !{i32 7, !"uwtable", i32 2}
)";

///////////////////////////////////////////////////////////////////////////////////

TEST(QuantumPlatformTest, 12qTest) {

  // Use on the new config format

  LLVMContext C;
  std::unique_ptr<Module> M = parseIR(C, *&Code);

  QuantumModule QM;

  // manually set json filename
  QM.sdk_json_filename =
      std::string(CONFIGURATION_DIR) + "/intel-quantum-sdk_12q_linear.json";
  QM.initializeData(*M);

  // Check that Platform is there
  ASSERT_TRUE(QM.platform->platformConfigLoaded());
  EXPECT_EQ(QM.platform->getQubitCount(), 12);
  EXPECT_FALSE(QM.platform->isConnectivityAlltoAll());
}
