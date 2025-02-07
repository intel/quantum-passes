//===- SpinNativeConvertToIMM.cpp ----------------------------------------====//
//
// Copyright (C) 2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//
//===----------------------------------------------------------------------===//
//
// This file implements a module pass which converts all spin native gates to
// their IMM version.
//
//===----------------------------------------------------------------------===//

#include "IntelQuantumPasses/QuantumCompile/SpinNativeConvertToIMM.h"
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

#define DEBUG_TYPE "convert-qid-to-imm"

// Creates a QSDA if it doesn't exist or returns it if it doesn't
Value *createOrLocateQSDArray(Module &M, QuantumModule &QM, StringRef name,
                              unsigned num) {

  if (QM.QSDA) {
    if (GlobalVariable *qsda = dyn_cast<GlobalVariable>(QM.QSDA)) {
      if (qsda->hasAttribute(QSDA_ATTR))
        return QM.QSDA;
    }
  }

  for (auto &global : M.globals()) {
    if (global.hasAttribute(QSDA_ATTR)) {
      QM.QSDA = dyn_cast<Value>(&global);
      return QM.QSDA;
    }
  }

  QM.QSDA = createNewQSDArray(M, name, num);
  return QM.QSDA;
}

// Returns a new ParaRef as an index value in the QSCbA
ParaRef getAsQSCbAIndex(ParaRef para, BasicBlock *QBB, QuantumModule &QM,
                        bool flip = false, int qid = -1) {

  LLVMContext &C = QBB->getContext();
  Type *IntTy = Type::getInt32Ty(C);

  if (para.isCbitRef()) {

    unsigned &max = QM.QSCbA_used_map[QBB];
    unsigned idx;
    Instruction *I = &*(QBB->getUniqueSuccessor()->begin());

    // create and insert QRTCall
    auto *F = dyn_cast<Function>(QM.QRT_measure_move);
    if (F == nullptr) {
      displayErrorAndExit("Spin Native Angles Convert To IMM Pass",
                          "Not a function.");
    }
    FunctionType *FTy = F->getFunctionType();
    // create a bitcast Instruction
    Value *bitcast = dyn_cast<Value>(new BitCastInst(
        para.getValue(), dyn_cast<Type>(Type::getInt8PtrTy(C)), "QRTcast", I));

    // Check for measurement return method
    if (QM.isMeasurementQueued())
      idx = max;
    else if (QM.isMeasurementMatched())
      idx = qid;
    else {
      displayErrorAndExit(
          "Spin Native Angles Convert To IMM Pass",
          "we don't have a measurement return method for the backend.");
    }

    std::vector<Value *> args = {
        bitcast, dyn_cast<Value>(ConstantInt::get(Type::getInt64Ty(C), idx))};

    if (flip)
      args.push_back(dyn_cast<Value>(ConstantInt::getTrue(Type::getInt1Ty(C))));
    else
      args.push_back(
          dyn_cast<Value>(ConstantInt::getFalse(Type::getInt1Ty(C))));
    CallInst::Create(FTy, QM.QRT_measure_move, args, "", I);

    // Check for attached instrucitons to move. They will be moved to the
    // predecessor by default
    if (Instruction *CbI = dyn_cast<Instruction>(para.getValue())) {
      Instruction *before = QBB->getUniquePredecessor()->getTerminator();
      moveAllAttachedInstOutOfQBB(CbI, before);
    }

    ParaRef out = ParaRef(idx, IntTy);
    if (QM.isMeasurementQueued()) {
      max++;
      if (QM.max_QSCbA_used < max)
        QM.max_QSCbA_used = max;
    }

    return out;

  } else
    displayErrorAndExit("Spin Native Angles Convert To IMM Pass",
                        "called for the wrong indexing function; this one is "
                        "for cbit refs.");

  // should not get here
  return ParaRef();
}

