//===- GetAnnotations.h - -------------------------------------------------===//
//
// Copyright (C) 2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//
//===----------------------------------------------------------------------===//
/// \file
///
/// This file provides interfaces used to gather global annotations.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_QUANTUM_GETANNOTATIONS_H
#define LLVM_QUANTUM_GETANNOTATIONS_H

#include "IntelQuantumPasses/InitializeQuantumPasses.h"
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
#include "llvm/Pass.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/SourceMgr.h"
#include "llvm/Support/raw_ostream.h"
#include <fstream>
#include <ios>
#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <string>

namespace llvm {

class GetAnnotationsPass : public ModulePass {

public:
  static char ID;
  GetAnnotationsPass() : ModulePass(ID) {
    initializeGetAnnotationsPassPass(*PassRegistry::getPassRegistry());
  }

  /// NOTE: Replacing the static member variable with a static getter function
  /// to ensure that when used / accessed, the variable is initialized.
  // // static std::map<StringRef, StringRef> *qGateMetadata;
  static std::map<StringRef, StringRef> &getInstance_qGateMetadata();

  virtual bool doInitialization(Module &M) override {
    locateAnnotatedFunctions(&M);
    return false;
  }
  void locateAnnotatedFunctions(Module *M);
  bool is_qgate(CallInst *CI);
  std::map<StringRef, StringRef> *getQGateMetadata(Module *M);
  bool runOnModule(Module &M);
};

ModulePass *createGetAnnotationsPass();

class GetAnnotationsWrapperPass : public ModulePass {
  GetAnnotationsPass GA;

public:
  static char ID; // Pass identification, replacement for typeid

  GetAnnotationsWrapperPass() : ModulePass(ID) {
    initializeGetAnnotationsWrapperPassPass(*PassRegistry::getPassRegistry());
  }

  GetAnnotationsPass &getAnnotations() { return GA; }
  const GetAnnotationsPass &getAnnotations() const { return GA; }
  std::map<StringRef, StringRef> *getQGateMetadata(Module *M) {
    return GA.getQGateMetadata(M);
  }

  /// Calculate the natural loop information for a given function.
  bool runOnModule(Module &M) override;
  void locateAnnotatedFunctions(Module *M);
};

ModulePass *createGetAnnotationsWrapperPass();

} // namespace llvm

#endif
