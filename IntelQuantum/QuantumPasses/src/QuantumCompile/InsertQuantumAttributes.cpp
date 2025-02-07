//===- InsertQuantumIntrinsics--.cpp ------------------------------------====//
//
// Copyright (C) 2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//
//===----------------------------------------------------------------------===//
//
// This file implements a module pass which inlines quantum kernel calls which
// are inside other quantum kernels.
//
//===----------------------------------------------------------------------===//

#include "IntelQuantumPasses/QuantumCompile/InsertQuantumAttributes.h"
#include "IntelQuantumPasses/InitializeQuantumPasses.h"
#include "IntelQuantumPasses/QuantumUtils/QuantumGeneralUtils.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/DenseSet.h"
#include "llvm/Analysis/CallGraph.h"
#include "llvm/Demangle/Demangle.h"
#include "llvm/IR/Attributes.h"
#include "llvm/IR/Intrinsics.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/Allocator.h"

#define DEBUG_TYPE "insert-quantum-attributes"

using namespace llvm;
using namespace aqcc;

/// This function identifies the usage pattern of arguments in functions
/// to their use as function call arguments in other contexts.
///
/// \param A - The Argument we are analyzing
/// \param CallArgs - The set of arguments connected to \p A
/// \param ArgConnections - The map of arguments to the set of argument
/// connected to that argument.
/// \param DenseSetAllocator - The memory allocator for new sets of Arguments.
static void FindArgumentToCalls(
    Argument *A, DenseSet<Argument *> *CallArgs,
    DenseMap<Argument *, DenseSet<Argument *> *> &ArgConnections,
    SpecificBumpPtrAllocator<DenseSet<Value *>> &DenseSetAllocator) {
  DenseMap<Argument *, DenseSet<Argument *> *>::iterator It;
  std::list<Value *> Frontier = {A};
  DenseSet<Value *> Visited;

  // A graph traversal using the function arguments as the nodes.  The edges
  // are defined as
  while (Frontier.size() > 0) {
    Value *V = Frontier.front();
    Frontier.pop_front();
    if (Visited.find(V) != Visited.end())
      continue;
    Visited.insert(V);
    for (Use &U : V->uses()) {
      Value *UV = dyn_cast<Value>(U.getUser());
      // Put each use of the argument into the frontier of the graph traversal.
      Frontier.push_back(UV);
      CallBase *CI = dyn_cast<CallBase>(UV);
      // We only want to analyze call instructions in this case.
      if (!CI)
        continue;
      if (CI->getCalledOperand() == V)
        continue;
      // If it is an indirect call, we can't tell anything about the arguments
      // so we do not consider them..
      if (CI->isIndirectCall())
        continue;

      // Get the called function, and the corresponidng argument where the
      // argument being analyzed was used.  Then add it to the set of arguments
      // that this argument is connected to.
      unsigned ArgNo = CI->getArgOperandNo(&U);
      if (CI->getCalledFunction()->isVarArg() &&
          (ArgNo >= CI->getCalledFunction()->arg_size()))
        continue;
      Argument *Arg = CI->getCalledFunction()->getArg(ArgNo);
      CallArgs->insert(Arg);
      It = ArgConnections.find(Arg);
      if (It == ArgConnections.end()) {
        bool Inserted = false;
        DenseSet<Argument *> *NewCallArgs =
            new (DenseSetAllocator.Allocate()) DenseSet<Argument *>();
        std::tie(It, Inserted) =
            ArgConnections.insert(std::make_pair(Arg, NewCallArgs));
      }
      It->second->insert(A);
    }
  }
}