// Returns a new ParaRef as an index value in the QSDA
ParaRef getAsQSDAIndex(QuantumModule &QM, ParaRef para, BasicBlock *QBB) {

  LLVMContext &C = QBB->getContext();
  Type *IntTy = Type::getInt32Ty(C);
  const double TOL = 0.00000001;

  // if the para is a constant, check for reserved positions
  if (para.isResolved() && para.isFloatingPoint()) {

    // Check for a reserved position
    if (std::abs(para.getValueResolved().value() - 0.) < TOL) {
      return ParaRef(0, IntTy);
    } else if (std::abs(para.getValueResolved().value() - FP_PIby2) < TOL) {
      return ParaRef(1, IntTy);
    } else if (std::abs(para.getValueResolved().value() - FP_PI) < TOL) {
      return ParaRef(2, IntTy);
    } else if (std::abs(para.getValueResolved().value() - FP_3PIby2) < TOL) {
      return ParaRef(3, IntTy);
    }
  }

  // otherwise, "create" a new position
  if (para.isFloatingPoint()) {

    unsigned &max = QM.QSDA_used_map[QBB];
    Instruction *I = QBB->getUniquePredecessor()->getTerminator();

    // create and insert StoreInst
    Value *arrayptr = QM.QSDA;
    auto *Ptr = dyn_cast<PointerType>(arrayptr->getType());
    if (Ptr == nullptr) {
      displayErrorAndExit("Spin Native Angles Convert To IMM Pass",
                          "not a pointer.");
    }

    GlobalValue *GV = dyn_cast<GlobalValue>(QM.QSDA);
    if (!GV)
      displayErrorAndExit("Spin Native Angles Convert To IMM Pass",
                          "not a gloabl value.");
    Type *arryty = GV->getValueType();
    SmallVector<Constant *, 2> IdxList = {
        ConstantInt::get(IntegerType::get(C, 64), 0),
        ConstantInt::get(IntegerType::get(C, 64), max)};
    auto gep = ConstantExpr::getInBoundsGetElementPtr(
        arryty, dyn_cast<Constant>(arrayptr), IdxList);
    Instruction *SI = dyn_cast<Instruction>(
        new StoreInst(para.getValue(), dyn_cast<Value>(gep), I));

    // move any attached instructions
    moveAllAttachedInstOutOfQBB(SI, I);
    ParaRef out = ParaRef(max, IntTy);
    max++;
    if (QM.max_QSDA_used < max)
      QM.max_QSDA_used = max;
    return out;

  } else
    displayErrorAndExit(
        "Spin Native Angles Convert To IMM Pass",
        "called for the wrong indexing function; this one is for doubles.");

  // should not get here
  return ParaRef();
}

// this function checks for multiple measurements on the same qubit and
// returns the first qid found to be measured more than once or -1 if none are
// found.
int checkForMultipleMeasuredQIDs(BasicBlock &QBB) {

  std::set<unsigned> qid_measured;

  for (QBBIter qit(QBB); !qit.isEnd(); ++qit) {
    int gate_id = qit.getIdentifier();
    if (gate_id == kSpinMeasZ_IMM_IDX || gate_id == kSpinMeasZ_IMM ||
        gate_id == kSpinMeasZ) {
      unsigned qid = qit.getQubitOperand(0).getIndexResolved().value();
      auto qid_iter = qid_measured.find(qid);
      if (qid_iter != qid_measured.end())
        return (int)qid;
      else
        qid_measured.insert(qid);
    }
  };

  return -1;
}

const std::string qsda_name = "aqcc.qsda";
const unsigned qsda_max = 65536;

