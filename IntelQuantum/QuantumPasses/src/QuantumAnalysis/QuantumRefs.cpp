//===- QuantumRefs.cpp ----------------------------------------------------===//
//
// Copyright (C) 2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//
//===----------------------------------------------------------------------===//

#include "IntelQuantumPasses/QuantumAnalysis/QuantumRefs.h"
#include "IntelQuantumPasses/QuantumAnalysis/QuantumClassicalSimplify.h"
#include "IntelQuantumPasses/QuantumAnalysis/QuantumIterator.h"
#include "IntelQuantumPasses/QuantumUtils/QuantumFLEQUtils.h"
#include "IntelQuantumPasses/QuantumUtils/QuantumGeneralUtils.h"
#include "llvm/Analysis/InstructionSimplify.h"
#include "llvm/Demangle/Demangle.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/IR/Intrinsics.h"
#include "llvm/IR/IntrinsicsFLEQ.h"
#include "llvm/IR/PassManager.h"
#include "llvm/IR/PatternMatch.h"
#include "llvm/IR/Type.h"
#include "llvm/Pass.h"
#include "llvm/Transforms/Utils/BasicBlockUtils.h"

#include <cmath>
#include <map>
#include <optional>

#define DEBUG_TYPE "quantum-iterator"

using namespace llvm;