static void
AddQExprAttrs(DenseSet<Value *> &IsQExprVal,
              DenseMap<Function *, DenseSet<Value *> *> &FunctionReturnToValue,
              DenseMap<Value *, Function *> &ValueToReturn,
              DenseMap<Argument *, DenseSet<Value *> *> &ArgumentToValue,
              DenseMap<Value *, DenseSet<Argument *> *> &ValueToArgument,
              AttrBuilder &QExprFuncAttr, AttrBuilder &QExprAttr) {
  DenseSet<Value *> Checked;
  std::list<Value *> Frontier;
  // Iterate over each value and mark it as a QExpr value where applicable.
  for (Value *V : IsQExprVal) {
    Frontier.clear();
    Frontier.push_back(V);
    while (Frontier.size() > 0) {
      Value *Visiting = Frontier.front();
      Frontier.pop_front();
      if (Checked.find(Visiting) != Checked.end())
        continue;
      Checked.insert(Visiting);
      // We've found a function, so it is marked as a function that returns
      // a qexpr rather than a qexpr itself.
      if (Function *F = dyn_cast<Function>(Visiting)) {
        F->addFnAttr("qexpr_func_val");
        auto FIt = FunctionReturnToValue.find(F);
        if (FIt != FunctionReturnToValue.end())
          for (Value *ReturnVal : *(FIt->second))
            Frontier.push_back(ReturnVal);
        // Need to handle whether function is an argument, then we can mark it
        // as a qexpr function as argument attribute in each other function
        // it is used.
        for (Use &U : Visiting->uses()) {
          User *UserVal = U.getUser();
          unsigned OpNum = U.getOperandNo();
          CallBase *CI = dyn_cast<CallBase>(UserVal);
          if (!CI)
            continue;
          Value *CalledVal = CI->getCalledOperand();
          if (CalledVal != F) {
            Argument *Arg = CI->getCalledFunction()->getArg(OpNum);
            Arg->addAttrs(QExprFuncAttr);
            Frontier.push_back(Arg);
            continue;
          }
          Frontier.push_back(UserVal);
        }
      } else if (Argument *Arg = dyn_cast<Argument>(Visiting)) {
        // Add case for whether it has been marked as QExpr Func arg
        // Then can mark add the CallBase it is used in, and add the arguments
        // to the frontier
        Function *F = Arg->getParent();
        bool isFunction = false;
        if (!Arg->getParent()
                 ->getAttributes()
                 .getParamAttrs(Arg->getArgNo())
                 .hasAttribute("qexpr_func_val")) {
          Arg->addAttrs(QExprAttr);
        } else {
          isFunction = true;
        }
        auto AIt = ArgumentToValue.find(Arg);
        if (AIt != ArgumentToValue.end())
          for (Value *UsedVal : *(AIt->second))
            Frontier.push_back(UsedVal);

        // Look at each use of this argument.
        for (Use &U : Visiting->uses()) {
          User *UserVal = U.getUser();
          CallBase *CI = dyn_cast<CallBase>(UserVal);
          if (CI) {
            Value *CalledVal = CI->getCalledOperand();
            // If the arugment is not being used as the callee, and we do not
            // have an indirect call, make sure we add the corresponding
            // argument to the frontier.
            if (CalledVal != Visiting && !CI->isIndirectCall()) {
              unsigned OpNum = CI->getArgOperandNo(&U);
              Argument *Arg = CI->getCalledFunction()->getArg(OpNum);
              Frontier.push_back(Arg);
            }
            continue;
          }
          if (isFunction)
            Frontier.push_back(UserVal);
        }
      } else if (CallBase *CI = dyn_cast<CallBase>(Visiting)) {
        // If we have a call instruction, we need to make sure we mark the
        // callee value as being a qexpr_func_val.
        Function *IF = CI->getCalledFunction();
        if (!CI->isIndirectCall()) {
          Frontier.push_back(IF);
        } else {
          // Add the Function to be checked and mark it with the appropriate
          // attribute if it is an argument.
          Value *CalleeVal = CI->getCalledOperand();
          if (Argument *A = dyn_cast<Argument>(CalleeVal)) {
            A->addAttrs(QExprFuncAttr);
            Frontier.push_back(CalleeVal);
          }
        }
        auto VIt = ValueToArgument.find(CI);
        if (VIt != ValueToArgument.end())
          for (Argument *UsedArg : *(VIt->second))
            Frontier.push_back(UsedArg);

        // Check to see if the value is used as a return value, if it is,
        // make sure the function is added to the frontier since it returns
        // a qexpr.
        DenseMap<Value *, Function *>::iterator RIt;
        RIt = ValueToReturn.find(CI);
        if (RIt != ValueToReturn.end()) {
          Frontier.push_back(RIt->second);
        }

        for (Use &U : Visiting->uses()) {
          User *UserVal = U.getUser();
          CallBase *UCI = dyn_cast<CallBase>(UserVal);
          if (UCI) {
            Value *CalledVal = UCI->getCalledOperand();
            if (CalledVal != Visiting && !UCI->isIndirectCall()) {
              unsigned OpNum = UCI->getArgOperandNo(&U);
              Argument *Arg = UCI->getCalledFunction()->getArg(OpNum);
              Frontier.push_back(Arg);
            }
            continue;
          }
          Frontier.push_back(UserVal);
        }
      } else if (SelectInst *SI = dyn_cast<SelectInst>(Visiting)) {
        // Make sure that qexpr attribute markings get added through select
        // instructions since both non-boolean operands are qexpr functions.
        for (Use &U : Visiting->uses()) {
          User *UserVal = U.getUser();
          Frontier.push_back(UserVal);
        }
        Frontier.push_back(SI->getTrueValue());
        Frontier.push_back(SI->getFalseValue());
      } else if (ReturnInst *RI = dyn_cast<ReturnInst>(Visiting)) {
        // Add the function for the return isntructions to the frontier since
        // the function returns a qexpr.
        DenseMap<Value *, Function *>::iterator RIt;
        RIt = ValueToReturn.find(RI->getReturnValue());
        if (RIt != ValueToReturn.end()) {
          Frontier.push_back(RIt->second);
        }
      }
    }
  }
}

