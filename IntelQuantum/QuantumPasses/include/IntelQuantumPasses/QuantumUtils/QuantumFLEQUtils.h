//===- QuantumFLEQUtils.h ---------------------------------------*- C++ -*-===//
//
// Copyright (C) 2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//
//===----------------------------------------------------------------------===//
/// \file "QuantumFLEQUtils.h"
///
/// This file defines a set of general utilities for the FLEQ compilation.
///
//===----------------------------------------------------------------------===//

#include "llvm/ADT/SCCIterator.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/Analysis/CFG.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/CFG.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/InstIterator.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Intrinsics.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Operator.h"
#include "llvm/IR/PassManager.h"
#include "llvm/IRReader/IRReader.h"
#include "llvm/InitializePasses.h"
#include "llvm/Pass.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/SourceMgr.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Transforms/Utils/Cloning.h"
#include <list>

#ifndef QUANTUM_FLEQ_UTILS_H
#define QUANTUM_FLEQ_UTILS_H

namespace llvm {

namespace aqcc {
class QbitRef;
}

namespace fleq {

class FLEQNode;
class FLEQGraph;

////////////////////////////////////////////////////////////////////////////////
// relevant constants across fleq compilation

// Builtin Frontend names
// eval functs
const std::string EVAL_HOLD_FRONT = "eval_hold";
const std::string EVAL_RELEASE_FRONT = "eval_release";
// basic Builtins
const std::string JOIN_FRONT = "qexpr::join";
const std::string BIND_FRONT = "qexpr::bind";
const std::string THIS_QEXPR_FRONT = "this_as_expr";
const std::string PROTECT_FRONT = "PROTECT";
const std::string IDENTITY_FRONT = "qexpr::identity";
// Coherent Builtins
const std::string INVERT_FRONT = "qexpr::invert";
const std::string CONTROL_FRONT = "qexpr::control";
const std::string MULTI_CONTROL_FRONT = "qexpr::control";
const std::string POWER_FRONT = "qexpr::power";
// Qbit Builtins
const std::string QBIT_JOIN_FRONT = "qlist::join";
const std::string QBIT_SLICE_FRONT = "qlist::slice";
const std::string QBIT_SIZE_FRONT = "qlist::size";
const std::string QBIT_AT_FRONT = "qlist::at";
// Classical control flow Builtins
const std::string BRANCH_FRONT = "qexpr::cIf";
const std::string GET_FRONT = "qexpr::get";
const std::string LET_FRONT = "qexpr::let";
// gate Builtin getter
std::string getFrontendNameFromGateIdentifier(int);

const std::string QEXPR_PRINT_FRONT = "qexpr::printQuantumLogic";
const std::string DATALIST_PRINT_FRONT = "qexpr::printDataList";
const std::string CT_EXIT_FRONT = "qexpr::exitAtCompile";
const std::string RT_EXIT_FRONT = "qexpr::exitAtRuntime";

// Builtin IR names
const std::string Builtin_PREFIX = "llvm.fleq";
const std::string Builtin_QEXPR = Builtin_PREFIX + ".qexpr";
const std::string Builtin_GATE = Builtin_PREFIX + ".gate.qexpr";
const std::string Builtin_QLIST = Builtin_PREFIX + ".qlist";
const std::string Builtin_DATALIST = Builtin_PREFIX + ".datalist";

// eval functs
const std::string EVAL_HOLD = Builtin_QEXPR + ".eval.hold";
const std::string EVAL_RELEASE = Builtin_QEXPR + ".eval.release";
// basic Builtins
const std::string JOIN = Builtin_QEXPR + ".join";
const std::string BIND = Builtin_QEXPR + ".bind";
const std::string THIS_QEXPR = Builtin_QEXPR + ".this.expression";
const std::string IDENTITY = Builtin_QEXPR + ".identity";
// Coherent Builtins
const std::string INVERT = Builtin_QEXPR + ".invert";
const std::string CONTROL = Builtin_QEXPR + ".control";
const std::string MULTI_CONTROL = Builtin_QEXPR + ".multi.control";
const std::string POWER = Builtin_QEXPR + ".power";
// gate Builtin getter
std::string getIRNameFromGateIdentifier(int);
int getGateIdentifierFromIRName(const std::string &);
// Classical control flow Builtins
const std::string BRANCH =
    Builtin_QEXPR + ".branch"; // Can also be a SelectInstruction
const std::string GET = Builtin_QEXPR + ".get";
const std::string LET = Builtin_QEXPR + ".let";

// qlist Builtins
const std::string QLIST_JOIN = Builtin_QLIST + ".join";
const std::string QLIST_SLICE = Builtin_QLIST + ".slice";
const std::string QLIST_SIZE = Builtin_QLIST + ".size";
const std::string QLIST_AT = Builtin_QLIST + ".at";
const std::string QLIST_EMPTY = Builtin_QLIST + ".empty";
const std::string QLIST_INTEGERIZE = Builtin_QLIST + ".integerize";
const std::string QLIST_BOOLEANIZE = Builtin_QLIST + ".booleanize";
const std::string QLIST_EQUAL = Builtin_QLIST + ".equal";
const std::string QLIST_BUILD = Builtin_QLIST + ".build";

// datalist Builtins
const std::string DATALIST_EMPTY = Builtin_DATALIST + ".empty";
const std::string DATALIST_FROM = Builtin_DATALIST + ".from";
const std::string DATALIST_BUILD = Builtin_DATALIST + ".build";
const std::string DATALIST_INTEGERIZE = Builtin_DATALIST + ".integerize";
const std::string DATALIST_BOOLEANIZE = Builtin_DATALIST + ".booleanize";
const std::string DATALIST_FLOATIZE = Builtin_DATALIST + ".floatize";
const std::string DATALIST_AT = Builtin_DATALIST + ".at";
const std::string DATALIST_SIZE = Builtin_DATALIST + ".size";
const std::string DATALIST_SLICE = Builtin_DATALIST + ".slice";
const std::string DATALIST_JOIN = Builtin_DATALIST + ".join";
const std::string DATALIST_FIND = Builtin_DATALIST + ".find";
const std::string DATALIST_LONGER = Builtin_DATALIST + ".longer";
const std::string DATALIST_SHORTER = Builtin_DATALIST + ".shorter";
const std::string DATALIST_CONTAINS = Builtin_DATALIST + ".contains";
const std::string DATALIST_COUNT = Builtin_DATALIST + ".count";
const std::string DATALIST_ALLOC = "_ZN15quantum_builtin17__datalist_alloca";

// QExpr debug and build control
const std::string PRINT_PASSTHRU = ".print.passthru";
const std::string QEXPR_PRINT_PASSTHRU = Builtin_QEXPR + PRINT_PASSTHRU;
const std::string DATALIST_PRINT_PASSTHRU = Builtin_DATALIST + PRINT_PASSTHRU;
const std::string CT_EXIT = Builtin_QEXPR + ".ct.exit";
const std::string RT_EXIT = Builtin_QEXPR + ".rt.exit";

// Relevant Twines
const std::string FLEQ_BRANCH_TWINE = "fleq.quantum.branch";
const std::string FLEQ_BRANCH_RELEASE_TWINE = FLEQ_BRANCH_TWINE + ".release";
const std::string FLEQ_QBB_TWINE = "fleq.quantum.temp";
const std::string FLEQ_EMPTY_TWINE = "fleq.empty";
const std::string FLEQ_EXIT_TWINE = "fleq.exit";

enum NodeType {
  // does not return a QExpr
  kNULL = 0,
  kEval,
  kLet,