namespace llvm {
namespace aqcc {

Module *ParaRef::StoredModule = nullptr;

std::map<Value *, bool> &QbitRef::getInstance_INDEX_OWNER_MAP_() {
  static std::map<Value *, bool> INDEX_OWNER_MAP_ = {{nullptr, false}};
  return INDEX_OWNER_MAP_;
}

std::map<Value *, bool> &ParaRef::getInstance_VALUE_OWNER_MAP_() {
  static std::map<Value *, bool> VALUE_OWNER_MAP_ = {{nullptr, false}};
  return VALUE_OWNER_MAP_;
}

int is_unresolved_but_equal(Value *, Value *);

std::string getPrettyName(Value *);

////////////////////////////////////////////////////////////////////////////////
// QbitRef Constructors/Destructor

QbitRef::QbitRef(Module::global_iterator reg_iter) { setRegister(reg_iter); }

QbitRef::QbitRef(Value *reg_ptr) { setRegister(reg_ptr); }

QbitRef::QbitRef(Instruction *reg_iter, unsigned ind) {
  setRegister(reg_iter);
  setIndexResolved(ind);
}

QbitRef::QbitRef(Module::global_iterator reg_iter, unsigned ind) {
  setRegister(reg_iter);
  setIndexResolved(ind);
}

QbitRef::QbitRef(Value *reg_ptr, unsigned ind) {
  setRegister(reg_ptr);
  setIndexResolved(ind);
}

QbitRef::QbitRef(Instruction *reg_iter, Value *index) {
  setRegister(reg_iter);
  setIndex(index);
}

QbitRef::QbitRef(Module::global_iterator reg_iter, Value *index) {
  setRegister(reg_iter);
  setIndex(index);
}

QbitRef::QbitRef(Value *reg_ptr, Value *index) {
  setRegister(reg_ptr);
  setIndex(index);
}

QbitRef::QbitRef(unsigned ind, LLVMContext &C) {
  setRegister(nullptr);
  setIndexResolved(ind, C);
}

///////////////////////////////////////
// "Rule of five" definitions

QbitRef::QbitRef(const QbitRef &other)
    : reg_ptr_(other.reg_ptr_), index_ptr_(other.index_ptr_) {}

QbitRef::QbitRef(QbitRef &&other) noexcept
    : reg_ptr_(other.reg_ptr_), index_ptr_(other.index_ptr_) {

  other.reg_ptr_ = nullptr;
  other.index_ptr_ = nullptr;
}

QbitRef &QbitRef::operator=(const QbitRef &other) {

  setRegister(other.reg_ptr_);
  setIndex(other.index_ptr_);

  return *this;
}

QbitRef &QbitRef::operator=(QbitRef &&other) noexcept {

  setRegister(other.reg_ptr_);
  setIndex(other.index_ptr_);

  if (other != *this) {
    other.reg_ptr_ = nullptr;
    other.index_ptr_ = nullptr;
  }

  return *this;
}

QbitRef::~QbitRef() {
  // setValue(nullptr);
}

void QbitRef::deleteQbitRef() {
  if (is_index_owned())
    index_ptr_->deleteValue();
  set_index_owned(false);
  reg_ptr_ = nullptr;
  index_ptr_ = nullptr;
}

////////////////////////////////////////////////////////////////////////////////
// Resolution Functions

bool QbitRef::isResolved() const {

  if (!index_ptr_)
    return false;

  if ((dyn_cast<ConstantInt>(index_ptr_)))
    return true;
  return false;
}

Value *QbitRef::tryToSimplifyIndex() {

  if (!index_ptr_)
    return nullptr;

  // since index in an integer, we use ParaRef to
  // perform the simplification
  ParaRef idx(index_ptr_);
  return idx.tryToSimplify();
}

bool QbitRef::updateToSimplified() {

  if (is_qbit_qid())
    return false;

  ParaRef idx(index_ptr_);
  bool idx_updated = idx.updateToSimplified();

  // if the idx is resolved and reg is FLEQ
  // try to reduce to a non-FLEQ equivalent
  if (idx.isResolved() && (is_qbit_fleq_reg())) {
    QRefArray *array = QRefArray::get(reg_ptr_);
    if (array) {
      operator=((*array)[(unsigned)idx.getValueResolved().value()]);
      return true;
    }
  }

  return idx_updated;
}

bool QbitRef::updateToSimplifiedRecursive() {

  if (isNull())
    return false;

  if (is_qbit_qid())
    return false;

  ParaRef idx(index_ptr_);
  bool idx_updated = idx.updateToSimplifiedRecursive();

  // if the idx is resolved and reg is FLEQ
  // try to reduce to a non-FLEQ equivalent
  if (idx.isResolved() && (is_qbit_fleq_reg())) {
    QRefArray *array = QRefArray::get(reg_ptr_);
    if (array) {
      if (array->size() == 0) {
        reg_ptr_ = nullptr;
        index_ptr_ = nullptr;
      } else
        operator=((*array)[(unsigned)idx.getValueResolved().value()]);
      return true;
    }
  }

  if (idx_updated) {
    setIndex(idx.getValue());
  }

  return idx_updated;
}

////////////////////////////////////////////////////////////////////////////////////////////
// Getters

bool QbitRef::isGlobal() { return (is_qbit_global() != nullptr); }

bool QbitRef::isInRegister(QbitRef &q) const { return reg_ptr_ == q.reg_ptr_; }

bool QbitRef::isLocalAlloc() const {
  if (is_qbit_alloc())
    return true;
  else if (CallBase *CB = is_qbit_fleq_reg()) {
    return (CB->getCalledFunction()->getName()).contains(fleq::DATALIST_ALLOC);
  }
  return false;
}

std::optional<unsigned> QbitRef::getRegisterSize() const {

  std::optional<unsigned> out;
  out.reset();

  if (AllocaInst *AI = is_qbit_alloc()) {

    // Is the allocation an array
    if (ArrayType *ArTy = dyn_cast<ArrayType>(AI->getAllocatedType())) {
      // is this an array of integer
      out = ArTy->getNumElements();
    }

    // Is this an allocation of integers
    else if ((dyn_cast<IntegerType>(AI->getAllocatedType()))) {
      if (ConstantInt *CI = dyn_cast<ConstantInt>(AI->getArraySize())) {
        out = CI->getZExtValue();
      } else {
        errs() << "ERROR: Quantum SDK - class QbitRef says: Array size not an "
                  "integer type.\n";
      }
    }
    // Is this an allocation of a pointer
    else if ((dyn_cast<PointerType>(AI->getAllocatedType()))) {
      if (ConstantInt *CI = dyn_cast<ConstantInt>(AI->getArraySize())) {
        out = CI->getZExtValue();
      } else {
        std::string ErrMsg = "Array size not an integer type.\n";
        displayErrorAndExit("QbitRef", ErrMsg);
      }
    }

  }

  // is register an argument
  else if (Argument *Arg = is_qbit_argument(reg_ptr_)) {
    // is the argument an array
    if (ArrayType *ArTy = dyn_cast<ArrayType>(Arg->getType())) {
      out = ArTy->getNumElements();
    }

    // Is the argument an integer
    else if ((dyn_cast<IntegerType>(Arg->getType()))) {
      out = 1;
    } else if ((dyn_cast<PointerType>(Arg->getType()))) {
      out.reset();
    }
  }

  else if (GlobalValue *GV = is_qbit_global()) {

    // Is this a pointer
    if (PointerType *PtrTy = dyn_cast<PointerType>(GV->getType())) {
      // Is the pointer to a integer
      Type *ValueTy = GV->getValueType();
      if (IntegerType *ITy = dyn_cast<IntegerType>(ValueTy)) {
        out = 1;
      }
      // Is the pointer to an Array
      else if (ArrayType *ArTy = dyn_cast<ArrayType>(ValueTy)) {
        out = ArTy->getNumElements();
      }
    }

  } else if (CallBase *CB = is_qbit_fleq_reg()) {

    if ((CB->getCalledFunction()->getName()).contains(fleq::DATALIST_ALLOC)) {
      ParaRef para_sz(CB->getArgOperand(1));
      para_sz.updateToSimplifiedRecursive();
      if (para_sz.isResolved())
        out = (unsigned)para_sz.getValueResolved().value();
      else
        out.reset();
    } else {
      QRefArray *array = QRefArray::get(reg_ptr_);
      if (array)
        out = array->size();
      else
        out.reset();
    }
  } else {
    std::string ErrMsg = "register type was not a qbit type.\n";
    displayErrorAndExit("QbitRef", ErrMsg);
  }

  return out;
}

bool QbitRef::isDistinguishable(const QbitRef &other) const {

  if (isNull() || other.isNull())
    return false;

  // if this is qid, the other has to also be qid
  if (is_qbit_qid()) {
    if (other.is_qbit_qid())
      return true;
    return false;
  }

  // clearly, if the registers are exactly the same, they are distinct
  if (reg_ptr_ == other.reg_ptr_)
    return true;

  // if either is a FLEQ call, then there is no way to
  // know source without extra work so we return false
  if ((is_qbit_fleq_reg()))
    return false;
  if ((other.is_qbit_fleq_reg()))
    return false;

  if (AllocaInst *AI = is_qbit_alloc()) {
    if (Argument *Arg = other.is_qbit_argument()) {
      // check they belong to the same function
      return AI->getParent()->getParent() == Arg->getParent();
    } else if (AllocaInst *AIother = other.is_qbit_alloc()) {
      return AI->getParent()->getParent() == AIother->getParent()->getParent();
    }
  } else if ((is_qbit_global())) {
    if ((other.is_qbit_argument())) {
      // no way to know here so false
      return false;
    }
  } else if (Argument *Arg = is_qbit_argument()) {
    if ((other.is_qbit_global()))
      return false;
    else if (Argument *Argother = other.is_qbit_argument()) {
      return Arg->getParent() == Argother->getParent();
    } else if (AllocaInst *AIother = other.is_qbit_alloc()) {
      return Arg->getParent() == AIother->getParent()->getParent();
    }
  }

  return true;
}

int QbitRef::isEqual(const QbitRef &q) const {

  // most obvious case, check registers
  if (!isDistinguishable(q))
    return -1;
  if (reg_ptr_ != q.reg_ptr_)
    return 0;

  else if (!index_ptr_ || !q.index_ptr_)
    return -1;

  if (index_ptr_ == q.index_ptr_)
    return 1;

  // Now try harder: check if we can resolve index to be equal
  ParaRef idx(index_ptr_);
  ParaRef q_idx(q.index_ptr_);

  return idx.isEqual(q_idx);
}

std::optional<unsigned> QbitRef::getIndexResolved() const {
  std::optional<unsigned> out;
  out.reset();
  if (ConstantInt *CInt = dyn_cast<ConstantInt>(index_ptr_)) {
    out = CInt->getZExtValue();
  }
  return out;
}

LLVMContext &QbitRef::getContext() {

  if (reg_ptr_)
    return reg_ptr_->getContext();
  return index_ptr_->getContext();
}

/////////////////////////////////////////////////////////////////////////////////////////////
// manipulators

void QbitRef::setRegister(Module::global_iterator reg_iter) {

  Value *temp = dyn_cast<Value>(&*reg_iter);
  setRegister(temp);
}

void QbitRef::setRegister(Value *reg_ptr) {

  // We set reg_ptr_ to be the output of is_qbit_argument in case we find an
  // argument
  if (reg_ptr == nullptr) {
    reg_ptr_ = nullptr;
  } else if (Argument *Arg = is_qbit_argument(reg_ptr))
    reg_ptr_ = dyn_cast<Value>(Arg);
  else if ((is_qbit_alloc(reg_ptr)))
    reg_ptr_ = reg_ptr;
  else if ((is_qbit_global(reg_ptr))) {
    reg_ptr_ = reg_ptr;
  } else if ((is_qbit_fleq_reg(reg_ptr)))
    reg_ptr_ = reg_ptr;
  else {
    LLVM_DEBUG(dbgs() << "QbitRef Error: register must be set to an "
                         "AllocaInst, GlobalValue or Argument.\n");
    reg_ptr_ = nullptr;
    index_ptr_ = nullptr;
  }

  // Set Index to zero if it is not already set
  if ((reg_ptr_)) {
    if (!(index_ptr_))
      setIndexResolved(0);
  }
}

void QbitRef::setIndexResolved(unsigned ind) {

  if (is_index_owned()) {
    set_index_owned(false);
    index_ptr_->deleteValue();
  }

  // if reg_ptr_ is null, we assume this is meant to be a QID. In that case,
  // the type should be i16 instead of i64, the default.
  if (reg_ptr_ == nullptr) {
    index_ptr_ = nullptr;
  } else if (is_in_register(ind)) {
    index_ptr_ = dyn_cast<Value>(
        ConstantInt::get(IntegerType::get(getContext(), 64), ind));
    set_index_owned(false);
  } else {
    errs() << "QbitRef Error: index " << ind
           << " is larger than qubit array for "
           << getPrettyName(reg_ptr_, false) << "\n";
    index_ptr_ = nullptr;
  }
}

void QbitRef::shiftIndexByResolved(int shift) {
  if (isResolved()) {
    int idx = getIndexResolved().value();
    idx += shift;
    if (idx < 0) {
      std::optional<unsigned> sz = getRegisterSize();
      if (!sz.has_value()) {
        errs() << "QbitRef Error: shifting " << (idx - shift) << " by " << shift
               << " goes past 0 for " << getPrettyName(reg_ptr_, false) << "\n";
        index_ptr_ = nullptr;
        return;
      }
      idx = (sz.value() + idx) % sz.value();
    }
    setIndexResolved((unsigned)idx);
  } else {
    Value *sft_val = dyn_cast<Value>(
        ConstantInt::get(IntegerType::get(getContext(), 64), (unsigned)shift));
    shiftIndexBy(sft_val);
  }
}

void QbitRef::setIndexResolved(unsigned ind, LLVMContext &C) {
  if (reg_ptr_) {
    setIndexResolved(ind);
  } else {
    index_ptr_ =
        dyn_cast<Value>(ConstantInt::get(IntegerType::get(C, 16), ind));
    set_index_owned(false);
  }
}

void QbitRef::shiftIndexBy(Value *shift) {
  if (isResolved()) {
    if (ConstantInt *CInt = dyn_cast<ConstantInt>(shift)) {
      shiftIndexByResolved(CInt->getSExtValue());
      return;
    } else {
      if (getIndexResolved().value() == 0)
        setIndex(shift);
      return;
    }
  }
  setIndex(binary_op_helper(Instruction::Add, shift));
}

void QbitRef::setIndex(Value *index) {

  if (index == index_ptr_)
    return;

  if (is_index_owned()) {
    set_index_owned(false);
    index_ptr_->deleteValue();
  }

  if (index == nullptr) {
    index_ptr_ = nullptr;
    return;
  } else if (index->getType()->isIntegerTy()) {
    index_ptr_ = index;
  } else {
    errs() << "QbitRef Error: Value reference is not an integer type: "
           << (*index) << "\n";
    index_ptr_ = nullptr;
    return;
  }

  set_index_owned(false);

  if (!isResolved()) {
    if (Instruction *I = dyn_cast<Instruction>(index_ptr_)) {
      if (BasicBlock *BB = (I->getParent())) {
        if (!(BB->getParent()))
          set_index_owned(true);
      } else
        set_index_owned(true);

      // Check reg and index are owned by same function or same Module in the
      // case of Global
      if (!is_index_owned()) {
        if (AllocaInst *AI = is_qbit_alloc()) {
          if (AI->getFunction() != I->getFunction()) {
            errs() << "QbitRef Error: alloc reg_ptr_ and index_ptr_ must "
                      "belong to the same Function.\n";
            index_ptr_ = nullptr;
          }
        } else if (Argument *Arg = is_qbit_argument()) {
          if (Arg->getParent() != I->getFunction()) {
            errs() << "QbitRef Error: argument reg_ptr_ and index_ptr_ must "
                      "belong to the same Function.\n";
            index_ptr_ = nullptr;
          }
        } else if (GlobalValue *GV = is_qbit_global()) {
          if (GV->getParent() != I->getModule()) {
            errs() << "QbitRef Error: Global reg_ptr_ and index_ptr_ must "
                      "belong to the same Module.\n";
            index_ptr_ = nullptr;
          }
        } else if (CallBase *CB = is_qbit_fleq_reg()) {
          if (CB->getParent()->getParent() != I->getParent()->getParent()) {
            errs() << "QbitRef Error: FLEQ reg_ptr_ and index_ptr_ must "
                      "belong to the same Function.\n";
            index_ptr_ = nullptr;
          }
        }
      }
    }
  } else if (!reg_ptr_) {
    if (!convertToQID()) {
      errs() << "QbitRef Error: Found a null register (QID) which is not a "
                "resolved constant.\n";
    }
  }
}

bool QbitRef::convertToQID(unsigned shift) {

  if (is_qbit_qid())
    return true;

  LLVMContext &C = getContext();
  reg_ptr_ = nullptr;

  if (isResolved()) {
    unsigned qid = shift + (*getIndexResolved());
    if (qid / 65536 > 0)
      errs() << "QbitRef Error: Coversion to QID resulted in a bit overflow.\n";
    index_ptr_ =
        dyn_cast<Value>(ConstantInt::get(IntegerType::get(C, 16), qid));
    set_index_owned(false);
    return true;
  }

  return false;
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Internal utility functions

LoadInst *QbitRef::get_index_load() {

  LoadInst *LI = nullptr;

  // Check if index is a LoadInst
  if ((LI = dyn_cast<LoadInst>(index_ptr_))) {
  }

  // Check if index is a CastInst: recasts type to some other type
  else if (CastInst *CastI = dyn_cast<CastInst>(index_ptr_)) {

    // check that this is a cast between integer types
    if (CastI->isIntegerCast()) {
      Value *op_ptr = CastI->getOperand(0);

      /* removed for now as this shouldn't happen
      //check if operand is a constant
      if( ConstantInt *ConstInt = dyn_cast<ConstantInt>(op_ptr) ) {
        return dyn_cast<Value>(ConstInt);
      }
      */

      // check if operand is a LoadInst
      if ((LI = dyn_cast<LoadInst>(op_ptr))) {
      }

    } else {
      std::string ErrMsg = "get_index_load: index_ptr_ is a CastInst "
                           "between not integer types.\n";
      displayErrorAndExit("QbitRef", ErrMsg);
    }
  }

  // Could be a argument in which case we return nullptr

  return LI;
}

AllocaInst *QbitRef::is_qbit_alloc(Value *alloc) const {

  // Is this a direct qubit allocation
  if (alloc == nullptr) {
    return nullptr;
  } else if (AllocaInst *AI = dyn_cast<AllocaInst>(alloc)) {
    // Is the allocation an array
    if (ArrayType *ArTy = dyn_cast<ArrayType>(AI->getAllocatedType())) {
      // is this an array of integer
      if (IntegerType *ITy =
              dyn_cast<IntegerType>(ArTy->getArrayElementType())) {
        if (ITy->getBitWidth() == 16)
          return AI;
      }
    }
    // If the allocation is a pointer, we can not tell anything about
    // the allocation itself due to opaque pointers.  We can potentially
    // explore the uses to derive if it is a qbit allocation.
    else if (PointerType *PtrTy =
                 dyn_cast<PointerType>(AI->getAllocatedType())) {
      // not used for now as it is unlikely a point to set of pointers will
      // be generated outside of an array context.
    }
    // Is the Allocation an integer
    else if (IntegerType *ITy = dyn_cast<IntegerType>(AI->getAllocatedType())) {
      if (ITy->getBitWidth() == 16)
        return AI;
    }
  }

  return nullptr;
}

GlobalValue *QbitRef::is_qbit_global(Value *global) const {
  if (global == nullptr) {
    return nullptr;
  }
  // Is this a direct qubit allocation
  else if (GlobalValue *GV = dyn_cast<GlobalValue>(global)) {
    // Is the global variable a pointer
    if (PointerType *PtrTy = dyn_cast<PointerType>(GV->getType())) {
      // Is the pointer to a variable
      Type *ValueTy = GV->getValueType();
      if (IntegerType *ITy = dyn_cast<IntegerType>(ValueTy)) {
        if (ITy->getBitWidth() == 16)
          return GV;
      }
      // Is the pointer to an Array
      else if (ArrayType *ArTy = dyn_cast<ArrayType>(ValueTy)) {
        // is this an array of integer
        if (IntegerType *ITy =
                dyn_cast<IntegerType>(ArTy->getArrayElementType())) {
          if (ITy->getBitWidth() == 16)
            return GV;
        }
      }
    }
  }

  return nullptr;
}

Argument *QbitRef::is_qbit_argument(Value *arg) const {

  if (arg == nullptr) {
    return nullptr;
  }
  // Is this a Function argument
  else if (Argument *Arg = dyn_cast<Argument>(arg)) {
    // Is the argument an array
    if (ArrayType *ArTy = dyn_cast<ArrayType>(Arg->getType())) {
      // is the this an array of integers
      if (IntegerType *ITy =
              dyn_cast<IntegerType>(ArTy->getArrayElementType())) {
        if (ITy->getBitWidth() == 16)
          return Arg;
      }
    }
    // Is the argument a pointer, use the attribute to determine whether the
    // pointer is a reference to a qubit.
    else if (PointerType *PtrTy = dyn_cast<PointerType>(Arg->getType())) {
      if (Arg->getParent()
              ->getAttributes()
              .getParamAttrs(Arg->getArgNo())
              .hasAttribute("qubit_ref"))
        return Arg;
    }
    // Is the argument an integer
    else if (IntegerType *ITy = dyn_cast<IntegerType>(Arg->getType())) {
      if (ITy->getBitWidth() == 16)
        return Arg;
    }
  }

  // Is this an allocation which is then loaded by with an argument
  if (AllocaInst *AI = is_qbit_alloc(arg)) {

    for (auto U : AI->users()) {
      // look for a store instruction to an argument
      if (StoreInst *SI = dyn_cast<StoreInst>(U)) {
        if (SI->getValueOperand() == AI)
          continue;
        if (Argument *Arg = is_qbit_argument(SI->getValueOperand()))
          return Arg;
      }
    };
  }

  return nullptr;
}

CallBase *QbitRef::is_qbit_fleq_reg(Value *val) const {
  if (val) {
    if (CallBase *CB = dyn_cast<CallBase>(val)) {
      StringRef called_name = CB->getCalledFunction()->getName();
      if (called_name.contains(fleq::Builtin_QLIST)) {
        if (called_name.contains(fleq::QLIST_SLICE) ||
            called_name.contains(fleq::QLIST_JOIN) ||
            called_name.contains(fleq::QLIST_EMPTY))
          return CB;
      } else if (called_name.contains(fleq::DATALIST_ALLOC))
        return CB;
    }
  }
  return nullptr;
}

bool QbitRef::is_in_register(unsigned ind) const {

  // for the sake of representing  registers themselves
  // we allways return true in ind is 0 which out checking it
  // against the size
  if (ind == 0)
    return true;

  std::optional<unsigned> temp = getRegisterSize();
  if (temp.has_value())
    return ind <= temp.value(); // allow equality for zero size arrays.

  return true;
}

Value *QbitRef::createValue(Instruction *instr, bool is_after, bool isQbitPtr,
                            QIter *QIt) {
  Value *out = nullptr;

  // check if reg_ptr_ is an integer type itself, meaning no Instructions
  // need to be inserted
  if (IntegerType *ITy = dyn_cast<IntegerType>(reg_ptr_->getType()))
    return reg_ptr_;

  // check if reg_ptr_ is a pointer
  else if (PointerType *PTy = dyn_cast<PointerType>(reg_ptr_->getType())) {

    Value *load_ptr = reg_ptr_;
    Type *load_ty = nullptr;
    if (AllocaInst *AI = is_qbit_alloc()) {
      load_ty = AI->getAllocatedType();
    } else if (Argument *Arg = is_qbit_argument()) {
      load_ty = Type::getInt16Ty(load_ptr->getContext());
    } else if (GlobalValue *GV = is_qbit_global()) {
      load_ty = GV->getValueType();
    } else if (CallBase *CB = is_qbit_fleq_reg()) {
      load_ty = Type::getInt16Ty(load_ptr->getContext());
    }

    assert(load_ty != nullptr);

    std::vector<Value *> gep_array;
    if ((dyn_cast<ArrayType>(load_ty)))
      gep_array.push_back(
          ConstantInt::get(IntegerType::get(getContext(), 64), 0));

    gep_array.push_back(index_ptr_);

    // getAlignment
    MaybeAlign maybe_align;
    Align alignment;
    if (AllocaInst *AI = is_qbit_alloc()) {
      alignment = AI->getAlign();
      maybe_align = MaybeAlign(alignment);
    } else if (Argument *Arg = is_qbit_argument())
      maybe_align = Arg->getParamAlign();
    else if (GlobalValue *GV = is_qbit_global()) {
      maybe_align = GV->getAliaseeObject()->getAlign();
    }

    bool flag = false;

    // check if index is zero in which case we don't need a get element ptr
    if ((dyn_cast<IntegerType>(load_ty))) {
      if (ConstantInt *CInt = dyn_cast<ConstantInt>(index_ptr_)) {
        if (CInt->getZExtValue() == 0)
          flag = true;
      }
    }

    // in the case of a fleq call, we need to use qlist.at to get correct
    // qubit
    if (CallBase *CB = is_qbit_fleq_reg()) {

      auto IntrID = Intrinsic::fleq_qlist_at;
      Function *qlist_at = Intrinsic::getDeclaration(CB->getModule(), IntrID);
      CallInst *CallAt = CallInst::Create(qlist_at->getFunctionType(), qlist_at,
                                          {reg_ptr_, index_ptr_});

      if (instr) {
        if (is_after)
          CallAt->insertAfter(instr);
        else
          CallAt->insertBefore(instr);
      }

      load_ptr = dyn_cast<Value>(CallAt);
      flag = true;
    }

    if (flag) {
      if (!isQbitPtr) {
        LoadInst *LI = new LoadInst(load_ty, load_ptr, "", false,
                                    maybe_align.valueOrOne());

        if (instr) {
          if (is_after)
            LI->insertAfter(instr);
          else
            LI->insertBefore(instr);
        }
        out = dyn_cast<Value>(LI);
      } else
        // Since we are passing qubits as reference, no need to insert a load
        // instruction directly pass through the allocainst reference
        out = load_ptr;

    } else {
      if (instr) {
        Instruction *MoveIdxTo = instr;

        // If index_ptr_ is an instruction, try to add or move Instruction it
        // represents before GEP
        // Never move a Phi, landing pad or allocation
        if (!isa<PHINode>(index_ptr_) && !isa<LandingPadInst>(index_ptr_) &&
            !isa<AllocaInst>(index_ptr_)) {
          if (Instruction *II = dyn_cast<Instruction>(index_ptr_)) {
            if (is_index_owned()) {
              if (is_after) {
                II->insertAfter(MoveIdxTo);
                instr = II;
              } else
                II->insertBefore(MoveIdxTo);
              set_index_owned(false);
            } else {
              // This instruction was already inserted into the IR, we may be
              // inserting an instruction in the incorrect order, add it to the
              // set of instructions to check for later.
              if (QIt)
                QIt->addToInstsToCheck(II);
            }
          }
        }
      }

      GetElementPtrInst *GI = GetElementPtrInst::CreateInBounds(
          load_ty, load_ptr, gep_array, "arrayidx");
      if (!GI) {
        return nullptr;
      }
      load_ptr = dyn_cast<Value>(GI);
      if (!load_ptr) {
        return nullptr;
      }

      // Insert Instructions
      if (instr) {
        if (is_after) {
          GI->insertAfter(instr);
        } else {
          GI->insertBefore(instr);
        }
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
        if (instr)
          LI->insertAfter(Instr);
        out = dyn_cast<Value>(LI);
      } else {
        out = load_ptr;
      }

      Instruction *MoveIdxTo = dyn_cast<Instruction>(GI);
      if (!MoveIdxTo) {
        return nullptr;
      }
    }
  }
  return out;
}

Value *QbitRef::binary_op_helper(Instruction::BinaryOps opcode,
                                 Value *other_op) const {
  Instruction *BI = BinaryOperator::Create(opcode, index_ptr_, other_op);
  // construct index as ParaRef so it take ownership of unowned instruction
  ParaRef dummy(dyn_cast<Value>(BI));
  return dummy.getValue();
}

bool QbitRef::is_index_owned() {
  return getInstance_INDEX_OWNER_MAP_()[index_ptr_];
}
void QbitRef::set_index_owned(bool set) {
  getInstance_INDEX_OWNER_MAP_()[index_ptr_] = set;
}

////////////////////////////////////////////////////////////////////////////////
// QbitRef friends

raw_ostream &operator<<(raw_ostream &output, const QbitRef &q) {
  return output << getPrettyName(q);
}

std::string getPrettyName(const QbitRef &q) {

  if (q.isQID()) {
    return "QID[" + getPrettyName(q.getIndex(), false) + "]";
  }
  if (CallBase *CB = fleq::isIQCAlloca(q)) {
    auto name = ParaRef::getDataList(CB->getArgOperand(0));
    if (name.first && !name.second.empty()) {
      return name.second + "[" + getPrettyName((Value *)q.getIndex(), false) +
             "]";
    } else {
      return "<IQC_alloca<qbit>>[" +
             getPrettyName((Value *)q.getIndex(), false) + "]";
    }
  }
  return getPrettyName((Value *)q.getRegister(), false) + "[" +
         getPrettyName((Value *)q.getIndex(), false) + "]";
}

QbitRef getQbitRefPointedToBy(Value *load_ref) {

  QbitRef out;

  // is a allocation
  if ((out.is_qbit_alloc(load_ref))) {
    out.setRegister(load_ref);
    out.setIndexResolved(0);
  }
  // is an argument
  else if ((out.is_qbit_argument(load_ref))) {
    out.setRegister(load_ref);
    out.setIndexResolved(0);
  }
  // is a global
  else if ((out.is_qbit_global(load_ref))) {
    out.setRegister(load_ref);
    out.setIndexResolved(0);
  } else if (LoadInst *LI = dyn_cast<LoadInst>(load_ref)) {
    out = getQbitRefPointedToBy(LI->getPointerOperand());
  }
  // return shallow version for FLEQ calls
  else if ((out.is_qbit_fleq_reg(load_ref))) {
    out.setRegister(load_ref);
    out.setIndexResolved(0);
  } else if (CallBase *CB = dyn_cast<CallBase>(load_ref)) {
    StringRef called_name = CB->getCalledFunction()->getName();
    if (called_name.contains(fleq::QLIST_AT)) {
      out = getQbitRefPointedToBy(CB->getArgOperand(0));
      out.shiftIndexBy(CB->getArgOperand(1));
    }
  }

  // is a GetElementPtrInst
  else if (GEPOperator *GO = dyn_cast<GEPOperator>(load_ref)) {

    out = getQbitRefPointedToBy(GO->getPointerOperand());

    if (!out.isNull()) {
      auto idx = GO->idx_end();
      auto beg = GO->idx_begin();
      --idx;
      out.shiftIndexBy(dyn_cast<Value>(idx));

      // check for a second index and Register size
      // to account for register overflow
      if (idx != beg) {
        --idx;
        ParaRef Idx2(dyn_cast<Value>(idx));
        auto num_op = out.getRegisterSize();
        if (num_op.has_value() && Idx2.isResolved()) {
          unsigned shft = num_op.value();
          shft *= (unsigned)Idx2.getValueResolved().value();
          if (shft != 0)
            out.shiftIndexByResolved(shft);
        }
      }
    }
  } else if (Value *qbptr = fleq::isQListStruct(load_ref)) {
    out = getQbitRefPointedToBy(qbptr);
  }

  return out;
}

////////////////////////////////////////////////////////////////////////////////
// QRefArray function definitions

QbitRef QRefArray::operator[](unsigned idx) {
  if (idx > size())
    return QbitRef();

  auto it = begin();
  advance(it, idx);
  return *it;
}

QRefArray *QRefArray::get(Value *root) {

  static std::map<Value *, std::unique_ptr<QRefArray>> ARRAY_MAP_;

  auto rt_entry = ARRAY_MAP_.find(root);
  if (rt_entry == ARRAY_MAP_.end()) {

    rt_entry =
        ARRAY_MAP_.insert(std::make_pair(root, std::unique_ptr<QRefArray>()))
            .first;

    QbitRef rt_qbit = getQbitRefPointedToBy(root);

    if (rt_qbit.isNull()) {
    } else if (CallBase *CB = rt_qbit.is_qbit_fleq_reg()) {

      // check for no-zero index
      if (rt_qbit.isResolved()) {

        unsigned rt_idx = rt_qbit.getIndexResolved().value();
        if (rt_idx != 0) {
          QRefArray *base = QRefArray::get(rt_qbit.getRegister());
          if (base) {
            auto rt_ptr =
                std::make_unique<QRefArray>(slice(*base, rt_idx, base->size()));
            std::swap(rt_entry->second, rt_ptr);
          }
        } else {

          StringRef called_name = CB->getCalledFunction()->getName();

          if (called_name.contains(fleq::QLIST_EMPTY)) {
            auto rt_ptr = std::make_unique<QRefArray>();
            std::swap(rt_entry->second, rt_ptr);
          } else if (called_name.contains(fleq::QLIST_SLICE)) {

            // get slice indices
            ParaRef sl_idx1(CB->getArgOperand(1));
            ParaRef sl_idx2(CB->getArgOperand(2));

            // try hard to resolve the indices
            sl_idx1.updateToSimplifiedRecursive();
            sl_idx2.updateToSimplifiedRecursive();

            if (sl_idx1.isResolved() && sl_idx2.isResolved()) {

              unsigned idx1 = (unsigned)sl_idx1.getValueResolved().value();
              unsigned idx2 = (unsigned)sl_idx2.getValueResolved().value();

              if (idx2 < idx1) {         // keep null
              } else if (idx1 == idx2) { // return empty array
                auto rt_ptr = std::make_unique<QRefArray>();
                std::swap(rt_entry->second, rt_ptr);
              } else {
                // get the passed ref after indices, incase indices depend on
                // arg(i.e. through qlist.size)
                QRefArray *arg = get(CB->getArgOperand(0));
                if (arg) {
                  if (arg->size() >= idx2) {
                    auto rt_ptr =
                        std::make_unique<QRefArray>(slice(*arg, idx1, idx2));
                    std::swap(rt_entry->second, rt_ptr);
                  }
                }
              }
            }
          } else if (called_name.contains(
                         fleq::QLIST_JOIN)) { // Should be a join

            // get both args
            QRefArray *arg0 = get(CB->getArgOperand(0));
            QRefArray *arg1 = get(CB->getArgOperand(1));

            if (arg0 && arg1) {
              auto rt_ptr = std::make_unique<QRefArray>(join(*arg0, *arg1));
              std::swap(rt_entry->second, rt_ptr);
            }
          } else if (called_name.contains(fleq::DATALIST_ALLOC)) {
            auto rt_ptr = std::make_unique<QRefArray>(rt_qbit);
            std::swap(rt_entry->second, rt_ptr);
          }
        }
      }
    } else {
      std::unique_ptr<QRefArray> rt_ptr = std::make_unique<QRefArray>();
      // try to simplify first
      rt_qbit.updateToSimplifiedRecursive();
      if (rt_ptr->isAddableToArray(rt_qbit)) {
        rt_ptr->push_back_without_check(rt_qbit);
        std::swap(rt_entry->second, rt_ptr);
      }
    }
  }

  assert(rt_entry != ARRAY_MAP_.end());
  return (rt_entry->second).get();
}

bool QRefArray::isAddableToArray(const QbitRef &q) {

  if (!q.isResolved())
    return false;
  unsigned idx = q.getIndexResolved().value();

  auto opt_idx_max = q.getRegisterSize();
  if (!opt_idx_max.has_value())
    return false;
  unsigned idx_max = opt_idx_max.value();

  return isAddableToArray(q, idx_max - idx);
}

bool QRefArray::isAddableToArray(const QbitRef &q, unsigned width) {
  // Look for failure cases
  //  - index is not resolved
  //  - register size is indeterminate
  //  - Overlap with other elements of the list
  //  - qbit is not distinguishable from others

  if (!q.isResolved()) {
    return false;
  }
  unsigned idx = q.getIndexResolved().value();
  auto opt_idx_max = q.getRegisterSize();

  if (!opt_idx_max.has_value()) {
    return false;
  }
  unsigned idx_max = opt_idx_max.value();
  unsigned max = idx + width;
  if (max > idx_max) {
    return false;
  }

  for (auto pair : array_list_) {
    if (!pair.first.isDistinguishable(q)) {
      return false;
    }
    if (pair.first.getRegister() == q.getRegister()) {
      // Check for overlap
      unsigned cur_begin = pair.first.getIndexResolved().value();
      unsigned cur_end = cur_begin + pair.second;
      if ((idx < cur_end && idx >= cur_begin) ||
          (idx_max < cur_end && idx_max > cur_begin)) {
        errs() << "QRefArray Warning: found slice overlap when attempting to "
                  "add to an array:\n"
               << "(" << pair.first << ", " << pair.second << ") and (" << q
               << ", " << width << ").\n";
        return false;
      }
    }
  }

  return true;
}

QRefArray join(const QRefArray &A, const QRefArray &B) {

  QRefArray out(A);

  for (auto pair : B.array_list_) {
    if (out.push_back(pair.first, pair.second) == out.end()) {
      return QRefArray();
    }
  }

  return out;
}

QRefArray slice(const QRefArray &A, unsigned first, unsigned last) {

  QRefArray out;

  if (last <= first) {
    return QRefArray();
  }

  if (A.size() < last) {
    return QRefArray();
  }

  // get first and last QRefArray::iterator
  auto first_it = A.begin();
  advance(first_it, first);

  auto last_it = A.begin();
  advance(last_it, last);

  // Check if the slice spans one or more elements of the array
  unsigned first_width = first_it.it_->second - first_it.idx_;
  unsigned total_width = last - first;

  if (total_width <= first_width) {
    out.push_back(*first_it, total_width);
    return out;
  }

  // add first entry
  auto result = out.push_back(*first_it, first_width);
  if (result == out.end()) {
    return QRefArray();
  }
  // add entries up to the last
  auto A_it = ++first_it.it_;
  while (A_it != last_it.it_) {
    result = out.push_back(A_it->first, A_it->second);
    if (result == out.end()) {
      return QRefArray();
    }
    ++A_it;
  }

  // add the last (if we are not already at the end)
  if (last_it != A.end()) {
    result = out.push_back(last_it.it_->first, last_it.idx_);
    if (result == out.end()) {
      return QRefArray();
    }
  }

  return out;
}

raw_ostream &operator<<(raw_ostream &stream, const QRefArray &array) {
  for (auto &pair : array.array_list_) {
    stream << "(" << pair.first << ", " << pair.second << ") ";
  }
  return stream;
}

bool QRefArray::operator==(const std::vector<QbitRef> &vec) {
  if (size() != vec.size())
    return false;

  auto it = begin();
  for (size_t i = 0; i < size(); i++) {
    if (*it != vec[i])
      return false;
    ++it;
  }

  return true;
}

static std::string
convert_to_string(const SmallVector<StringRef, 5> &datalist) {
  std::string out;
  for (size_t i = 0; i < datalist.size(); i++) {
    out += datalist[i].str();
  }
  return out;
}

static unsigned get_size(const SmallVector<StringRef, 5> &datalist) {
  unsigned out = 0;
  for (size_t i = 0; i < datalist.size(); i++) {
    out += datalist[i].size();
  }
  return out;
}

std::pair<bool, std::string> ParaRef::getDataList(Value *root) {

  static std::map<Value *, SmallVector<StringRef, 5>> DATALIST_MAP;

  auto rt_it = DATALIST_MAP.find(root);

  if (rt_it != DATALIST_MAP.end())
    return std::make_pair(true, convert_to_string(rt_it->second));

  // first check for string data without FLEQ calls
  unsigned shft = 0;
  if (Constant *base_data = fleq::getKey(root, shft)) {
    if (ConstantDataSequential *CDS =
            dyn_cast<ConstantDataSequential>(base_data)) {
      StringRef rt_string = CDS->getAsCString();
      rt_string = rt_string.slice(shft, rt_string.size());
      DATALIST_MAP[root].push_back(rt_string);
      return std::make_pair(true, rt_string.str());
    } else if (isa<ConstantAggregateZero>(base_data)) {
      DATALIST_MAP[root].push_back("");
      return std::make_pair(true, "");
    }
  }
  // getKey will fail if the pointer operand is a FLEQ call, thus
  // we handle that case here
  else if (GEPOperator *GO = dyn_cast<GEPOperator>(root)) {
    Value *ptr = GO->getPointerOperand();
    if (!getDataList(ptr).first)
      return std::make_pair(false, "");
    auto &ptr_dl = DATALIST_MAP[ptr];
    auto &rt_dl = DATALIST_MAP[root];

    auto idx = GO->idx_end();
    --idx;
    ParaRef PR_idx(dyn_cast<Value>(idx));
    PR_idx.updateToSimplifiedRecursive();
    if (!PR_idx.isResolved()) {
      return std::make_pair(false, "");
    }

    unsigned shft = (unsigned)PR_idx.getValueResolved().value();

    unsigned i = 0;
    while (i != ptr_dl.size()) {
      unsigned sz = ptr_dl[i].size();
      if (shft > sz) {
        shft -= sz;
        i++;
      } else
        break;
    }

    for (; i < ptr_dl.size(); i++) {
      StringRef to_add = ptr_dl[i].slice(shft, ptr_dl[i].size());

      shft = 0;
      rt_dl.push_back(to_add);
    }

    return std::make_pair(true, convert_to_string(rt_dl));
  } else if (CallBase *CB = dyn_cast<CallBase>(root)) {
    StringRef called_name = CB->getCalledFunction()->getName();
    if (called_name.contains(fleq::Builtin_DATALIST)) {

      if (called_name.contains(fleq::DATALIST_EMPTY)) {
        DATALIST_MAP[root]; // construct empty list
        return std::make_pair(true, "");
      } else if (called_name.contains(fleq::DATALIST_SLICE)) {

        Value *sl_ref = CB->getArgOperand(0);
        // add arg datalist to map
        auto ref = getDataList(sl_ref);
        if (!ref.first)
          return std::make_pair(false, "slice ref " + ref.second);

        auto &arg_dl = DATALIST_MAP[sl_ref];

        // get indices
        ParaRef idx1(CB->getArgOperand(1));
        idx1.updateToSimplifiedRecursive();
        ParaRef idx2(CB->getArgOperand(2));
        idx2.updateToSimplifiedRecursive();

        if (idx1.isResolved() && idx2.isResolved()) {
          // beyond this point, slice is valid
          auto &this_dl = DATALIST_MAP[root];
          unsigned ui_1 = (unsigned)idx1.getValueResolved().value();
          unsigned ui_2 = (unsigned)idx2.getValueResolved().value();
          if (ui_1 < ui_2) {

            for (size_t i = 0; i < arg_dl.size(); i++) {
              unsigned sz = arg_dl[i].size();
              if (ui_1 < sz) {
                StringRef to_add = arg_dl[i].slice(ui_1, ui_2);
                if (!to_add.empty())
                  this_dl.push_back(to_add);
                ui_1 = 0;
                if (ui_2 > sz)
                  ui_2 -= sz;
                else
                  break;
              } else {
                ui_1 -= sz;
                ui_2 -= sz;
                continue;
              }
            }
          }
          return std::make_pair(true, convert_to_string(this_dl));
        }
      } else if (called_name.contains(fleq::DATALIST_JOIN)) {

        Value *arg1 = CB->getArgOperand(0);
        Value *arg2 = CB->getArgOperand(1);

        // populate map
        auto arg1_str = getDataList(arg1);
        if (!arg1_str.first)
          return std::make_pair(false, "join arg1 " + arg1_str.second);
        auto arg2_str = getDataList(arg2);
        if (!arg2_str.first)
          return std::make_pair(false, "join arg2 " + arg2_str.second);

        auto &this_dl = DATALIST_MAP[root];
        this_dl = DATALIST_MAP[arg1];
        auto dl_2 = DATALIST_MAP[arg2];
        for (size_t i = 0; i < dl_2.size(); i++) {
          this_dl.push_back(dl_2[i]);
        }

        return std::make_pair(true, convert_to_string(this_dl));
      } else if (called_name.contains(fleq::DATALIST_LONGER)) {

        Value *arg1 = CB->getArgOperand(0);
        Value *arg2 = CB->getArgOperand(1);

        // populate map
        if (!getDataList(arg1).first)
          return std::make_pair(false, "");
        if (!getDataList(arg2).first)
          return std::make_pair(false, "");

        auto &this_dl = DATALIST_MAP[root];
        auto &dl_1 = DATALIST_MAP[arg1];
        auto &dl_2 = DATALIST_MAP[arg2];

        if (get_size(dl_2) > get_size(dl_1))
          this_dl = dl_2;
        else
          this_dl = dl_1;

        return std::make_pair(true, convert_to_string(this_dl));
      } else if (called_name.contains(fleq::DATALIST_SHORTER)) {

        Value *arg1 = CB->getArgOperand(0);
        Value *arg2 = CB->getArgOperand(1);

        // populate map
        if (!getDataList(arg1).first)
          return std::make_pair(false, "");
        if (!getDataList(arg2).first)
          return std::make_pair(false, "");

        auto &this_dl = DATALIST_MAP[root];
        auto &dl_1 = DATALIST_MAP[arg1];
        auto &dl_2 = DATALIST_MAP[arg2];
        if (get_size(dl_2) < get_size(dl_1))
          this_dl = dl_2;
        else
          this_dl = dl_1;

        return std::make_pair(true, convert_to_string(this_dl));
      } else if (called_name.contains(fleq::DATALIST_FROM)) {
        ParaRef to_convert(CB->getArgOperand(0));
        to_convert.updateToSimplifiedRecursive();
        if (!to_convert.isResolved())
          return std::make_pair(false, "");
        int conv_i = (int)to_convert.getValueResolved().value();
        std::string conv_str = std::to_string(conv_i);

        // need to store sting for numbers or else the StringRef
        // points at stale pointer
        static std::map<int, std::string> FROM_MAP;
        FROM_MAP[conv_i] = conv_str;
        DATALIST_MAP[root].push_back(FROM_MAP[conv_i]);
        return std::make_pair(true, conv_str);
      }
    }
  }

  return std::make_pair(false, "");
}

////////////////////////////////////////////////////////////////////////////////
// This deletes all values owned by ParaRef
// NOTE: Best practice is to call this at the end of any code which
// uses any arithmetic or other non-inserted values passed to ParaRef
// even if one expects that all instructions were inserted.
void ParaRef::deleteAllOwnedParaRefs() {
  std::vector<std::map<Value *, bool>::iterator> to_rm;
  auto &VOM = getInstance_VALUE_OWNER_MAP_();
  for (auto pair_it = VOM.begin(), end = VOM.end(); pair_it != end; ++pair_it) {
    auto &pair = *pair_it;
    if (pair.second) {
      Type *Ty = pair.first->getType();
      UndefValue *undef = UndefValue::get(Ty);
      pair.first->replaceAllUsesWith(undef);
      to_rm.push_back(pair_it);
    }
  }
  for (auto pair_it : to_rm) {
    pair_it->first->deleteValue();
    VOM.erase(pair_it);
  }
}
////////////////////////////////////////////////////////////////////////////////
// ParaRef function definitions

ParaRef::ParaRef(Value *val) { setValue(val); }

ParaRef::ParaRef(ParaType para, Type *type) { setValueResolved(para, type); }

ParaRef::ParaRef(ParaType para) { setValueResolved(para); }

void ParaRef::deleteParaRef() {
  if (is_value_owned()) {
    value_ptr_->deleteValue();
  }
  value_ptr_ = nullptr;
}

///////////////////////////////////////
// "Rule of five" definitions

ParaRef::ParaRef(const ParaRef &other)
    : value_ptr_(other.value_ptr_), PR_TYPE_(other.PR_TYPE_) {}

ParaRef::ParaRef(ParaRef &&other) noexcept
    : value_ptr_(other.value_ptr_), PR_TYPE_(other.PR_TYPE_) {

  other.value_ptr_ = nullptr;
  other.PR_TYPE_ = nullptr;
};

ParaRef &ParaRef::operator=(const ParaRef &other) {

  setValue(other.value_ptr_);

  return *this;
}

ParaRef &ParaRef::operator=(ParaRef &&other) noexcept {

  setValue(other.value_ptr_);
  if (other != *this) {
    other.value_ptr_ = nullptr;
    other.PR_TYPE_ = nullptr;
  }

  return *this;
}

ParaRef::~ParaRef() {
  setValue(nullptr);
  // deleteParaRef();
}

////////////////////////////////////////////////////////////////////////////////
// Resolve functions

bool ParaRef::isResolved() const {
  if (!value_ptr_)
    return false;
  if ((dyn_cast<ConstantFP>(value_ptr_)))
    return true;
  else if ((dyn_cast<ConstantInt>(value_ptr_)))
    return true;
  return false;
}

template <> bool isNearEq(const double &a, const double &b) {
  return std::abs(a - b) < 0.0000000001;
}

template <> std::string ParaRefPolynomial::Term::print() const {
  std::string out;
  for (auto const &pair : factors_) {
    out += "(" + getPrettyName(pair.first) + ")^" + std::to_string(pair.second);
  }
  return out;
}

template <> bool Polynomial<Module, ParaRef, double>::build(const ParaRef &V) {

  if (V.isNull()) {
    return false;
  }
  if (!V.isFloatingPoint())
    return false;

  if (V.isResolved()) {
    double c = V.getValueResolved().value();
    add(Term(), c);
    return true;
  }

  if (isa<Argument>(V.getValue())) {
    add(Term(V), 1.0);
    return true;
  }

  if (BinaryOperator *BO = dyn_cast<BinaryOperator>(V.getValue())) {
    // recursively build the left and right operands
    Polynomial Lop(getModule());
    if (!Lop.build(ParaRef(BO->getOperand(0))))
      return false;
    Polynomial Rop(getModule());
    if (!Rop.build(ParaRef(BO->getOperand(1))))
      return false;

    switch (BO->getOpcode()) {

    case Instruction::FAdd: {
      operator+=(Lop + Rop);
      return true;
    }
    case Instruction::FSub: {
      operator+=(Lop - Rop);
      return true;
    }
    case Instruction::FMul: {
      operator+=(Lop *Rop);
      return true;
    }
    case Instruction::FDiv: {
      operator+=(Lop / Rop);
      return true;
    }
    }
  }

  if (UnaryOperator *UO = dyn_cast<UnaryOperator>(V.getValue())) {
    if (UO->getOpcode() == Instruction::FNeg) {
      Polynomial Op(getModule());
      if (!Op.build(ParaRef(UO->getOperand(0))))
        return false;
      operator+=(-Op);
      return true;
    }
  }

  // check for different math built-ins
  if (CallInst *CI = dyn_cast<CallInst>(V.getValue())) {

    StringRef called_name = CI->getCalledFunction()->getName();

    // pow function
    if (called_name.contains("llvm.pow") || called_name == "pow" ||
        called_name.contains("exp2")) {
      // only convert if power is a resolved integer
      Value *base = nullptr;
      Value *power = nullptr;
      if (called_name.contains("exp2")) {
        base = dyn_cast<Value>(
            ConstantFP::get(getModule().getContext(), APFloat(2.)));
        power = CI->getArgOperand(0);
      } else {
        base = CI->getArgOperand(0);
        power = CI->getArgOperand(1);
      }

      ParaRef PRpow(power);
      if (!PRpow.isNull() && PRpow.isResolved()) {
        double Dpow = PRpow.getValueResolved().value();
        double frac = std::modf(Dpow, &Dpow);
        if (isNearEq(frac, 0.)) {
          int Ipow = (int)Dpow;
          Polynomial Base(getModule());
          if (!Base.build(ParaRef(base)))
            return false;

          // To avoid costly high-power, multi-term cases, we impose a limit
          // such that we treat the base as its own variable
          const int power_limit = 3;
          if ((Ipow > power_limit) && !Base.hasUniqueTerm()) {
            add(Term(Base.constructVariable(), Ipow), 1.0);
            return true;
          }

          if (Ipow < 0) {
            Base = !Base;
            Ipow = -Ipow;
          }

          Polynomial Outcome(getModule(), 1.0);
          unsigned UIpow = (unsigned)Ipow;
          while (true) {
            if (UIpow % 2)
              Outcome *= Base;
            UIpow = UIpow >> 1;
            if (UIpow == 0)
              break;
            Base *= Base;
          }

          operator+=(Outcome);
          return true;
        }
      }
    }
  }

  // If we have gotten here, we only have the case to that we
  // covert V directly to a variable.
  // But first, we try to use LLVM simplifyInstruction before doing so
  Value *Var = nullptr;
  if (Instruction *I = dyn_cast<Instruction>(V.getValue())) {
    if (BasicBlock *BB = I->getParent()) {
      if (Module *parent_m = I->getModule()) {
        Var = simplifyInstruction(I, SimplifyQuery(parent_m->getDataLayout()));
      }
    }
  }
  if (Var != nullptr) {
    Polynomial NewI(getModule());
    if (!NewI.build(ParaRef(Var)))
      return false;
    operator+=(NewI);
  } else
    add(Term(V), 1.0);

  return true;
}

Value *ParaRef::tryToSimplify() {

  if (!value_ptr_)
    return nullptr;

  // Check if it is already a constant
  if (isResolved())
    return value_ptr_;

  Value *out = nullptr;

  // First, check for FLEQ calls

  if (CallBase *CB = dyn_cast<CallBase>(value_ptr_)) {
    StringRef called_name = CB->getCalledOperand()->getName();
    if (called_name.contains(fleq::Builtin_PREFIX)) {

      if (called_name.contains(fleq::QLIST_SIZE)) {
        QRefArray *q_arg = QRefArray::get(CB->getArgOperand(0));
        if (q_arg) {
          return dyn_cast<Value>(ConstantInt::get(PR_TYPE_, q_arg->size()));
        }
        return value_ptr_;
      } else if (called_name.contains(fleq::QLIST_EQUAL)) {
        QbitRef q1 = getQbitRefPointedToBy(CB->getArgOperand(0));
        q1.updateToSimplifiedRecursive();
        QbitRef q2 = getQbitRefPointedToBy(CB->getArgOperand(1));
        q2.updateToSimplifiedRecursive();

        bool is_equal = (q1.isEqual(q2) == 1);
        return dyn_cast<Value>(ConstantInt::getBool(PR_TYPE_, is_equal));
      } else if (called_name.contains(fleq::DATALIST_INTEGERIZE)) {
        auto data = getDataList(CB->getArgOperand(0));
        if (!data.first)
          return nullptr;
        int data_int = 0;
        try {
          if (!data.second.empty())
            data_int = std::stoi(data.second);
          return dyn_cast<Value>(ConstantInt::get(PR_TYPE_, data_int));
        } catch (...) {
          throw std::string("Unable to integerize DataList \"") + data.second +
              std::string("\"\n");
          return nullptr;
        }
      } else if (called_name.contains(fleq::DATALIST_BOOLEANIZE)) {
        auto data = getDataList(CB->getArgOperand(0));
        if (!data.first)
          return nullptr;
        bool data_bool = false;
        try {
          if (!data.second.empty())
            data_bool = std::stoi(data.second);
          return dyn_cast<Value>(ConstantInt::get(PR_TYPE_, data_bool));
        } catch (...) {
          throw std::string("Unable to booleanize DataList \"") + data.second +
              std::string("\"\n");
          return nullptr;
        }
      } else if (called_name.contains(fleq::DATALIST_FLOATIZE)) {
        auto data = getDataList(CB->getArgOperand(0));
        if (!data.first)
          return nullptr;
        double data_flt = 0.;
        try {
          if (!data.second.empty())
            data_flt = std::stod(data.second);
          return dyn_cast<Value>(ConstantFP::get(PR_TYPE_, data_flt));
        } catch (...) {
          throw std::string("Unable to floatize DataList \"") + data.second +
              std::string("\"\n");
          return nullptr;
        }
      } else if (called_name.contains(fleq::DATALIST_AT)) {
        auto data = getDataList(CB->getArgOperand(0));
        if (!data.first)
          return nullptr;

        ParaRef idx(CB->getArgOperand(1));
        idx.updateToSimplifiedRecursive();
        if (!idx.isResolved())
          return nullptr;

        unsigned ui = (unsigned)idx.getValueResolved().value();

        char data_char = '\0';
        if (ui < data.second.size())
          data_char = data.second[ui];
        return dyn_cast<Value>(ConstantInt::get(PR_TYPE_, data_char));
      } else if (called_name.contains(fleq::DATALIST_SIZE)) {
        auto data = getDataList(CB->getArgOperand(0));
        if (!data.first)
          return nullptr;
        int data_size = data.second.size();
        return dyn_cast<Value>(ConstantInt::get(PR_TYPE_, data_size));
      } else if (called_name.contains(fleq::DATALIST_FIND)) {
        auto data = getDataList(CB->getArgOperand(0));
        if (!data.first)
          return nullptr;
        auto target = getDataList(CB->getArgOperand(1));
        if (!target.first)
          return nullptr;

        // get the find code
        ParaRef code(CB->getArgOperand(2));
        code.updateToSimplifiedRecursive();
        if (!code.isResolved())
          return nullptr;
        int icd = (int)code.getValueResolved().value();

        unsigned sz = data.second.size();
        unsigned pos = sz;
        switch (icd) {
        case -2: // not last
          pos = data.second.find_last_not_of(target.second);
          break;
        case -1: // not
          pos = data.second.find_first_not_of(target.second);
          break;
        case 0: // find
          pos = data.second.find(target.second);
          break;
        case 1: // find last
          pos = data.second.rfind(target.second);
          break;
        case 2: // find any
          pos = data.second.find_first_of(target.second);
          break;
        case 3: // find any last
          pos = data.second.find_last_of(target.second);
          break;
        default:
          return nullptr;
        }

        if (pos > sz)
          pos = sz;

        return dyn_cast<Value>(ConstantInt::get(PR_TYPE_, pos));
      } else if (called_name.contains(fleq::DATALIST_CONTAINS)) {
        auto data = getDataList(CB->getArgOperand(0));
        if (!data.first) {
          return nullptr;
        }
        auto target = getDataList(CB->getArgOperand(1));
        if (!target.first) {
          return nullptr;
        }
        bool contains = (data.second.find(target.second) < data.second.size());
        return dyn_cast<Value>(ConstantInt::getBool(PR_TYPE_, contains));
      } else if (called_name.contains(fleq::DATALIST_COUNT)) {
        auto data = getDataList(CB->getArgOperand(0));
        if (!data.first)
          return nullptr;
        auto target = getDataList(CB->getArgOperand(1));
        if (!target.first)
          return nullptr;

        unsigned cnt = 0;
        unsigned cur = 0;
        while (true) {
          cur = data.second.find(target.second, cur);
          if (cur >= data.second.size())
            break;
          cnt++;
          cur += target.second.size();
        }
        return dyn_cast<Value>(ConstantInt::get(PR_TYPE_, cnt));
      }
    }
  }

  // we now use the Polynomial class to perform the simplification
  // for floating point types
  if (isFloatingPoint()) {
    if (Instruction *I = dyn_cast<Instruction>(value_ptr_)) {
      if (StoredModule != nullptr) {
        ParaRefPolynomial Simplified(*StoredModule);
        if (Simplified.build(*this)) {
          out = Simplified.constructVariable().getValue();
        }
      }
    }
  }
  // in other cases, we just use the LLVM simplifyInstruction
  else {
    if (PHINode *Phi = dyn_cast<PHINode>(value_ptr_)) {
      if (Value *outcome = Phi->hasConstantValue()) {
        return outcome;
      }
    }
    if (Instruction *I = dyn_cast<Instruction>(value_ptr_)) {
      if (BasicBlock *BB = I->getParent()) {
        if (Module *parent_m = I->getModule()) {
          out =
              simplifyInstruction(I, SimplifyQuery(parent_m->getDataLayout()));
        }
      }
    }
  }

  if (out == nullptr) {
    return value_ptr_;
  }
  // avoid duplicating instructions when they are otherwise equivalent
  if (Instruction *Inew = dyn_cast<Instruction>(out)) {
    if (Instruction *Iold = dyn_cast<Instruction>(value_ptr_)) {
      if (Iold->isIdenticalToWhenDefined(Inew))
        return value_ptr_;
    }
  }

  return out;
}

// utility for cleaning up dead branching
static void cleanUpBranchInst(BranchInst *Br, bool cond) {
  if (!Br)
    return;
  if (!(Br->isConditional()))
    return;

  // get BBs
  BasicBlock *trueBB = Br->getSuccessor(0);
  BasicBlock *falseBB = Br->getSuccessor(1);
  BasicBlock *parent = Br->getParent();

  // create new unconditional branch
  if (cond) {
    BranchInst::Create(trueBB, Br);
    Br->eraseFromParent();
    // not sure if above updates successor list for BBs
    // Now check if false BB has no or only parent predessor
    if (falseBB->hasNPredecessorsOrMore(2))
      return;
    if (BasicBlock *pred = falseBB->getUniquePredecessor()) {
      if (pred != parent)
        return;
      else
        falseBB->removePredecessor(parent);
    }
    // if we get here, we can remove this BB
    DeleteDeadBlock(falseBB, nullptr, true);
  } else {
    BranchInst::Create(falseBB, Br);
    Br->eraseFromParent();
    // not sure if above updates successor list for BBs
    // Now check if false BB has no or only parent predessor
    if (trueBB->hasNPredecessorsOrMore(2))
      return;
    if (BasicBlock *pred = trueBB->getUniquePredecessor()) {
      if (pred != parent)
        return;
      else
        trueBB->removePredecessor(parent);
    }
    // if we get here, we can remove this BB
    DeleteDeadBlock(trueBB, nullptr, true);
  }
}

bool ParaRef::updateToSimplifiedRecursive() {
  std::set<PHINode *> visited;
  return updateToSimplifiedRecursive(visited);
}

bool ParaRef::updateToSimplifiedRecursive(std::set<PHINode *> &visited) {

  if (isNull())
    return false;

  // it can happen that phi nodes cause loops in the
  // following recursion. Thus we look to see if we
  // have already looked at this, if it is a phi.
  PHINode *Phi = dyn_cast<PHINode>(value_ptr_);
  if (Phi) {
    if (visited.find(Phi) != visited.end()) {
      return false;
    } else {
      visited.insert(Phi);
    }
  }

  if (Instruction *I = dyn_cast<Instruction>(value_ptr_)) {
    for (auto op : I->operand_values()) {
      ParaRef Op(op);
      if (!Op.isNull()) {
        Op.updateToSimplifiedRecursive(visited);
      }
    }
  }

  // Phi nodes can cause lots of tricky optimizable cases
  // The following tries to see if we can navigate the incoming
  // BBs, and resolve the branching conditions to see if we
  // can simplify the Phi.
  if (Phi) {
    if (Value *outcome = Phi->hasConstantValue()) {
      value_ptr_ = outcome;
      return updateToSimplified();
    }
    SmallVector<BasicBlock *> to_rm;
    SmallVector<std::pair<BranchInst *, bool>> to_clean;
    BasicBlock *phi_BB = Phi->getParent();
    for (auto BB : Phi->blocks()) {
      if (BranchInst *Br = dyn_cast<BranchInst>(BB->getTerminator())) {
        if (Br->isConditional()) {
          ParaRef cond(Br->getCondition());
          bool result;
          try {
            result = cond.updateToSimplifiedRecursive(visited);
          } catch (std::string err) {
          }
          if (result) {
            if (ConstantInt *CInt = dyn_cast<ConstantInt>(cond.getValue())) {
              uint64_t cond_i = CInt->getZExtValue();
              // as we have resolved the condition, we can simplify.
              // how we handle it depends on if the outcome BB is the
              // phi's BB or not:
              // If it is not, then we can remove the outcome BB from the Phi
              // If it is, we try to remove the other non-outcome BB to
              // see if this simplifies the Phi
              // it seems the successor number is opposition the condition value
              if (cond_i == 1) {
                if ((Br->getSuccessor(0)) != phi_BB) {
                  to_rm.push_back(BB);
                } else
                  to_clean.push_back(std::make_pair(Br, true));
              } else {
                if ((Br->getSuccessor(1)) != phi_BB) {
                  to_rm.push_back(BB);
                } else
                  to_clean.push_back(std::make_pair(Br, false));
              }
            }
          }
        }
      }
    }
    for (auto BB : to_rm)
      Phi->removeIncomingValue(BB);
    for (auto pair : to_clean)
      cleanUpBranchInst(pair.first, pair.second);
  }
  return updateToSimplified();
}

bool ParaRef::updateToSimplified() {

  if (Value *result = tryToSimplify()) {

    // replace all uses in IR
    if (result != value_ptr_) {
      Value *old_ptr = value_ptr_;
      // check if we need to insert instructions if not owned (in LLVM IR)
      if (!is_value_owned()) {
        if (Instruction *I = dyn_cast<Instruction>(old_ptr)) {
          // make a dummy Qiter for insertion
          QBBIter dummy;
          setValue(result);
          dummy.add_para_call(I, *this, false);
        }
      } else
        setValue(result);
      old_ptr->replaceAllUsesWith(result);
      return true;
    }
  }

  return false;
}

bool ParaRef::removeFromParent() {

  if (is_value_owned())
    return true;

  if (Instruction *I = dyn_cast<Instruction>(value_ptr_)) {
    I->removeFromParent();
    set_value_owned(true);
    return true;
  }

  return false;
}

void ParaRef::eraseFromParent() {

  auto &VOM = getInstance_VALUE_OWNER_MAP_();
  UndefValue *undef = UndefValue::get(PR_TYPE_);
  value_ptr_->replaceAllUsesWith(undef);

  if (is_value_owned()) {
    value_ptr_->deleteValue();
    VOM.erase(value_ptr_);
  } else {
    if (Instruction *I = dyn_cast<Instruction>(value_ptr_)) {
      I->eraseFromParent();
      VOM.erase(value_ptr_);
    }
  }

  value_ptr_ = nullptr;
  PR_TYPE_ = nullptr;
}

////////////////////////////////////////////////////////////////////////////////
// Getters

int ParaRef::isEqual(ParaRef &para) {

  if (isNull())
    return (int)para.isNull();
  else if (para.isNull())
    return 0;

  // check for both resolved
  if (ConstantFP *CFP1 = dyn_cast<ConstantFP>(value_ptr_)) {
    if (ConstantFP *CFP2 = dyn_cast<ConstantFP>(para.value_ptr_)) {
      return (CFP1->getValueAPF()).bitwiseIsEqual(CFP2->getValueAPF());
    }
  } else if (ConstantInt *CInt1 = dyn_cast<ConstantInt>(value_ptr_)) {
    if (ConstantInt *CInt2 = dyn_cast<ConstantInt>(para.value_ptr_)) {
      return CInt1->getValue() == CInt2->getValue();
    }
  } else {
    return is_unresolved_but_equal(value_ptr_, para.value_ptr_);
  }

  // If we've gotten here, we can't resolve if they are equal or not.
  return -1;
}

std::optional<ParaType> ParaRef::getValueResolved() const {

  std::optional<ParaType> out;
  out.reset();
  if (ConstantInt *CInt = dyn_cast<ConstantInt>(value_ptr_)) {
    out = (ParaType)CInt->getSExtValue();
  } else if (ConstantFP *CFP = dyn_cast<ConstantFP>(value_ptr_)) {
    // Assume ParaType= double
    out = (ParaType)(CFP->getValueAPF()).convertToDouble();
  }

  return out;
}

////////////////////////////////////////////////////////////////////////////////
// Setters

void ParaRef::setValueResolved(ParaType val, Type *type) {

  if (type->isFloatingPointTy()) {
    value_ptr_ = ConstantFP::get(type, val);
    PR_TYPE_ = type;
  } else if (type->isIntegerTy()) {
    value_ptr_ = ConstantInt::get(type, val);
    PR_TYPE_ = type;
  } else {
    errs() << "ParaRef Error: ParaRef must be of float or integer type.\n";
    value_ptr_ = nullptr;
  }

  set_value_owned(false);
}

void ParaRef::setValueResolved(ParaType val) {
  if (!PR_TYPE_) {
    // default ParaRef type will be a double
    PR_TYPE_ = Type::getDoubleTy(StoredModule->getContext());
  }

  if (PR_TYPE_) {
    setValueResolved(val, PR_TYPE_);
  } else {
    std::string ErrMsg = "setValueResolved(double) didn't find another "
                         "example to get the LLVM::Context().\n";
    displayErrorAndExit("ParaRef", ErrMsg);
  }
}

void ParaRef::setValue(Value *val) {

  if (value_ptr_ == val)
    return;

  if (val == nullptr) {
    value_ptr_ = nullptr;
    PR_TYPE_ = nullptr;
    return;
  }

  PR_TYPE_ = val->getType();

  if (PR_TYPE_->isFloatingPointTy()) {
    value_ptr_ = val;
  } else if (PR_TYPE_->isIntegerTy()) {
    value_ptr_ = val;
  } else if (is_cbit_ref(val)) {
    value_ptr_ = val;
  } else {
    value_ptr_ = nullptr;
    PR_TYPE_ = nullptr;
    return;
  }

  set_value_owned(false);

  if (!isResolved()) {
    if (Instruction *I = dyn_cast<Instruction>(value_ptr_)) {
      if (BasicBlock *BB = (I->getParent())) {
        if (!(BB->getParent()))
          set_value_owned(true);
      } else
        set_value_owned(true);
    }
  }
}

////////////////////////////////////////////////////////////////////////////////
// Arithmetic overloads

ParaRef ParaRef::operator-() const {

  ParaRef out;
  if (!isFloatingPoint()) {
    std::string ErrMsg = "arithmetic only supported for floating type.\n";
    displayErrorAndExit("ParaRef", ErrMsg);
  }

  if (isResolved()) {
    APFloat APF_val = dyn_cast<ConstantFP>(value_ptr_)->getValueAPF();
    APF_val.changeSign();
    Constant *neg = ConstantFP::get(PR_TYPE_, APF_val);
    out.setValue(dyn_cast<Value>(neg));
  } else {
    // out.setValue(binary_op_helper(Instruction::FMul, neg.value_ptr_));
    out.setValue(
        dyn_cast<Value>(UnaryOperator::Create(Instruction::FNeg, value_ptr_)));
  }

  return out;
}

ParaRef operator+(const ParaRef &a, const ParaRef &b) {

  ParaRef out;

  if (!a.isFloatingPoint() || !b.isFloatingPoint()) {
    std::string ErrMsg = "arithmetic only supported for floating type.\n";
    displayErrorAndExit("ParaRef", ErrMsg);
  }

  if (a.isResolved() && b.isResolved()) {
    ParaType add_val =
        a.getValueResolved().value() + b.getValueResolved().value();
    out.setValueResolved(add_val, a.PR_TYPE_);
  } else {
    out.setValue(a.binary_op_helper(Instruction::FAdd, b.value_ptr_));
  }

  return out;
}

ParaRef ParaRef::operator+(ParaType cnst) {

  ParaRef temp(cnst, PR_TYPE_);
  return (*this) + temp;
}

ParaRef operator-(const ParaRef &a, const ParaRef &b) {

  ParaRef out;

  if (!a.isFloatingPoint() || !b.isFloatingPoint()) {
    std::string ErrMsg = "arithmetic only supported for floating type.\n";
    displayErrorAndExit("ParaRef", ErrMsg);
  }

  if (a.isResolved() && b.isResolved()) {

    ParaType sub_val =
        a.getValueResolved().value() - b.getValueResolved().value();
    out.setValueResolved(sub_val, a.PR_TYPE_);
  } else {
    out.setValue(a.binary_op_helper(Instruction::FSub, b.value_ptr_));
  }
  return out;
}

ParaRef ParaRef::operator-(ParaType cnst) {

  ParaRef temp(cnst, PR_TYPE_);
  return (*this) - temp;
}

ParaRef operator*(const ParaRef &a, const ParaRef &b) {

  ParaRef out;

  if (!a.isFloatingPoint() || !b.isFloatingPoint()) {
    std::string ErrMsg = "arithmetic only supported for floating type.\n";
    displayErrorAndExit("ParaRef", ErrMsg);
  }

  if (a.isResolved() && b.isResolved()) {

    ParaType mul_val =
        a.getValueResolved().value() * b.getValueResolved().value();
    out.setValueResolved(mul_val, a.PR_TYPE_);
  } else {
    out.setValue(a.binary_op_helper(Instruction::FMul, b.value_ptr_));
  }
  return out;
}

ParaRef ParaRef::operator*(ParaType cnst) {

  ParaRef temp(cnst, PR_TYPE_);
  return (*this) * temp;
}

ParaRef operator/(const ParaRef &a, const ParaRef &b) {

  ParaRef out;

  if (!a.isFloatingPoint() || !b.isFloatingPoint()) {
    std::string ErrMsg = "arithmetic only supported for floating type.\n";
    displayErrorAndExit("ParaRef", ErrMsg);
  }

  if (a.isResolved() && b.isResolved()) {
    ParaType div_val =
        a.getValueResolved().value() / b.getValueResolved().value();
    out.setValueResolved(div_val, a.PR_TYPE_);
  } else {
    out.setValue(a.binary_op_helper(Instruction::FDiv, b.value_ptr_));
  }
  return out;
}

ParaRef ParaRef::operator/(ParaType cnst) {

  ParaRef temp(cnst, PR_TYPE_);
  return (*this) / temp;
}

void ParaRef::operator+=(const ParaRef &other) {

  if (!isFloatingPoint()) {
    std::string ErrMsg = "arithmetic only supported for floating type.\n";
    displayErrorAndExit("ParaRef", ErrMsg);
  }

  if (isResolved() && other.isResolved()) {

    ParaType add_val =
        getValueResolved().value() + other.getValueResolved().value();
    setValueResolved(add_val);
  } else {
    setValue(binary_op_helper(Instruction::FAdd, other.value_ptr_));
  }
}

void ParaRef::operator+=(ParaType cnst) {

  ParaRef temp(cnst, PR_TYPE_);

  (*this) += temp;
}

void ParaRef::operator-=(const ParaRef &other) {

  if (!isFloatingPoint()) {
    std::string ErrMsg = "arithmetic only supported for floating type.\n";
    displayErrorAndExit("ParaRef", ErrMsg);
  }

  if (isResolved() && other.isResolved()) {

    ParaType sub_val =
        getValueResolved().value() - other.getValueResolved().value();
    setValueResolved(sub_val);
  } else {
    setValue(binary_op_helper(Instruction::FSub, other.value_ptr_));
  }
}

void ParaRef::operator-=(ParaType cnst) {

  ParaRef temp(cnst, PR_TYPE_);

  (*this) -= temp;
}

void ParaRef::operator*=(const ParaRef &other) {

  if (!isFloatingPoint()) {
    std::string ErrMsg = "arithmetic only supported for floating type.\n";
    displayErrorAndExit("ParaRef", ErrMsg);
  }

  if (isResolved() && other.isResolved()) {

    ParaType mul_val =
        getValueResolved().value() * other.getValueResolved().value();
    setValueResolved(mul_val);
  } else {
    setValue(binary_op_helper(Instruction::FMul, other.value_ptr_));
  }
}

void ParaRef::operator*=(ParaType cnst) {

  ParaRef temp(cnst, PR_TYPE_);

  (*this) *= temp;
}

void ParaRef::operator/=(const ParaRef &other) {

  if (!isFloatingPoint()) {
    std::string ErrMsg = "arithmetic only supported for floating type.\n";
    displayErrorAndExit("ParaRef", ErrMsg);
  }

  if (isResolved() && other.isResolved()) {

    ParaType div_val =
        getValueResolved().value() / other.getValueResolved().value();
    setValueResolved(div_val);
  } else {
    setValue(binary_op_helper(Instruction::FDiv, other.value_ptr_));
  }
}

void ParaRef::operator/=(ParaType cnst) {

  ParaRef temp(cnst, PR_TYPE_);

  (*this) /= temp;
}

////////////////////////////////////////////////////////////////////////////////
// private ParaRef utilities

bool ParaRef::is_cbit_ref(Value *val) const {
  if (val == nullptr)
    return false;
  Type *Ty = val->getType();
  if (PointerType *PTy = dyn_cast<PointerType>(Ty)) {
    if (GlobalVariable *GV = dyn_cast<GlobalVariable>(val)) {
      if (IntegerType *ITy = dyn_cast<IntegerType>(GV->getValueType()))
        return true;
    }

    if (AllocaInst *AI = dyn_cast<AllocaInst>(val)) {
      if (IntegerType *ITy = dyn_cast<IntegerType>(AI->getAllocatedType()))
        return true;
    }

    Argument *Arg = dyn_cast<Argument>(val);
    if (Arg && Arg->getParent()
                   ->getAttributes()
                   .getParamAttrs(Arg->getArgNo())
                   .hasAttribute("cbit_ref")) {
      return true;
    }

    GetElementPtrInst *GEPI = dyn_cast<GetElementPtrInst>(val);
    if (GEPI && is_cbit_ref(GEPI->getPointerOperand())) {
      return true;
    }

    for (Use &U : val->uses()) {
      User *User = U.getUser();
      if (CallInst *CI = dyn_cast<CallInst>(User)) {
        if (CI->getCalledOperand() == val)
          continue;
        if (CI->getCalledFunction() == nullptr)
          continue;
        unsigned No = CI->getArgOperandNo(&U);
        if (CI->getCalledFunction()
                ->getAttributes()
                .getParamAttrs(No)
                .hasAttribute("cbit_ref"))
          return true;

        if (CI->getCalledFunction()->getName() == "llvm.quantum.measz" &&
            No == 1)
          return true;
      }
    }
    return true;
  }
  return false;
}

Value *ParaRef::binary_op_helper(Instruction::BinaryOps opcode,
                                 Value *other_op) const {
  if (!value_ptr_) {
    std::string ErrMsg = "parameter has no value to reference.\n";
    displayErrorAndExit("ParaRef", ErrMsg);
  }
  if (!other_op) {
    std::string ErrMsg = "no operation to reference.\n";
    displayErrorAndExit("ParaRef", ErrMsg);
  }
  Instruction *BI = BinaryOperator::Create(opcode, value_ptr_, other_op);
  return dyn_cast<Value>(BI);
}

bool ParaRef::is_value_owned() const {
  return getInstance_VALUE_OWNER_MAP_()[value_ptr_];
}
void ParaRef::set_value_owned(bool set) {
  getInstance_VALUE_OWNER_MAP_()[value_ptr_] = set;
}

/////////////////////////////////////////////////////////////////////////////////
//<< overload

raw_ostream &operator<<(raw_ostream &output, const ParaRef &para) {
  output << getPrettyName(para.getValue(), false);
  return output;
}

std::string getPrettyName(const ParaRef &para) {
  return getPrettyName(para.getValue(), false);
}

std::string getPrettyName(Value *value, bool maybe_bracket) {

  if (!value)
    return "NULL";
  else if (ConstantFP *FP = dyn_cast<ConstantFP>(value)) {
    if (FP->isNaN())
      return "nan";
    double val = (FP->getValueAPF()).convertToDouble();
    if (std::abs(val) < 0.000000000001)
      return "0.";
    std::stringstream stream_temp;
    double sig_fig = 4.;
    sig_fig -= 1.;
    double top = 1., bot = -6.;

    // calculate the truncated value
    double exp = std::floor(std::log10(std::abs(val)));
    double trunc = std::floor(val * std::pow(10, -exp + sig_fig) + 0.5);

    // check for the printing condition:
    // exp between top and bottom - standard numerical style
    // exp outside of top and bottom - 'e' notation.
    if ((exp - sig_fig) > top) {
      trunc *= std::pow(10, -sig_fig);
      stream_temp << std::fixed << std::setprecision((unsigned)sig_fig)
                  << trunc;
    } else if ((exp - sig_fig) > 0) {
      trunc *= std::pow(10, exp - sig_fig);
      stream_temp << std::fixed << std::setprecision(0) << trunc;
      exp = 0.;
    } else if ((exp - sig_fig) > bot) {
      unsigned prec = (unsigned)(sig_fig - exp);
      double dump;
      while (prec != 0) {
        trunc *= 0.1;
        if (std::modf(std::abs(trunc), &dump) > 0.00000001)
          break;
        prec--;
      }
      if (prec > 0)
        trunc *= std::pow(10, -(double)(prec - 1));
      stream_temp << std::fixed << std::setprecision(prec) << trunc;
      exp = 0.;
    } else {
      trunc *= std::pow(10, -sig_fig);
      stream_temp << std::fixed << std::setprecision((unsigned)sig_fig)
                  << trunc;
    }

    std::string out = stream_temp.str();
    int iexp = (int)exp;
    if (iexp != 0)
      out += "e" + std::to_string(iexp);
    return out;

  } else if (ConstantInt *Int = dyn_cast<ConstantInt>(value)) {
    uint64_t val = (Int->getValue()).getZExtValue();
    return std::to_string(val);
  } else if (LoadInst *LI = dyn_cast<LoadInst>(value)) {
    return getPrettyName(LI->getPointerOperand(), false);
  } else if (GEPOperator *GEP = dyn_cast<GEPOperator>(value)) {
    auto idx = GEP->idx_end();
    --idx;
    return getPrettyName(GEP->getPointerOperand(), false) + "[" +
           getPrettyName(dyn_cast<Value>(idx), false) + "]";
  } else if (AllocaInst *AI = dyn_cast<AllocaInst>(value)) {
    StringRef name = AI->getName();
    if (name.empty()) {
      Type *type = AI->getAllocatedType();
      std::string type_name;
      llvm::raw_string_ostream rs_name(type_name);
      type->print(rs_name);
      return "<alloca " + rs_name.str() + ">";
    }
    return name.str();
  } else if (GlobalObject *GO = dyn_cast<GlobalObject>(value)) {
    if (isa<Function>(GO)) {
      std::string demang = llvm::demangle((GO->getName()).str());
      auto posL = demang.find('(');
      return demang.substr(0, posL);
    } else if (GlobalVariable *GV = dyn_cast<GlobalVariable>(GO)) {
      StringRef name = GV->getName();
      if (name.empty()) {
        Type *type = GV->getType();
        std::string type_name;
        llvm::raw_string_ostream rs_name(type_name);
        type->print(rs_name);
        return "<global " + (GO->getType()->getTargetExtName()).str() + ">";
      }
      return name.str();
    }
  } else if (CallBase *CB = dyn_cast<CallBase>(value)) {
    std::string out = getPrettyName(CB->getCalledOperand(), false) + "(";
    if (!CB->arg_empty()) {
      out = out + getPrettyName(CB->getArgOperand(0), false);
      for (int i = 1; i < CB->arg_size(); i++) {
        out = out + ", " + getPrettyName(CB->getArgOperand(i), false);
      }
    }
    return out + ")";
  } else if (BinaryOperator *BO = dyn_cast<BinaryOperator>(value)) {
    std::string out; //= getPrettyName(BO->getOperand(0), true);
    bool need_bracket = false;
    bool additive = false;
    switch (BO->getOpcode()) {
    case Instruction::FMul:
    case Instruction::Mul:
      out = out + " * ";
      break;
    case Instruction::FDiv:
    case Instruction::UDiv:
    case Instruction::SDiv:
      out = out + " / ";
      break;
    case Instruction::FSub:
    case Instruction::Sub:
      out = out + " - ";
      need_bracket = true;
      additive = true;
      break;
    case Instruction::FAdd:
    case Instruction::Add:
      out = out + " + ";
      need_bracket = true;
      additive = true;
      break;
    case Instruction::URem:
    case Instruction::SRem:
    case Instruction::FRem:
      out = out + " % ";
      need_bracket = true;
      break;
    case Instruction::Shl:
      out = out + " << ";
      need_bracket = true;
      break;
    case Instruction::LShr:
    case Instruction::AShr:
      out = out + " >> ";
      need_bracket = true;
      break;
    case Instruction::And:
      out = out + " && ";
      need_bracket = true;
      break;
    case Instruction::Or:
      out = out + " || ";
      need_bracket = true;
      break;
    case Instruction::Xor:
      out = out + " ^ ";
      need_bracket = true;
      break;
    }
    // bool argument is used to make order of operations clear
    out = getPrettyName(BO->getOperand(0), !additive) + out +
          getPrettyName(BO->getOperand(1), !additive);
    if (need_bracket && maybe_bracket)
      out = "(" + out + ")";
    return out;
  } else if (UnaryOperator *UO = dyn_cast<UnaryOperator>(value)) {
    if (UO->getOpcode() == Instruction::FNeg) {
      return "-" + getPrettyName(UO->getOperand(0), true);
    } else if (UO->isCast())
      return getPrettyName(UO->getOperand(0), false);
  } else if (Argument *Arg = dyn_cast<Argument>(value)) {
    StringRef name = Arg->getName();
    if (name.empty()) {
      auto num = Arg->getArgNo();
      return "<arg no. " + std::to_string(num) + ">";
    }
    return name.str();
  } else if (CastInst *CI = dyn_cast<CastInst>(value)) {
    return getPrettyName(CI->getOperand(0), false);
  } else if (ConstantExpr *CE = dyn_cast<ConstantExpr>(value)) {
    if (CE->isCast())
      return getPrettyName(CE->getOperand(0), false);
  } else if (CmpInst *CI = dyn_cast<CmpInst>(value)) {
    std::string out;
    switch (CI->getPredicate()) {
    case CmpInst::Predicate::ICMP_EQ: ///< equal
      out = " == ";
      break;
    case CmpInst::Predicate::ICMP_NE: ///< not equal
      out = " != ";
      break;
    case CmpInst::Predicate::ICMP_UGT: ///< unsigned greater than
      out = " > ";
      break;
    case CmpInst::Predicate::ICMP_UGE: ///< unsigned greater or equal
      out = " >= ";
      break;
    case CmpInst::Predicate::ICMP_ULT: ///< unsigned less than
      out = " < ";
      break;
    case CmpInst::Predicate::ICMP_ULE: ///< unsigned less or equal
      out = " <= ";
      break;
    case CmpInst::Predicate::ICMP_SGT: ///< signed greater than
      out = " > ";
      break;
    case CmpInst::Predicate::ICMP_SGE: ///< signed greater or equal
      out = " >= ";
      break;
    case CmpInst::Predicate::ICMP_SLT: ///< signed less than
      out = " < ";
      break;
    case CmpInst::Predicate::ICMP_SLE: ///< signed less or equal
      out = " <= ";
      break;
    case CmpInst::Predicate::FCMP_OEQ: ///< signed less or equal
      out = " == ";
      break;
    case CmpInst::Predicate::FCMP_OGT: ///< True if ordered and greater than
      out = " > ";
      break;
    case CmpInst::Predicate::FCMP_OGE: ///< True if ordered and greater than or
                                       ///< equal
      out = " >= ";
      break;
    case CmpInst::Predicate::FCMP_OLT: ///< True if ordered and less than
      out = " < ";
      break;
    case CmpInst::Predicate::FCMP_OLE: ///< True if ordered and less than or
                                       ///< equal
      out = " <= ";
      break;
    case CmpInst::Predicate::FCMP_ONE: ///< True if ordered and operands are
                                       ///< unequal
      out = " != ";
      break;
    default:
      out = "<unknown cmp>";
    }
    out = getPrettyName(CI->getOperand(0), true) + out +
          getPrettyName(CI->getOperand(1), true);
    if (maybe_bracket)
      out = "(" + out + ")";
    return out;
  } else if (SelectInst *Sel = dyn_cast<SelectInst>(value)) {
    std::string out = getPrettyName(Sel->getCondition(), true) + " ? " +
                      getPrettyName(Sel->getTrueValue(), true) + " : " +
                      getPrettyName(Sel->getFalseValue(), true);
    if (maybe_bracket)
      out = "(" + out + ")";
    return out;
  } else if (UndefValue *UD = dyn_cast<UndefValue>(value)) {
    return "<undefined>";
  }

  return "<unknown>";
}

////////////////////////////////////////////////////////////////////////////////////////
// Common utilities

int is_unresolved_but_equal(Value *val1, Value *val2) {

  if (!val1 || !val2)
    return -1;

  User *use1 = nullptr;
  User *use2 = nullptr;
  Value *source1 = val1;
  Value *source2 = val2;
  std::vector<Value *> source_idx;

  if (Constant *C1 = dyn_cast<Constant>(val1)) {
    if (Constant *C2 = dyn_cast<Constant>(val2)) {
      return (int)(C1 == C2);
    }
  }

  // first check if the two are instructions and use the isIdenticalTo API.
  // If they are identical and don't read from memory, then we can safely
  // return 1.

  if (Instruction *I1 = dyn_cast<Instruction>(val1)) {
    if (Instruction *I2 = dyn_cast<Instruction>(val2)) {
      if (I1->isIdenticalToWhenDefined(I2))
        if (!(I1->mayReadFromMemory()))
          return 1;
    }
  }

  // We expect three unresolved possibliities (both have to fall under same
  // catagory to be equal): 1) they are both directly consuming arguments 2)
  // they are both load instructions 3) They are both getelementptrs We also
  // first check if they are CastInst

  if (CastInst *cast1 = dyn_cast<CastInst>(source1)) {
    source1 = cast1->getOperand(0);
  }
  if (CastInst *cast2 = dyn_cast<CastInst>(source2)) {
    source2 = cast2->getOperand(0);
  }

  // check for load instruction
  if (LoadInst *LI1 = dyn_cast<LoadInst>(source1)) {
    if (LoadInst *LI2 = dyn_cast<LoadInst>(source2)) {

      use1 = dyn_cast<User>(LI1);
      use2 = dyn_cast<User>(LI2);
      source1 = LI1->getPointerOperand();
      source2 = LI2->getPointerOperand();

      // check for getelementptr
      if (GetElementPtrInst *GI1 = dyn_cast<GetElementPtrInst>(source1)) {
        if (GetElementPtrInst *GI2 = dyn_cast<GetElementPtrInst>(source2)) {

          use1 = dyn_cast<User>(GI1);
          use2 = dyn_cast<User>(GI2);
          source1 = GI1->getPointerOperand();
          source2 = GI2->getPointerOperand();

          // check the if indices are the same

          auto idx1 = GI1->idx_begin();
          auto idx2 = GI2->idx_begin();

          while ((idx1 != GI1->idx_end()) && (idx2 != GI2->idx_end())) {
            source_idx.push_back(dyn_cast<Value>(idx1));
            if (dyn_cast<Value>(idx1) != dyn_cast<Value>(idx2))
              return -1;
            ++idx1;
            ++idx2;
          }
          // check if pointers are loads (of a pointer from a frome a pointer to
          // a pointer) this is as far a we'll go
          if (LoadInst *LII1 = dyn_cast<LoadInst>(source1)) {
            if (LoadInst *LII2 = dyn_cast<LoadInst>(source2)) {

              use1 = dyn_cast<User>(LII1);
              use2 = dyn_cast<User>(LII2);
              source1 = LII1->getPointerOperand();
              source2 = LII2->getPointerOperand();
            }
          }
        }
      }
    }
  }
  // check for direct arthmetic maipulation
  else if (BinaryOperator *BO1 = dyn_cast<BinaryOperator>(source1)) {
    if (BinaryOperator *BO2 = dyn_cast<BinaryOperator>(source2)) {
      // TODO:in future, we want to check for constant adding/mult etc.
      if (BO1->getOpcode() == BO2->getOpcode()) {
        int res1, res2, res3, res4;
        Value *val11 = BO1->getOperand(0);
        Value *val12 = BO1->getOperand(1);
        Value *val21 = BO2->getOperand(0);
        Value *val22 = BO2->getOperand(1);

        res1 = is_unresolved_but_equal(val11, val21);
        res2 = is_unresolved_but_equal(val11, val22);
        res3 = is_unresolved_but_equal(val12, val21);
        res4 = is_unresolved_but_equal(val12, val22);

        if (res1 == 1) {
          if (res4 == 1)
            return 1;
        } else if (res2 == 1) {
          if (res3 == 1) {
            auto op = BO1->getOpcode();
            if (op == Instruction::FAdd || op == Instruction::FMul ||
                op == Instruction::Add || op == Instruction::Mul) {
              return 1;
            }
          }
        }
      }
    }
  }

  // check if they are loaded from the same register
  if (source1 == source2) {

    // if there is no uses or they are the same, then sources are direct values
    if (use1 == use2)
      return 1;

    // check if there is a StoreInst between the two uses; if no store exists,
    // they are the same

    bool start = false, end = false;
    auto uit = source1->user_begin();
    StoreInst *SI = nullptr;
    bool first, check = true;
    unsigned pos = 0;

    while (!end) {

      if (uit == source1->user_end())
        end = true;
      else if (!start) {
        first = (*uit == use2);
        start = (*uit == use1) || first;
        ++uit;
      } else {
        // Check if we hit the second use
        if ((*uit == use1) || (*uit == use2))
          return 1;

        // Check for a LoadInst of a Pointer which is then used for a store
        else if (LoadInst *LI = dyn_cast<LoadInst>(*uit)) {

          if (PointerType *PTy = dyn_cast<PointerType>(source1->getType())) {
            // Is this a pointer to a pointer
            if ((dyn_cast<PointerType>(LI->getType()))) {

              for (auto uuit = LI->user_begin(); uuit != LI->user_end();
                   ++uuit) {

                if (GetElementPtrInst *GI =
                        dyn_cast<GetElementPtrInst>(*uuit)) {
                  pos = 0;
                  for (auto idx = GI->idx_begin(); idx != GI->idx_end();
                       ++idx) {
                    check = check && (dyn_cast<Value>(idx) == source_idx[pos]);
                    pos++;
                  }
                  if (check) {
                    for (auto uuuit = GI->user_begin(); uuuit != GI->user_end();
                         ++uuuit) {
                      if ((SI = dyn_cast<StoreInst>(*uuuit)))
                        break;
                    };
                  }
                } else
                  SI = dyn_cast<StoreInst>(*uuit);
              };
            }
          }
        }

        // Check if we hit a Store
        else if ((SI = dyn_cast<StoreInst>(*uit))) {

          // check type is not a pointer to a pointer
          if (PointerType *PTy = dyn_cast<PointerType>(source1->getType())) {
            // POINTER FIX
            if ((dyn_cast<PointerType>(SI->getValueOperand()->getType()))) {
              errs() << "Pointer to Array pointer was Changed!\n";
              return -1;
            }
          }
        }

        // If SI is not null, our answer is either -1 or 0.
        // For now if we have a store, we return -1 i.e. we don't know

        if ((SI)) {
          return -1;
        }
        ++uit;
      }
    }
  }
  return -1;
}

//////////////////////////////////////////////////////////////////
// ParaRef function call generation functions

// template<>
ParaRef takePower(Module &M, const ParaRef &base, int power) {
  if (base.isResolved()) {
    double base_val = base.getValueResolved().value();
    return ParaRef(std::pow(base_val, (double)power));
  }

  switch (power) {
  case -1:
    return ParaRef(1.0) / base;
    break;
  case 0:
    return ParaRef(1.0);
    break;
  case 1:
    return base;
    break;
  case 2:
    return base * base;
    break;
  default:
    Function *std_pow = Intrinsic::getDeclaration(
        &M, Intrinsic::pow,
        {Type::getDoubleTy(M.getContext()), Type::getDoubleTy(M.getContext())});
    ParaRef PRpow((double)power);
    CallInst *call =
        CallInst::Create(std_pow->getFunctionType(), std_pow,
                         {base.getValue(), PRpow.getValue()}, "aqcc.pow");
    return ParaRef(dyn_cast<Value>(call));
  }
}

ParaRef getCallToSignFromInt(ParaRef int_ref, unsigned pos, Module *M) {
  if (!int_ref.isInteger())
    return ParaRef();

  LLVMContext &C = M->getContext();

  int_ref.updateToSimplified();
  if (int_ref.isResolved()) {
    unsigned int_val = (unsigned)int_ref.getValueResolved().value();
    double sign = 1. - 2. * (double)((int_val >> pos) & 1);
    return ParaRef(sign, Type::getDoubleTy(C));
  }

  // get or insert the SignFromInt function
  const std::string sign_from_int_name = "_ZN6qsmath14getSignFromIntEjj";
  Type *ret_ty = Type::getDoubleTy(C);
  FunctionCallee funct = M->getOrInsertFunction(
      sign_from_int_name, ret_ty, int_ref.getType(), Type::getInt32Ty(C));

  ParaRef pos_ref(pos, Type::getInt32Ty(C));
  std::vector<Value *> args = {int_ref.getValue(), pos_ref.getValue()};
  // get new uninserted call to function
  CallInst *CI = CallInst::Create(funct, args, "aqcc.call");

  return ParaRef(dyn_cast<Value>(CI));
}

ParaRef handleDivisionByZero(const ParaRef &V) {

  // hold a map for already generated cases
  static std::map<Value *, Value *> DBZ_MAP;

  if (!V.isFloatingPoint())
    return ParaRef();

  auto dbz_iter = DBZ_MAP.find(V.getValue());

  if (dbz_iter == DBZ_MAP.end()) {
    // get V == 0 instruction
    Value *cmp = dyn_cast<Value>(
        CmpInst::Create(Instruction::FCmp, CmpInst::Predicate::FCMP_OEQ,
                        V.getValue(), ConstantFP::get(V.getType(), 0.)));
    // create ParaRef on cmp, as someone has to own this
    ParaRef dummy(cmp);
    // get select and return it
    dbz_iter = DBZ_MAP
                   .insert(std::make_pair(
                       V.getValue(), dyn_cast<Value>(SelectInst::Create(
                                         cmp, ConstantFP::getNaN(V.getType()),
                                         ConstantFP::get(V.getType(), 1.0)))))
                   .first;
  }
  return ParaRef(dbz_iter->second);
}

bool isHandleDivisionByZero(const ParaRef &V) {
  if (SelectInst *sel = dyn_cast<SelectInst>(V.getValue())) {
    if (ConstantFP *Cfp = dyn_cast<ConstantFP>(sel->getTrueValue()))
      return Cfp->isNaN();
  }
  return false;
}

ParaRef conditionOnHandleDivisionByZeros(const ParaRef &V,
                                         std::set<ParaRef> &div_by_zeros) {
  ParaRef out(V);
  for (auto &dbz : div_by_zeros) {
    if (SelectInst *sel = dyn_cast<SelectInst>(dbz.getValue())) {
      sel->setFalseValue(out.getValue());
      out = dbz;
    } else {
      return V;
    }
  }
  return out;
}

} // namespace aqcc
} // namespace llvm

#undef DEBUG_TYPE
