//===- SeparateAndReplaceQBB.cpp -----------------------------------------====//
//
// Copyright (C) 2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//
//===----------------------------------------------------------------------===//
//
// This file implements a Module pass which extracts the simple QBBs from the
// quantum kernels, inserts them into a new stand-alone function, and replaces
// the simple QBB with a QRT call. The qbb_link_map is then populated to link
// the two.
//
//===----------------------------------------------------------------------===//

#include "IntelQuantumPasses/QuantumCompile/SeparateAndReplaceQBB.h"
#include "IntelQuantumPasses/InitializeQuantumPasses.h"
#include "IntelQuantumPasses/QuantumAnalysis/QuantumAnnotationGetters.h"
#include "IntelQuantumPasses/QuantumAnalysis/QuantumCompilerLinkage.h"
#include "IntelQuantumPasses/QuantumAnalysis/QuantumGateIdentifiers.h"
#include "IntelQuantumPasses/QuantumAnalysis/QuantumIterator.h"
#include "IntelQuantumPasses/QuantumAnalysis/QuantumModule.h"
#include "IntelQuantumPasses/QuantumAnalysis/QuantumRefs.h"
#include "IntelQuantumPasses/QuantumAnalysis/QuantumSpinNativeToJson.h"
#include "IntelQuantumPasses/QuantumCompile/ValidateAndConditionQBB.h"
#include "IntelQuantumPasses/QuantumUtils/QuantumGeneralUtils.h"
#include "llvm/Demangle/Demangle.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/GlobalVariable.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/Operator.h"

#include "llvm/ADT/SCCIterator.h"
#include "llvm/IR/CFG.h"
#include "llvm/Pass.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;
using namespace aqcc;

bool separateAndReplaceQBB(Module &M, QuantumModule &QM) {

  // running qbb index
  unsigned qbb_idx = 0;

  for (auto &QK : QM) {
    for (auto &QBB : QK) {

      // create the empty QK for the simple QBB

      std::string simple_qbb_name =
          DemangleQuantumKernelName(QK.getFunction()->getName());
      simple_qbb_name =
          simple_qbb_name + ".QBB." + std::to_string(qbb_idx) + ".";
      simple_qbb_name =
          std::to_string(simple_qbb_name.size()) + simple_qbb_name;
      Function *simple_qbb = createEmptyQKernel(simple_qbb_name, M);

      BasicBlock &new_bb = simple_qbb->getEntryBlock();
      Instruction *new_ret = new_bb.getTerminator();

      // move all instruction from bb to new_bb
      // and count the instructions
      Instruction *ret = QBB.getTerminator();

      auto iter = QBB.begin();
      unsigned cnt = 0;
      while (&*iter != ret) {
        iter->moveBefore(new_ret);
        cnt++;
        iter = QBB.begin();
      }

      // Create QRT call
      FunctionType *FTy = QM.QRT_run_qbb->getFunctionType();
      std::vector<Value *> args = {dyn_cast<Value>(simple_qbb),
                                   dyn_cast<Value>(ConstantInt::getSigned(
                                       Type::getInt32Ty(M.getContext()), cnt))};
      CallInst::Create(FTy, dyn_cast<Value>(QM.QRT_run_qbb), args, "QRTcall",
                       ret);

      // update QBB_link_map
      QM.QBB_link_map[simple_qbb] = &QBB;
      qbb_idx++;
    }

    // Check that all Quantum gates are removed
    aqcc::QIter gate_search(*QK.getFunction());

    if (!gate_search.isEnd())
      displayErrorAndExit(
          "Separate And Replace QBB Pass",
          "Unable to seperate out all gates from '" +
              DemangleQuantumKernelName(QK.getFunction()->getName()) + "'.");

    // Now that the left over QK has no quantum gates, we will remove the
    // qbbs_text section This is so passes which query if the function is a QK
    // will not bother looking at this function
    QK.getFunction()->setSection("");
  };

  QM.setSimpleQBBSplit();
  return true;
}

namespace {

struct SeparateAndReplaceQBBLegacyPass : public ModulePass {

public:
  static char ID;
  QuantumModule &QM = QuantumCompilerLinkageLegacyPass::QM;

  SeparateAndReplaceQBBLegacyPass() : ModulePass(ID) {
    initializeSeparateAndReplaceQBBLegacyPassPass(
        *PassRegistry::getPassRegistry());
  }

  void getAnalysisUsage(AnalysisUsage &AU) const override {
    AU.setPreservesAll();
    // AU.addRequired<QuantumCompilerLinkageWrapperPass>();
  }

  bool runOnModule(Module &M) override {
    separateAndReplaceQBB(M, QM);

    return false;
  }
};

} // namespace

char SeparateAndReplaceQBBLegacyPass::ID = 0;

INITIALIZE_PASS_BEGIN(SeparateAndReplaceQBBLegacyPass,
                      "separate-and-replace-qbb",
                      "Separate and replace qbb legacy pass", false, false)
INITIALIZE_PASS_END(SeparateAndReplaceQBBLegacyPass, "separate-and-replace-qbb",
                    "Separate and replace qbb legacy pass", false, false)

PreservedAnalyses SeparateAndReplaceQBBPass::run(Module &M,
                                                 ModuleAnalysisManager &MAM) {
  QuantumModuleProxy QMP = MAM.getResult<QuantumCompilerLinkageAnalysis>(M);
  separateAndReplaceQBB(M, *QMP.QM);

  return PreservedAnalyses::all();
}