/// Add a given set of reference attributes to a set of arguments.
///
/// \param IsRef - A DenseSet of arguments that need \p Attrs applied to them.
/// \param ArgConnections - A map of Arguments to a set of other Arguments where
/// the key is connected to in the call graph.
/// \param Attrs - The attributes to be added to the specified Arguments.
static void AddFunctionArgAttrs(
    DenseSet<Argument *> &IsRef,
    DenseMap<Argument *, DenseSet<Argument *> *> &ArgConnections,
    AttrBuilder &Attrs) {
  DenseMap<Argument *, DenseSet<Argument *> *>::iterator It;
  DenseSet<Argument *> Checked;
  std::list<Argument *> Frontier;
  // Do a graph traversal of the argument connections using IsRef as a list
  // of base nodes.
  for (Argument *Arg : IsRef) {
    Frontier.clear();
    Frontier.push_back(Arg);
    while (Frontier.size() > 0) {
      Argument *CurrArg = Frontier.front();
      Frontier.pop_front();
      if (!CurrArg)
        continue;
      if (Checked.find(CurrArg) != Checked.end())
        continue;
      if (!isa<PointerType>(CurrArg->getType()))
        continue;
      Checked.insert(CurrArg);
      CurrArg->addAttrs(Attrs);
      It = ArgConnections.find(CurrArg);
      if (It == ArgConnections.end() || It->second->size() == 0)
        continue;
      for (Argument *NextArg : *(It->second))
        Frontier.push_back(NextArg);
    }
  }
}

