//===- QuantumSpinNativeToJson.h -----------------------------------------===//
//
// Copyright (C) 2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//
//===----------------------------------------------------------------------===//
/// \file "QuantumSpinNativeToJson.h"
///
/// This file provides the module pass which adds the quantum metadata
/// as a a map between the StringRef gate name and the JSON-parsed annotation
/// for spin dot qubit native gates.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_QUANTUM_SPINNATIVETOJSON_H
#define LLVM_QUANTUM_SPINNATIVETOJSON_H

#include "IntelQuantumPasses/InitializeQuantumPasses.h"
#include "IntelQuantumPasses/QuantumAnalysis/QuantumAnnotationsToJson.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/InstIterator.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/PassManager.h"
#include "llvm/IRReader/IRReader.h"
#include "llvm/InitializePasses.h"
#include "llvm/Pass.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/JSON.h" // JSON package
#include "llvm/Support/SourceMgr.h"
#include "llvm/Support/raw_ostream.h"

#include <map>

namespace llvm {

class QuantumSpinNativeToJson {
  static std::vector<std::string> SpinNativeGateAnnotations;

public:
  bool addSpinNativeFunctions(Module &M, QuantumModule &QM);
};

class QuantumSpinNativeToJsonLegacyPass : public ModulePass {

public:
  static char ID;
  static std::vector<std::string> SpinNativeGateAnnotations;
  QuantumModule &QM = QuantumCompilerLinkageLegacyPass::QM;
  QuantumSpinNativeToJson QSNJ;

  /// Minimal constructor.
  QuantumSpinNativeToJsonLegacyPass() : ModulePass(ID) {
    initializeQuantumSpinNativeToJsonLegacyPassPass(
        *PassRegistry::getPassRegistry());
  }

  void getAnalysisUsage(AnalysisUsage &AU) const override {

    AU.setPreservesAll();
    AU.addRequired<QuantumAnnotationsToJsonWrapperPass>();
  }

  /// Destructor.
  ~QuantumSpinNativeToJsonLegacyPass(){};

  /// Clear and fill qGateMetadata.
  virtual bool doInitialization(Module &M) override {

    /// Fill qGateMetadata
    QSNJ.addSpinNativeFunctions(M, QM);
    return false;
  }

  bool runOnModule(Module &M) { return false; }
};

/// Return pointer to new QuantumSpinNativeToJsonLegacyPass.
ModulePass *createQuantumSpinNativeToJsonLegacyPass();

////////////////////////////////////////////////////////////////////////////////

class QuantumSpinNativeToJsonWrapperPass : public ModulePass {
  QuantumSpinNativeToJsonLegacyPass QSNJ;

public:
  static char ID; // Pass identification, replacement for typeid

  QuantumSpinNativeToJsonWrapperPass() : ModulePass(ID) {
    initializeQuantumSpinNativeToJsonWrapperPassPass(
        *PassRegistry::getPassRegistry());
  }

  QuantumSpinNativeToJsonLegacyPass &quantumSpinNativeToJson() { return QSNJ; }

  const QuantumSpinNativeToJsonLegacyPass &quantumSpinNativeToJson() const {
    return QSNJ;
  }

  bool runOnModule(Module &M) override { return false; }
  virtual bool doInitialization(Module &M) override {
    return QSNJ.doInitialization(M);
  }
};

ModulePass *createQuantumSpinNativeToJsonWrapperPass();

class QuantumSpinNativeToJsonPass
    : public PassInfoMixin<QuantumSpinNativeToJsonPass> {
public:
  QuantumSpinNativeToJson QSNJ;
  PreservedAnalyses run(Module &M, ModuleAnalysisManager &MAM);
};

} // namespace llvm

#endif // for header guard LLVM_QUANTUM_SPINNATIVETOJSON_H
