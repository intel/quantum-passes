//===- PluginImplementation.cpp -------------------------------------------===//
//
// Copyright (C) 2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//
//===----------------------------------------------------------------------===//
//
// This implements a pass to decompose Toffoli gates to CNOT, H, and T(dag)
// gates.
//
//===----------------------------------------------------------------------===//

#include "GatePrinterPass.h"
#include "XToHZHPass.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"

using namespace llvm;

llvm::PassPluginLibraryInfo getExampleMultiPassPluginInfo() {
  return {LLVM_PLUGIN_API_VERSION, "ExampleMultiPassPlugin",
          LLVM_VERSION_STRING, [](PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
                [](StringRef Name, ModulePassManager &MPM,
                   ArrayRef<PassBuilder::PipelineElement>) {
                  if (Name == "print-all-gates") {
                    MPM.addPass(PrintAllGatesPass());
                    return true;
                  } else if (Name == "x-to-hzh") {
                    MPM.addPass(XToHZHPass());
                    return true;
                  }
                });
          }};
}

extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo() {
  return getExampleMultiPassPluginInfo();
}