static bool InsertQuantumAttributes(Module &M) {
  AttrBuilder QubitAttr(M.getContext());
  AttrBuilder CBitAttr(M.getContext());
  AttrBuilder QExprAttr(M.getContext());
  AttrBuilder QExprFuncAttr(M.getContext());
  QubitAttr.addAttribute("qubit_ref");
  CBitAttr.addAttribute("cbit_ref");
  QExprAttr.addAttribute("qexpr_val");
  QExprFuncAttr.addAttribute("qexpr_func_val");

  DenseMap<Argument *, DenseSet<Argument *> *> ArgConnections;
  DenseMap<Function *, DenseSet<Value *> *> FunctionReturnToValue;
  DenseMap<Value *, Function *> ValueToReturn;
  DenseMap<Argument *, DenseSet<Value *> *> ArgumentToValue;
  DenseMap<Value *, DenseSet<Argument *> *> ValueToArgument;

  // Attribute collectors
  DenseSet<Argument *> IsQubitRef;
  DenseSet<Value *> IsQExprVal;
  DenseSet<Argument *> IsCbitRef;

  // Utility Iterators
  DenseMap<Argument *, DenseSet<Value *>>::iterator It;
  DenseMap<Argument *, DenseSet<Argument *> *>::iterator ArgIt;

  // Branches to replace with select instructions
  std::vector<CallBase *> BranchToReplace;

  SpecificBumpPtrAllocator<DenseSet<Value *>> DenseSetAllocator;

  for (Function &F : M) {
    bool Inserted = false;

    // For each argument, find the connections to other argument uses across
    // function calls.
    for (unsigned Idx = 0, Edx = F.arg_size(); Idx < Edx; Idx++) {
      Argument *Arg = F.getArg(Idx);
      ArgIt = ArgConnections.find(Arg);
      if (ArgIt == ArgConnections.end()) {
        DenseSet<Argument *> *CallArgs =
            new (DenseSetAllocator.Allocate()) DenseSet<Argument *>();
        std::tie(ArgIt, Inserted) =
            ArgConnections.insert(std::make_pair(Arg, CallArgs));
      }
      FindArgumentToCalls(Arg, ArgIt->second, ArgConnections,
                          DenseSetAllocator);
    }

    for (BasicBlock &BB : F)
      for (Instruction &I : BB) {
        CallBase *CI = dyn_cast<CallBase>(&I);
        ReturnInst *RI = dyn_cast<ReturnInst>(&I);

        // Return Instruction Analysis
        if (RI) {
          Value *RV = RI->getReturnValue();
          if (!RV)
            continue;
          DenseMap<Function *, DenseSet<Value *> *>::iterator FIt;
          bool Inserted = false;
          FIt = FunctionReturnToValue.find(&F);
          if (FIt == FunctionReturnToValue.end()) {
            DenseSet<Value *> *ReturnVals =
                new (DenseSetAllocator.Allocate()) DenseSet<Value *>();
            std::tie(FIt, Inserted) =
                FunctionReturnToValue.insert(std::make_pair(&F, ReturnVals));
          }
          FIt->second->insert(RV);
          // Connection return value to a function
          ValueToReturn.insert(std::make_pair(RV, &F));
          continue;
        }
        if (!CI)
          continue;

        if (CI->isInlineAsm()) {
          continue;
        }

        // Intrinsics form the basis for our qexpr_val, qexpr_func_val,
        // qbit_ref, and cbit_ref.  Use the arguments and functions to mark it
        // as such here.
        if (!CI->isIndirectCall() && dyn_cast<IntrinsicInst>(CI)) {
          Function *CF = CI->getCalledFunction();
          StringRef CalledName = CF->getName();

          if (CalledName == "llvm.quantum.qubit") {
            Argument *Arg = dyn_cast<Argument>(CI->getArgOperand(0));
            IsQubitRef.insert(Arg);
          } else if (CalledName == "llvm.quantum.cbit") {
            Argument *Arg = dyn_cast<Argument>(CI->getArgOperand(0));
            IsCbitRef.insert(Arg);
          } else if (CalledName.contains("llvm.fleq.qexpr.identity")) {
            IsQExprVal.insert(CF);
          } else if (CalledName.contains("llvm.fleq.qexpr.eval.hold")) {
            Argument *Arg = CF->getArg(0);
            IsQExprVal.insert(Arg);
          } else if (CalledName.contains("llvm.fleq.qexpr.eval.release")) {
            Argument *Arg = CF->getArg(0);
            IsQExprVal.insert(Arg);
          } else if (CalledName.contains("llvm.fleq.qexpr.invert")) {
            IsQExprVal.insert(CF);
            Argument *Arg = CF->getArg(0);
            IsQExprVal.insert(Arg);
          } else if (CalledName.contains("llvm.fleq.gate.qexpr")) {
            IsQExprVal.insert(CF);
          } else if (CalledName.contains("llvm.fleq.qexpr.join")) {
            IsQExprVal.insert(CF);
            Argument *Arg = CF->getArg(0);
            IsQExprVal.insert(Arg);
            Arg = CF->getArg(1);
            IsQExprVal.insert(Arg);
          } else if (CalledName.contains("llvm.fleq.qexpr.power")) {
            IsQExprVal.insert(CF);
            Argument *Arg = CF->getArg(1);
            IsQExprVal.insert(Arg);
          } else if (CalledName.contains("llvm.fleq.qexpr.control")) {
            IsQExprVal.insert(CF);
            Argument *Arg = CF->getArg(2);
            IsQExprVal.insert(Arg);
            Arg = CF->getArg(0);
            IsQubitRef.insert(Arg);
          } else if (CalledName.contains("llvm.fleq.qexpr.multi.control")) {
            IsQExprVal.insert(CF);
            Argument *Arg = CF->getArg(2);
            IsQExprVal.insert(Arg);
            Arg = CF->getArg(0);
            IsQubitRef.insert(Arg);
          } else if (CalledName.contains("llvm.fleq.qexpr.branch")) {
            BranchToReplace.push_back(CI);
            IsQExprVal.insert(CF);
            Argument *Arg = CF->getArg(1);
            IsQExprVal.insert(Arg);
            Arg = CF->getArg(2);
            IsQExprVal.insert(Arg);
          } else if (CalledName.contains("llvm.fleq.qexpr.let")) {
            Argument *Arg = CF->getArg(1);
            IsQExprVal.insert(Arg);
          } else if (CalledName.contains("llvm.fleq.qexpr.get")) {
            IsQExprVal.insert(CF);
          } else if (CalledName.contains("llvm.fleq.qexpr.this.expression")) {
            IsQExprVal.insert(CF);
          }
        }

        Function *CF = CI->getCalledFunction();
        for (unsigned Idx = 0, Edx = CI->arg_size(); Idx < Edx; Idx++) {
          Value *V = CI->getArgOperand(Idx);
          if (!CI->isIndirectCall()) {
            // Mark the connections of the value used in a call instruction
            // to the Argument in the Called Function.
            if (CF->isVarArg() && Idx >= CF->arg_size())
              continue;
            Argument *Arg = CF->getArg(Idx);
            DenseMap<Argument *, DenseSet<Value *> *>::iterator AIt;
            DenseMap<Value *, DenseSet<Argument *> *>::iterator VIt;
            bool Inserted = false;
            AIt = ArgumentToValue.find(Arg);
            if (AIt == ArgumentToValue.end()) {
              DenseSet<Value *> *ValArgs =
                  new (DenseSetAllocator.Allocate()) DenseSet<Value *>();
              std::tie(AIt, Inserted) =
                  ArgumentToValue.insert(std::make_pair(Arg, ValArgs));
            }
            AIt->second->insert(V);

            VIt = ValueToArgument.find(V);
            if (VIt == ValueToArgument.end()) {
              DenseSet<Argument *> *ArgVals =
                  new (DenseSetAllocator.Allocate()) DenseSet<Argument *>();
              std::tie(VIt, Inserted) =
                  ValueToArgument.insert(std::make_pair(Arg, ArgVals));
            }
            VIt->second->insert(Arg);
          }

          // Check arguments for global values, and add them to list of
          // references if it matches our expectations.
          Type *GVTy = nullptr;
          if (GlobalValue *GV = dyn_cast<GlobalValue>(V)) {
            if (!GV->getType()->isPointerTy())
              continue;
            GVTy = GV->getValueType();
          } else if (AllocaInst *AI = dyn_cast<AllocaInst>(V)) {
            GVTy = AI->getAllocatedType();
          }
          if (!GVTy)
            continue;
          IntegerType *IntTy = dyn_cast<IntegerType>(GVTy);
          if (IntTy && IntTy->getBitWidth() == 16)
            IsQubitRef.insert(CF->getArg(Idx));
          if (IntTy && IntTy->getBitWidth() == 1)
            IsCbitRef.insert(CF->getArg(Idx));
        }
      }
  }

  AddQExprAttrs(IsQExprVal, FunctionReturnToValue, ValueToReturn,
                ArgumentToValue, ValueToArgument, QExprFuncAttr, QExprAttr);
  AddFunctionArgAttrs(IsQubitRef, ArgConnections, QubitAttr);
  AddFunctionArgAttrs(IsCbitRef, ArgConnections, CBitAttr);

  return false;
}

struct InsertQuantumArgumentAttributesLegacyPass : public ModulePass {

  static char ID; // Pass identification
  InsertQuantumArgumentAttributesLegacyPass() : ModulePass(ID) {
    initializeInsertQuantumArgumentAttributesLegacyPassPass(
        *PassRegistry::getPassRegistry());
  }

  bool runOnModule(Module &M) override;

  bool run(Module &M, CallGraph &CG);

  virtual void getAnalysisUsage(AnalysisUsage &AU) const {}

}; // End of struct InsertQuantumIntrinsicsPass

char InsertQuantumArgumentAttributesLegacyPass::ID = 0;

bool InsertQuantumArgumentAttributesLegacyPass::runOnModule(Module &M) {
  // We will use the call graph scc ordering to order the inlining
  return InsertQuantumAttributes(M);
}

PreservedAnalyses
InsertQuantumArgumentAttributesPass::run(Module &M, ModuleAnalysisManager &AM) {
  InsertQuantumAttributes(M);

  return PreservedAnalyses::none();
}

#undef DEBUG_TYPE