//===- QuantumIterator.h - ------------------------------------------------===//
//
// Copyright (C) 2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//
//===----------------------------------------------------------------------===//
//
/// \Qiter.h
///
/// This class defines the "quantum iterator" or QIter class. A QIter object
/// is used to efficiently traverse quantum instructions and extract their
/// attributes with minimal code overhead to the developer.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_AQCC_QITER_H
#define LLVM_AQCC_QITER_H

#include "IntelQuantumPasses/QuantumAnalysis/QuantumGateIdentifiers.h"
#include "IntelQuantumPasses/QuantumAnalysis/QuantumRefs.h"
#include "IntelQuantumPasses/QuantumUtils/QuantumFLEQUtils.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/InstIterator.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/Instructions.h"
#include "llvm/Support/Alignment.h"
#include "llvm/Support/JSON.h" //JSON PACKAGE

#include <map>
#include <sstream>
#include <string>
#include <vector>

using namespace llvm;

namespace llvm {
namespace aqcc {

// Helper functions to get iterators at specific points
// of basic blocks.
inline inst_iterator inst_bb_begin(BasicBlock *b) {
  inst_iterator II(*b->getParent());
  while (!II.atEnd() && &(*II.getBasicBlockIterator()) != b)
    ++II;
  return II;
}

inline inst_iterator inst_bb_end(BasicBlock *b) {
  inst_iterator II(*b->getParent());
  while (!II.atEnd() && &(*II.getBasicBlockIterator()) != b)
    ++II;
  if (!II.atEnd())
    while (!II.atEnd() && &(*II.getInstructionIterator()) != &(b->back()))
      ++II;
  if (!II.atEnd() && &(*II.getInstructionIterator()) == &(b->back()))
    ++II;
  return II;
}

inline inst_iterator inst_at(BasicBlock::iterator it) {
  BasicBlock *b = it->getParent();
  inst_iterator II(*b->getParent());
  while (!II.atEnd() && &(*II.getBasicBlockIterator()) != b)
    ++II;
  if (!II.atEnd())
    while (&(*II.getInstructionIterator()) != &(*it))
      ++II;
  return II;
}

/// primary type for angle parameters is double
typedef double ParaType;
typedef std::pair<inst_iterator, inst_iterator> qiter_range;

class QbitRef;
class ParaRef;
class QBBIter;

/// QIter and QBBIter are a pair of classes for iterating and manipulating QIR
/// while avoiding some of the finer details of LLVM. See below for available
/// APIs.
///
/// As an example of its use, loops can be written as:
///
///   llvm::Qiter QI(F);
///   for(QI.gotoBegin(); !QI.isEnd(); ++QI){ ...}
///
/// If one wants to iterate only on gates of a certain type, use the "if,do
/// while"
/// pattern
///
///   if(QI.gotoFirstGateInstance(<gate of interest>)){
///     do{
///       ...
///     }
///     while(QI.gotoNextGateInstance(<gate of interest>))
///   }
///
///
/// The only difference between QIter and QBBIter is that while QIter is
/// constructed on an LLVM::Function and ignores basic block interfaces,
/// QBBIter is constructed on an LLVM::BasicBlock and only operates within
/// that basic block. Avoid using both in the same program if one manipulates
/// the gates in IR as they don't communicate these changes between the two.
/// But if you must, calling the constuctor of either after any change of gate
/// will fix any inaccurate internal range values.

class QIter {

protected:
  /// "constant" pointer to metadata
  std::map<StringRef, json::Object> *q_GATEMETADATA_;
  /// pointer to gate data for current gate pointed to by QIter
  std::map<StringRef, json::Object>::iterator cur_gate_data_;
  /// current gate QIter points to
  inst_iterator q_iterator_;

  /// static map getter for updating qiter_range
  /// This is static so that the range can be updated between different
  /// QIter instances when the beginning or end is changed by the adding or
  /// removal of quantum instructions.
  static std::map<Function *, qiter_range> &getInstance_q_RANGE_MAP_();

  /// "constant" pointer to function
  Function *FUNCT_;

  /// returns iterator pointing to the first Q call
  virtual inst_iterator &q_BEGIN() {
    return getInstance_q_RANGE_MAP_()[FUNCT_].first;
  }
  /// returns iterator pointing to the last Q call
  virtual inst_iterator &q_END() {
    return getInstance_q_RANGE_MAP_()[FUNCT_].second;
  }

public:
  QIter() = default;

  /// The constructor requires you to pass the function it points into
  QIter(Function &F);

  // This constructor initializes on the current or next nearest gate to
  // inst_iterator
  QIter(Function &F, inst_iterator it);

  ~QIter(){};

