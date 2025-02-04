//===- QuantumCompilerLinkagePass.cpp -------------------------------------===//
//
// Copyright (C) 2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//
//===----------------------------------------------------------------------===//
/// \file "QuantumCompilerLinkagePass.cpp"
///
/// This file implements the methods from "QuantumCompilerLinkagePass.h"
///
//===----------------------------------------------------------------------===//

#include "IntelQuantumPasses/QuantumAnalysis/QuantumCompilerLinkage.h"
#include "IntelQuantumPasses/QuantumAnalysis/QuantumAnnotationsToJson.h"
#include "IntelQuantumPasses/QuantumAnalysis/QuantumGateIdentifiers.h"
#include "IntelQuantumPasses/QuantumAnalysis/QuantumIterator.h"
#include "IntelQuantumPasses/QuantumAnalysis/QuantumSpinNativeToJson.h"
#include "IntelQuantumPasses/QuantumUtils/QuantumGeneralUtils.h"

#define DEBUG_TYPE "quantum-compiler-linkage"

using namespace llvm;
using namespace aqcc;

static cl::opt<std::string>
    Config("q-config",
           cl::desc("filename for the Quantum SDK JSON configuration file."));

static cl::opt<bool> RetMethod("queue-measurements", cl::Hidden);

static cl::opt<std::string>
    SDKPath("q-sdk",
            cl::desc("SDK path for the location of the driver script."));

static cl::opt<bool>
    FLEQ("fleq-enabled",
         cl::desc("flag to indicate to the rest of the compiler that FLEQ "
                  "compilation should be used."),
         cl::init(true));

