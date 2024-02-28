//===- QuantumRefs.h - ----------------------------------------------------===//
//
// Copyright (C) 2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//
//===----------------------------------------------------------------------===//
//
// This file describes the different reference types, QbitRefs, QRefArrays and
// ParaRef types.  It containshelper functions to get allocations and check
// equality between different references.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_AQCC_QREFS_H
#define LLVM_AQCC_QREFS_H

#include "llvm/Analysis/ConstantFolding.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/InstIterator.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Module.h"
#include "llvm/Support/raw_ostream.h"

#include <iostream>
#include <list>
#include <map>
#include <optional>
#include <set>
#include <sstream>
#include <string>
#include <vector>

using namespace llvm;

namespace llvm {
namespace aqcc {

/// primary type for angle parameters is double
typedef double ParaType;
class QIter;
class QBBIter;
class QbitRef;
class QRefArray;
class ParaRef;
template <class Module_t, class Variable, class Coeficient> class Polynomial;
typedef Polynomial<Module, ParaRef, double> ParaRefPolynomial;

///////////////////////////////////////////////////////////////////////////////
// QBitRef retrieval helper
QbitRef getQbitRefPointedToBy(Value *load_ref);

////////////////////////////////////////////////////////////////////////////////
// helper for returning a string representing the LLVM Value

/// passed bool is used for recursive method of the function
std::string getPrettyName(const ParaRef &);
std::string getPrettyName(const QbitRef &);
std::string getPrettyName(Value *, bool maybe_bracket = false);

class QbitRef {

protected:
  /// llvm::AllocaInst reference to qubit instance or array allocation
  /// instruction
  Value *reg_ptr_ = nullptr;
  /// llvm::Value reference to index in qubit array
  /// If there is no array, this points to a ConstantInt =0;
  Value *index_ptr_ = nullptr;

  // ownership is given by a static map so that QbitRefs can be safely copied
  // without generating contradictions in ownership
  /// NOTE: Replacing the static member variable with a static getter function
  /// to ensure that when used / accessed, the variable is initialized.
  // // static std::map<Value *, bool> INDEX_OWNER_MAP_;
  static std::map<Value *, bool> &getInstance_INDEX_OWNER_MAP_();

  bool is_index_owned();
  void set_index_owned(bool);

public:
  // Constructors
  /// If any constructor arguments fail to meet their requirements,
  /// the ctor defaults the the default which is null and can be checked
  /// via the functions isNull()

  /// Defaults to nullptr
  QbitRef(){};

  /// constructor for new QbitRef with no index
  /// inst_iterator must be an AllocaInst
  QbitRef(inst_iterator);

  /// constructor for new QbitRef with no index
  /// global_iterator must be an Global qbit allocation
  QbitRef(Module::global_iterator);

  /// constructor for new QbitRef with no index
  /// Value must be an AllocaInst or Argument
  QbitRef(Value *);

  /// constructor for a new QbitRef with constant index
  /// inst_iterator must point to an allocation instruction
  QbitRef(inst_iterator, unsigned);

  /// constructor for a new QbitRef with constant index
  /// global_iterator must point to a Global qubit allocation
  QbitRef(Module::global_iterator, unsigned);

  /// constructor for a new QbitRef with constant index
  /// Value must point to an AllocaInst or Arguement
  QbitRef(Value *, unsigned);

  /// constructor for a new QbitRef representing an QID
  QbitRef(unsigned, LLVMContext &);

  /// constructor for a new QbitRef with unresolved index
  /// inst_iterator must point to an AllocaInst
  /// Value* must point at a load or ConstantInt type
  /// This ctor checks if Value* is owned by QbitRef or some other function
  QbitRef(inst_iterator, Value *);

  /// constructor for a new QbitRef with unresolved index
  /// global_iterator must point to a Global qbit allocation
  /// Value* must point at a load or ConstantInt type
  /// This ctor checks if Value* is owned by QbitRef or some other function
  QbitRef(Module::global_iterator, Value *);

