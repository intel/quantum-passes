//===- QuantumFLEQUtils.cpp -----------------------------*- C++ -*-===//
//
// Copyright (C) 2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//
//===----------------------------------------------------------------------===//
/// \file "QuantumFLEQUtils.cpp"
///
/// This file implements a set of general utilities for the FLEQ compilation.
///
//===----------------------------------------------------------------------===//

#include "IntelQuantumPasses/QuantumUtils/QuantumFLEQUtils.h"
#include "IntelQuantumPasses/QuantumAnalysis/QuantumFLEQGraph.h"
#include "IntelQuantumPasses/QuantumAnalysis/QuantumGateIdentifiers.h"
#include "IntelQuantumPasses/QuantumAnalysis/QuantumIterator.h"
#include "IntelQuantumPasses/QuantumAnalysis/QuantumRefs.h"
#include "IntelQuantumPasses/QuantumUtils/QuantumGeneralUtils.h"

#include "llvm/ADT/DepthFirstIterator.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/InstIterator.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Intrinsics.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/PassManager.h"
#include "llvm/IRReader/IRReader.h"
#include "llvm/InitializePasses.h"
#include "llvm/Pass.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/SourceMgr.h"
#include "llvm/Support/raw_ostream.h"
#include <list>
#include <string>

using namespace aqcc;