  // has no dependencies
  kIdentity,
  kThisExpr,
  kGate,
  kLoadInst,
  kArg,
  kCtExit,
  kRtExit,
  kUnDef,

  // has one dependency
  kInvert,
  kCtrl,
  kMCtrl,
  kPower,
  kGet,
  kInvertInst,
  kPowerInst,
  kPrint,

  // has two dependencies
  kJoin,
  kBind,
  kBranch,
  kJoinInst,
  kBranchInst,

  // uspecified number of dependencies
  kPhiInst,
  kUnspecifiedCall,
  kCustomCall,
  kDead,
  kEnd,

  kNoReturnBegin = kNULL,
  kNoReturnEnd = kIdentity,
  kNoDepsBegin = kIdentity,
  kNoDepsEnd = kInvert,
  kOneDepBegin = kInvert,
  kOneDepEnd = kJoin,
  kTwoDepsBegin = kJoin,
  kTwoDepsEnd = kPhiInst,
  kMulDepsBegin = kPhiInst,
  kMulDepsEnd = kEnd

};

/// returns a node type representing the passed Value
NodeType characterizeNodeType(Value *);

///////////////////////////////////////////////////////////////////////////////

bool isQExpr(Value *val);
bool isQExprFunc(Value *val);

/// returns the Key pointed to by the arguement as a Constant for let/get
/// returns null if the arguement is not attacted to a key value
/// second arg is a return by ref to recode any GEP index shifts
Constant *getKey(Value *, unsigned &);

/// returns a SmallVector of dependencies for the FLEQ call (as a Value*)
SmallVector<Value *, 5>
getFLEQDependencies(Value *val, NodeType node_type = NodeType::kEnd);

//////////////////////////////////////////////////////////////////////////////
// used for custom call inlining

/// Checks that a node is not Builtin, i.e. is a custom call.
/// custom call must also be defined not just declared!
CallBase *isNodeACustomCall(FLEQNode *FN);

std::vector<Function *> getPassedCustomCalls(CallBase *CB);

/// Checks that a node is a SelectInst (branch in the front) and returns it
SelectInst *isNodeASelect(FLEQNode *FN);

/// Checks node is a join call and returns the call
CallBase *isNodeACallTo(const std::string name, FLEQNode *FN);

/// checks if a call base is a hold/release fixing call
bool isHoldCall(CallBase *);
bool isReleaseCall(CallBase *);

/// For Node types with multiple representations
bool isNodeAnyJoin(FLEQNode *FN);
bool isNodeAnyInvert(FLEQNode *FN);
bool isNodeAnyPower(FLEQNode *FN);
bool isNodeAnyBranch(FLEQNode *FN);

bool isThisQExprCall(CallBase *);
/// is the passed value a QList struct = {qbit*}?
/// returns the stored qbit* if it is found and
/// nullptr otherwise.
Value *isQListStruct(Value *val);

CallBase *isQListSizeCall(Value *val);

CallBase *isBuiltin(Value *);

/// This function imitates the InlineFuction function from "Cloning.h" but pares
/// down the internal logic by making assumptions relevant to QExpr-returning
/// functions in FLEQ. This is done to simplify the use and increase the
/// efficiency since inlining is a major part of the FLEQ compilation process.
/// The assumption are as follows:
///  - the called function has a fixed number of arguments.
///  - (big one) there is a single return instruction. This assumes all
///  branching on
///    the returned QExpr is controlled through branching Builtins. This is
///    reasonable as branching in the functional paradigm should be controlled
///    through this function.
///  - non-QExpr byValue arguement do not need to be copied; they are read-only.
///    This is just to avoid having to deal with local copies of these argument.
///    If need be, we can change this.
/// returned value the inlined value which represents the return.

Value *inlineFLEQFunction(CallBase *);
Value *inlineFLEQFunction(Function *Caller, Function *CalledFunc,
                          ValueToValueMapTy &VMap);

/// checks for above conditions without inlining
Value *validateQExpr(Function *);

/// This function checks to see if the FLEQGraph contains only Builtin nodes
bool isBuiltinOnly(FLEQGraph &FG);

/// Helper for replacing qubit based on FLEQ calls
bool replaceQbitWithSimplified(User *val, Instruction *I, bool is_after);

Value *addQbitCall(Instruction *I, aqcc::QbitRef to_add, bool is_after,
                   bool as_ptr = true);

CallBase *isIQCAlloca(const aqcc::QbitRef &q);

} // namespace fleq
} // namespace llvm

#endif