namespace llvm {

char QuantumCompilerLinkageLegacyPass::ID = 0;
char QuantumCompilerLinkageWrapperPass::ID = 0;

QuantumModule QuantumCompilerLinkageLegacyPass::QM;

void QuantumCompilerLinkage::doInitialization(Module &M, QuantumModule &QM) {

  // populate argument type lists for QRT

  LLVMContext &C = M.getContext();
  QRT_initialize_args = {/*ptr*/ PointerType::getInt8PtrTy(C),
                         /*ptr*/ PointerType::getInt8PtrTy(C),
                         /*ptr*/ PointerType::getInt8PtrTy(C)};
  QRT_measure_move_args = {/*ptr*/ Type::getInt8PtrTy(C), Type::getInt64Ty(C),
                           Type::getInt1Ty(C)};
  QRT_exit_args = {/*ptr*/ PointerType::getInt8PtrTy(C)};
  Type *FTy = dyn_cast<Type>(FunctionType::get(Type::getVoidTy(C), false));
  if (!FTy)
    return;
  Type *FPtrTy = dyn_cast<Type>(PointerType::getUnqual(FTy));
  QRT_run_qbb_args = {/* void*() */ FPtrTy, Type::getInt32Ty(C)};
  QRTErrTy = Type::getInt32Ty(C);

  // initialize QM
  QM.base = &M;
  ParaRef::setModule(M);
  QIter::setQuantumModule(&QM);
  if (QM.sdk_json_filename == "")
    QM.sdk_json_filename = Config;
  if (QM.sdk_path == "")
    QM.sdk_path = SDKPath;
  QM.is_meas_queued = RetMethod;
  if (QM.sdk_json_filename[0] == '=')
    QM.sdk_json_filename.erase(0, 1);

  QM.has_fleq = FLEQ;

  QM.initializeData(M);

  if (QM.getErrorString() != "")
    displayErrorAndExit("QuantumCompilerLinkagePass", QM.getErrorString());
}

void QuantumCompilerLinkage::locateOrDeclareQRTFunctions(Module &M,
                                                         QuantumModule &QM) {

  std::string QRT_missing;
  QRT_missing.clear();

  FunctionCallee func;
  LLVMContext &C = M.getContext();

  func = M.getOrInsertFunction(QRT_initialize_name, QRTErrTy,
                               QRT_initialize_args[0], QRT_initialize_args[1],
                               QRT_initialize_args[2]);
  QM.QRT_initialize = dyn_cast<Function>(func.getCallee());
  if (!QM.QRT_initialize)
    QRT_missing += QRT_initialize_name + "\n";

  func = M.getOrInsertFunction(QRT_is_initialized_name, Type::getInt1Ty(C));
  QM.QRT_is_initialized = dyn_cast<Function>(func.getCallee());
  if (!QM.QRT_is_initialized)
    QRT_missing += QRT_is_initialized_name + "\n";

  func = M.getOrInsertFunction(
      QRT_measure_move_name, Type::getVoidTy(C), QRT_measure_move_args[0],
      QRT_measure_move_args[1], QRT_measure_move_args[2]);
  QM.QRT_measure_move = dyn_cast<Function>(func.getCallee());
  if (!QM.QRT_measure_move)
    QRT_missing += QRT_measure_move_name + "\n";

  func = M.getOrInsertFunction(QRT_run_qbb_name, QRTErrTy, QRT_run_qbb_args[0],
                               QRT_run_qbb_args[1]);
  QM.QRT_run_qbb = dyn_cast<Function>(func.getCallee());
  if (!QM.QRT_run_qbb)
    QRT_missing += QRT_run_qbb_name + "\n";

  func = M.getOrInsertFunction(QRT_dismantle_name, QRTErrTy);
  QM.QRT_dismantle = dyn_cast<Function>(func.getCallee());
  if (!QM.QRT_dismantle)
    QRT_missing += QRT_dismantle_name + "\n";

  func = M.getOrInsertFunction(QRT_exit_name, Type::getVoidTy(C),
                               QRT_exit_args[0]);
  QM.QRT_exit = dyn_cast<Function>(func.getCallee());
  if (!QM.QRT_exit)
    QRT_missing += QRT_exit_name + "\n";

  if (!QRT_missing.empty())
    displayErrorAndExit("QuantumCompilerLinkagePass",
                        "QRT library functions not found:\n" + QRT_missing);
}

void QuantumCompilerLinkage::checkForConditioning(Module &M,
                                                  QuantumModule &QM) {

  if (QM.isConditioned())
    return;

  if (QM.q_kernel_list.size() == 0)
    QM.setConditioned();

  // ASSUMPTION: if there are QBBs, then the module is conditioned
  for (auto &QK : QM) {
    if (QK.begin() != QK.end())
      QM.setConditioned();
  }
}

void QuantumCompilerLinkage::checkForNative(Module &M, QuantumModule &QM) {

  std::map<std::string, json::Object> *q_GATEMETADATA_ = &QM.q_gate_metadata;

  // We can't check this until quantum annotations to json is run.
  if (!q_GATEMETADATA_ || q_GATEMETADATA_->size() == 0)
    return;

  if (QM.isNative())
    return;

  if (QM.q_kernel_list.size() == 0)
    QM.setNative();

  // Look for canonical gates in the circuit.
  bool foundAny = false;
  bool foundCanonical = false;
  for (auto &QK : QM) {
    for (auto &QBB : QK) {
      aqcc::QBBIter Qit(QBB);

      while (!Qit.isEnd()) {
        int id = Qit.getIdentifier();
        ++Qit;
        foundAny = true;
        if (id <= kStartPulse) {
          foundCanonical = true;
        }
      }
    }
  }

  // We we didn't find canonical gates, then
  // set that we only have native gates remaining.
  if (foundAny && !foundCanonical)
    QM.setNative();
}

void QuantumCompilerLinkage::checkForIMM(Module &M, QuantumModule &QM) {

  std::map<std::string, json::Object> *q_GATEMETADATA_ = &QM.q_gate_metadata;

  // We can't check this until quantum annotations to json is run.
  if (!q_GATEMETADATA_ || q_GATEMETADATA_->size() == 0)
    return;

  if (QM.isAllIMM())
    return;

  if (QM.q_kernel_list.size() == 0)
    QM.setAllIMM();

  bool foundAny = false;
  bool foundNative = false;
  bool foundCanonical = false;
  // Iterate over the circuit to see if any canonical or native gates remain.
  for (auto &QK : QM) {
    for (auto &QBB : QK) {
      aqcc::QBBIter Qit(QBB);

      while (!Qit.isEnd()) {
        int id = Qit.getIdentifier();
        ++Qit;

        foundAny = true;
        if (id <= kSWAPA) {
          foundCanonical = true;
        } else if (id > kSWAPA && id < kSpinEnd) {
          foundNative = true;
        }
      }
    }
  }

  // If we get to this point it means that all the gates are in their
  // immediate form.
  if (foundAny && !foundCanonical && !foundNative)
    QM.setAllIMM();
}

void QuantumCompilerLinkage::checkForMapped(Module &M, QuantumModule &QM) {

  std::map<std::string, json::Object> *q_GATEMETADATA_ = &QM.q_gate_metadata;

  // We can't check this until quantum annotations to json is run.
  if (!q_GATEMETADATA_ || q_GATEMETADATA_->size() == 0)
    return;

  if (QM.isMapped())
    return;

  if (QM.q_kernel_list.size() == 0)
    QM.setMapped();

  // Iterate over all instructions and check if all the references
  // to qubits are QIDs instead.  If one is found, we know that it
  // is not mapped.
  bool foundQubit = false;
  bool foundAny = false;

  for (auto &QK : QM) {
    for (auto &QBB : QK) {

      for (QBBIter qit(QBB); !qit.isEnd(); ++qit) {
        foundAny = true;
        std::vector<QbitRef> qubits = qit.getQubitOperands();
        QbitRef q1_copy = qubits[0];
        for (int i = 0; i < qubits.size(); i++) {
          if (!qubits[i].isQID()) {
            foundQubit = true;
          }
        };
      }
    }
  };

  if (foundAny && !foundQubit)
    QM.setMapped();
}

void QuantumCompilerLinkage::checkForInRange(Module &M, QuantumModule &QM) {

  std::map<std::string, json::Object> *q_GATEMETADATA_ = &QM.q_gate_metadata;

  // We can't check this until quantum annotations to json is run.
  if (!q_GATEMETADATA_ || q_GATEMETADATA_->size() == 0)
    return;

  if (QM.isAnglesInRange() || !QM.isMapped())
    return;

  if (QM.q_kernel_list.size() == 0)
    QM.setAnglesInRange();

  bool foundAny = false;
  bool foundOutOfRange = false;

  for (auto &QK : QM) {
    for (auto &QBB : QK) {

      for (QBBIter qit(QBB); !qit.isEnd(); ++qit) {
        foundAny = true;
        // kSpinBegin is max non-native identifier value
        if (qit.getIdentifier() >= kSpinBegin) {

          std::vector<ParaRef> angles = qit.getParametricOperands();

          if (qit.getIdentifier() == kSpinRotXY) {
            if (angles[0].isResolved() && angles[1].isResolved()) {
              if ((angles[0].getValueResolved().value() >= FP_PI ||
                   angles[0].getValueResolved().value() < 0) ||
                  (angles[1].getValueResolved().value() >= FP_2PI ||
                   angles[1].getValueResolved().value() < 0)) {
                foundOutOfRange = true;
              }
            } else
              foundOutOfRange = true;
            continue;
          }

          for (int i = 0; i < angles.size(); i++) {

            if (angles[i].isResolved()) {
              if (angles[i].isFloatingPoint()) { // only need for FP angles
                if (angles[i].getValueResolved().value() > FP_2PI)
                  foundOutOfRange = true;
              }
            } else
              foundOutOfRange = true;
          };
        }
      } // for qit
    }   // for QBBs
  }

  if (foundAny && !foundOutOfRange)
    QM.setAnglesInRange();
}

void QuantumCompilerLinkage::checkForSeparation(Module &M, QuantumModule &QM) {

  // first check to see if is_separated is true

  if (QM.isSimpleQBBSplit())
    return;

  if (QM.q_kernel_list.size() == 0)
    QM.setSimpleQBBSplit();

  // ASSUMPTION: This function will assume that if QRT run_qbb function has any
  // uses, then the simple QBBs have been split.

  if (QM.QRT_run_qbb->user_empty())
    return;

  for (auto user : QM.QRT_run_qbb->users()) {

    if (CallInst *CI = dyn_cast<CallInst>(user)) {

      // add this entry to the QBB link map
      if (Function *F_qbb = dyn_cast<Function>(CI->getArgOperand(0))) {
        QM.QBB_link_map[F_qbb] = CI->getParent();
        QM.setSimpleQBBSplit();
        if (F_qbb->isDeclaration())
          QM.setModSplit();
      } else {
        displayErrorAndExit("QuantumCompilerLinkagePass",
                            "found a QRT call with unanticipated signature.");
      }

      // Now check if the containing function has an entry in the QSDA_used_map
      BasicBlock *BB = CI->getParent();
      if (QM.QSDA_used_map.find(BB) == QM.QSDA_used_map.end()) {
        QM.QSDA_used_map[BB] = aqcc::QSDA_RESERVED_POSITIONS;
      }
    } else {
      displayErrorAndExit(
          "QuantumCompilerLinkagePass",
          "found a QRT library use that was not a call to the function.");
    }
  };
}

/*
Deprecated method for QRT linking Keeping incase this method ever comes back
void QuantumCompilerLinkage::findQBBBinaries(Module &M){
  for(auto &global : M.globals()){
    if(global.getSection() == aqcc::QBBS_TEXT_SECTION)
      module_linkage.QBBS_TEXT = dyn_cast<Value>(&global);
    else if(global.getSection() == aqcc::QBBS_SECTION)
      module_linkage.QBBS = dyn_cast<Value>(&global);
    if(module_linkage.QBBS_TEXT && module_linkage.QBBS) break;
  };
}
*/

void QuantumCompilerLinkage::findMaxQSDAUsage(Module &M, QuantumModule &QM) {

  // get the QSDA
  for (auto &global : M.globals()) {
    if (global.hasAttribute(aqcc::QSDA_ATTR))
      QM.QSDA = dyn_cast<Value>(&global);
  }

  // If no QSDA is found, no need to search.
  if (!QM.QSDA)
    return;

  LLVMContext &C = M.getContext();
  Constant *gep;
  PointerType *QSDATy = dyn_cast<PointerType>(QM.QSDA->getType());
  if (!QSDATy)
    return;
  GlobalValue *GV = dyn_cast<GlobalValue>(QM.QSDA);
  if (!GV)
    return;
  Type *arryty = GV->getValueType();

  // to do the search, we use the constexp GEPs starting at QSDA_max_used
  do {
    SmallVector<Constant *, 2> IdxList = {
        ConstantInt::get(IntegerType::get(C, 64), 0),
        ConstantInt::get(IntegerType::get(C, 64), QM.max_QSDA_used)};
    gep = ConstantExpr::getInBoundsGetElementPtr(
        arryty, dyn_cast<Constant>(QM.QSDA), IdxList);

    QM.max_QSDA_used++;

    for (auto user : gep->users()) {
      if (StoreInst *SI = dyn_cast<StoreInst>(user)) {

        BasicBlock *key = SI->getParent()->getUniqueSuccessor();
        QM.QSDA_used_map[key] = QM.max_QSDA_used;
      }
    };

  } while (gep->hasNUsesOrMore(1));
  // have to decrement because last do loop should have been on an unused value
  QM.max_QSDA_used--;
}

void updateCompilerLinkage(Module &M, QuantumModule &QM, bool start_fresh) {
  // We just run through the different checks here. start_fresh is not currently
  // used, but it may be necessary in the future.
  QuantumCompilerLinkage QCL;
  QCL.doInitialization(M, QM);
  QCL.locateOrDeclareQRTFunctions(M, QM);
  QM.setSure();
  QCL.checkForConditioning(M, QM);
  QCL.checkForNative(M, QM);
  QCL.checkForMapped(M, QM);
  QCL.checkForInRange(M, QM);
  QCL.checkForIMM(M, QM);
  QCL.checkForSeparation(M, QM);
  QCL.findMaxQSDAUsage(M, QM);
}

} // namespace llvm

