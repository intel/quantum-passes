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

////////////////////////////////////////////////////////////////////////////////
// Constructor

QIter::QIter(Function &F) : FUNCT_(&F) {

  // Set Qmetadata
  // QIter assumes that the QuantumAnnotationsToJsonPass has been run

  q_GATEMETADATA_ = QuantumAnnotationsToJson::qGateMetadata;

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
  inst_iterator begin = inst_begin(*FUNCT_);
  inst_iterator end = inst_end(*FUNCT_);
  // use last terminator as end;
  --end;

  // initialize range map so it's there for is_qgate()
  getInstance_q_RANGE_MAP_()[FUNCT_] = qiter_range(begin, end);

  q_iterator_ = begin;

  while (!is_qgate() && q_iterator_ != end)
    ++q_iterator_;
  begin = q_iterator_;
  end = q_iterator_;

  while (q_iterator_ != q_END()) {
    if (is_qgate())
      end = q_iterator_;
    ++q_iterator_;
  };

  if (end != q_END())
    ++end;

  // Add or uppdate range to the q_RANGE_MAP_
  getInstance_q_RANGE_MAP_()[FUNCT_] = qiter_range(begin, end);

  q_iterator_ = q_BEGIN();
  is_qgate(); // sets cur_gate_data_
}

QIter::QIter(Function &F, inst_iterator it) : QIter(F) {

  // Set q_iterator_ to it or next nearest gate

  // if it is beyond q_END(), we set it to q_END()
  if (std::distance(it, q_END()) < 0)
    gotoEnd();
  else {
    q_iterator_ = it;
    while (!is_qgate() && q_iterator_ != q_END())
      ++q_iterator_;
  }
}

QBBIter::QBBIter(BasicBlock &B) : BB_(&B) {

  FUNCT_ = BB_->getParent();

  // Set Qmetadata
  // QBBIter assumes that the QuantumAnnotationsToJsonPass has been run

  q_GATEMETADATA_ = QuantumAnnotationsToJson::qGateMetadata;

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
  inst_iterator begin = inst_bb_begin(BB_);
  inst_iterator end = inst_bb_end(BB_);
  --end;
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
    ++q_iterator_;
  begin = q_iterator_;

  // Add or uppdate range to the q_BB_RANGE_MAP_
  getInstance_q_BB_RANGE_MAP_()[BB_] = qiter_range(begin, end);

  q_iterator_ = q_BEGIN();
  is_qgate(); // sets cur_gate_data_
}