  /// constructor for a new QbitRef with unresolved index
  /// Value* (1) must point to a AllocaInst, Argument or a Global qubit
  /// allocation Value* (2) must point at a LoadInst, CastInst or ConstantInt
  /// type This ctor checks if Value* is owned by QbitRef or some other function
  QbitRef(Value *, Value *);

  // QbitRef(QbitRef& q): reg_ptr_(q.reg_ptr_), index_ptr_(q.index_ptr_){};
  // QbitRef(QbitRef &q) : QbitRef() {
  //  setRegister(q.reg_ptr_);
  //  setIndex(q.index_ptr_);
  //}

  // Copy Constructor: Rule 1 of 5
  QbitRef(const QbitRef &);

  // Move Constructor : Rule 2 of 5
  QbitRef(QbitRef &&) noexcept;

  // Copy Assignment : Rule 3 of 5
  QbitRef &operator=(const QbitRef &);

  // Move Assignment : Rule 4 of 5
  QbitRef &operator=(QbitRef &&) noexcept;

  // Destructor : Rule 5 of 5
  ~QbitRef();

  // User interface for deleting value if is owned and sets back to null.
  // This is a manual fix for a destructor problem, especially when using
  // unresolved arithmetic
  // TODO:Ideally Fix this so it is not needed
  void deleteQbitRef();

  // Is the Register or index null?
  bool isNull() const {
    return (reg_ptr_ == nullptr) && (index_ptr_ == nullptr);
  }

  // is the QbitRef a global allocation?
  bool isGlobal();

  // is the QbitRef register based on a FLEQ call?
  bool isFLEQCall() { return is_qbit_fleq_reg(); }

  // is this qbit distinguishable from the one passed?
  // for example, an allocation inside a function and arguments to that function
  // are distinguishable, but an argument is not distinquishable from a
  // global allocation. The function is generally conservative, i.e. two
  // different arguments to the same function are not considered distguishable
  bool isDistinguishable(const QbitRef &) const;

  /// Check to see if qubit index is currently resolved (CHEAP)
  bool isResolved() const;

  /// Try to propogate index to a resolved value and return Value* to
  /// ConstantInt if possible or return nullptr if it can not. This uses
  /// ConstantFoldInstruction on a load instruction, if it reaches one. Thus it
  /// can be more COSTLY than IsResolved
  Value *tryToSimplifyIndex();

  /// Uses tryToSimplify and updates index_ptr_ with outcome and returns true or
  /// returns false if tryToSimplify fails
  bool updateToSimplified();

  bool updateToSimplifiedRecursive();

  /// Returns if input has the same qubit allocation pointer
  bool isInRegister(QbitRef &) const;

  /// returns if two QbitRefs represent the same qubit
  /// If both QbitRefs can be currently resolved (uses tryToSimplify i.e. does
  /// not mutate), it will check to see if the indices are equal in value. If
  /// either is not resolved, it will check to see if value can be traced back
  /// to same integer allocation with no possible store inbetween. If the qubits
  /// can not be confirmed to be equal or unequal, the function returns -1.
  int isEqual(QbitRef &);

  // Following returns which type of qubit allocations it is
  bool isLocalAlloc() const;

  bool isArgument() const { return is_qbit_argument(); }
  bool isGlobalAlloc() { return is_qbit_global(); }
  bool isQID() const { return is_qbit_qid(); }

  Value *getRegister() const { return reg_ptr_; }

  Value *getIndex() const { return index_ptr_; }

  std::optional<unsigned> getIndexResolved() const;

  // returns LLVMContext of the register's type
  LLVMContext &getContext();

  // Returns the size of the register (may not be resolvable
  std::optional<unsigned> getRegisterSize() const;

  /// sets register

  /// inst_iterator must be a allocation instruction or QbitRef is set to Null
  void setRegister(inst_iterator);

  /// global_iterator must be a Global qbit allocatio or QbitRef is set to Null
  void setRegister(Module::global_iterator);

  /// Value must be a proper allocation or QbitRef is set to Null
  void setRegister(Value *);

  void operator=(QbitRef &q) {
    setRegister(q.reg_ptr_);
    setIndex(q.index_ptr_);
  };

