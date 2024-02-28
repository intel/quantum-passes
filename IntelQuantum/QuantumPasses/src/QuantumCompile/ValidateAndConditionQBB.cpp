//===- ValidateAndConditionQBB.cpp ---------------------------------------====//
//
// Copyright (C) 2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//
//===----------------------------------------------------------------------===//
//
// This file implements a function pass which checks for valid hybrid QBBs, and
// condition the hybrid QBBs with the sequential gates and measurement move
// BBs.
//
// In the first version of this pass, valid hybrid QBBs have the following
// properties:
//      1) All qubit gate operands are fully resolved.
//      2) The strongly connected components (SCC) of the BB control flow
//         form an unbranching tree (a line basically)
//      3) if an SCC contains a loop, all BBs in the SCC contain no quantum
//         instructions.
//
//  These conditions may change in later iterations of the pass.
//===----------------------------------------------------------------------===//
#include "IntelQuantumPasses/QuantumCompile/ValidateAndConditionQBB.h"
#include "IntelQuantumPasses/QuantumAnalysis/QuantumIterator.h"

#define DEBUG_TYPE "validate-and-condition-qbb"

using namespace llvm;
using namespace aqcc;

void ValidateAndConditionQBB::locateMeasurementControlInst(Module &M) {

  // start by locating release calls
  int IntrID =
      Function::lookupIntrinsicID("llvm.quantum.release.quantum.state");
  Function *release = Intrinsic::getDeclaration(&M, IntrID);
  if (release) {
    meas_ctrl_functs.push_back(release);

    // Move through the user list to add the the release_calls map
    for (auto user : release->users()) {
      if (CallBase *CB = dyn_cast<CallBase>(user)) {
        Function *parent_parent = CB->getParent()->getParent();
        if (release_calls.find(parent_parent) == release_calls.end()) {
          release_calls[parent_parent] = dyn_cast<Instruction>(CB);
        } else
          displayErrorAndExit(
              "ValidateAndConditionQBBPass",
              "cannot release the quantum system more than once in '" +
                  DemangleQuantumKernelName(parent_parent->getName()) + "'.");
      }
    }
  }
}

void ValidateAndConditionQBB::conditionIntoQBBs(Function &F,
                                                std::vector<QBBIter> &F_gates) {

  if (F_gates.empty())
    return;

  std::string func_name = F.getName().str();
  bool signal_gate_removal = false;
  // conditioning starts by creating the quantum simple QBB and
  // meas_move BB. This is done using the splitBasicBlock
  // functionality on the first block in the list, just after
  // the last quantum gate.

  // NOTE: We currently assume a single QBB and F_gates is in reverse order

  BasicBlock *endBB = F_gates[0].getBasicBlock();
  size_t start = 0;
  Instruction *split_inst;
  std::string qbb_twine = QBB_TWINE;

  F_gates[0].gotoBegin();

  // first, check for a release call
  auto rel_pair = release_calls.find(&F);

  if (rel_pair != release_calls.end()) {
    qbb_twine = QBB_TWINE_RELEASE;
    BasicBlock *release_BB = rel_pair->second->getParent();
    // Iterate through F_gates to find the BB of the release call
    for (size_t i = 0; i < F_gates.size(); i++) {
      QBBIter &qit = F_gates[i];
      qit.gotoBegin();
      if (qit.getBasicBlock() == release_BB) {
        endBB = release_BB;
        start = i;
        // remove any remaining gates after the release call
        inst_iterator inst_after = inst_at(rel_pair->second->eraseFromParent());
        for (QBBIter to_rm(*release_BB, inst_after); !to_rm.isEnd();) {
          to_rm.removeGate();
          signal_gate_removal = true;
        }
        // rebuild QBBIter to fix range
        F_gates[i] = QBBIter(*release_BB);
        break;
      }
      if (!qit.isEnd()) {
        for (; !qit.isEnd(); ++qit)
          qit.removeGate();
        signal_gate_removal = true;
      }
    }
  }

  // now set the split instruction
  if (F_gates[start].isEnd()) {
    // If there are not gates in this block, insert at begining of the block
    auto iter = endBB->begin();
    // split after Phis and Allocs
    while (isa<PHINode>(&*iter) || isa<AllocaInst>(&*iter))
      iter++;
    split_inst = &*iter;
  } else {
    // Split off the instruction immediately after the last gate.
    // This should also be guarenteed to not be the end because there
    // should always be a terminator.
    F_gates[start].gotoEnd();
    --F_gates[start];
    inst_iterator temp = F_gates[start].getGateInstIterator();
    ++temp;
    split_inst = &*temp;
    F_gates[start].gotoBegin();
  }

  // Now split twice
  // first split the simple QBB
  BasicBlock *QBB = endBB->splitBasicBlock(split_inst, qbb_twine);
  // next, split the meas move section
  BasicBlock *postQBB = QBB->splitBasicBlock(split_inst, aqcc::MEAS_MOVE_TWINE);

  // TODO: if the measurement control instruction calls has backend QuISA
  // support,
  // we will need to move it to the QBB, but for now we remove it later.

  // create a QBBIter on the newly made QBB and also reconstruct first QBBIter
  // in F_gates as its basic block has been altered and beg/end may have been
  // affected.
  F_gates[start] = QBBIter(*endBB);
  QBBIter gate_new(*QBB);

  // Now move gates out of other BB and into the new "QBB" block

  for (size_t i = start; i < F_gates.size(); i++) {
    QBBIter &cur = F_gates[i];

    while (!cur.isEnd()) {
      // recall QIter gates are inserted before position pointed to,
      // so we insert last to first.

      // go to last gate
      cur.gotoEnd();
      --cur;

      // get gate data
      unsigned gate_id = cur.getIdentifier();
      std::vector<QbitRef> qubits = cur.getQubitOperands();
      std::vector<ParaRef> paras = cur.getParametricOperands();

      // recall gates are inserted before position pointed to,
      // so we insert last to first.
      cur.removeGate();
      gate_new.insertGate(gate_id, qubits, paras);

      // moving between begin and end is cheap so
      // loop only exits if begin == end
      cur.gotoBegin();
    }
  };

  // If gates were removed due to a release call, issue warning
  // so that users are aware.

  if (signal_gate_removal)
    errs() << "WARNING: Quantum SDK - Some gates were removed in '"
           << DemangleQuantumKernelName(F.getName())
           << "' as they were found after a call to " << RELEASE_ALL_FRONT
           << ".\nIf this was a mistake, please review source code or report "
              "as a bug.\n";
}