QBBIter::QBBIter(BasicBlock &B, inst_iterator it) : QBBIter(B) {

  // Set q_iterator_ to it or next nearest gate

  // if it is beyond q_END(), we set it to q_END()
  if (std::distance(it, q_END()) < 0)
    gotoEnd();
  else {
    q_iterator_ = it;
    while (!is_qgate() && q_iterator_ != q_END())
      ++q_iterator_;
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
            // or if the basis is the identity for either
            else if (basis_this[i] < 0 || basis_other[j] < 0)
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

inst_iterator QIter::operator++() {

  if (isEnd())
    return q_iterator_;

  do {
    ++q_iterator_;
  } while (!is_qgate() && !isEnd());

  return q_iterator_;
}

inst_iterator QIter::operator--() {

  if (isBegin())
    return q_iterator_;

  do {
    --q_iterator_;
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

inst_iterator QIter::gotoBegin() {

  q_iterator_ = this->q_BEGIN();
  if (!isEnd())
    // assert(is_qgate()); // sets cur_gate_data_
    if (!is_qgate()) {
      std::string ErrMsg = "Beginning is not a quantum gate.\n";
      displayErrorAndExit("QuantumIterator", ErrMsg);
    }
  return q_iterator_;
}

inst_iterator QIter::gotoEnd() {

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

    // if successful, update range map
    if (update && out) {
      this->q_BEGIN() = q_iterator_;
    }
  }

  else {
    errs() << "QIter Error: remove found a non-gate instruction iterator.\n";
    return false;
  }

  return out;
}

bool QIter::insertGate(int identifier, std::vector<QbitRef> &qbits,
                       std::vector<ParaRef> &paras) {

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
  // if we are at the begining, we insert after the latest Instruction for gate
  // arguments (or begin if this is an argument) We will also have to update the
  // range map
  bool update = isBegin();

  BasicBlock *beg_BB = this->q_BEGIN()->getParent();
  if (update) {

    std::set<Instruction *> new_insts;

    for (size_t i = 0; i < qbits.size(); i++) {
      assert(!qbits[i].isNull() && "can't insert a NULL qubit!");
      if (!qbits[i].isQID()) {
        if (Instruction *Itemp = dyn_cast<Instruction>(qbits[i].getRegister()))
          new_insts.insert(Itemp);
        if (!qbits[i].is_index_owned()) {
          if (Instruction *Itemp = dyn_cast<Instruction>(qbits[i].getIndex()))
            new_insts.insert(Itemp);
        }
      }
    };

    for (size_t i = 0; i < paras.size(); i++) {
      if (!paras[i].is_value_owned()) {
        if (Instruction *Itemp = dyn_cast<Instruction>(paras[i].getValue()))
          new_insts.insert(Itemp);
      }
    };

    // Find any allocations, phis or stores in the current BB to added to the
    // list Value *val_inst;
    for (inst_iterator it = inst_bb_begin(beg_BB); it != q_iterator_; ++it) {
      if ((dyn_cast<AllocaInst>(&*it)))
        new_insts.insert(&*it);
      else if ((dyn_cast<StoreInst>(&*it)))
        new_insts.insert(&*it);
      else if ((dyn_cast<PHINode>(&*it)))
        new_insts.insert(&*it);
    }

    // Now search for latest instruction
    // current value of q_iterator_ should be a backstop such that we don't
    // accidentally insert after it. If that ends up being the case, we need to
    // insert everything before the current value of q_interator, i.e. we need
    // to set is_after to false First, stash current value of q_iterator_

    inst_iterator backstop = q_iterator_;
    q_iterator_ = inst_bb_begin(beg_BB);
    inst_iterator insert_bfr = q_iterator_;
    II = &*q_iterator_;
    bool inst_found = false;

    while (q_iterator_ != backstop) {

      Instruction *IItemp = &*q_iterator_;
      auto IIit = new_insts.find(IItemp);
      if (IIit != new_insts.end()) {
        insert_bfr = q_iterator_;
        inst_found = true;
      }
      ++q_iterator_;
    }
    if (!(q_iterator_->isTerminator()))
      ++insert_bfr;
    if (inst_found)
      II = &*insert_bfr;

  } else {
    operator--();
    if (!(q_iterator_->isTerminator()))
      ++q_iterator_;
    II = &*q_iterator_;
  }

  bool insert_after = false;
  // Checks for consistency when adding a new instruction or parameters for
  // new instructions.
  for (unsigned i = 0; i < qbits.size(); i++) {
    if (!qbits[i].getRegister())
      continue;
    // Check that the qubit register instruction actually comes before the
    // instruction we are inserting after.  If it isn't, we update the
    // instruction we are adding after to be the register allocation.
    if (Instruction *I = dyn_cast<Instruction>(qbits[i].getRegister())) {
      if (I->getParent() == II->getParent() && II->comesBefore(I)) {
        II = I;
        insert_after = true;
      }
    }

    if (!qbits[i].getIndex())
      continue;

    // Check that the qubit index instruction actually comes before the
    // instruction we are inserting after.  If it isn't, we update the
    // instruction we are adding after to be the register allocation.
    if (Instruction *I = dyn_cast<Instruction>(qbits[i].getIndex())) {
      if (I->getParent() == II->getParent() && II->comesBefore(I)) {
        II = I;
        insert_after = true;
      }
    }
  }

  // Check that the parameter instruction actually comes before the
  // instruction we are inserting after.  If it isn't, we update the
  // instruction we are adding after to be the register allocation.
  for (unsigned i = 0; i < paras.size(); i++) {
    if (!paras[i].getValue())
      continue;
    if (Instruction *I = dyn_cast<Instruction>(paras[i].getValue())) {
      if (I->getParent() == II->getParent() && II->comesBefore(I)) {
        II = I;
        insert_after = true;
      }
    }
  }

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

  for (int cnt = 0; cnt != num_tot; cnt++) {
    unsigned int pos = 0;
    while (operands[pos] != cnt)
      pos++;
    if (pos < num_q) {
      bool isPtrTy = FTy->getParamType(cnt)->isPointerTy();
      arg_list[cnt] = add_qbit_call(II, qbits[pos], insert_after, isPtrTy);
    } else {
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

  // Check that each argument instruction actually comes before the
  // instruction we are inserting after.  If it isn't, we update the
  // instruction we are adding after to be the register allocation.
  for (unsigned i = 0; i < arg_list.size(); i++) {
    if (Instruction *I = dyn_cast<Instruction>(arg_list[i])) {
      if (I->getParent() == II->getParent() && II->comesBefore(I)) {
        II = I;
        insert_after = true;
      }
    }
  }

  // Create CallInst and insert after II (or before in edge case)
  CallInst *NewCall = CallInst::Create(FTy, NewGate, arg_list, "");
  if (insert_after)
    NewCall->insertAfter(II);
  else
    NewCall->insertBefore(II);

  // update q_iterator_ to point at new call inst

  if (update) {
    q_iterator_ = inst_bb_begin(beg_BB);
    while (!is_qgate() && !isEnd())
      ++q_iterator_;
  } else
    operator--();

  // update range map
  // if successful, update range map
  if (update) {
    this->q_BEGIN() = q_iterator_;
  }

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
  --q_iterator_;
  CI->eraseFromParent();

  if (!is_qgate()) {
    std::string ErrMsg = "Gate was losts when changing gate.\n";
    displayErrorAndExit("QuantumIterator", ErrMsg);
  }

  // update range map
  if (update) {
    this->q_BEGIN() = q_iterator_;
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

  Instruction *temp = &*q_iterator_;
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
  Instruction *temp = &*q_iterator_;

  out = qubit_backtrace_helper(temp, qbit_ops[i]);

  return out;
}

QbitRef QIter::qubit_backtrace_helper(Instruction *I, unsigned i) {

  QbitRef out;

  if (CallInst *CI = dyn_cast<CallInst>(I)) {

    Value *q_arg = CI->getArgOperand(i);

    // if it is a constant int, this is a QID
    if (ConstantInt *CI = dyn_cast<ConstantInt>(q_arg)) {
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
    } else {
      errs() << "QIter Warning: qubit argument " << i
             << " does not represent a known qubit type:" << (*q_arg) << "\n";
      out.setRegister(q_arg);
      out.setIndex(nullptr);
    }
  } else {
    std::string ErrMsg = "Current instruction is not a CallInst.\n";
    displayErrorAndExit("QuantumIterator", ErrMsg);
  }
  if (out.isNull())
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

ParaRef QIter::parametric_backtrace_helper(Instruction *I, unsigned i) {

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
    else {
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

  Value *out = nullptr;

  // check if reg_ptr_ is an integer type itself, meaning no Instructions
  // need to be inserted
  if (IntegerType *ITy = dyn_cast<IntegerType>(q.reg_ptr_->getType()))
    return q.reg_ptr_;

  // check if reg_ptr_ is a pointer
  else if (PointerType *PTy = dyn_cast<PointerType>(q.reg_ptr_->getType())) {

    Value *load_ptr = q.reg_ptr_;
    Type *load_ty = nullptr;
    if (AllocaInst *AI = q.is_qbit_alloc()) {
      load_ty = AI->getAllocatedType();
    } else if (Argument *Arg = q.is_qbit_argument())
      load_ty = Type::getInt16Ty(load_ptr->getContext());
    else if (GlobalValue *GV = q.is_qbit_global()) {
      load_ty = GV->getValueType();
    } else if (CallBase *CB = q.is_qbit_fleq_reg()) {
      load_ty = Type::getInt16Ty(load_ptr->getContext());
    }

    assert(load_ty != nullptr);

    std::vector<Value *> gep_array;
    if ((dyn_cast<ArrayType>(load_ty)))
      gep_array.push_back(
          ConstantInt::get(IntegerType::get(q.getContext(), 64), 0));

    gep_array.push_back(q.index_ptr_);

    // getAlignment
    MaybeAlign maybe_align;
    Align alignment;
    if (AllocaInst *AI = q.is_qbit_alloc()) {
      alignment = AI->getAlign();
      maybe_align = MaybeAlign(alignment);
    } else if (Argument *Arg = q.is_qbit_argument())
      maybe_align = Arg->getParamAlign();
    else if (GlobalValue *GV = q.is_qbit_global()) {
      maybe_align = GV->getAliaseeObject()->getAlign();
    }

    bool flag = false;

    // check if index is zero in which case we don't need a get element ptr
    if ((dyn_cast<IntegerType>(load_ty))) {
      if (ConstantInt *CInt = dyn_cast<ConstantInt>(q.index_ptr_)) {
        if (CInt->getZExtValue() == 0)
          flag = true;
      }
    }

    // in the case of a fleq call, we need to use qlist.at to get correct
    // qubit
    if (CallBase *CB = q.is_qbit_fleq_reg()) {

      auto IntrID = Intrinsic::fleq_qlist_at;
      Function *qlist_at = Intrinsic::getDeclaration(CB->getModule(), IntrID);
      CallInst *CallAt = CallInst::Create(qlist_at->getFunctionType(), qlist_at,
                                          {q.reg_ptr_, q.index_ptr_});

      if (is_after)
        CallAt->insertAfter(instr);
      else
        CallAt->insertBefore(instr);

      load_ptr = dyn_cast<Value>(CallAt);
      flag = true;
    }

    if (flag) {
      if (!isQbitPtr) {
        LoadInst *LI = new LoadInst(load_ty, load_ptr, "", false,
                                    maybe_align.valueOrOne());

        if (is_after)
          LI->insertAfter(instr);
        else
          LI->insertBefore(instr);
        out = dyn_cast<Value>(LI);
      } else
        // Since we are passing qubits as reference, no need to insert a load
        // instruction directly pass through the allocainst reference
        out = load_ptr;

    } else {
      Instruction *MoveIdxTo = instr;

      // If index_ptr_ is an instruction, try to add or move Instruction it
      // represents before GEP
      // Never move a Phi, landing pad or allocation
      if (!isa<PHINode>(q.index_ptr_) && !isa<LandingPadInst>(q.index_ptr_) &&
          !isa<AllocaInst>(q.index_ptr_)) {
        if (Instruction *II = dyn_cast<Instruction>(q.index_ptr_)) {
          for (Value *Op : II->operands()) {
            if (Instruction *IO = dyn_cast<Instruction>(Op)) {
              if ((IO->getParent() == MoveIdxTo->getParent() &&
                   !IO->comesBefore(MoveIdxTo)) ||
                  IO == MoveIdxTo) {
                is_after = true;
                MoveIdxTo = IO;
              }
            }
          }

          if (q.is_index_owned()) {
            if (is_after)
              II->insertAfter(MoveIdxTo);
            else
              II->insertBefore(MoveIdxTo);
            q.set_index_owned(false);
          } else {
            if (is_after)
              II->moveAfter(MoveIdxTo);
            else
              II->moveBefore(MoveIdxTo);
          }

          if ((II->getParent() == instr->getParent() &&
               instr->comesBefore(II)) ||
              II == instr) {
            is_after = true;
            instr = II;
          }
        }
      }

      GetElementPtrInst *GI = GetElementPtrInst::CreateInBounds(
          load_ty, load_ptr, gep_array, "arrayidx");
      if (!GI)
        return nullptr;
      load_ptr = dyn_cast<Value>(GI);
      if (!load_ptr)
        return nullptr;

      // Insert Instructions
      if (is_after) {
        GI->insertAfter(instr);
      } else {
        GI->insertBefore(instr);
      }
      auto *Instr = dyn_cast<Instruction>(GI);
      if (!Instr) {
        return nullptr;
      }

      if (!isQbitPtr) {
        auto *LP = dyn_cast<PointerType>(load_ptr->getType());
        if (!LP) {
          return nullptr;
        }
        auto *AP = dyn_cast<ArrayType>(load_ty);
        if (!AP) {
          return nullptr;
        }
        load_ty = AP->getArrayElementType();
        LoadInst *LI = new LoadInst(load_ty, load_ptr, "", false,
                                    maybe_align.valueOrOne());

        LI->insertAfter(Instr);
        out = dyn_cast<Value>(LI);
      } else {
        out = load_ptr;
      }

      MoveIdxTo = dyn_cast<Instruction>(GI);
      if (!MoveIdxTo) {
        return nullptr;
      }
    }
  }

  return out;
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