static void spinNativeConvertToIMM(Module &M, QuantumModule &QM) {
  Type *IntTy = Type::getInt32Ty(M.getContext());

  // get the QSDA
  createOrLocateQSDArray(M, QM, qsda_name, qsda_max);

  for (auto &QK : QM) {
    for (auto &QBB : QK) {
      QBBIter qit(QBB);
      for (; !qit.isEnd(); ++qit) {

        // set max to reserved value
        QM.QSDA_used_map[&QBB] =
            std::max(QM.QSDA_used_map[&QBB], aqcc::QSDA_RESERVED_POSITIONS);

        unsigned id = qit.getIdentifier();

        switch (id) {
        case kSpinRotXY: {
          std::vector<ParaRef> paras = qit.getParametricOperands();
          if (!paras[0].isResolved() || !paras[1].isResolved()) {

            // we do a new insert instead of changeGate
            std::vector<QbitRef> qubits = qit.getQubitOperands();
            qit.removeGate();
            paras[0] = getAsQSDAIndex(QM, paras[0], &QBB);
            paras[1] = getAsQSDAIndex(QM, paras[1], &QBB);
            qit.insertGate(kSpinRotXY_IMM_IDX, qubits, paras, false);
            break;
          }
        }
        case kSpinCPhase:
        case kSpinSwapalp:
        case kSpinRotZ: {
          std::vector<ParaRef> paras = qit.getParametricOperands();
          if (!paras[0].isResolved()) {

            // we do a new insert instead of changeGate
            std::vector<QbitRef> qubits = qit.getQubitOperands();
            qit.removeGate();
            paras[0] = getAsQSDAIndex(QM, paras[0], &QBB);
            qit.insertGate(getIMM_IDXVersionForSpinNative(id), qubits, paras,
                           false);
            break;
          }
        }
        case kSpinMeasZ: {
          std::vector<ParaRef> paras = qit.getParametricOperands();
          if (paras[0].isCbitRef()) {
            bool flip = (bool)paras[1].getValueResolved().value();

            std::vector<QbitRef> qubits = qit.getQubitOperands();
            // assumes this is a QID at this point
            unsigned qid = qubits[0].getIndexResolved().value();
            std::vector<ParaRef> Nparas(2);
            Nparas[0] = getAsQSCbAIndex(paras[0], &QBB, QM, flip, qid);
            Nparas[1] = paras[2]; // slice
            // we do an insert instead of a changGate
            qit.removeGate();
            qit.insertGate(kSpinMeasZ_IMM_IDX, qubits, Nparas, false);
            break;
          }
        }
        case kSpinPrepZ:
          qit.changeGate(getIMMVersionForSpinNative(id));
          break;

        case kSpinRotZ_IMM:
        case kSpinRotXY_IMM:
        case kSpinCPhase_IMM:
        case kSpinSwapalp_IMM:
        case kSpinMeasZ_IMM:
        case kSpinPrepZ_IMM:
        case kSpinRotZ_IMM_IDX:
        case kSpinRotXY_IMM_IDX:
        case kSpinCPhase_IMM_IDX:
        case kSpinSwapalp_IMM_IDX:
        case kSpinMeasZ_IMM_IDX:
          break;

        default:
          displayErrorAndExit(
              "Spin Native Angles Convert To IMM Pass",
              "found a non-native quantum gate: " + std::to_string(id) +
                  "in function '" +
                  DemangleQuantumKernelName(QK.getFunction()->getName()) +
                  "'.");
        }
      };

      if (QM.isMeasurementMatched()) {
        int maybe_qid = checkForMultipleMeasuredQIDs(QBB);
        if (maybe_qid > -1)
          displayErrorAndExit("Spin Native Angles Convert To IMM Pass",
                              "Cannot measure a qubit more than once with "
                              "the current measurement return method: " +
                                  std::to_string(maybe_qid));
      }
      qit.updateGateDependencies();
    };
  };
}

namespace {

struct SpinNativeConvertToIMMLegacyPass : public ModulePass {

public:
  static char ID; // Pass identification, replacement for typeid
  QuantumModule &QM = QuantumCompilerLinkageLegacyPass::QM;

  const std::string qsda_name = "aqcc.qsda";
  const unsigned qsda_max = 65536;

  SpinNativeConvertToIMMLegacyPass() : ModulePass(ID) {
    initializeSpinNativeConvertToIMMLegacyPassPass(
        *PassRegistry::getPassRegistry());
  }

  void getAnalysisUsage(AnalysisUsage &AU) const override {

    AU.setPreservesAll();
    // AU.addRequired<QuantumSpinNativeToJsonWrapperPass>();
  }

  bool runOnModule(Module &M) override {
    spinNativeConvertToIMM(M, QM);

    return false;
  } // runOnModule

}; // end of ConvertQIDToIMMPass

} // end of anonymous namespace

char SpinNativeConvertToIMMLegacyPass::ID = 0;

INITIALIZE_PASS_BEGIN(SpinNativeConvertToIMMLegacyPass, "spin-convert-to-imm",
                      "Spin native convert to imm legacy pass", false, false)
INITIALIZE_PASS_END(SpinNativeConvertToIMMLegacyPass, "spin-convert-to-imm",
                    "Spin native convert to imm legacy pass", false, false)

PreservedAnalyses SpinNativeConvertToIMMPass::run(Module &M,
                                                  ModuleAnalysisManager &MAM) {
  QuantumModuleProxy &QMP = MAM.getResult<QuantumCompilerLinkageAnalysis>(M);
  spinNativeConvertToIMM(M, *QMP.QM);

  return PreservedAnalyses::all();
}

#undef DEBUG_TYPE
