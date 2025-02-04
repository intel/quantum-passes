//===- QuantumIterator.cpp ------------------------------------------------===//
//
// Copyright (C) 2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//
//===----------------------------------------------------------------------===//

#include "IntelQuantumPasses/QuantumAnalysis/QuantumIterator.h"
#include "IntelQuantumPasses/QuantumAnalysis/QuantumAnnotationGetters.h"
#include "IntelQuantumPasses/QuantumAnalysis/QuantumAnnotationsToJson.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/IntrinsicsFLEQ.h"
#include "llvm/IR/PassManager.h"
#include "llvm/Pass.h"

#include <cstddef>
#include <set>

#define DEBUG_TYPE "quantum-iterator"

using namespace llvm;

namespace llvm {
namespace aqcc {

QuantumModule *QIter::QM = nullptr;

std::map<Function *, qiter_range> &QIter::getInstance_q_RANGE_MAP_() {
  static std::map<Function *, qiter_range> q_RANGE_MAP_ =
      std::map<Function *, qiter_range>();
  return q_RANGE_MAP_;
}

std::map<BasicBlock *, qiter_range> &QBBIter::getInstance_q_BB_RANGE_MAP_() {
  static std::map<BasicBlock *, qiter_range> q_BB_RANGE_MAP_ =
      std::map<BasicBlock *, qiter_range>();
  return q_BB_RANGE_MAP_;
}

void QIter::setQuantumModule(QuantumModule *NewQM) { QM = NewQM; }

QuantumModule *QIter::getQuantumModule() { return QM; }

////////////////////////////////////////////////////////////////////////////////
// Constructor

QIter::QIter(Function &F) : FUNCT_(&F) {

  // Set Qmetadata
  // QIter assumes that the QuantumAnnotationsToJsonPass has been run

  q_GATEMETADATA_ = &(QM->q_gate_metadata);

  if (q_GATEMETADATA_->size() == 0) {
    LLVM_DEBUG(
        dbgs() << "ERROR: Quantum SDK - class QIter says: Quantum gate "
                  "metadata does not appear to be populated with gates.\n");
  }

  if (F.empty() && (!F.isIntrinsic()) && (!F.isDeclaration())) {
    std::string ErrMsg = "Non-intrinsic function is empty.\n";
    displayErrorAndExit("QIter", ErrMsg);
  }

  // iterate through to set beginning and end gate call
  Instruction *begin = &*inst_begin(*FUNCT_);
  BBBegin = begin;
  inst_iterator end_iter = inst_end(*FUNCT_);
  end_iter--;
  // use last terminator as end;
  Instruction *end = &(*(end_iter));

  // initialize range map so it's there for is_qgate()
  getInstance_q_RANGE_MAP_()[FUNCT_] = qiter_range(begin, end);

  q_iterator_ = begin;

  while (!is_qgate() && q_iterator_ != end)
    advance_q_iterator_();
  begin = q_iterator_;
  end = q_iterator_;

  while (q_iterator_ != q_END()) {
    if (is_qgate())
      end = q_iterator_;
    advance_q_iterator_();
  };

  if (end != q_END())
    end = end->getNextNode();

  // Add or uppdate range to the q_RANGE_MAP_
  getInstance_q_RANGE_MAP_()[FUNCT_] = qiter_range(begin, end);

  q_iterator_ = q_BEGIN();
  is_qgate(); // sets cur_gate_data_

  for (Instruction *it = inst_bb_begin(q_iterator_->getParent());
       it != q_iterator_; it = it->getNextNode()) {
    if ((dyn_cast<AllocaInst>(it)))
      RelevantBeginningOps.insert(it);
    else if ((dyn_cast<StoreInst>(it)))
      RelevantBeginningOps.insert(it);
    else if ((dyn_cast<PHINode>(it)))
      RelevantBeginningOps.insert(it);
  }
}

QIter::QIter(Function &F, Instruction *it) : QIter(F) {

  // Set q_iterator_ to it or next nearest gate

  // if it is beyond q_END(), we set it to q_END()
  if (std::distance(it->getIterator(), q_END()->getIterator()) < 0)
    gotoEnd();
  else {
    q_iterator_ = it;
    while (!is_qgate() && q_iterator_ != q_END())
      advance_q_iterator_();
  }
}

QBBIter::QBBIter(BasicBlock &B) : BB_(&B) {

  FUNCT_ = BB_->getParent();

  // Set Qmetadata
  // QBBIter assumes that the QuantumAnnotationsToJsonPass has been run

  q_GATEMETADATA_ = &(QM->q_gate_metadata);

  if (q_GATEMETADATA_->size() == 0) {
    LLVM_DEBUG(
        dbgs() << "ERROR: Quantum SDK - class QBBIter says: Quantum gate "
                  "metadata does not appear to be populated with gates.\n");
  }

  if (FUNCT_->empty() && (!FUNCT_->isIntrinsic()) &&
      (!FUNCT_->isDeclaration())) {
    std::string ErrMsg = "Non-intrinsic function is empty.\n";
    displayErrorAndExit("QuantumIterator", ErrMsg);
  }

  // iterate through to set beginning and end gate call
  Instruction *begin = inst_bb_begin(BB_);
  BBBegin = begin;
  Instruction *end = inst_bb_end(BB_);
  // setting end to always be the terminator of the BB
  if (&*end != BB_->getTerminator()) {
    std::string ErrMsg =
        "End of BasicBlock is not the terminator instruction.\n";
    displayErrorAndExit("QuantumIterator", ErrMsg);
  }

  // initialize range map so it's there for is_qgate()
  getInstance_q_BB_RANGE_MAP_()[BB_] = qiter_range(begin, end);

  q_iterator_ = begin;

  while (!is_qgate() && q_iterator_ != end)
    advance_q_iterator_();
  begin = q_iterator_;

  // Add or uppdate range to the q_BB_RANGE_MAP_
  getInstance_q_BB_RANGE_MAP_()[BB_] = qiter_range(begin, end);

  q_iterator_ = q_BEGIN();
  is_qgate(); // sets cur_gate_data_

  for (Instruction *it = inst_bb_begin(BB_); it != q_iterator_;
       it = it->getNextNode()) {
    if ((dyn_cast<AllocaInst>(it)))
      RelevantBeginningOps.insert(it);
    else if ((dyn_cast<StoreInst>(it)))
      RelevantBeginningOps.insert(it);
    else if ((dyn_cast<PHINode>(it)))
      RelevantBeginningOps.insert(it);
  }
}

QBBIter::QBBIter(BasicBlock &B, Instruction *it) : QBBIter(B) {

  // Set q_iterator_ to it or next nearest gate

  // if it is beyond q_END(), we set it to q_END()
  if (std::distance(it->getIterator(), q_END()->getIterator()) < 0)
    gotoEnd();
  else {
    q_iterator_ = it;
    while (!is_qgate() && q_iterator_ != q_END())
      advance_q_iterator_();
  }
}

////////////////////////////////////////////////////////////////////////////////
// Boolean Accessors

bool QIter::isBegin() { return q_iterator_ == this->q_BEGIN(); }

bool QIter::isEnd() { return q_iterator_ == this->q_END(); }

bool QIter::operator==(QIter &call) { return q_iterator_ == call.q_iterator_; }

bool QIter::operator!=(QIter &call) { return q_iterator_ != call.q_iterator_; }

Instruction &QIter::operator*() { return *q_iterator_; }

bool QIter::isParametric() {
  return (getGateNumParametricOperands(cur_gate_data_) != 0);
}

bool QIter::isCanonicalGate() {
  int id = getGateIdentifier(cur_gate_data_);
  return (id > 0 && id < kNumGates);
}

bool QIter::isUnitary() { return isGateUnitary(cur_gate_data_); }

bool QIter::isHermitian() { return isGateHermitian(cur_gate_data_); }

bool QIter::isMutable() { return isGateMutable(cur_gate_data_); }

bool QIter::isUserPulse() { return isGateUserPulse(cur_gate_data_); }

int QIter::isSupportedOnQubit(QbitRef &qubit) {

  if (isEnd())
    return 0;

  std::vector<QbitRef> this_qbits = getQubitOperands();
  int temp = 0;

  for (size_t i = 0; i < this_qbits.size(); i++) {
    temp = this_qbits[i].isEqual(qubit);
    if (temp != 0)
      return temp;
  };

  return 0;
}

////////////////////////////////////////////////////////////////////////////////
// General accessors

int QIter::getIdentifier() { return getGateIdentifier(cur_gate_data_); }

std::vector<QbitRef> QIter::getQubitOperands() {

  // get qubit function argument indices from metadata

  std::vector<unsigned> qbit_ops;

  getGateList(cur_gate_data_, "qubit_list", qbit_ops);

  // trace back for each qubit argument

  unsigned num_ops = qbit_ops.size();
  std::vector<QbitRef> out(num_ops);

  for (unsigned int i = 0; i < num_ops; i++)
    out[i] = qubit_backtrace_helper(i);

  return out;
}

unsigned QIter::getNumQubitOperands() {
  return getGateNumQubitOperands(cur_gate_data_);
}

std::vector<QbitRef> QIter::getUserPulseQubits() {

  // temporary storage that can be expanded
  /// TODO: change this to a vector when push_back is fixed
  std::list<QbitRef> temp_list;

  // First, check this is a user pulse
  if (isUserPulse()) {

    if (CallInst *CI = dyn_cast<CallInst>(&*q_iterator_)) {

      Function *func = CI->getCalledFunction();

      // we can get use the QIter to extract the qubits
      QIter qit(*func);

      if (qit.gotoFirstGateInstance(kStartPulse))
        do {

          QbitRef qubit = qit.getQubitOperand(0);
          if (!qubit.isNull())
            temp_list.emplace_back(qubit);

          qubit = qit.getQubitOperand(1);
          if (!qubit.isNull())
            temp_list.emplace_back(qubit);

          qubit = qit.getQubitOperand(2);
          if (!qubit.isNull())
            temp_list.emplace_back(qubit);

        } while (qit.gotoNextGateInstance(kStartPulse));
    }
  }

  std::vector<QbitRef> out{std::begin(temp_list), std::end(temp_list)};

  return out;
}

std::vector<ParaRef> QIter::getParametricOperands() {

  // get qubit function argument indices from metadata

  std::vector<unsigned> para_ops;

  getGateList(cur_gate_data_, "parametric_list", para_ops);

  // trace back for each qubit argument

  unsigned num_ops = para_ops.size();
  std::vector<ParaRef> out(num_ops);

  for (unsigned int i = 0; i < num_ops; i++)
    out[i] = parametric_backtrace_helper(i);

  return out;
}

unsigned QIter::getNumParametricOperands() {
  return getGateNumParametricOperands(cur_gate_data_);
}

std::vector<QbitRef> QIter::getControlQubits() {

  std::vector<unsigned> qbit_ops, ctl_ops;

  // get ctl ops list
  getGateList(cur_gate_data_, "control_qubit_list", ctl_ops);

  std::vector<QbitRef> out(ctl_ops.size());
  if (out.size() == 0)
    return out;

  // get qubit ops list
  getGateList(cur_gate_data_, "qubit_list", qbit_ops);

  unsigned pos;

  for (size_t i = 0; i < ctl_ops.size(); i++) {
    pos = 0;
    while (ctl_ops[i] != qbit_ops[pos])
      pos++;
    out[i] = qubit_backtrace_helper(pos);
  };

  return out;
}

int QIter::commutesWith(QIter &other) {

  // First test: check qubit support and local basis
  // First get qubits
  std::vector<QbitRef> qbits_this = getQubitOperands();
  std::vector<QbitRef> qbits_other = other.getQubitOperands();

  // get qubits using different method for user pulses
  if (isUserPulse())
    qbits_this = getUserPulseQubits();
  if (other.isUserPulse())
    qbits_other = other.getUserPulseQubits();

  // now get local basis lists
  std::vector<unsigned> basis_this;
  std::vector<unsigned> basis_other;

  getGateList(cur_gate_data_, "local_basis_list", basis_this);
  getGateList(other.cur_gate_data_, "local_basis_list", basis_other);

  // if local basis sizes doesn't match qbit list sizes, then we only check qbit
  // overlap
  bool check_basis = (basis_this.size() == qbits_this.size()) &&
                     (basis_other.size() == qbits_other.size());
  int result;

  for (size_t i = 0; i < qbits_this.size(); i++) {
    for (size_t j = 0; j < qbits_other.size(); j++) {

      result = qbits_this[i].isEqual(qbits_other[j]);

      if (result != 0) {
        // now check local basis
        if (check_basis) {
          // Check if local basis is resolved
          if ((basis_this[i] != 0) && (basis_other[j] != 0)) {
            // zero result if the bases are the same
            if (basis_this[i] == basis_other[j])
              result = 0;
          }
          // with unresolved local basis, can not resolve if commutes or not
          else
            result = -1;
        }
        // Without local basis, we can not resolve if commutes or not
        else
          result = -1;
      }
      // If result is anything but zero, we have our answer
      if (result != 0)
        return -1;
    };
  };
  // If we have gotten here, we know the two gates commute
  return 1;
}

////////////////////////////////////////////////////////////////////////////////
// QIter movers

Instruction *QIter::operator++() {

  if (isEnd())
    return q_iterator_;

  do {
    advance_q_iterator_();
  } while (!is_qgate() && !isEnd());

  return q_iterator_;
}

Instruction *QIter::operator--() {

  if (isBegin())
    return q_iterator_;

  do {
    reverse_q_iterator_();
  } while (!is_qgate() && !isBegin());

  return q_iterator_;
}

int QIter::translateIterator(int steps) {

  int out = 0;

  if (steps < 0) {

    while (out > steps && !isBegin()) {
      operator--();
      out--;
    }

  } else {

    while (out < steps && !isEnd()) {
      operator++();
      out++;
    }
  }

  return out;
}

Instruction *QIter::gotoBegin() {

  q_iterator_ = this->q_BEGIN();
  if (!isEnd())
    // assert(is_qgate()); // sets cur_gate_data_
    if (!is_qgate()) {
      std::string ErrMsg = "Beginning is not a quantum gate.\n";
      displayErrorAndExit("QuantumIterator", ErrMsg);
    }
  return q_iterator_;
}

Instruction *QIter::gotoEnd() {

  q_iterator_ = this->q_END();
  // assert(!is_qgate()); // sets cur_gate_data_
  if (is_qgate()) {
    std::string ErrMsg = "End is a quantum gate.\n";
    displayErrorAndExit("QuantumIterator", ErrMsg);
  }
  return q_iterator_;
}

bool QIter::gotoNextGateInstance(int gate_id) {

  if (isEnd())
    return false;

  do {
    operator++();
  } while (!isEnd() && (getIdentifier() != gate_id));

  return !isEnd();
}

bool QIter::gotoPreviousGateInstance(int gate_id) {

  if (isBegin())
    return false;

  do {
    operator--();
  } while (!isBegin() && (getIdentifier() != gate_id));

  return (getIdentifier() == gate_id);
}

bool QIter::gotoNextQubitInstance(std::vector<QbitRef> &qbits) {

  if (isEnd())
    return false;

  bool flag = false;

  do {
    operator++();
    for (size_t i = 0; i < qbits.size(); i++)
      flag = flag || (isSupportedOnQubit(qbits[i]) != 0);
  } while (!flag && !isEnd());

  return flag;
}

bool QIter::gotoPreviousQubitInstance(std::vector<QbitRef> &qbits) {

  if (isBegin())
    return false;

  bool flag = false;

  do {
    operator--();
    for (size_t i = 0; i < qbits.size(); i++)
      flag = flag || (isSupportedOnQubit(qbits[i]) != 0);
  } while (!flag && !isBegin());

  return flag;
}

bool QIter::gotoFirstGateInstance(int identifier) {
  gotoBegin();
  if (getIdentifier() != identifier)
    return gotoNextGateInstance(identifier);
  return true;
}

bool QIter::gotoFirstQubitInstance(std::vector<QbitRef> &qbits) {
  gotoBegin();
  bool is_not_sup = true;
  for (size_t i = 0; i < qbits.size(); i++)
    is_not_sup = is_not_sup && (isSupportedOnQubit(qbits[i]) == 0);
  if (is_not_sup)
    return gotoNextQubitInstance(qbits);
  return true;
}

bool QIter::gotoLastGateInstance(int identifier) {
  gotoEnd();
  return gotoPreviousGateInstance(identifier);
}

bool QIter::gotoLastQubitInstance(std::vector<QbitRef> &qbits) {
  gotoEnd();
  return gotoPreviousQubitInstance(qbits);
}

////////////////////////////////////////////////////////////////////////////////
// QIR Manipulators

bool QIter::setQubitOperands(std::vector<QbitRef> &qbits_new) {

  if (isEnd())
    return false;
  bool out = true;

  // first check that members of qbits_new are not equal
  for (size_t i = 0; i < qbits_new.size() - 1; i++) {
    for (int j = i + 1; j < qbits_new.size(); j++) {
      if (qbits_new[i].isEqual(qbits_new[j]) == 1) {
        errs() << "QIter Error: Qubit operands can not be equal for a single "
                  "gate.\n";
        return false;
      }
    };
  };

  // get the current qubit arguments
  std::vector<QbitRef> qbits_old = getQubitOperands();

  // better be the same size
  if (qbits_new.size() != qbits_old.size()) {
    errs() << "QIter Error:  QbitRef list must have the same number of gate "
              "qubit operands.\n";
    return false;
  }

  // get qubit_list
  std::vector<unsigned> qbit_ops;
  getGateList(cur_gate_data_, "qubit_list", qbit_ops);

  // get Instruction
  Instruction *gate_call = &*q_iterator_;

  for (int i = qbits_new.size() - 1; i >= 0; i--) {
    if (qbits_new[i].isEqual(qbits_old[i]) != 1)
      out = out && replace_qbit_call(gate_call, qbit_ops[i], qbits_new[i]);
  };

  return out;
}

bool QIter::setParametricOperands(std::vector<ParaRef> &paras_new) {

  if (isEnd())
    return false;
  bool out = true;

  // get the current parametric arguments
  std::vector<ParaRef> paras_old = getParametricOperands();

  // better be the same size
  if (paras_new.size() != paras_old.size()) {
    errs() << "QIter Error:  QbitRef list must have the same number of gate "
              "qubit operands.\n";
    return false;
  }

  // get parametric_list
  std::vector<unsigned> paras_ops;
  getGateList(cur_gate_data_, "parametric_list", paras_ops);

  // get Instruction
  Instruction *gate_call = &*q_iterator_;

  for (int i = paras_new.size() - 1; i >= 0; i--) {
    if (paras_new[i].isEqual(paras_old[i]) != 1)
      out = out && replace_para_call(gate_call, paras_ops[i], paras_new[i]);
  };

  return out;
}

bool QIter::removeGate() {
  if (isEnd())
    return false;
  bool out = true;

  // first, get the current gate call
  if (CallInst *CI = dyn_cast<CallInst>(&*q_iterator_)) {

    // get gate operands
    std::vector<unsigned> qbit_ops;
    std::vector<unsigned> para_ops;
    getGateList(cur_gate_data_, "qubit_list", qbit_ops);
    getGateList(cur_gate_data_, "parametric_list", para_ops);
    std::vector<Value *> qbit_vals;
    std::vector<Value *> para_vals;

    // put iterator at gate just after this one
    // if we are at the beginning, we need to update range map if successful
    bool update = isBegin();
    bool update_BBBegin = q_iterator_ == BBBegin;
    BasicBlock *CurrentBB = q_iterator_->getParent();
    operator++();

    for (size_t i = 0; i < qbit_ops.size(); i++)
      qbit_vals.push_back(CI->getArgOperand(qbit_ops[i]));
    for (size_t i = 0; i < para_ops.size(); i++)
      para_vals.push_back(CI->getArgOperand(para_ops[i]));

    // remove Call instruction
    CI->eraseFromParent();

    // remove Qbit calls
    for (size_t i = 0; i < qbit_vals.size(); i++) {
      out = out && remove_qbit_call(qbit_vals[i]);
    };

    // remove Para calls
    // Don't need to remove anything
    // let dead instruction clean-up handle any parameter instructions

    if (update) {
      BBBegin = &*CurrentBB->begin();
    }

    // if successful, update range map
    if (update && out) {
      this->set_q_BEGIN(q_iterator_);
    }
  }

  else {
    errs() << "QIter Error: remove found a non-gate instruction iterator.\n";
    return false;
  }

  return out;
}

void QIter::updateGateDependencies() {
  if (InstsToCheck.size() < 1)
    return;
  if (!FUNCT_)
    return;
  for (BasicBlock &BB : *FUNCT_) {
    updateGatesForBB(BB, InstsToCheck);
  }
}

void QBBIter::updateGateDependencies() {
  if (InstsToCheck.size() < 1)
    return;
  updateGatesForBB(*BB_, InstsToCheck);
}

bool QIter::insertGate(int identifier, std::vector<QbitRef> &qbits,
                       std::vector<ParaRef> &paras, bool FixOrder) {
  // check that identifier corresponds to a gate
  if (!isAGate(identifier)) {
    errs() << "QIter Error: insertGate identifier " << identifier
           << " does not correspond to a gate.\n";
    return false;
  }

  // check that argument inputs match function arguments
  unsigned num_q = getGateNumQubitOperands(identifier);
  unsigned num_p = getGateNumParametricOperands(identifier);

  if (num_q != qbits.size()) {
    errs()
        << "QIter Error: Incorrect number of qubit operands for insertGate on "
        << identifier << ":\n";
    errs() << "needed " << num_q << " but gave " << qbits.size() << "\n";
    return false;
  }

  if (num_p != paras.size()) {
    errs()
        << "QIter Error: Incorrect number of parametric operands insertGate on "
        << identifier << ":\n";
    errs() << "needed " << num_p << " but gave " << paras.size() << "\n";
    return false;
  }

  Instruction *II;

  // if we are at the beginning, we will need to update the range map
  // appropriately at the end. This creates a reference value to come back to
  // later.
  bool update = isBegin();

  // Get the original basic block for reference.
  BasicBlock *beg_BB = this->q_BEGIN()->getParent();

  // If we are at the beginning of the function, we want to insert before the
  // operands and parameters for the  current quantum gate. This helps keep the
  // IR clean and readable.
  if (isBegin()) {
    // So, starting from the current location, we iterate backwards through the
    // instructions, until we hit the beginning of the iterator, or we hit an
    // instruction that we cannot insert before, like Alloca, PHINodes and Store
    // instructions.
    bool Done = false;
    Instruction *StartInst = &*beg_BB->begin();
    while (!Done && (q_iterator_ != StartInst)) {
      Done = isa<PHINode>(q_iterator_) || isa<AllocaInst>(q_iterator_) ||
             isa<StoreInst>(q_iterator_);
      reverse_q_iterator_();
    }
    Done = isa<PHINode>(q_iterator_) || isa<AllocaInst>(q_iterator_) ||
           isa<StoreInst>(q_iterator_);
    if (Done)
      advance_q_iterator_();
  } else {
    // If we are inserting into the middle of the iterator, we go backwards to
    // the previous operation then move the instruction pointer forward once.
    operator--();
    if (!(q_iterator_->isTerminator()))
      advance_q_iterator_();
  }

  II = q_iterator_;
  bool insert_after = false;

  // Get the Function for identifier
  Module *M = II->getModule();
  Function *NewGate = getGateFunction(identifier, M);
  if (!NewGate) {
    return false;
  }
  FunctionType *FTy = NewGate->getFunctionType();

  // Added the Qubit and Parameter arguments
  unsigned num_tot = num_q + num_p;
  std::vector<Value *> arg_list(num_tot);
  std::vector<unsigned> operands;
  getGateList(identifier, "qubit_list", operands);
  getGateList(identifier, "parametric_list", operands);

  // Handle the arguments to the call, and insert them before the
  // current instruction pointer as appropriate.
  for (int cnt = 0; cnt != num_tot; cnt++) {
    bool UpdateAfter = false;
    unsigned int pos = 0;
    while (operands[pos] != cnt)
      pos++;
    if (pos < num_q) {
      bool isPtrTy = FTy->getParamType(cnt)->isPointerTy();
      arg_list[cnt] = add_qbit_call(II, qbits[pos], insert_after, isPtrTy);
    } else {
      bool UpdateAfter = false;
      arg_list[cnt] = add_para_call(II, paras[pos - num_q], insert_after);
    }
  }

  // check that all arguments have been created
  for (unsigned int i = 0; i < num_tot; i++) {
    if (!(arg_list[i])) {
      errs() << "QIter Error: insertGate failed to generate argument " << i
             << ".\n";
      errs() << getGateName(identifier);
      return false;
    }
  }

  // Create CallInst and insert before II in most cases.
  CallInst *NewCall = CallInst::Create(FTy, NewGate, arg_list, "");
  if (insert_after)
    NewCall->insertAfter(II);
  else
    NewCall->insertBefore(II);

  // update q_iterator_ to point at new call instruction. We also
  // update the beginning operation if the iterator previously pointed
  // at the beginning of the block of quatnum operations.
  if (update) {
    q_iterator_ = inst_bb_begin(beg_BB);
    BBBegin = q_iterator_;
    while (!is_qgate() && !isEnd()) {
      advance_q_iterator_();
    }
    this->set_q_BEGIN(q_iterator_);
  } else
    operator--();

  // If specified, we fix the ordering of the arguments to the quantum calls
  // here. We have been keeping track of when arguments already exist in the
  // block. As we have always inserted before the iterator, and not cared about
  // the order we may need to move the operations in the block at this time so
  // that each argument is defined at the correct time. We fix the ordering at
  // the end of insertion to prevent repeated iteration when determining the
  // current order of the block. The updateGateDependencies function has more
  // details for the implementation.
  if (FixOrder)
    updateGateDependencies();

  return true;
}

bool QIter::changeGate(int identifier) {

  if (isEnd())
    return false;

  // check that identifier corresponds to a gate
  if (!isAGate(identifier)) {
    errs() << "QIter Error: changeGate identifier does not correspond to a "
              "gate.\n";
    return false;
  }

  if (getIdentifier() == identifier)
    return true;

  // check that argument inputs match function arguments
  unsigned num_q = getGateNumQubitOperands(identifier);
  unsigned num_p = getGateNumParametricOperands(identifier);

  auto temp = getGateDataPtr(identifier);

  if (num_q != getNumQubitOperands()) {
    errs() << "QIter Error: Incorrect number of qubit operands for gate "
           << identifier << ":\n";
    errs() << "needed " << num_q << " but gave " << getNumQubitOperands()
           << "\n";
    return false;
  }

  if (num_p != getNumParametricOperands()) {
    errs() << "QIter Error: Incorrect number of parametric operands for gate "
           << identifier << ":\n";
    errs() << "needed " << num_p << " but gave " << getNumParametricOperands()
           << "\n";
    return false;
  }

  // because we are creating a new instruction, we will have to update range map
  bool update = isBegin();

  // Get the Function for identifier
  CallInst *CI = dyn_cast<CallInst>(&*q_iterator_);
  if (!CI) {
    return false;
  }
  Module *M = CI->getModule();
  Function *NewGate = getGateFunction(identifier, M);
  if (!NewGate) {
    return false;
  }
  FunctionType *FTy = NewGate->getFunctionType();

  // Added the Qubit and Parameter arguments
  unsigned num_tot = num_q + num_p;
  std::vector<Value *> arg_list(num_tot);
  std::vector<unsigned> new_operands, old_operands;
  getGateList(identifier, "qubit_list", new_operands);
  getGateList(identifier, "parametric_list", new_operands);
  getGateList(cur_gate_data_, "qubit_list", old_operands);
  getGateList(cur_gate_data_, "parametric_list", old_operands);

  for (unsigned int i = 0; i < num_q; i++) {
    arg_list[new_operands[i]] = CI->getArgOperand(old_operands[i]);
  }

  // for Parametric arguments, make sure types are compatible
  for (unsigned int i = num_q; i < num_tot; i++) {
    if ((NewGate->getArg(new_operands[i])->getType()) ==
        (CI->getArgOperand(old_operands[i])->getType())) {
      arg_list[new_operands[i]] = CI->getArgOperand(old_operands[i]);
    } else
      return false;
  };

  CallInst::Create(FTy, NewGate, arg_list, "", dyn_cast<Instruction>(CI));
  reverse_q_iterator_();
  CI->eraseFromParent();

  if (!is_qgate()) {
    std::string ErrMsg = "Gate was lost when changing gate.\n";
    displayErrorAndExit("QuantumIterator", ErrMsg);
  }

  // update range map
  if (update) {
    this->set_q_BEGIN(q_iterator_);
  }

  return true;
}

////////////////////////////////////////////////////////////////////////////////
// Internal Helper functions

// this function is also used to set current gate data pointer
bool QIter::is_qgate() {

  if (q_iterator_ == this->q_END()) {
    cur_gate_data_ = q_GATEMETADATA_->end();
    return false;
  }

  if (!q_iterator_)
    return false;

  Instruction *temp = q_iterator_;
  if (CallInst *CI = dyn_cast<CallInst>(temp)) {

    auto *CF = CI->getCalledFunction();
    if (!CF) {
      return false;
    }

    std::string GateName = CF->getName().str();
    cur_gate_data_ = q_GATEMETADATA_->find(GateName);
    return cur_gate_data_ != q_GATEMETADATA_->end();
  }

  return false;
}

QbitRef &QIter::qubit_backtrace_helper(unsigned i) {

  static QbitRef out;
  out.deleteQbitRef();

  // get qubit list
  std::vector<unsigned> qbit_ops;
  getGateList(cur_gate_data_, "qubit_list", qbit_ops);
  if (i >= qbit_ops.size()) {
    std::string ErrMsg = "Qubit argument index is larger than qubits in "
                         "operation.\n";
    displayErrorAndExit("QuantumIterator", ErrMsg);
  }

  // get the CallInst
  Instruction *temp = q_iterator_;

  out = qubit_backtrace_helper(temp, qbit_ops[i]);

  return out;
}

QbitRef QIter::qubit_backtrace_helper(Instruction *I, unsigned i, bool warn) {

  QbitRef out;

  if (CallInst *CI = dyn_cast<CallInst>(I)) {

    Value *q_arg = CI->getArgOperand(i);

    // if it is a constant int, this is a QID
    if (ConstantInt *CI = dyn_cast<ConstantInt>(q_arg)) {
      if (CI->getBitWidth() != 16) {
        return out;
      }
      out.reg_ptr_ = nullptr;
      out.index_ptr_ = q_arg;
      out.convertToQID();
      return out;
    }
    // is an allocation
    else if ((out.is_qbit_alloc(q_arg))) {
      out.setRegister(q_arg);
      out.setIndexResolved(0);
    }
    // is an argument
    else if ((out.is_qbit_argument(q_arg))) {
      out.setRegister(q_arg);
      out.setIndexResolved(0);
    }
    // is a global (don't  think this can happen)
    else if ((out.is_qbit_global(q_arg))) {
      out.setRegister(q_arg);
      out.setIndexResolved(0);
    }

    // is a LoadInst
    else if (LoadInst *LI = dyn_cast<LoadInst>(q_arg)) {

      Value *load_ref = LI->getPointerOperand();

      out = getQbitRefPointedToBy(load_ref);

    } else if (isa<GEPOperator>(q_arg)) {
      out = getQbitRefPointedToBy(q_arg);
    } else if (out.is_qbit_fleq_reg(q_arg)) {
      out = getQbitRefPointedToBy(q_arg);
    } else if (CallBase *CB = dyn_cast<CallBase>(q_arg)) {
      StringRef called_name = CB->getCalledFunction()->getName();
      if (called_name.contains(fleq::QLIST_AT)) {
        out = getQbitRefPointedToBy(CB->getArgOperand(0));
        if (!out.isNull())
          out.shiftIndexBy(CB->getArgOperand(1));
      }
    } else if (warn) {
      errs() << "QIter Warning: qubit argument " << i
             << " does not represent a known qubit type:" << (*q_arg) << "\n";
      out.setRegister(q_arg);
      out.setIndex(nullptr);
    }
  } else {
    std::string ErrMsg = "Current instruction is not a CallInst.\n";
    displayErrorAndExit("QuantumIterator", ErrMsg);
  }
  if (warn && out.isNull())
    errs() << "QIter Warning: qubit backtrace didn't result in a proper "
              "QbitRef.\n";
  return out;
}

ParaRef &QIter::parametric_backtrace_helper(unsigned i) {

  static ParaRef out;

  // get parametric list
  std::vector<unsigned> para_ops;
  getGateList(cur_gate_data_, "parametric_list", para_ops);
  // assert(i < para_ops.size());
  if (i >= para_ops.size()) {
    std::string ErrMsg = "Parameter argument index is larger than qubits in "
                         "operation.\n";
    displayErrorAndExit("QuantumIterator", ErrMsg);
  }

  Instruction *temp = &*q_iterator_;

  out = parametric_backtrace_helper(temp, para_ops[i]);

  return out;
}

ParaRef QIter::parametric_backtrace_helper(Instruction *I, unsigned i,
                                           bool warn) {

  ParaRef out;

  if (CallInst *CI = dyn_cast<CallInst>(I)) {

    Value *para_arg = CI->getArgOperand(i);
    Type *Ty = para_arg->getType();

    // check that it is a float, integer or cbit ref
    if (Ty->isFloatingPointTy())
      out.setValue(para_arg);
    else if (Ty->isIntegerTy())
      out.setValue(para_arg);
    else if (out.is_cbit_ref(para_arg))
      out.setValue(para_arg);
    else if (warn) {
      errs() << "QIter Error: Parameter argument is not a float, integer or "
                "cbit reference.\n";
    }
  }

  return out;
}

////////////////////////////////////////////////////////////////////////////////
// IR manipulator utilities

Value *QIter::add_qbit_call(Instruction *instr, QbitRef &q, bool is_after,
                            bool isQbitPtr) {
  if (q.isNull())
    return nullptr;

  // First check for QID. If yes, insert possible truncation and return index.
  if (q.is_qbit_qid()) {
    return q.index_ptr_;
  }
  // Check that Instruction and q.reg_ptr_ belong to same Function

  if (AllocaInst *AI = q.is_qbit_alloc()) {
    if ((AI->getFunction()) != (instr->getFunction()))
      return nullptr;
  } else if (Argument *Arg = q.is_qbit_argument()) {
    if ((Arg->getParent()) != (instr->getFunction()))
      return nullptr;
  } else if (GlobalValue *GV = q.is_qbit_global()) {
    if ((GV->getParent() != instr->getModule()))
      return nullptr;
  } else if (CallBase *CB = q.is_qbit_fleq_reg()) {
    if ((CB->getFunction() != instr->getFunction()))
      return nullptr;
  }

  return q.createValue(instr, is_after, isQbitPtr, this);
}

bool QIter::remove_qbit_call(Value *q_val) {

  QbitRef temp; // this is just to access methods of QbitRef

  Value *qbit_check = q_val; // for verifying this is a qubit call
  Instruction *LII = nullptr;
  Instruction *GII = nullptr;

  if ((dyn_cast<ConstantInt>(q_val)))
    return true;

  // Check for qbit_at and removed
  if (CallBase *CB = dyn_cast<CallBase>(q_val)) {
    if ((CB->getCalledFunction()->getName()).contains(fleq::QLIST_AT)) {
      CB->eraseFromParent();
      return true;
    }
  }
  // Check for LoadInst
  if (LoadInst *LI = dyn_cast<LoadInst>(q_val)) {

    // get pointer value
    Value *load_ptr = LI->getPointerOperand();

    // get Load instruction for later removal
    LII = dyn_cast<Instruction>(LI);

    if (GetElementPtrInst *GI = dyn_cast<GetElementPtrInst>(load_ptr)) {

      // get pointer value
      qbit_check = GI->getPointerOperand();

      // get GEP instruction for later removal
      GII = dyn_cast<Instruction>(GI);
    } else if (AllocaInst *AI = dyn_cast<AllocaInst>(load_ptr)) {
      qbit_check = load_ptr;
    } else if (GlobalValue *GV = dyn_cast<GlobalValue>(load_ptr)) {
      qbit_check = load_ptr;
    }

  } else if (GetElementPtrInst *GI = dyn_cast<GetElementPtrInst>(q_val)) {

    // get pointer value
    qbit_check = GI->getPointerOperand();

    // get GEP instruction for later removal
    GII = dyn_cast<Instruction>(GI);

  } else if (ConstantExpr *GEP = dyn_cast<ConstantExpr>(q_val)) {
    qbit_check = GEP->getOperand(0);
  } else
    qbit_check = q_val;

  // Check this is a qubit allocation

  if (!(temp.is_qbit_alloc(qbit_check)) &&
      !(temp.is_qbit_argument(qbit_check)) &&
      !(temp.is_qbit_global(qbit_check)) &&
      !(temp.is_qbit_fleq_reg(qbit_check))) {
    errs() << "QIter Error: Can't remove non-qubit call.\n";
    return false;
  } else {

    // Now can try to delete Instructions
    if ((LII)) {
      // delete Load
      if (LII->use_empty())
        LII->eraseFromParent();
      // delete GEP
      if ((GII)) {
        if (GII->use_empty())
          GII->eraseFromParent();
      }
    }
  }

  return true;
}

bool QIter::replace_qbit_call(Instruction *inst, unsigned arg_num, QbitRef &q) {
  // First check inst is a CallInst
  if (CallInst *CI = dyn_cast<CallInst>(inst)) {

    // check if arg_num less then number of args
    if (arg_num < CI->arg_size()) {

      // 1) add qubit call
      Function *Func = CI->getCalledFunction();
      FunctionType *FTy = Func->getFunctionType();
      bool isPtrTy = FTy->getParamType(arg_num)->isPointerTy();

      Value *new_arg = add_qbit_call(inst, q, false, isPtrTy);

      // 2) get argument and replace old arg with new
      Value *old_arg = CI->getArgOperand(arg_num);
      if ((new_arg))
        CI->setOperand(arg_num, new_arg);
      else
        return false;

      // 3)remove old qubit call
      return remove_qbit_call(old_arg);

    } else
      errs() << "Qiter Error: can't replace an argument that isn't there.\n";
  } else
    errs() << "QIter Error: Instruction in replace is not a CallInst.\n";

  return false;
}

bool QIter::replace_para_call(Instruction *inst, unsigned arg_num,
                              ParaRef &ang) {

  // First check inst is a CallInst
  if (CallInst *CI = dyn_cast<CallInst>(inst)) {

    // check if arg_num less than number of args
    if (arg_num < CI->getNumOperands()) {

      // 1) add parameter call
      Value *new_arg = add_para_call(inst, ang, false);

      // 2) get argument and replace old arg with new
      Value *old_arg = CI->getArgOperand(arg_num);
      if ((new_arg)) {
        if ((old_arg->getType()) == (new_arg->getType())) {
          CI->setOperand(arg_num, new_arg);
        } else {
          errs() << "QIter Error: replace_para_call is trying to swap "
                    "parameters of a different type.\n";
          return false;
        }
      } else
        return false;

      // no clean-up advised for parameters
      // let dead Instruction clean-up pass handle this
      return true;

    } else
      errs() << "Qiter Error: can't replace an argument that isn't there.\n";
  } else
    errs() << "QIter Error: Instruction in replace is not a CallInst.\n";

  return false;
}

Value *QIter::add_para_call(Instruction *inst, ParaRef &ang, bool is_after) {

  if (inst == nullptr || ang.isNull())
    return nullptr;

  if (Instruction *II = dyn_cast<Instruction>(ang.value_ptr_)) {

    if (ang.is_value_owned()) {

      if (is_after)
        II->insertAfter(inst);
      else
        II->insertBefore(inst);

      ang.set_value_owned(false);

      // Recursively add owned operands
      for (auto op : II->operand_values()) {
        ParaRef ParaOp(op);
        if (!ParaOp.isNull()) {
          add_para_call(II, ParaOp, false);
        }
      };

    } else {
      // This instruction was already inserted into the IR, we may be inserting
      // an instruction in the incorrect order, add it to the set of
      // instructions to check for later.
      InstsToCheck.push_back(inst);
      if (II->getFunction() != inst->getFunction()) {
        errs() << "QIter Error: cannot use instruction for ParaRef in this "
                  "Function!\n";
        return nullptr;
      }
    }
  }

  // otherwise, there is nothing else to do
  return ang.value_ptr_;
}

} // namespace aqcc
} // namespace llvm

// Clear out the debug logging macro.
#undef DEBUG_TYPE