  static void clearIndexOwnedMap() {
    getInstance_INDEX_OWNER_MAP_().clear();
    getInstance_INDEX_OWNER_MAP_().insert({nullptr, false});
  }

  /// sets index to a resolved value
  void setIndexResolved(unsigned);

  // Shifts index by the passed resolved value
  void shiftIndexByResolved(int);

  /// sets index to a resolved value, when it is expected
  /// their is no register to get the LLVMContext from;
  /// typically used for QIDs
  void setIndexResolved(unsigned, LLVMContext &);

  /// set index to unresolved value
  // Value must be a proper index or else QbitRef(index) is set to Null
  void setIndex(Value *);

  // Shifts index by the passed value
  void shiftIndexBy(Value *);

  // convert QbitRef to a QID which:
  //  -has no register value
  //  -index value in 16 bits
  // shift displaces the current index by a fixed number
  bool convertToQID(unsigned shift = 0);

  // These compare the pointers directly
  // DO NOT USE TO COMPARE CONTENTS OF REF; instead use isEqual function
  friend bool operator<(const QbitRef &a, const QbitRef &b) {

    if (std::less<void *>{}((void *)a.reg_ptr_, (void *)b.reg_ptr_))
      return true;
    else if (std::less<void *>{}((void *)b.reg_ptr_, (void *)a.reg_ptr_))
      return false;

    return (std::less<void *>{}((void *)a.index_ptr_, (void *)b.index_ptr_));
  }

  /// overload of == for use in sorted data structures
  // NOTE: SHOULD NOT USE FOR EQUALITY of QUBITS; USE isEqual funciton
  friend bool operator==(const QbitRef &a, const QbitRef &b) {
    return (a.reg_ptr_ == b.reg_ptr_) && (a.index_ptr_ == b.index_ptr_);
  }

  friend bool operator!=(const QbitRef &a, const QbitRef &b) {
    return (a.reg_ptr_ != b.reg_ptr_) || (a.index_ptr_ != b.index_ptr_);
  }

  // string stream printing overload
  friend raw_ostream &operator<<(raw_ostream &, const QbitRef &);

  friend QbitRef getQbitRefPointedToBy(Value *);
  friend class QRefArray;
  friend class QIter;

private:
  // Internal utility functions

  // this assumes that index_ptr_ is unresolved
  // It retrieves the LoadInst which determines the index
  LoadInst *get_index_load();

  // Is Value a qubit allocation
  // returns AllocaInst if it is
  // returns nullptr if not
  AllocaInst *is_qbit_alloc(Value *) const;
  AllocaInst *is_qbit_alloc() const { return is_qbit_alloc(reg_ptr_); }

  // Is Value a qubit argument; array or single qubit
  // returns Argument if it is
  // returns nullptr if not
  Argument *is_qbit_argument(Value *) const;
  Argument *is_qbit_argument() const { return is_qbit_argument(reg_ptr_); }

  // Is Value a qubit Global; array or single qubit
  // returns GlobalValue if it is
  // returns nullptr if not
  GlobalValue *is_qbit_global(Value *) const;
  GlobalValue *is_qbit_global() const { return is_qbit_global(reg_ptr_); }

  bool is_qbit_qid() const { return reg_ptr_ == nullptr && isResolved(); }

  CallBase *is_qbit_fleq_reg(Value *) const;
  CallBase *is_qbit_fleq_reg() const { return is_qbit_fleq_reg(reg_ptr_); }

  // does the unsigned fit in the register
  bool is_in_register(unsigned) const;

  Value *binary_op_helper(Instruction::BinaryOps, Value *) const;
};

////////////////////////////////////////////////////////////////////////////////

// This class is used for slicing and joining of qubit arrays

class QRefArray {

public:
  class iterator;

private:
  // pair elements represent (first QbitRef in an array slice, width of the
  // slice)
  std::vector<std::pair<QbitRef, unsigned>> array_list_;
  unsigned size_ = 0;
  iterator push_back_without_check(const QbitRef &q) {
    unsigned width = q.getRegisterSize().value() - q.getIndexResolved().value();
    array_list_.push_back(std::make_pair(q, width));
    size_ += width;
    return iterator(--array_list_.end());
  }

public:
  QRefArray() = default;
  ~QRefArray() = default;

