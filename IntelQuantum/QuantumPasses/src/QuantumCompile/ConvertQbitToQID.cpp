//===- ConvertQbitToQID.cpp ----------------------------------------------====//
//
// Copyright (C) 2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//
//===----------------------------------------------------------------------===//
//
// This file implements a module pass which converts all qubit gate arguments
// to QID values.
// ASSUMPTION: this assumes a previous pass (namely scheduling) has converted
// all qbits to a single qubit register.
//
//===----------------------------------------------------------------------===//

#include "IntelQuantumPasses/QuantumCompile/ConvertQbitToQID.h"
#include "IntelQuantumPasses/InitializeQuantumPasses.h"
#include "IntelQuantumPasses/QuantumAnalysis/QuantumAnnotationGetters.h"
#include "IntelQuantumPasses/QuantumAnalysis/QuantumCompilerLinkage.h"
#include "IntelQuantumPasses/QuantumAnalysis/QuantumGateIdentifiers.h"
#include "IntelQuantumPasses/QuantumAnalysis/QuantumIterator.h"
#include "IntelQuantumPasses/QuantumAnalysis/QuantumModule.h"
#include "IntelQuantumPasses/QuantumAnalysis/QuantumRefs.h"
#include "IntelQuantumPasses/QuantumAnalysis/QuantumSpinNativeToJson.h"
#include "IntelQuantumPasses/QuantumUtils/QuantumGeneralUtils.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/GlobalVariable.h"
#include "llvm/IR/Intrinsics.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/Operator.h"

#include "llvm/Pass.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;
using namespace aqcc;

#define DEBUG_TYPE "convert-qbit-to-qid"

static void backfillQIDToQbitGlobal(Module &M, QuantumModule &QM) {
  LLVMContext &C = M.getContext();

  // Change Initializer for each qubit array using global placement
  for (auto qbit_reg : *QM.global_qubit_map) {

    if (GlobalVariable *GV = dyn_cast<GlobalVariable>(qbit_reg.getRegister())) {

      // create a constant value to load
      Constant *store_cnst = nullptr;
      std::vector<uint16_t> qid_array =
          QM.global_qubit_map->getQbitRegisterToPhysMap<uint16_t>(qbit_reg);

      if (PointerType *PtrTy = dyn_cast<PointerType>(GV->getType())) {
        Type *ValTy = GV->getValueType();
        if (ValTy->isArrayTy())
          store_cnst = dyn_cast<Constant>(ConstantDataArray::get(C, qid_array));
        else if (ValTy->isIntegerTy())
          store_cnst = dyn_cast<Constant>(
              ConstantInt::get(Type::getInt16Ty(C), qid_array[0]));
        else
          displayErrorAndExit("Convert Qbit to QID Pass",
                              "Found a strange type for a qbit\n");
      } else {
        displayErrorAndExit("Convert Qbit to QID Pass",
                            "Found a non-pointer type.");
      }
      GV->setInitializer(store_cnst);
    } else
      displayErrorAndExit("Convert Qbit to QID Pass",
                          "Found an improper qbit: must a global variable!\n");
  }
}