  // External-facing accessors
  Function *getFunction() { return FUNCT_; }

  /// Is Qiter point at the first Q call?
  bool isBegin();

  /// Is Qiter pointing at entry just past last Q call?
  bool isEnd();

  /// Is QIter pointing at (not) the same gate call
  /// Note this is equality of the actual gate call, not the gate type
  bool operator==(QIter &);
  bool operator!=(QIter &);

  /// Is the current gate parametric?
  bool isParametric();

  /// Is the current gate unitary?
  bool isUnitary();

  /// Is the current gate Hermitian?
  bool isHermitian();

  // Is the current gate mutable?
  bool isMutable();

  // Is the current gate a user pulse?
  bool isUserPulse();

  /// get a list of QbitRefs representing qubit operands
  std::vector<QbitRef> getQubitOperands();

  /// get the number of Qbit operands
  unsigned getNumQubitOperands();

  /// get a list of ParaRefs representing parametric operands
  std::vector<ParaRef> getParametricOperands();

  /// get the number of parametric arguments
  unsigned getNumParametricOperands();

  QbitRef &getQubitOperand(unsigned i) { return qubit_backtrace_helper(i); }

  ParaRef &getParametricOperand(unsigned i) {
    return parametric_backtrace_helper(i);
  }

  // This is a function specific for User defined pulses to get the qubits from
  // body of the function
  std::vector<QbitRef> getUserPulseQubits();

  /// get the gate code value of the current gate as definded in an enum (custom
  /// gates have negative values)
  int getIdentifier();

  /// Is the current gate a standard gate type as defined in the enum (is
  /// GetStdGateType != nod-standard code value)
  bool isCanonicalGate();

  /// Is the current gate supported on a given qubit?
  int isSupportedOnQubit(QbitRef &);

  /// get the list of Control qubits in the control qubit list
  std::vector<QbitRef> getControlQubits();

  // returns the iterator into F which the current q call instruction
  inst_iterator getGateInstIterator() { return q_iterator_; };

  /// Returns 1 if the current gate commutes with gate pointed at by the input,
  /// 0 if they do not commute and -1 if not determined by methods used.
  /// Current method only checks qubit overlap and local_basis on overlapping
  /// qubits due to this, output is currently only 1(commutes) or -1 (not sure)
  // TODO: implement more sophisticated methods
  int commutesWith(QIter &);

  // External-facing iterator Maniputators
  //
  /// Advance Qiter to the next q call instruction
  inst_iterator operator++();

  /// Advance to previous q call instruction
  inst_iterator operator--();

  /// Dereference to the call instuction pointed to by QIter
  Instruction &operator*();

  /// Translate backward (-int) or forward (+int) a set number of q call
  /// instructions Return is the number actually translated if Qiter hits BEGIN
  /// of END
  int translateIterator(int);

  /// Move QIter to BEGIN
  inst_iterator gotoBegin();

  /// Move QIter to END
  inst_iterator gotoEnd();

  /// Moves QIter to the next instance of a gate with given gate identifier and
  /// returns "true", or to END and returns "false"
  bool gotoNextGateInstance(int);

  /// Moves QIter to the next instance of a gate with possible given qubit
  /// indices (including unresolved cases) and returns "true", or to END and
  /// returns "false"
  bool gotoNextQubitInstance(std::vector<QbitRef> &);

  /// Moves QIter to the previous instance of the gate with given std gate code
  /// and returns "true", or to BEGIN and returns "false"
  bool gotoPreviousGateInstance(int);

  /// Moves QIter to the previous instance of a gate with possible given qubit
  /// indices (including unresolved cases) and returns "true", or to BEGIN and
  /// returns "false"
  bool gotoPreviousQubitInstance(std::vector<QbitRef> &);

  /// starts at begin and looks for first instance of gate identifier given by
  /// int and returns "true" or to END and returns "false"
  bool gotoFirstGateInstance(int);

  /// starts at begin and looks for first instance of gate supported on QubitRef
  /// in list and returns "true" or to BEGIN and returns "false"
  bool gotoFirstQubitInstance(std::vector<QbitRef> &);

  /// starts at end and looks for last instance of gate identifier given by int
  /// and returns "true" or to END and returns "false"
  bool gotoLastGateInstance(int);

  /// starts at end and looks for last instance of gate supported on QubitRef in
  /// list and returns "true" or to BEGIN and returns "false"
  bool gotoLastQubitInstance(std::vector<QbitRef> &);

  // External-facing gate manipulators

  /// Sets the qubit operands and returns "true", or returns "false"
  bool setQubitOperands(std::vector<QbitRef> &);

  /// Sets the parametric operands and returns "true", or returns "false"
  bool setParametricOperands(std::vector<ParaRef> &);