void ValidateAndConditionQBB::cleanUpMeasurementControlInst(Module &M) {

  // remove instrinsics
  for (auto F : meas_ctrl_functs) {
    F->dropAllReferences();
    F->removeFromParent();
  }
}

bool ValidateAndConditionQBB::conditionQBB(Module &M, QuantumModule &QM) {

  std::set<Function *> to_rm;
  locateMeasurementControlInst(M);

  for (auto &QK : QM) {

    std::vector<QBBIter> F_gates;
    Function *F = QK.getFunction();

    switch (validateQuantumKernel(QK, F_gates)) {
    case 2: { // release QBB
      conditionIntoQBBs(QK, F_gates);
      break;
    }
    case 1: { // hold QBB
      conditionIntoQBBs(QK, F_gates);
      break;
    }
    case 0: {
      // Do nothing
      break;
    }
    case -1: {
      if (F->user_empty()) {
        to_rm.insert(F);
      } else
        displayErrorAndExit(
            "ValidateAndConditionQBBPass",
            "Invalid loops stuck around in '" +
                DemangleQuantumKernelName(QK.getFunction()->getName()) + "'.");
      break;
    }
    case -2: {
      if (F->user_empty()) {
        to_rm.insert(F);
      } else
        displayErrorAndExit(
            "ValidateAndConditionQBBPass",
            "Invalid branching stuck around in '" +
                DemangleQuantumKernelName(QK.getFunction()->getName()) + "'.");
      break;
    }
    case -3: {
      if (F->user_empty()) {
        to_rm.insert(F);
      } else
        displayErrorAndExit(
            "ValidateAndConditionQBBPass",
            "Unresolved qubit arguments stuck around in '" +
                DemangleQuantumKernelName(QK.getFunction()->getName()) + "'.");
      break;
    }
    case -4: {
      if (F->user_empty()) {
        to_rm.insert(F);
      } else {
        displayErrorAndExit(
            "ValidateAndConditionQBBPass",
            "Found a multi-qubit gate with non-unique qubit arguments in '" +
                DemangleQuantumKernelName(QK.getFunction()->getName()) + "'.");
      }
      break;
    }
    }
  }

  // TODO: remove or refine if QuISA backend has support for measurement control
  // inst
  cleanUpMeasurementControlInst(M);

  // remove QKs from both the Module and the QM
  // we do this by reconstructing the q_kernel_list
  std::vector<QuantumKernel> new_qk_list;
  auto rm_end = to_rm.end();

  for (auto &QK : QM) {

    Function *F = QK.getFunction();

    if (to_rm.find(F) != rm_end) {
      F->clearMetadata();
      F->dropAllReferences();
      F->replaceAllUsesWith(UndefValue::get(F->getType()));
      F->removeFromParent();
    } else
      new_qk_list.push_back(QK);
  }

  std::swap(new_qk_list, QM.q_kernel_list);

  QM.setConditioned();

  return false;
}

PreservedAnalyses ValidateAndConditionQBBPass::run(Module &M,
                                                   ModuleAnalysisManager &MAM) {
  QuantumModuleProxy QMP = MAM.getResult<QuantumCompilerLinkageAnalysis>(M);
  QBB.conditionQBB(M, *QMP.QM);

  return PreservedAnalyses::all();
}

bool ValidateAndConditionQBBLegacyPass::runOnModule(Module &M) {
  QBB.conditionQBB(M, QM);
  return false;
}

char ValidateAndConditionQBBLegacyPass::ID = 0;

INITIALIZE_PASS_BEGIN(ValidateAndConditionQBBLegacyPass,
                      "validate-and-condition-qbb",
                      "Validate and condition QBB legacy pass", false, false)
INITIALIZE_PASS_END(ValidateAndConditionQBBLegacyPass,
                    "validate-and-condition-qbb",
                    "Validate and condition QBB legacy pass", false, false)

#undef DEBUG_TYPE