static void backfillQIDToQbitLocal(BasicBlock &QBB, QuantumModule &QM) {

  LLVMContext &C = QBB.getContext();

  // Get the QbitToPhysMap for this QBB:
  // one must use the placement at the end of the QBB.
  auto qubit_map = QM.final_qubit_maps[&QBB];

  if (!qubit_map)
    return;

  for (auto qbit_reg : *qubit_map) {
    // get the store-to ptr
    Value *store_ptr = qbit_reg.getRegister();

    // create a constant value to load
    Value *store_cnst;
    std::vector<uint16_t> qid_array =
        qubit_map->getQbitRegisterToPhysMap<uint16_t>(qbit_reg);

    // check for all excluded and don't backfill if so
    bool all_excluded = true;
    for (auto qid : qid_array) {
      if (qid != (uint16_t)scheduler::MapProgToPhysQubits::GetExcludedValue()) {
        all_excluded = false;
        break;
      }
    }

    if (all_excluded)
      continue;

    if (PointerType *PtrTy = dyn_cast<PointerType>(store_ptr->getType())) {
      // Unsure if this is always global, maybe we need to fix that?
      Type *ValueTy = nullptr;
      if (GlobalValue *GV = dyn_cast<GlobalValue>(store_ptr)) {
        ValueTy = GV->getValueType();
      } else if (AllocaInst *AI = dyn_cast<AllocaInst>(store_ptr)) {
        ValueTy = AI->getAllocatedType();
      } else {
        displayErrorAndExit("Convert Qbit to QID Pass",
                            "Found a strange allocation for a qbit\n");
      }
      if (ValueTy->isArrayTy())
        store_cnst = dyn_cast<Value>(ConstantDataArray::get(C, qid_array));
      else if (ValueTy->isIntegerTy())
        store_cnst = dyn_cast<Value>(
            ConstantInt::get(Type::getInt16Ty(C), qid_array[0]));
      else
        displayErrorAndExit("Convert Qbit to QID Pass",
                            "Found a strange type for a qbit\n");
    } else {
      displayErrorAndExit("Convert Qbit to QID Pass",
                          "Found an improper qbit: must a global variable!\n");
    }

    // get MM basic block terminator to insert before
    BasicBlock *Successor = QBB.getUniqueSuccessor();
    QBBIter QIt(*Successor);
    while (!QIt.isEnd()) {
      Successor = Successor->getUniqueSuccessor();
      QIt = QBBIter(*Successor);
    }
    Instruction *I = &*(Successor->begin());

    // create the StoreInst

    new StoreInst(store_cnst, store_ptr, I);
  };
}

static void convertQbitToQID(Module &M, QuantumModule &QM) {
  LLVMContext &C = M.getContext();

  for (auto &QK : QM) {
    for (auto &QBB : QK) {

      // Get final qubit map for this QBB to verify that the Qbits are QIDs.
      auto qubit_map = QM.final_qubit_maps[&QBB];

      for (QBBIter qit(QBB); !qit.isEnd(); ++qit) {
        std::vector<QbitRef> qubits = qit.getQubitOperands();
        QbitRef q1_copy = qubits[0];
        for (int i = 0; i < qubits.size(); i++) {
          if (!qubits[i].isQID()) {
            qubits[i] = QbitRef(qubit_map->getProgToPhys(qubits[i]), C);
          }
        };
        qit.setQubitOperands(qubits);
      }

      // backfill the QID to the Qbits for this QBB
      backfillQIDToQbitLocal(QBB, QM);
    }
  };

  // finally backfill the Global QID to the Qbits
  backfillQIDToQbitGlobal(M, QM);

  QM.setMapped();
}

namespace {

struct ConvertQbitToQIDLegacyPass : public ModulePass {

public:
  static char ID; // Pass identification, replacement for typeid
  QuantumModule &QM = QuantumCompilerLinkageLegacyPass::QM;

  ConvertQbitToQIDLegacyPass() : ModulePass(ID) {
    initializeConvertQbitToQIDLegacyPassPass(*PassRegistry::getPassRegistry());
  }

  void getAnalysisUsage(AnalysisUsage &AU) const override {

    AU.setPreservesAll();
    // AU.addRequired<QuantumCompilerLinkageWrapperPass>();
    // AU.addRequired<QuantumSpinNativeToJsonWrapperPass>();
  }

  bool runOnModule(Module &M) override {
    convertQbitToQID(M, QM);
    return false;
  } // runOnModule

}; // end of ConvertQbitToQIDPass

} // end of anonymous namespace

char ConvertQbitToQIDLegacyPass::ID = 0;

INITIALIZE_PASS_BEGIN(ConvertQbitToQIDLegacyPass, "convert-qbit-to-qid",
                      "Convert qbit to qid legacy pass", false, false)
INITIALIZE_PASS_END(ConvertQbitToQIDLegacyPass, "convert-qbit-to-qid",
                    "Convert qbit to qid legacy pass", false, false)

PreservedAnalyses ConvertQbitToQIDPass::run(Module &M,
                                            ModuleAnalysisManager &MAM) {
  QuantumModuleProxy &QMP = MAM.getResult<QuantumCompilerLinkageAnalysis>(M);
  convertQbitToQID(M, *QMP.QM);

  return PreservedAnalyses::all();
}

#undef DEBUG_TYPE