  QRefArray(const QbitRef &q) { push_back(q); }

  iterator push_back(const QbitRef &q) {
    if (isAddableToArray(q)) {
      return push_back_without_check(q);
    }
    return end();
  }

  iterator push_back(const QbitRef &q, unsigned width) {
    if (isAddableToArray(q, width)) {
      array_list_.push_back(std::make_pair(q, width));
      size_ += width;
      auto out = array_list_.end();
      --out;
      return iterator(out);
    }
    return end();
  }

  bool isAddableToArray(const QbitRef &);
  bool isAddableToArray(const QbitRef &, unsigned);

  size_t size() const { return size_; }

  void clear() {
    array_list_.clear();
    size_ = 0;
  }

  QbitRef operator[](unsigned);
  QbitRef at(unsigned i) { return operator[](i); }

  // Performs a recursive DFS to build the QRefArray from the passed
  // argument and passes out a pointer to it. The underlying object
  // is owned by the function, so do not call delete on the pointer.
  // These objects are cached in the function as well to avoid
  // redundant DFS's.
  // If the build fails, nullptr is returned
  static QRefArray *get(Value *);

  // friend function
  friend QRefArray join(const QRefArray &, const QRefArray &);
  friend QRefArray slice(const QRefArray &, unsigned, unsigned);

  class iterator {
  private:
    std::vector<std::pair<QbitRef, unsigned>>::const_iterator it_;
    int idx_ = 0;

  public:
    iterator() = delete;
    iterator(std::vector<std::pair<QbitRef, unsigned>>::iterator it)
        : it_(it) {}
    iterator(std::vector<std::pair<QbitRef, unsigned>>::const_iterator it)
        : it_(it) {}
    ~iterator() = default;

    QbitRef operator*() {
      QbitRef out = it_->first;
      unsigned out_idx = (unsigned)out.getIndexResolved().value() + idx_;
      out.setIndexResolved(out_idx);
      return out;
    }

    iterator operator++() {
      idx_++;
      if (it_->second == idx_) {
        ++it_;
        idx_ = 0;
      }
      return *this;
    }

    friend iterator &advance(iterator &iter, unsigned i) {
      iter.idx_ += i;
      while (iter.idx_ >= iter.it_->second) {
        iter.idx_ -= iter.it_->second;
        ++iter.it_;
        if (iter.idx_ == 0)
          break;
      }
      return iter;
    }

    iterator operator--() {
      idx_--;
      while (idx_ < 0) {
        --it_;
        idx_ = it_->second - 1;
      }
      return *this;
    }

    friend bool operator==(iterator a, iterator b) {
      if (a.it_ == b.it_) {
        return a.idx_ == b.idx_;
      }
      return false;
    }

    friend bool operator!=(iterator a, iterator b) { return !(a == b); }

    friend class QRefArray;
    friend QRefArray slice(const QRefArray &, unsigned, unsigned);
  };

  iterator begin() { return iterator(array_list_.begin()); }
  iterator end() { return iterator(array_list_.end()); }
  iterator begin() const { return iterator(array_list_.begin()); }
  iterator end() const { return iterator(array_list_.end()); }

  friend raw_ostream &operator<<(raw_ostream &, const QRefArray &);
  // check that the QRefArray represents the equivalent sequence of qubits as
  // the vector of QbitRefs
  bool operator==(const std::vector<QbitRef> &);
};

////////////////////////////////////////////////////////////////////////////////

class ParaRef {

private:
  Value *value_ptr_ = nullptr;
  Type *PR_TYPE_ = nullptr;

  // ownership is given by a static map so that ParaRefs can be safely copied
  // without generating contradictions in ownership
  /// NOTE: Replacing the static member variable with a static getter function
  /// to ensure that when used / accessed, the variable is initialized.
  // // static std::map<Value *, bool> VALUE_OWNER_MAP_;
  static std::map<Value *, bool> VALUE_OWNER_MAP_;
  static std::map<Value *, bool> &getInstance_VALUE_OWNER_MAP_();