  /// Removes the gate Qiter points to and related instructions, leaves Qiter at
  /// the next gate and returns true, or leaves Qiter as is and returns "false"
  bool removeGate();

  /// Adds the gate specified by "identifier" with qubit operands before the
  /// gate currently pointed to, leaves Qiter at new gate and returns true, or
  /// leaves Qiter as is and returns "false" If QbitRef and ParaRef Lists don't
  /// match those of the identifier, then the retun is "false".
  bool insertGate(int identifier, std::vector<QbitRef> &,
                  std::vector<ParaRef> &);

  // For gates which are known to have no parameters
  bool insertGate(int identifier, std::vector<QbitRef> &q) {
    std::vector<ParaRef> temp;
    return insertGate(identifier, q, temp);
  }

  // Changes the gate type to "identifier"
  // This will check that arguments are the same and return false if not the
  // same
  bool changeGate(int identifier);

  friend Value *fleq::addQbitCall(Instruction *, aqcc::QbitRef, bool, bool);

protected:
  // Internal helper functions

  // Returns bool value as to where q_iterator_ is currently point at a quantum
  // gate it does this by first check to see if it is a call instance, and then
  // checks to see if the function name can be found in the qmetadata. This also
  // sets the cur_gate_data_ pointer
  bool is_qgate();

  /// Backtraces through FUNCTION_ instructions to find qubit operands returns

  QbitRef &qubit_backtrace_helper(unsigned);

  /// Bactraces through FUNCTION_ instructions to find parametric operands
  ParaRef &parametric_backtrace_helper(unsigned);

public:
  // backtrace split for use in other functions
  QbitRef qubit_backtrace_helper(Instruction *, unsigned);

  // backtrace split for use in other functions
  ParaRef parametric_backtrace_helper(Instruction *, unsigned);

protected:
  // The following are private IR manipulators

  // first this calls add_qubit_call to add qubit call instructions immediately
  // before Instruction. second, this uses replaceAllUsesWith between the input
  // and new created call and third, we call remove_qubit_call to clean up the
  // now dead instructions
  bool replace_qbit_call(Instruction *, unsigned, QbitRef &);

  // This traces back and removes dead instructions regarding a qubit call
  bool remove_qbit_call(Value *);

  // This adds the necessary instructions to generate a call to QbitRef and the
  // returns the SSA Value to be included. bool argument determines if the
  // Instructions are added before(false) or after(true) the input Instruction
  // and if the qbit being passed in is a pointer or not
  Value *add_qbit_call(Instruction *, QbitRef &, bool, bool);

  // first this calls add_para_call to add para call instructions immediately
  // before Instruction. second, this uses replaceAllUsesWith between the input
  // and new created call third, we call remove_para_call to clean up the now
  // dead instructions
  bool replace_para_call(Instruction *, unsigned, ParaRef &);

  // This adds the necessary instructions to generate a call to ParaRef and
  // returns the SSA Value to be included. bool argument determines if the
  // Instructions are added before(false) or after(true) the input Instruction
  Value *add_para_call(Instruction *, ParaRef &, bool);
};

class QBBIter : public QIter {

protected:
  // "constant" pointer to metadata
  using QIter::q_GATEMETADATA_;
  /// pointer to gate data for current gate pointed to by QIter
  using QIter::cur_gate_data_;
  /// current gate QBBIter points to
  using QIter::q_iterator_;

  // /// static map getter for updating qiter_range
  // /// This is static so that the range can be updated between different
  // /// QIter instances when the being or end are changed by the adding or
  // /// removal of quantum instructions.
  static std::map<BasicBlock *, qiter_range> &getInstance_q_BB_RANGE_MAP_();

  /// "constant" pointer to function
  BasicBlock *BB_;
  using QIter::FUNCT_;

  /// returns iterator pointing to the first Q call
  inst_iterator &q_BEGIN() override {
    return getInstance_q_BB_RANGE_MAP_()[BB_].first;
  }
  /// returns iterator pointing to the last Q call
  inst_iterator &q_END() override {
    return getInstance_q_BB_RANGE_MAP_()[BB_].second;
  }

  // for internal use only, thus it is protected
  QBBIter() = default;

public:
  /// The constructor requires you to pass the BasicBlock it points into
  QBBIter(BasicBlock &B);

  // This constructor initializes on the current or next nearest gate to
  // inst_iterator
  QBBIter(BasicBlock &B, inst_iterator it);

  ~QBBIter(){};

  BasicBlock *getBasicBlock() { return BB_; }

  friend class ParaRef;
};

} // namespace aqcc
} // namespace llvm

#endif // LLVM_AQCC_QITER_H
