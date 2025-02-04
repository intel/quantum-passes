//===- QuantumCompilerLinkagePass.h ---------------------------------------===//
//
// Copyright (C) 2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//
//===----------------------------------------------------------------------===//
/// \file "QuantumCompilerLinkagePass.h"
///
/// This file defines the analysis pass which finds or stores
/// all elements for linkage between different parts of the quantum compiler.
/// This includes:
///     - all QRT functions
///     - the quantum shared double array (QSDA)
///     - map of max usage per kernel of the QSDA
///     - entry BB for hybrid QBBs with link to generated simple QBB
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_QUANTUM_COMPILER_LINKAGE_H
#define LLVM_QUANTUM_COMPILER_LINKAGE_H

#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/PassManager.h"
#include "llvm/IRReader/IRReader.h"
#include "llvm/InitializePasses.h"
#include "llvm/Pass.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/SourceMgr.h"
#include "llvm/Support/raw_ostream.h"

#include "IntelQuantumPasses/InitializeQuantumPasses.h"
#include "IntelQuantumPasses/QuantumAnalysis/QuantumModule.h"
#include "IntelQuantumPasses/QuantumAnalysis/QuantumRefs.h"

#include <map>

using namespace llvm;
using namespace aqcc;

namespace llvm {

////////////////////////////////////////////////////////////////////////////////

class QuantumCompilerLinkage {
  static aqcc::QuantumModule QM;

public:
  // QRT headers with ABI encoding.
  const std::string QRT_initialize_name =
      "_ZN4qrt021initializeQrtSystem64EPdPcS1_j";
  std::vector<Type *> QRT_initialize_args;
  const std::string QRT_is_initialized_name = "_ZN4qrt013isInitializedEv";
  const std::string QRT_measure_move_name =
      "_ZN4qrt020updateMeasurementMapEPbmb";
  std::vector<Type *> QRT_measure_move_args;
  const std::string QRT_run_qbb_name = "_ZN4qrt014scheduleKernelEPFvvEj";
  std::vector<Type *> QRT_run_qbb_args;
  const std::string QRT_dismantle_name = "_ZN4qrt018dismantleQrtSystemEv";
  const std::string QRT_exit_name = "_ZN4qrt04exitEPc";
  std::vector<Type *> QRT_exit_args;

  Type *QRTErrTy = nullptr;

  void doInitialization(Module &M, QuantumModule &QM);
  void locateOrDeclareQRTFunctions(Module &M, QuantumModule &QM);
  void checkForSeparation(Module &M, QuantumModule &QM);
  void checkForNative(Module &M, QuantumModule &QM);
  void checkForIMM(Module &M, QuantumModule &QM);
  void checkForMapped(Module &M, QuantumModule &QM);
  void checkForInRange(Module &M, QuantumModule &QM);
  void checkForConditioning(Module &M, QuantumModule &QM);
  void findQBBBinaries(Module &M);
  void findMaxQSDAUsage(Module &M, QuantumModule &QM);
};

////////////////////////////////////////////////////////////////////////////////

void updateCompilerLinkage(Module &M, QuantumModule &QM,
                           bool start_fresh = false);

class QuantumCompilerLinkageLegacyPass : public ModulePass {

public:
  static char ID;
  // This is a temporary method of contain the QModule inside this pass.
  // It should not be used if possible, was used with the old pass
  // manager.
  static aqcc::QuantumModule QM;

  QuantumCompilerLinkage QCL;

  /// Minimal constructor.
  QuantumCompilerLinkageLegacyPass() : ModulePass(ID) {
    initializeQuantumCompilerLinkageLegacyPassPass(
        *PassRegistry::getPassRegistry());
  }
  /// Destructor.
  ~QuantumCompilerLinkageLegacyPass() {}

  virtual bool doInitialization(Module &M) override {
    QCL.doInitialization(M, QM);

    return false;
  }

  bool runOnModule(Module &M) override {
    QCL.locateOrDeclareQRTFunctions(M, QM);
    QM.setSure();
    QCL.checkForConditioning(M, QM);
    QCL.checkForSeparation(M, QM);
    QCL.findMaxQSDAUsage(M, QM);
    // findQBBBinaries(M);

    return false;
  }
};

ModulePass *createQuantumCompilerLinkageLegacyPass();

////////////////////////////////////////////////////////////////////////////////

class QuantumCompilerLinkageWrapperPass : public ModulePass {
  QuantumCompilerLinkageLegacyPass QCL;

public:
  static char ID; // Pass identification, replacement for typeid

  QuantumCompilerLinkageWrapperPass() : ModulePass(ID) {
    initializeQuantumCompilerLinkageWrapperPassPass(
        *PassRegistry::getPassRegistry());
  }

  ~QuantumCompilerLinkageWrapperPass(){};

  QuantumCompilerLinkageLegacyPass &getQuantumCompilerLinkageLegacyPass() {
    return QCL;
  }

  virtual bool doInitialization(Module &M) override {
    return QCL.doInitialization(M);
  };

  bool runOnModule(Module &M) override { return QCL.runOnModule(M); }
};

ModulePass *createQuantumCompilerLinkageWrapperPass();

// New Pass Manager method.
class QuantumCompilerLinkageAnalysis
    : public AnalysisInfoMixin<QuantumCompilerLinkageAnalysis> {
public:
  typedef QuantumModuleProxy Result;

  Result run(Module &M, ModuleAnalysisManager &MAM);

private:
  friend AnalysisInfoMixin<QuantumCompilerLinkageAnalysis>;
  static AnalysisKey Key;
};

} // namespace llvm

// Clear out the debug logging macro.

#endif // for header guard LLVM_QUANTUM_COMPILER_LINKAGE_H