AnalysisKey QuantumCompilerLinkageAnalysis::Key;
QuantumModuleProxy
llvm::QuantumCompilerLinkageAnalysis::run(Module &M,
                                          ModuleAnalysisManager &AM) {
  QuantumCompilerLinkage QCL;
  // QuantumModule QM = new QuantumModule;
  // updateCompilerLinkage(M, *QM);
  auto QMP = QuantumModuleProxy();
  QuantumModule *TempQM = new QuantumModule;
  if (QMP.QM)
    TempQM->q_gate_metadata = QMP.QM->q_gate_metadata;
  QMP.QM.reset(TempQM);
  QuantumAnnotationsToJson QAJ;
  QuantumSpinNativeToJson QSNJ;
  QAJ.doInitialization(M, *QMP.QM);
  QSNJ.addSpinNativeFunctions(M, *QMP.QM);
  updateCompilerLinkage(M, *QMP.QM);
  return QMP;
}

ModulePass *llvm::createQuantumCompilerLinkageLegacyPass() {
  return new QuantumCompilerLinkageLegacyPass();
}

INITIALIZE_PASS_BEGIN(QuantumCompilerLinkageLegacyPass, "q-compiler-linkage",
                      "finds QRT", true, true)
INITIALIZE_PASS_END(QuantumCompilerLinkageLegacyPass, "q-compiler-linkage",
                    "finds QRT", true, true)

ModulePass *llvm::createQuantumCompilerLinkageWrapperPass() {
  return new QuantumCompilerLinkageWrapperPass();
}

INITIALIZE_PASS_BEGIN(QuantumCompilerLinkageWrapperPass,
                      "q-compiler-linkage-wrap", "finds QRT wrap", true, true)
INITIALIZE_PASS_END(QuantumCompilerLinkageWrapperPass,
                    "q-compiler-linkage-wrap", "finds QRT wrap", true, true)

#undef DEBUG_TYPE