  bool is_value_owned() const;
  void set_value_owned(bool);
  bool is_cbit_ref(Value *) const;

  static Module *StoredModule;

public:
  // constructors
  /// If any of the arguments don't meet their requirements, ParaRef is set to
  /// Null which can be checked with isNull()

  ParaRef() = default;

  /// Constructor from instruction iterator
  ParaRef(inst_iterator);

  /// Constructor from Value
  ParaRef(Value *);

  /// contructor from contant value
  // need type to resolve properly initialize cast and get LLVMContext
  ParaRef(ParaType, Type *);

  /// Same as the above but defaults to a double type
  /// NOTE: if you use this function, be sure to set the LLVM::Module using the
  /// static method "setModule" so that it can get the LLVMContext
  ParaRef(ParaType);

  // Copy Constructor: Rule 1 of 5
  ParaRef(const ParaRef &);

  // Move Constructor : Rule 2 of 5
  ParaRef(ParaRef &&) noexcept;

  // Copy Assignment : Rule 3 of 5
  ParaRef &operator=(const ParaRef &);

  // Move Assignment : Rule 4 of 5
  ParaRef &operator=(ParaRef &&) noexcept;

  // Destructor : Rule 5 of 5
  ~ParaRef();

  // User interface for deleting value if is owned and sets back to null.
  // This is a manual fix for a destructor problem, especially when using
  // unresolved arithmetic
  // DON'T USE!
  void deleteParaRef();

  // This deletes all values owned by ParaRef
  // NOTE: Best practice is to call this at the end of any code which
  // uses any arithmetic or other non-inserted values passed to ParaRef
  // even if one expects that all instructions were inserted.
  static void deleteAllOwnedParaRefs();

  static void clearValueOwnedMap() {
    deleteAllOwnedParaRefs();
    getInstance_VALUE_OWNER_MAP_().clear();
    getInstance_VALUE_OWNER_MAP_().insert({nullptr, false});
  }

  static void clearUnownedInValueOwnedMap() {
    auto &VOM = getInstance_VALUE_OWNER_MAP_();
    auto it = VOM.begin();
    auto end = VOM.end();
    while (it != end) {
      if (!(it->second)) {
        it = VOM.erase(it);
      } else
        ++it;
    }
  }

  static void setModule(Module &M) { StoredModule = &M; }

  /// Is the ParaRef null?
  bool isNull() const { return value_ptr_ == nullptr; }

  /// Is the value curently resolved to a constant
  bool isResolved() const;

  /// boolean getter for ParaRef Type
  bool isFloatingPoint() const { return PR_TYPE_->isFloatingPointTy(); }

  bool isInteger() const { return PR_TYPE_->isIntegerTy(); }

  bool isCbitRef() const { return is_cbit_ref(value_ptr_); }

  /// resolves datalists as a std::string to use for simplification
  static std::pair<bool, std::string> getDataList(Value *);

  /// Try to simplify the Value and return an equivalent (may be owned by
  /// ParaRef). the return will be the same Value if no simplifications are
  /// found.
  Value *tryToSimplify();

  /// Try to simplify using the tryToSimplify() function. If a simplified
  /// version is found, the Value is updated here and in IR (if already inserted
  /// in IR) and returns true. If no simplification is found, returns false.
  bool updateToSimplified();

  /// Similar to the above but this function uses a much more aggressive
  /// recursive method to simplifiy and update arguments to instruction ParaRefs
  /// and so on for descendant instructions before trying to simplify this one.
  bool updateToSimplifiedRecursive();

  /// The same as the above but a set of previously visited PHI nodes is passed
  /// to avoid loops in the dependencies of the contained value.
  bool updateToSimplifiedRecursive(std::set<PHINode *> &);

  /// If ParaRef is an unowned, unresolved Value (Instruction) removed from
  /// parent Basic block and set value as owned by ParaRef returns ownership of
  /// Value (i.e. true = is owned by ParaRef now)
  bool removeFromParent();