namespace llvm {
namespace fleq {

//////////////////////////////////////////////////////////////////////////////////////////

std::string getFrontendNameFromGateIdentifier(int id) {
  switch (id) {
  case kH:
    return "_H";
  case kX:
    return "_X";
  case kY:
    return "_Y";
  case kZ:
    return "_Z";
  case kS:
    return "_S";
  case kSDAG:
    return "_Sdag";
  case kT:
    return "_T";
  case kTDAG:
    return "_Tdag";
  case kRX:
    return "_RX";
  case kRY:
    return "_RY";
  case kRZ:
    return "_RZ";
  case kCZ:
    return "_CZ";
  case kCNOT:
    return "_CNOT";
  case kSWAP:
    return "_SWAP";
  case kTOFF:
    return "_Toffoli";
  case kPREPX:
    return "_PrepX";
  case kPREPY:
    return "_PrepY";
  case kPREPZ:
    return "_PrepZ";
  case kMEASX:
    return "_MeasX";
  case kMEASY:
    return "_MeasY";
  case kMEASZ:
    return "_MeasZ";
  case kCPHASE:
    return "_CPhase";
  case kSWAPA:
    return "_SwapA";
  case kRXY:
    return "_RXY";
  }
  return "";
}

std::string getIRNameFromGateIdentifier(int id) {

  switch (id) {
  case kH:
    return Builtin_GATE + ".h";
  case kX:
    return Builtin_GATE + ".x";
  case kY:
    return Builtin_GATE + ".y";
  case kZ:
    return Builtin_GATE + ".z";
  case kS:
    return Builtin_GATE + ".s";
  case kSDAG:
    return Builtin_GATE + ".s.dag";
  case kT:
    return Builtin_GATE + ".t";
  case kTDAG:
    return Builtin_GATE + ".t.dag";
  case kRX:
    return Builtin_GATE + ".rx";
  case kRY:
    return Builtin_GATE + ".ry";
  case kRZ:
    return Builtin_GATE + ".rz";
  case kCZ:
    return Builtin_GATE + ".cz";
  case kCNOT:
    return Builtin_GATE + ".cnot";
  case kSWAP:
    return Builtin_GATE + ".swap";
  case kTOFF:
    return Builtin_GATE + ".tof";
  case kPREPX:
    return Builtin_GATE + ".prepx";
  case kPREPY:
    return Builtin_GATE + ".prepy";
  case kPREPZ:
    return Builtin_GATE + ".prepz";
  case kMEASX:
    return Builtin_GATE + ".measx";
  case kMEASY:
    return Builtin_GATE + ".measy";
  case kMEASZ:
    return Builtin_GATE + ".measz";
  case kCPHASE:
    return Builtin_GATE + ".cphase";
  case kSWAPA:
    return Builtin_GATE + ".swapa";
  case kRXY:
    return Builtin_GATE + ".rxy";
  }
  return "";
}

int getGateIdentifierFromIRName(const std::string &name) {

  // those with substring equal to another gate must come first
  // in the if-else sequence

  if (name.find(Builtin_GATE + ".h") != std::string::npos)
    return kH;
  else if (name.find(Builtin_GATE + ".x") != std::string::npos)
    return kX;
  else if (name.find(Builtin_GATE + ".y") != std::string::npos)
    return kY;
  else if (name.find(Builtin_GATE + ".z") != std::string::npos)
    return kZ;
  else if (name.find(Builtin_GATE + ".s.dag") != std::string::npos)
    return kSDAG;
  else if (name.find(Builtin_GATE + ".swapa") != std::string::npos)
    return kSWAPA;
  else if (name.find(Builtin_GATE + ".swap") != std::string::npos)
    return kSWAP;
  else if (name.find(Builtin_GATE + ".s") != std::string::npos)
    return kS;
  else if (name.find(Builtin_GATE + ".tof") != std::string::npos)
    return kTOFF;
  else if (name.find(Builtin_GATE + ".t.dag") != std::string::npos)
    return kTDAG;
  else if (name.find(Builtin_GATE + ".t") != std::string::npos)
    return kT;
  else if (name.find(Builtin_GATE + ".rxy") != std::string::npos)
    return kRXY;
  else if (name.find(Builtin_GATE + ".rx") != std::string::npos)
    return kRX;
  else if (name.find(Builtin_GATE + ".ry") != std::string::npos)
    return kRY;
  else if (name.find(Builtin_GATE + ".rz") != std::string::npos)
    return kRZ;
  else if (name.find(Builtin_GATE + ".cz") != std::string::npos)
    return kCZ;
  else if (name.find(Builtin_GATE + ".cnot") != std::string::npos)
    return kCNOT;
  else if (name.find(Builtin_GATE + ".prepx") != std::string::npos)
    return kPREPX;
  else if (name.find(Builtin_GATE + ".prepy") != std::string::npos)
    return kPREPY;
  else if (name.find(Builtin_GATE + ".prepz") != std::string::npos)
    return kPREPZ;
  else if (name.find(Builtin_GATE + ".measx") != std::string::npos)
    return kMEASX;
  else if (name.find(Builtin_GATE + ".measy") != std::string::npos)
    return kMEASY;
  else if (name.find(Builtin_GATE + ".measz") != std::string::npos)
    return kMEASZ;
  else if (name.find(Builtin_GATE + ".cphase") != std::string::npos)
    return kCPHASE;
  return -1;
}

NodeType characterizeNodeType(Value *val) {
  // first check for a function call
  if (CallBase *CB = dyn_cast<CallBase>(val)) {

    Function *F = CB->getCalledFunction();

    // Check for calls to arguments
    if (!F) {
      if (Argument *A = dyn_cast<Argument>(CB->getCalledOperand())) {
        if (A->getParent()
                ->getAttributes()
                .getParamAttrs(A->getArgNo())
                .hasAttribute("qexpr_func_val"))
          return kUnspecifiedCall;
      } else
        return kNULL;
    }

    StringRef called_name = F->getName();

    // check for builtin
    if (called_name.contains(Builtin_PREFIX)) {

      if (called_name.contains(EVAL_HOLD))
        return kEval;
      else if (called_name.contains(EVAL_RELEASE))
        return kEval;
      else if (called_name.contains(IDENTITY))
        return kIdentity;
      else if (called_name.contains(JOIN))
        return kJoin;
      else if (called_name.contains(BIND))
        return kBind;
      else if (called_name.contains(THIS_QEXPR))
        return kThisExpr;
      else if (called_name.contains(INVERT))
        return kInvert;
      else if (called_name.contains(CONTROL))
        return kCtrl;
      else if (called_name.contains(MULTI_CONTROL))
        return kMCtrl;
      else if (called_name.contains(POWER))
        return kPower;
      else if (called_name.contains(BRANCH))
        return kBranch;
      else if (called_name.contains(GET))
        return kGet;
      else if (called_name.contains(LET))
        return kLet;
      else if (called_name.contains(Builtin_GATE))
        return kGate;
      else if (called_name.contains(PRINT_PASSTHRU))
        return kPrint;
      else if (called_name.contains(CT_EXIT))
        return kCtExit;
      else if (called_name.contains(RT_EXIT))
        return kRtExit;
      else {
        return kNULL;
      }
    } else if (F->hasFnAttribute("qexpr_func_val"))
      return kCustomCall;
  } else if (Argument *A = dyn_cast<Argument>(val)) {
    if (A->getParent()
            ->getAttributes()
            .getParamAttrs(A->getArgNo())
            .hasAttribute("qexpr_val"))
      return kArg;
  } else if (SelectInst *SI = dyn_cast<SelectInst>(val)) {
    if (isQExpr(SI->getOperand(1)) && isQExpr(SI->getOperand(2)))
      return kBranchInst;
  } else if (PHINode *PN = dyn_cast<PHINode>(val)) {
    for (Value *V : PN->incoming_values())
      if (!isQExpr(V)) {
        return kNULL;
      }
    return kPhiInst;
  } else if (isa<UndefValue>(val)) {
    // at times, we may need to pass through an undef and handle it at
    // compilation
    return kUnDef;
  } else if (isa<LoadInst>(val)) {
    return kLoadInst;
  } else if (Instruction *I = dyn_cast<Instruction>(val)) {
    if (I->isBinaryOp()) {
      bool lhs = isQExpr(I->getOperand(0));
      bool rhs = isQExpr(I->getOperand(1));
      unsigned op = I->getOpcode();
      // check for "join"
      if (lhs && rhs)
        if (op == Instruction::Add || op == Instruction::Mul)
          return kJoinInst;
        else {
          return kNULL;
        }

      // check for "power"
      if (lhs && op == Instruction::Xor)
        return kPowerInst;

      // check for "invert"
      if (rhs) {
        if (op == Instruction::Sub) {
          if (ConstantInt *CInt = dyn_cast<ConstantInt>(I->getOperand(0))) {
            if (CInt->isZero())
              return kInvertInst;
          }
        }
      }
    }
  }

  return kNULL;
}

//////////////////////////////////////////////////////////////////////////////////////////

// IsQExpr function
bool isQExpr(Value *val) {
  return characterizeNodeType(val) >= NodeType::kNoReturnEnd;
}

// We have a set of intrinsics that return intrinsics. The function attributes
// on these do not propogate from pass to pass, so we need to have these hard
// coded. We should either set this up with a macro and a .def file, or a
// function map.
bool isQExprReturningIntrinsics(Function *IF) {
  if (IF->getName().contains("llvm.fleq.qexpr.identity")) {
    return true;
  } else if (IF->getName().contains("llvm.fleq.gate.qexpr")) {
    return true;
  } else if (IF->getName().contains("llvm.fleq.qexpr.join")) {
    return true;
  } else if (IF->getName().contains("llvm.fleq.qexpr.bind")) {
    return true;
  } else if (IF->getName().contains("llvm.fleq.qexpr.invert")) {
    return true;
  } else if (IF->getName().contains("llvm.fleq.qexpr.power")) {
    return true;
  } else if (IF->getName().contains("llvm.fleq.qexpr.control")) {
    return true;
  } else if (IF->getName().contains("llvm.fleq.qexpr.multi.control")) {
    return true;
  } else if (IF->getName().contains("llvm.fleq.qexpr.branch")) {
    return true;
  } else if (IF->getName().contains("llvm.fleq.qexpr.let")) {
    return true;
  } else if (IF->getName().contains("llvm.fleq.qexpr.get")) {
    return true;
  } else if (IF->getName().contains("llvm.fleq.qexpr.this.expression")) {
    return true;
  } else if (IF->getName().contains("llvm.fleq.qexpr.barrier")) {
    return true;
  } else if (IF->getName().contains(QEXPR_PRINT_PASSTHRU)) {
    return true;
  } else if (IF->getName().contains(DATALIST_PRINT_PASSTHRU)) {
    return true;
  } else if (IF->getName().contains(CT_EXIT)) {
    return true;
  } else if (IF->getName().contains(RT_EXIT)) {
    return true;
  }

  return false;
}

// New way to tell that a value is a function that returns a QExpr since we
// won't be able to use pointer types anymore.
bool isQExprFunc(Value *val) {
  Function *F = dyn_cast<Function>(val);
  // If it's a function, we just need to see if there's a function attribute
  // there.
  if (F) {
    if (isQExprReturningIntrinsics(F))
      return true;
    return F->hasFnAttribute("qexpr_func_val");
  }

  // If it's a argument, we see if it has an attribute marking it as a
  // qexpr_func this is determined through argument attributes in
  // InsertQuantumAttributes.cpp
  Argument *A = dyn_cast<Argument>(val);
  if (A) {
    return A->getParent()
        ->getAttributes()
        .getParamAttrs(A->getArgNo())
        .hasAttribute("qexpr_func_val");
  }

  return false;
}

// Using llvm SmallVector
SmallVector<Value *, 5> getFLEQDependencies(Value *val, NodeType node_ty) {

  SmallVector<Value *, 5> out;
  NodeType type = NodeType::kNULL;

  if (node_ty == NodeType::kEnd)
    type = characterizeNodeType(val);
  else
    type = node_ty;

  switch (type) {
  case NodeType::kNULL:
    errs() << *val << "\n";
    assert(false && "Bad FLEQ node!\n");
  case NodeType::kEval:
    if (CallBase *CB = dyn_cast<CallBase>(val))
      out.push_back(CB->getArgOperand(0));
    break;
  case NodeType::kLet:
    assert(false && "no 'let' nodes!\n");
  // no dependency
  case NodeType::kIdentity:
  case NodeType::kThisExpr:
  case NodeType::kGate:
  case NodeType::kLoadInst: // not valid, but handled by compilation
  case NodeType::kArg:
  case NodeType::kCtExit:
  case NodeType::kRtExit:
  case NodeType::kUnDef:
    break;
  // one dependency
  case NodeType::kInvert:
    if (CallBase *CB = dyn_cast<CallBase>(val)) {
      Value *arg = CB->getArgOperand(0);
      // unary ! can introduce a zext and cmp instruction.
      // check for this
      if (ZExtInst *ext = dyn_cast<ZExtInst>(arg)) {
        Value *ext_arg = ext->getOperand(0);
        if (ICmpInst *cmp = dyn_cast<ICmpInst>(ext_arg)) {
          if (cmp->isEquality()) {
            arg = cmp->getOperand(0);
            if (isQExpr(cmp->getOperand(0)))
              arg = cmp->getOperand(0);
            else
              arg = cmp->getOperand(1);
          }
        }
      }
      out.push_back(arg);
    }
    break;
  case NodeType::kCtrl:
  case NodeType::kMCtrl:
  case NodeType::kPower:
  case NodeType::kPrint:
    if (CallBase *CB = dyn_cast<CallBase>(val)) {
      unsigned n_arg = CB->arg_size();
      out.push_back(CB->getArgOperand(n_arg - 1));
    }
    break;
  case NodeType::kGet:
    assert(false && "no 'get' nodes!\n");
  case NodeType::kInvertInst:
    if (Instruction *I = dyn_cast<Instruction>(val))
      out.push_back(I->getOperand(1));
    break;
  case NodeType::kPowerInst:
    if (Instruction *I = dyn_cast<Instruction>(val))
      out.push_back(I->getOperand(0));
    break;
  // two dependencies
  case NodeType::kJoin:
  case NodeType::kBind:
    if (CallBase *CB = dyn_cast<CallBase>(val)) {
      out.push_back(CB->getArgOperand(0));
      out.push_back(CB->getArgOperand(1));
    }
    break;
  case NodeType::kBranch:
    if (CallBase *CB = dyn_cast<CallBase>(val)) {
      out.push_back(CB->getArgOperand(1));
      out.push_back(CB->getArgOperand(2));
    }
    break;
  case NodeType::kJoinInst:
    if (Instruction *I = dyn_cast<Instruction>(val)) {
      if (I->getOpcode() == Instruction::Add) {
        out.push_back(I->getOperand(0));
        out.push_back(I->getOperand(1));
      } else if (I->getOpcode() == Instruction::Mul) {
        out.push_back(I->getOperand(1));
        out.push_back(I->getOperand(0));
      }
    }
    break;
  case NodeType::kBranchInst:
    if (Instruction *I = dyn_cast<Instruction>(val)) {
      out.push_back(I->getOperand(1));
      out.push_back(I->getOperand(2));
    }
    break;
  // unspecified number of dependencies
  case NodeType::kPhiInst:
    if (PHINode *Phi = dyn_cast<PHINode>(val)) {
      for (auto &incoming : Phi->incoming_values()) {
        out.push_back(dyn_cast<Value>(&incoming));
      }
    }
    break;
  case NodeType::kUnspecifiedCall:
  case NodeType::kCustomCall:
    if (CallBase *CB = dyn_cast<CallBase>(val)) {
      int numArgs = CB->arg_size();
      for (int i = 0; i < numArgs; i++) {
        Value *vl = CB->getArgOperand(i);
        if (isQExpr(vl))
          out.push_back(vl);
        // check for passed QExpr returning functions
        // if the function is not an intrinsic, but returns
        // an QExpr, add it's return to the dependency list.
        else if (Function *Farg = dyn_cast<Function>(vl)) {
          if (isQExprFunc(Farg) && !(Farg->isIntrinsic())) {
            for (Function::iterator bl = Farg->begin(), ble = Farg->end();
                 bl != ble; ++bl) {
              if (ReturnInst *Rinst =
                      dyn_cast<ReturnInst>(bl->getTerminator())) {
                if (isQExpr(Rinst->getReturnValue()))
                  out.push_back(Rinst->getReturnValue());
              }
            }
          }
        }
      }
      // check if called function is specified
      if (Function *F = CB->getCalledFunction()) {
        // Add the return value of user defined function
        // Iterate through function basic blocks
        // Getting the value returned by the function
        for (Function::iterator bl = F->begin(), ble = F->end(); bl != ble;
             ++bl) {
          if (ReturnInst *Rinst = dyn_cast<ReturnInst>(bl->getTerminator())) {
            if (isQExpr(Rinst->getReturnValue()))
              out.push_back(Rinst->getReturnValue());
          }
        }
      }
    }
    break;
  case kDead:
    break;
  }
  return out;
}

Constant *getKey(Value *val, unsigned &shft) {
  if (!val)
    return nullptr;
  // check that it is a GlobalVariable
  if (GlobalVariable *GV = dyn_cast<GlobalVariable>(val)) {
    Type *GVType = GV->getValueType();
    if (GVType->isArrayTy()) {
      return GV->getInitializer();
    } else if (GVType->isPointerTy() || GVType->isStructTy()) {
      // look for a global startup call to store into the pointer
      Value *to_check = nullptr;
      for (auto user : GV->users()) {
        if (StoreInst *SI = dyn_cast<StoreInst>(user)) {
          StringRef section = SI->getParent()->getParent()->getSection();
          if (section.contains("text.startup") && !(to_check)) {
            to_check = SI->getValueOperand();
          } else
            return nullptr;
        }
      }
      return getKey(to_check, shft);
    }
  } else if (AllocaInst *AI = dyn_cast<AllocaInst>(val)) {
    // look for a memcpy to AI
    for (auto user_ai : AI->users()) {
      if (CallBase *CB = dyn_cast<CallBase>(user_ai)) {
        if ((CB->getCalledFunction()->getName()).contains("memcpy")) {
          return getKey(CB->getArgOperand(1), shft);
        }
      } else if (GEPOperator *gep_u = dyn_cast<GEPOperator>(user_ai)) {
        for (auto user_gep : gep_u->users()) {
          if (CallBase *CB = dyn_cast<CallBase>(user_gep)) {
            if ((CB->getCalledFunction()->getName()).contains("memcpy")) {
              return getKey(CB->getArgOperand(1), shft);
            }
          }
        }
      }
    }
  } else if (GEPOperator *GO = dyn_cast<GEPOperator>(val)) {

    Constant *key = getKey(GO->getPointerOperand(), shft);

    if (key) {

      auto idx = GO->idx_end();
      auto beg = GO->idx_begin();
      --idx;
      ParaRef PR_idx(dyn_cast<Value>(idx));
      PR_idx.updateToSimplifiedRecursive();

      if (PR_idx.isResolved()) {
        shft += (unsigned)PR_idx.getValueResolved().value();
      } else
        return nullptr;

      // check for a second index and Register size
      // to account for register overflow
      if (idx != beg) {
        --idx;
        ParaRef Idx2(dyn_cast<Value>(idx));
        PR_idx.updateToSimplifiedRecursive();
        ConstantDataSequential *CDS = dyn_cast<ConstantDataSequential>(key);
        if (!CDS)
          return nullptr;
        unsigned num_el = CDS->getNumElements();
        if (Idx2.isResolved()) {
          shft += (unsigned)Idx2.getValueResolved().value() * num_el;
        }
      }
    }
    return key;
  } else if (LoadInst *LI = dyn_cast<LoadInst>(val)) {
    return getKey(LI->getPointerOperand(), shft);
  }
  return nullptr;
}

CallBase *isNodeACustomCall(FLEQNode *FN) {
  if (FN->getNodeType() == kCustomCall)
    return dyn_cast<CallBase>(FN->getFunctionCall());
  return nullptr;
}

std::vector<Function *> getPassedCustomCalls(CallBase *CB) {
  std::vector<Function *> out;
  for (size_t i = 0; i < CB->arg_size(); i++) {
    if (Function *F = dyn_cast<Function>(CB->getArgOperand(i))) {
      if (isQExprFunc(F) && !(F->isIntrinsic())) {
        out.push_back(F);
      }
    }
  }
  return out;
}

SelectInst *isNodeASelect(FLEQNode *FN) {
  if (FN->getNodeType() == kBranchInst)
    return dyn_cast<SelectInst>(FN->getFunctionCall());
  return nullptr;
}

CallBase *isNodeACallTo(const std::string name, FLEQNode *FN) {
  if (CallBase *CB = dyn_cast<CallBase>(FN->getFunctionCall())) {
    if (Function *called = CB->getCalledFunction()) {
      if (called->getName().contains(name))
        return CB;
    }
  }
  return nullptr;
}

bool isNodeAnyJoin(FLEQNode *FN) {
  NodeType type = FN->getNodeType();
  return type == kJoin || type == kJoinInst;
}

bool isNodeAnyInvert(FLEQNode *FN) {
  NodeType type = FN->getNodeType();
  return type == kInvert || type == kInvertInst;
}

bool isNodeAnyPower(FLEQNode *FN) {
  NodeType type = FN->getNodeType();
  return type == kPower || type == kPowerInst;
}

bool isNodeAnyBranch(FLEQNode *FN) {
  NodeType type = FN->getNodeType();
  return type == kBranch || type == kBranchInst;
}

bool isThisQExprCall(CallBase *CB) {
  if (!CB)
    return false;
  Function *F = CB->getCalledFunction();
  if (!F)
    return false;
  return (F->getName()).contains(THIS_QEXPR);
}

bool isHoldCall(CallBase *CB) {
  if (!CB)
    return false;
  return (CB->getCalledFunction()->getName()).contains(EVAL_HOLD);
}

bool isReleaseCall(CallBase *CB) {
  if (!CB)
    return false;
  return (CB->getCalledFunction()->getName()).contains(EVAL_RELEASE);
}

CallBase *isQListSizeCall(Value *val) {
  if (CallBase *CB = dyn_cast<CallBase>(val)) {
    StringRef called_name = CB->getCalledFunction()->getName();
    if (called_name.contains(QLIST_SIZE))
      return CB;
  }
  return nullptr;
}

Value *isQListStruct(Value *val) {
  // first, check the type to find that it is a
  // - struct
  // - contains a single pointer
  // - has qlist::QList name
  Value *out = nullptr;
  Type *alloca_ty = nullptr;
  if (GlobalVariable *GV = dyn_cast<GlobalVariable>(val)) {
    alloca_ty = GV->getValueType();
    // attempting to find a unique stored value
    bool found = false;
    for (auto user : GV->users()) {
      if (StoreInst *SI = dyn_cast<StoreInst>(user)) {
        if (found)
          return nullptr;
        found = true;
        StringRef section = SI->getParent()->getParent()->getSection();
        if (section.contains("text.startup")) {
          out = SI->getValueOperand();
        }
      }
    }
  } else if (AllocaInst *AI = dyn_cast<AllocaInst>(val)) {
    alloca_ty = AI->getAllocatedType();
    // look for a unique store to AI
    bool found = false;
    for (auto user_ai : AI->users()) {
      if (StoreInst *SI = dyn_cast<StoreInst>(user_ai)) {
        if (found)
          return nullptr;
        found = true;
        out = SI->getValueOperand();
      }
      if (CallBase *CB = dyn_cast<CallBase>(user_ai)) {
        if ((CB->getCalledFunction()->getName()).contains("memcpy")) {
          return isQListStruct(CB->getArgOperand(1));
        }
      }
    }
  } else if (GEPOperator *gep = dyn_cast<GEPOperator>(val)) {
    return isQListStruct(gep->getPointerOperand());
  }

  if (!alloca_ty)
    return nullptr;

  if (StructType *Sty = dyn_cast<StructType>(alloca_ty)) {
    if (Sty->getNumElements() == 1) {
      if (Sty->getTypeAtIndex((unsigned)0)->isPointerTy() &&
          (Sty->getName()).contains("qlist::QList")) {
        return out;
      }
    }
  }
  return nullptr;
}

CallBase *isBuiltin(Value *val) {
  if (CallBase *CB = dyn_cast<CallBase>(val)) {
    StringRef called_name = CB->getCalledFunction()->getName();
    if (called_name.contains(Builtin_PREFIX))
      return CB;
  }
  return nullptr;
}

Value *inlineFLEQFunction(CallBase *CB) {

  Function *CalledFunc = CB->getCalledFunction();
  Instruction *I = dyn_cast<Instruction>(CB);
  Function *Caller = I->getParent()->getParent();

  // first check that the called function is defined
  // in IR
  if (!CalledFunc || CalledFunc->isDeclaration())
    return nullptr;

  // Check that the function has no argument bundles
  if (CB->hasOperandBundles())
    return nullptr;

  // Split at the point of the call and move newly insert BBs in between
  // BasicBlock * pre_split = I->getParent();
  // std::string exit_twine = "exit" + called_name +".fleq";
  // BasicBlock * split = I->getParent()->splitBasicBlock(I, exit_twine);

  // Now map call operands to formal arguments
  ValueToValueMapTy VMap;

  // auto &DL = Caller->getParent()->getDataLayout();

  auto AI = CB->arg_begin();
  unsigned ArgNo = 0;
  for (auto I = CalledFunc->arg_begin(), E = CalledFunc->arg_end(); I != E;
       ++I, ++AI, ++ArgNo) {
    Value *ActualArg = *AI;

    // When byval arguments actually inlined, we need to make the copy implied
    // by them explicit.  However, we don't do this if the callee is readonly
    // or readnone, because the copy would be unneeded: the callee doesn't
    // modify the struct.
    /* Currently not handling this
    if (CS.isByValArgument(ArgNo)) {
      ActualArg = HandleByValArgument(ActualArg, TheCall, CalledFunc, IFI,
                                      CalledFunc->getParamAlignment(ArgNo));
    if (ActualArg != *AI)
      ByValInit.push_back(std::make_pair(ActualArg, (Value*) *AI));
    }
    */

    VMap[&*I] = ActualArg;
  }

  return inlineFLEQFunction(Caller, CalledFunc, VMap);
}

Value *inlineFLEQFunction(Function *Caller, Function *CalledFunc,
                          ValueToValueMapTy &VMap) {

  std::string called_name = (CalledFunc->getName()).str();

  // We use the CloneFunctionInto (which is also used by InlineFunction)
  // to insert instruction. No IR clean up as it undoes our conditioning
  SmallVector<ReturnInst *, 3> Returns;
  std::string entry_twine = called_name + ".fleq";
  // When we're inlining, it's not going to be the case that the calling
  // function is going to be returning a qexpr like the called function.  So we
  // make sure that we remove it from the calling function if needed.
  bool CallerHasQExpr = Caller->hasFnAttribute("qexpr_func_val");
  bool CalledHasQExpr = CalledFunc->hasFnAttribute("qexpr_func_val");
  bool CallerHasNoundef = Caller->hasRetAttribute(Attribute::AttrKind::NoUndef);
  bool CalledHasNoundef =
      CalledFunc->hasRetAttribute(Attribute::AttrKind::NoUndef);
  bool CallerHasQBBSection = Caller->getSection() == QBBS_TEXT_SECTION;

  // recode attrs of caller arguments to re-assert them after clone
  SmallVector<unsigned> hasQbitAttr;
  SmallVector<unsigned> hasQExprAttr;
  SmallVector<unsigned> hasQExprFuncAttr;
  for (size_t arg = 0; arg < Caller->arg_size(); arg++) {
    auto attrs = (Caller->getAttributes());
    if (attrs.hasParamAttr(arg, "qubit_ref"))
      hasQbitAttr.push_back(arg);
    if (attrs.hasParamAttr(arg, "qexpr_val"))
      hasQExprAttr.push_back(arg);
    if (attrs.hasParamAttr(arg, "qexpr_func_val"))
      hasQExprAttr.push_back(arg);
  }

  CloneFunctionInto(Caller, CalledFunc, VMap,
                    CloneFunctionChangeType::LocalChangesOnly, Returns,
                    entry_twine.c_str());
  if (!CallerHasQExpr && CalledHasQExpr)
    Caller->removeFnAttr("qexpr_func_val");
  if (!CallerHasNoundef && CalledHasNoundef)
    Caller->removeRetAttr(Attribute::AttrKind::NoUndef);
  if (CallerHasQBBSection)
    Caller->setSection(QBBS_TEXT_SECTION);

  // reassert argument attributes
  AttrBuilder QbitAttr(Caller->getContext());
  QbitAttr.addAttribute("qubit_ref");
  for (size_t i = 0; i < hasQbitAttr.size(); i++) {
    Caller->addParamAttrs(hasQbitAttr[i], QbitAttr);
  }
  AttrBuilder QExprAttr(Caller->getContext());
  QExprAttr.addAttribute("qexpr_val");
  for (size_t i = 0; i < hasQExprAttr.size(); i++) {
    Caller->addParamAttrs(hasQExprAttr[i], QExprAttr);
  }
  AttrBuilder QExprFuncAttr(Caller->getContext());
  QExprFuncAttr.addAttribute("qexpr_func_val");
  for (size_t i = 0; i < hasQExprFuncAttr.size(); i++) {
    Caller->addParamAttrs(hasQExprFuncAttr[i], QExprFuncAttr);
  }

  if (Returns.size() != 1) {
    for (int i = 0; i < Returns.size(); i++)
      Returns[i]->eraseFromParent();
    return nullptr;
  }

  // NOTE: This will result in Broken IR
  // Assumes later passes will fix it

  Value *out = Returns[0]->getReturnValue();
  Returns[0]->eraseFromParent();

  return out;
}

Value *validateQExpr(Function *F) {

  // first check that the called function is defined
  // in IR
  if (!F || F->isDeclaration())
    return nullptr;

  // Check that the function has no argument bundles
  if (F->isVarArg())
    return nullptr;

  // use SCCIterator to get the last SCC to get the return

  auto ret_scc = scc_begin(F);

  if (ret_scc->size() != 1)
    return nullptr;

  BasicBlock *ret_block = ret_scc->front();
  Instruction *term = ret_block->getTerminator();
  if (ReturnInst *ret = dyn_cast<ReturnInst>(term)) {
    // move the return block to the end
    ret_block->moveBefore(F->end());
    return ret->getReturnValue();
  }

  return nullptr;
}

// Helper for checking for BuiltinOnly function
// returns flag if a "bad" node is found
// kill argument is a way to stop the recursive DFS if a "bad" node is found
bool visitBuiltinChildren(FLEQNode *FN, std::set<FLEQNode *> &visited,
                          bool kill = false) {

  if (FN->getFunctionCall() == nullptr)
    return false;
  if (kill)
    return true;

  if (isNodeACustomCall(FN))
    return true;

  if (dyn_cast<Function>(FN->getFunctionCall()))
    return true;

  if (visited.find(FN) != visited.end())
    return false;

  bool kill_next = false;
  visited.insert(FN);

  for (auto iter = FN->begin(), end = FN->end(); iter != end; ++iter) {
    kill_next = visitBuiltinChildren(*iter, visited, kill_next);
  }

  return kill_next;
}

bool isBuiltinOnly(FLEQGraph &FG) {

  std::set<FLEQNode *> visited;

  return !visitBuiltinChildren(FG.getRoot(), visited);
}

bool replaceQbitWithSimplified(User *val, Instruction *I, bool is_after) {

  QbitRef passed = getQbitRefPointedToBy(dyn_cast<Value>(val));
  if (passed.isNull())
    return false;

  bool was_fleq = passed.isFLEQCall();
  if (was_fleq && !passed.updateToSimplifiedRecursive())
    return false;
  // use dummy Qiter for insert
  if (passed.isNull())
    return false;
  Value *replacement = addQbitCall(I, passed, is_after, true);
  val->replaceAllUsesWith(replacement);

  return true;
}

Value *addQbitCall(Instruction *I, QbitRef to_add, bool is_after, bool as_ptr) {
  QIter dummy;
  return dummy.add_qbit_call(I, to_add, is_after, as_ptr);
}

CallBase *isIQCAlloca(const QbitRef &q) {
  if (CallBase *CB = dyn_cast<CallBase>(q.getRegister())) {
    if (Function *F = CB->getCalledFunction()) {
      if (F->getName().contains(DATALIST_ALLOC))
        return CB;
    }
  }
  return nullptr;
}

} // namespace fleq
} // namespace llvm