  /// same as above but also erases it from IR and the value_owned_map
  void eraseFromParent();

  /// returns if two ParaRefs represent the same value
  /// If both ParaRefs can be currently resolved (uses tryToSimplify i.e. does
  /// not mutate), If either is not resolved, it will check to see if value can
  /// be traced back to same ParaType allocation with no possible store
  /// inbetween. If the parameter value can not be confirmed to be equal or
  /// unequal, the function returns -1.
  int isEqual(ParaRef &);

  Value *getValue() const { return value_ptr_; }

  Type *getType() const { return PR_TYPE_; }

  std::optional<ParaType> getValueResolved() const;

  /// Set value to a resolved constant with Type
  /// NOTE: make sure if this is new, get a type from the function/module to get
  /// proper LLVMContext
  void setValueResolved(ParaType, Type *);

  /// Same as the above but defaults a double type
  /// NOTE: if you use this function, be sure to set the LLVM::Module using the
  /// static method "setModule" so that it can get the LLVMContext
  void setValueResolved(ParaType);

  /// assignment overload
  ParaRef &operator=(ParaType val) {
    setValueResolved(val);
    return *this;
  }

  /// Set value to that represented by inst_iterator
  void setValue(inst_iterator);

  /// Set value to desired value
  void setValue(Value *);

  /// These compare the pointers directly
  /// DO NOT USE TO COMPARE CONTENTS OF REF; instead use isEqual function
  friend bool operator<(const ParaRef &a, const ParaRef &b) {
    return a.value_ptr_ < b.value_ptr_;
  }

  friend bool operator==(const ParaRef &a, const ParaRef &b) {
    return a.value_ptr_ == b.value_ptr_;
  }
  friend bool operator!=(const ParaRef &a, const ParaRef &b) {
    return a.value_ptr_ != b.value_ptr_;
  }

  /// Arithemetic operator overloads
  /// in all cases, if both values are resolved, the return is the resolved
  /// version representing the arithmetic outcome. If one of them is unresolved,
  /// the return is an unresolved instruction which represents the arithmetic
  /// operation, and is owned by the ParaRef. These can then be inserted via the
  /// QIter NOTE: for inplace operators, the ParaRef acted on must not own its
  /// Value, or a assert is thrown. Currently only support arithmetic for
  /// ParaType = double

  ParaRef operator-() const;

  // Other ParaRefs
  friend ParaRef operator+(const ParaRef &, const ParaRef &);
  friend ParaRef operator-(const ParaRef &, const ParaRef &);
  friend ParaRef operator*(const ParaRef &, const ParaRef &);
  friend ParaRef operator/(const ParaRef &, const ParaRef &);
  void operator+=(const ParaRef &);
  void operator-=(const ParaRef &);
  void operator*=(const ParaRef &);
  void operator/=(const ParaRef &);

  // Constants
  ParaRef operator+(ParaType);
  ParaRef operator-(ParaType);
  ParaRef operator*(ParaType);
  ParaRef operator/(ParaType);
  void operator+=(ParaType);
  void operator-=(ParaType);
  void operator*=(ParaType);
  void operator/=(ParaType);

  // helper functions for get a ParaRef representing a function call

  /// returns a quantum runtime support math function
  /// which returns a sign (double) from the integer ParaRef passed
  /// such that if the "pos" bit of that integer is 0, the sign is +1
  /// and if the bit is 1, then the sign is -1.
  friend ParaRef getCallToSignFromInt(ParaRef para_int, unsigned pos, Module *);

  /// returns a function call which represents val ^ pow
  friend ParaRef takePower(Module &, const ParaRef &val, int pow);

private:
  Value *binary_op_helper(Instruction::BinaryOps opcode, Value *other_op) const;

  friend raw_ostream &operator<<(raw_ostream &, const ParaRef &);

  friend QIter;
};

} // namespace aqcc
} // namespace llvm

////////////////////////////////////////////////////////////////////////////////

#endif // LLVM_AQCC_QREFS_H
