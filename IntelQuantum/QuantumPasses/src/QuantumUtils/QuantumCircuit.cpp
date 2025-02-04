//===- QuantumCircuit.cpp ---------------------------------------------====//
//
// Copyright (C) 2024 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//
//===----------------------------------------------------------------------===//
//
// This file is the implementation file for QuantumCircuit object.
//
//===----------------------------------------------------------------------===//

#include "llvm/Demangle/Demangle.h"

#include "IntelQuantumPasses/QuantumAnalysis/QuantumIterator.h"
#include "IntelQuantumPasses/QuantumUtils/QuantumCircuit.h"
#include "IntelQuantumPasses/QuantumUtils/QuantumGeneralUtils.h"

#include <map>
#include <set>

using namespace llvm;
using namespace aqcc;

unsigned QuantumCircuit::GateIdCounter = 0;
unsigned QuantumCircuit::CircuitIdCounter = 0;
std::set<QbitRef, QubitCompare> QuantumCircuit::GlobalQubits =
    std::set<QbitRef, QubitCompare>();

QuantumCircuit::QuantumCircuit(QuantumModule &QM, QuantumKernel &QK) {
  QuantumCircuit *QC = nullptr;
  BaseValue = QK.getFunction();
  StartOp = new StartingOp(*this);
  EndOp = new EndingOp(*this);
  // Map from the BasicBlock value to quantum circuit for
  // creating links between circuits.
  std::map<BasicBlock *, QuantumCircuit *> BBToQC;
  std::map<BasicBlock *, QuantumCircuit *>::iterator BBToQCIt;
  CircuitId = CircuitIdCounter++;

  this->QM = &QM;
  this->QM->OwnedQuantumCircuits.insert(this);

  // For each BasicBlock, create a sub function, based on the number
  // of quantum gates, record whether this circuit contains quantum
  // operations, classical operations or both.
  Kind = CircuitKind::None;
  for (BasicBlock &BB : *QK.getFunction()) {
    QC = new QuantumCircuit(QM, BB);
    QC->ParentCircuit = this;
    if (QC->getNumberGates() > 0) {
      if (Kind == CircuitKind::None)
        Kind = CircuitKind::AllQuantum;
      else if (Kind == CircuitKind::NoQuantum)
        Kind = CircuitKind::Combined;
    } else {
      if (Kind == CircuitKind::None)
        Kind = CircuitKind::NoQuantum;
      else if (Kind == CircuitKind::AllQuantum)
        Kind = CircuitKind::Combined;
    }

    // Add the circuit to the list and map of block to circuit.
    CircuitList.push_back(QC);
    BBToQC.insert(std::make_pair(&BB, QC));

    for (const QbitRef &QR : QC->Qubits) {
      addQubit(QR);
    }
  }

  for (Argument &Arg : QK.getFunction()->args()) {
    if (Arg.getParent()
            ->getAttributes()
            .getParamAttrs(Arg.getArgNo())
            .hasAttribute("qubit_ref")) {
      QbitRef NewQR = QbitRef(&Arg);
      ArgumentParams.emplace_back(OpParamTy::QUBITPTR, NewQR);
    } else if (Arg.getType()->isDoubleTy()) {
      ParaRef NewPR = ParaRef(&Arg);
      ArgumentParams.emplace_back(OpParamTy::GATEPARAM, NewPR);
    } else if (Arg.getType()->isIntegerTy()) {
      ParaRef NewPR = ParaRef(&Arg);
      ArgumentParams.emplace_back(OpParamTy::GATEPARAM, NewPR);
    } else {
      ArgumentParams.emplace_back(OpParamTy::OTHER, Arg.getType());
    }
  }

  if (!QK.getFunction()->empty()) {
    // If the kernel's function is not empty, get the entry block.
    BasicBlock *Entry = &QK.getFunction()->getEntryBlock();
    // Get the circuit for that block, and create an edge from
    // the start of the circuit to the entry block. A function
    // only every has a single entry point.
    QuantumCircuit *CurrCircuit = BBToQC[Entry];
    StartOp->addSuccessor(CurrCircuit->StartOp);
    CurrCircuit->StartOp->addPredecessor(StartOp);
  } else {
    // If there are no contents of the kernel, connect the start
    // and end operations and exit.
    EndOp->addPredecessor(StartOp);
    StartOp->addSuccessor(EndOp);
    return;
  }

  // Iterate over the blocks of a quantum kernel.
  for (BasicBlock &BB : *QK.getFunction()) {
    // Get the current circuit constructed from this BasicBlock.
    QuantumCircuit *CurrCircuit = BBToQC[&BB];

    // Get the end instruction of the block, and get the
    // successor blocks from this instruction.
    Instruction *Term = BB.getTerminator();
    unsigned NumSuccs = Term->getNumSuccessors();
    if (NumSuccs == 0) {
      // This is a return instruction, connect the end operation
      // of BasicBlock circuit to the end operation of the
      // kernel circuit.
      CurrCircuit->EndOp->addSuccessor(EndOp);
      EndOp->addPredecessor(CurrCircuit->EndOp);
    }
    QuantumCircuit *SuccCircuit = nullptr;
    // Iterate over each successor and add an edge between
    // the start and end of the two circuits.
    for (unsigned Idx = 0; Idx < NumSuccs; Idx++) {
      BasicBlock *Succ = Term->getSuccessor(Idx);
      SuccCircuit = BBToQC[Succ];
      CurrCircuit->EndOp->addSuccessor(SuccCircuit->StartOp);
      SuccCircuit->StartOp->addPredecessor(CurrCircuit->EndOp);
    }
  }
}

QuantumCircuit::QuantumCircuit(QuantumModule &QM, BasicBlock &BB) {
  // Map where we find the operation that a given qubit was last used in,
  // this lets us create edges based on qubit dependencies between
  // operations.
  std::map<QbitRef, QuantumCircuitOp *, QubitCompare> QbitRefToPrevOp;
  std::map<QbitRef, QuantumCircuitOp *, QubitCompare>::iterator It;

  StartOp = new StartingOp(*this);
  EndOp = new EndingOp(*this);

  BaseValue = &BB;

  CircuitId = CircuitIdCounter++;

  Instruction *BaseInst = nullptr;
  QuantumOp *NewQOC = nullptr;
  QbitRef DummyRef;

  this->QM = &QM;
  this->QM->OwnedQuantumCircuits.insert(this);

  if (BB.getParent()) {
    for (Argument &Arg : BB.getParent()->args()) {
      if (Arg.getParent()
              ->getAttributes()
              .getParamAttrs(Arg.getArgNo())
              .hasAttribute("qubit_ref")) {
        QbitRef NewQR = QbitRef(&Arg);
        ArgumentParams.emplace_back(OpParamTy::QUBITPTR, NewQR);
      } else if (Arg.getType()->isDoubleTy()) {
        ParaRef NewPR = ParaRef(&Arg);
        ArgumentParams.emplace_back(OpParamTy::GATEPARAM, NewPR);
      } else if (Arg.getType()->isIntegerTy()) {
        ParaRef NewPR = ParaRef(&Arg);
        ArgumentParams.emplace_back(OpParamTy::GATEPARAM, NewPR);
      } else {
        ArgumentParams.emplace_back(OpParamTy::OTHER, Arg.getType());
      }
    }
  }

  QuantumCircuitOp *DefaultStart = StartOp;
  // Iterate over the BasicBlock, analyze each instruction.
  for (Instruction &I : BB) {
    // We will only be examining the call instructions, as there are no
    // non-call quantum instructions.
    CallInst *CB = dyn_cast<CallInst>(&I);
    if (!CB)
      continue;
    bool IndirectCall = CB->isIndirectCall();
    Function *CalledVal = CB->getCalledFunction();

    std::vector<std::pair<QbitRef, unsigned>> Qubits;
    std::vector<std::pair<ParaRef, unsigned>> ResolvedClassicalParameters;
    std::vector<std::pair<Value *, unsigned>> OtherParameters;
    unsigned NumArgs = CB->arg_size();

    // Collect the parameters for each operation.
    for (unsigned Idx = 0; Idx < NumArgs; Idx++) {
      Value *V = CB->getArgOperand(Idx);

      QbitRef QR = getQbitRefFromValue(CB, Idx);
      if (!QR.isNull()) {
        Qubits.push_back(std::make_pair(QR, Idx));
        continue;
      }

      ParaRef PR = getParaRefFromValue(CB, Idx);
      if (!PR.isNull()) {
        ResolvedClassicalParameters.push_back(std::make_pair(PR, Idx));
        continue;
      }

      OtherParameters.push_back(std::make_pair(V, Idx));
    }

    // If there's no qubit operands, and it is not a quantum kernel
    // we skip over this call instruction, it is not a quantum operation.
    if (Qubits.size() == 0 && !isQKernel(*CalledVal))
      continue;

    // Create a new operation based on the collected information.
    NewQOC = new QuantumOp(&I, Qubits, ResolvedClassicalParameters,
                           OtherParameters, *this);
    NewQOC->setQuantumModule(QM);

    // For each qubit, get the previous instruction it was used.  And add an
    // edge. If it was not used yet in this circuit, get the default start
    // instruction.  This may not be the start operation, a call instruction to
    // a different quantum kernel will force an edge through that operation for
    // each qubit.
    for (std::pair<QbitRef, unsigned> &QR : Qubits) {
      std::set<QbitRef, QubitCompare>::iterator TempIt;
      bool Inserted = false;
      Inserted = this->addQubit(QR.first);
      if (Inserted && BlockingOps.size() > 0) {
        QuantumCircuitOp *Prev = StartOp;
        for (QuantumOp *QO : BlockingOps) {
          QO->addPredecessor(Prev);
          Prev->addSuccessor(QO);
          QuantumCircuitOp::createDependence(*Prev, *QO, QR.first);
          Prev = QO;
        }
      }
      It = QbitRefToPrevOp.find(QR.first);
      if (It == QbitRefToPrevOp.end()) {
        NewQOC->addPredecessor(DefaultStart);
        DefaultStart->addSuccessor(NewQOC);
        QuantumCircuitOp::createDependence(*DefaultStart, *NewQOC, QR.first);
      } else {
        NewQOC->addPredecessor(It->second);
        It->second->addSuccessor(NewQOC);
        if (It->second == DefaultStart)
          QuantumCircuitOp::createDependence(*It->second, *NewQOC, QR.first);
      }
      QbitRefToPrevOp[QR.first] = NewQOC;
    }

    // If there are no qubits in the operation, create an edge from the
    // previously used operation to this operation for each qubit.
    if (Qubits.size() == 0) {
      BlockingOps.push_back(NewQOC);
      for (std::pair<QbitRef, QuantumCircuitOp *> QROp : QbitRefToPrevOp) {
        QROp.second->addSuccessor(NewQOC);
        NewQOC->addPredecessor(QROp.second);
        QbitRefToPrevOp[QROp.first] = NewQOC;
        QuantumCircuitOp::createDependence(*QROp.second, *NewQOC, QROp.first);
      }

      if (QbitRefToPrevOp.size() < 1) {
        DefaultStart->addSuccessor(NewQOC);
        NewQOC->addPredecessor(DefaultStart);
      }

      // Set a new default start operation, all qubits go through this
      // operation.
      DefaultStart = NewQOC;
    }

    addToGates();
  }

  // If we have created any qubit operations, then this is a
  // quantum circuit, and need to finish connecting the last
  // used operations to the ending operation.
  for (std::pair<const QbitRef, QuantumCircuitOp *> &RefToOp :
       QbitRefToPrevOp) {
    Kind = AllQuantum;
    RefToOp.second->addSuccessor(EndOp);
    EndOp->addPredecessor(RefToOp.second);
  }

  // No quantum operations found for this block. Mark it as such
  // and connect the start operation to the ending operation.
  if (QbitRefToPrevOp.size() == 0) {
    Kind = NoQuantum;
    DefaultStart->addSuccessor(EndOp);
    EndOp->addPredecessor(DefaultStart);
  }
}

QuantumCircuit *QuantumCircuit::Create(QuantumModule &QM,
                                       std::vector<QuantumOpParam> &Args,
                                       std::string Name) {
  LLVMContext &C = QM.getBaseModule()->getContext();
  SmallVector<Type *> TypeArray;
  for (QuantumOpParam &Param : Args) {
    TypeArray.push_back(Param.getType(C));
  }

  // Create a new empty quantum kernel with one block.
  Function *F =
      createEmptyQKernel(Name, *QM.getBaseModule(), TypeArray, nullptr, true);
  QuantumKernel QK(*F);
  QuantumCircuit *QC = new QuantumCircuit(QM, QK);
  unsigned Idx = 0;
  AttrBuilder QubitAttr(QM.getBaseModule()->getContext());
  QubitAttr.addAttribute("qubit_ref");

  // Add different parameters to the internal structure, and propagate
  // any qubits and parameters to the subcircuits.
  QC->ArgumentParams.clear();
  for (QuantumOpParam &Param : Args) {
    TypeArray.push_back(Param.getType(C));
    if (Param.ParamType == OpParamTy::QUBITPTR) {
      F->getArg(Idx)->addAttrs(QubitAttr);
      QbitRef QR(F->getArg(Idx), (unsigned)0);
      QC->addQubit(QR);
      QC->ArgumentParams.emplace_back(OpParamTy::QUBITPTR, QR);
      for (QuantumCircuit *SubQC : QC->CircuitList) {
        SubQC->addQubit(QR);
      }
    } else {
      QC->ArgumentParams.emplace_back(Param.ParamType, Param.getType(C));
    }
    Idx++;
  }
  return QC;
}

QuantumCircuit *QuantumCircuit::Create(QuantumModule &QM, std::string Name,
                                       bool IsParent) {
  // If we are creating a parent circuit, use the other method with no
  // arguments.
  if (IsParent) {
    std::vector<QuantumOpParam> TempArgs;
    return QuantumCircuit::Create(QM, TempArgs, Name);
  }

  // Create a new basic block and add a single return instruction.
  BasicBlock *BB = BasicBlock::Create(QM.getBaseModule()->getContext(), Name);
  ReturnInst *RI = ReturnInst::Create(QM.getBaseModule()->getContext(), BB);
  QuantumCircuit *QC = new QuantumCircuit(QM, *BB);
  return QC;
}

QuantumCircuit::~QuantumCircuit() {
  if (CircuitList.size() > 0) {
    for (QuantumCircuit *QC : CircuitList) {
      if (QM->OwnedQuantumCircuits.find(QC) != QM->OwnedQuantumCircuits.end())
        delete QC;
    }
  } else {
    std::vector<QuantumCircuitOp *> Ops;
    iterateGates(Ops);
    for (QuantumCircuitOp *QCO : Ops)
      delete QCO;
  }
  delete StartOp;
  delete EndOp;

  if (BasicBlock *BB = dyn_cast<BasicBlock>(BaseValue)) {
    if (!BB->getParent()) {
      BB->deleteValue();
    }
  }

  QM->OwnedQuantumCircuits.erase(this);
}

void moveAllocas(BasicBlock *From, BasicBlock *To) {
  std::vector<Instruction *> Allocas;
  // Iterate over instructions in the block and collect them.
  for (Instruction &I : *From) {
    if (AllocaInst *AI = dyn_cast<AllocaInst>(&I)) {
      Allocas.push_back(AI);
    }
  }

  // Move into first viable spot in the new block.
  Instruction *InsertAfter = &*To->getFirstNonPHIOrDbgOrAlloca();
  for (Instruction *I : Allocas) {
    I->moveAfter(InsertAfter);
    InsertAfter = I;
  }
}

bool QuantumCircuit::insertCircuit(
    QuantumCircuit *NewQC, QuantumCircuit *After,
    std::map<unsigned, QuantumOpParam> &Mapping) {

  // We can not insert into a circuit based on a QBB. Need to be a parent
  // circuit.
  if (isa<BasicBlock>(getBaseValue())) {
    return false;
  }

  // Can't insert a nullptr.
  if (!NewQC) {
    return false;
  }

  Function *CurrF = dyn_cast<Function>(getBaseValue());
  BasicBlock *CurrEntry = &CurrF->getEntryBlock();
  QuantumCircuit *Before = nullptr;
  std::vector<QuantumCircuitOp *> Preds;
  std::vector<QuantumCircuitOp *> Succs;

  if (!After) {
    // If no after circuit specified, we collect the predecessors of the end
    // block. The new circuit will go after all the predecessors and will be the
    // final block of the function.
    for (QuantumCircuitOp *Op : EndOp->getPredecessors())
      Preds.push_back(Op);
    Succs.push_back(EndOp);
  } else if (!isa<BasicBlock>(After->getBaseValue())) {
    // If the after circuit is not a basic block, we cannot insert after it.
    return false;
  } else {
    // If the after circuit is not contained in the circuit we are inserting
    // into, fail
    if (After->getParentCircuit() != this) {
      return false;
    }

    // Get end operation of the after circuit as the predecessor.
    Preds.push_back(After->getEndOp());

    // Add any following circuits as successors.
    for (QuantumCircuitOp *Op : After->getEndOp()->getSuccessors()) {
      Succs.push_back(Op);
    }
  }

  // Get the entry block for each situation.
  BasicBlock *EntryFromNew = nullptr;
  Function *CircuitFunction = nullptr;
  if (Function *NewF = dyn_cast<Function>(NewQC->getBaseValue())) {
    // For a function based circuit, this is just the entry block.
    CircuitFunction = NewF;
    EntryFromNew = &NewF->getEntryBlock();

    if (!EntryFromNew)
      return false;
  } else {
    // Get the base value for the basic block.
    EntryFromNew = dyn_cast<BasicBlock>(NewQC->getBaseValue());
    CircuitFunction = EntryFromNew->getParent();

    // If the circuit is already contained in a parent circuit, we do not
    // remove it from the previous circuit.
    if (CircuitFunction) {
      return false;
    }
  }

  Instruction *NewTerm = nullptr;

  // Remove the connections between the operations on either side of the After
  // operation.
  for (QuantumCircuitOp *Pred : Preds) {
    for (QuantumCircuitOp *Succ : Succs) {
      if (Pred->isPredecessorTo(Succ))
        QuantumCircuitOp::unlinkOperations(*Pred, *Succ);
    }

    // If the circuit being inserted in a function, we want to get the start
    // block of the entry circuit instead.
    QuantumCircuitOp *NewStart = NewQC->getStartOp();
    if (NewQC->isAFunction()) {
      // Unlink the start of the entry block from the start of the old parent
      // circuit.
      NewStart = *NewStart->getSuccessors().begin();
      QuantumCircuitOp::unlinkOperations(*NewQC->getStartOp(), *NewStart);
    }

    // Add new connection.
    NewStart->addPredecessor(Pred);
    Pred->addSuccessor(NewStart);

    // Replace the terminator operation in the predecessor with a branch
    // to the new block.
    QuantumCircuit *ParentQC = Pred->getParentCircuit();
    BasicBlock *BB = dyn_cast<BasicBlock>(ParentQC->getBaseValue());
    Instruction *Term = BB->getTerminator();
    if (!NewTerm) {
      NewTerm = Term;
    }
    BranchInst *BI = BranchInst::Create(EntryFromNew, Term);
    Term->removeFromParent();
  }

  // Get the end operation of the circuit we are inserting.
  std::set<QuantumCircuitOp *> Ends = {NewQC->getEndOp()};
  if (NewQC->isAFunction()) {
    // If is a function, get all the predecessors to the end operation.
    Ends = NewQC->getEndOp()->getPredecessors();
  }

  // Add the connections between the end operations and the new successors. Also
  // unlink new operation and the end operation.
  for (QuantumCircuitOp *Succ : Succs) {
    for (QuantumCircuitOp *End : Ends) {
      End->addSuccessor(Succ);
      Succ->addPredecessor(End);
      QuantumCircuitOp::unlinkOperations(*End, *NewQC->getEndOp());
    }
  }

  std::vector<BasicBlock *> BlocksToRemap;

  // If we are inserting from a function, we need to replace any arguments.
  if (NewQC->isAFunction()) {
    for (QuantumCircuit *QC : NewQC->getCircuitList()) {
      // Add each subcircuit to the circuit list of the new parent.
      CircuitList.push_back(QC);
      QC->ParentCircuit = this;
      BasicBlock *BB = dyn_cast<BasicBlock>(QC->getBaseValue());
      // Remove block from the parent circuit.
      if (BB->getParent()) {
        BB->removeFromParent();
      }
      Instruction *Term = BB->getTerminator();
      BB->insertInto(CurrF);
      // If we have a return instruction, add branch, or return instruction to
      // the new end.
      if (ReturnInst *RI = dyn_cast<ReturnInst>(Term)) {
        NewTerm->clone()->insertBefore(RI);
        RI->eraseFromParent();
      }
      // Move the alloca instructions from the entry to the
      // entry of the new circuit.
      moveAllocas(BB, CurrEntry);
      BlocksToRemap.push_back(BB);
    }
    // Clear the list of the old circuit.
    NewQC->CircuitList.clear();
  } else {
    // Add new circuit to the list.
    CircuitList.push_back(NewQC);
    NewQC->ParentCircuit = this;
    BasicBlock *BB = dyn_cast<BasicBlock>(NewQC->getBaseValue());
    if (BB->getParent()) {
      BB->removeFromParent();
    }

    // Insert into the new parent.
    BB->insertInto(CurrF);

    // Replace the existing terminating instruction with branch to the next
    // circuit or return instruction.
    Instruction *Term = BB->getTerminator();
    NewTerm->clone()->insertBefore(Term);
    Term->eraseFromParent();
    moveAllocas(BB, CurrEntry);
    BlocksToRemap.push_back(BB);
  }

  // Remove the original terminator.
  NewTerm->deleteValue();

  // Use the mapping to replace the argument values if moving blocks out of a
  // function.
  if (CircuitFunction) {
    ValueToValueMapTy VMap;
    for (Argument &Arg : CircuitFunction->args()) {
      unsigned ArgNo = Arg.getArgNo();
      std::map<unsigned, QuantumOpParam>::iterator It = Mapping.find(ArgNo);
      if (It == Mapping.end())
        continue;
      Value *V = It->second.getValue(this);
      if (Instruction *I = dyn_cast<Instruction>(V)) {
        if (!I->getParent()) {
          I->insertBefore(EntryFromNew->getFirstNonPHIOrDbgOrAlloca());
        }
      }
      VMap.insert(std::make_pair(&Arg, V));
    }
    remapInstructionsInBlocks(BlocksToRemap, VMap);
  }

  // Propagate argument parameters from the circuit we are inserting into to the
  // circuit we are insterting.
  NewQC->ArgumentParams.clear();
  for (QuantumOpParam &QOP : ArgumentParams) {
    NewQC->ArgumentParams.push_back(QOP);
  }

  return true;
}

bool QuantumCircuit::insertCircuit(
    QuantumCircuit *NewQC, QuantumOp &After,
    std::map<unsigned, QuantumOpParam> &Mapping) {
  // We are inserting after an operation. We need to split the basic block after
  // the instruction before inserting more operations.
  Value *V = After.getBaseValue();
  if (!isa<Instruction>(V))
    return false;

  // If the operation has no parent circuit, or is not in the circuit we
  // are inserting into, reject the circuit.
  if (!After.getParentCircuit() ||
      After.getParentCircuit()->getParentCircuit() != this)
    return false;

  // Make a new circuit, insert it after the current circuit, then move all the
  // operations after the given instruction into the new circuit.
  QuantumCircuit *TempQC = QuantumCircuit::Create(*QM);
  this->insertCircuit(TempQC, After.getParentCircuit());

  bool FoundStart = false;
  std::map<QbitRef, QuantumOp *, QubitCompare> LastUsed;
  std::vector<QuantumOp *> ToMove;
  for (QuantumCircuitOp *QO : *After.getParentCircuit()) {
    if (QO == &After) {
      FoundStart = true;
      continue;
    } else if (!FoundStart)
      continue;
    ToMove.push_back(static_cast<QuantumOp *>(QO));
  }

  for (QuantumOp *QO : ToMove) {
    QO->moveGate(TempQC);
  }

  // Insert the given circuit between the circuit After is contained in, and
  // before the new created one.
  return this->insertCircuit(NewQC, After.getParentCircuit(), Mapping);
}

bool QuantumCircuit::insertCircuit(QuantumCircuit *NewQC, QuantumOp &After) {
  std::map<unsigned, QuantumOpParam> TempMapping;
  return this->insertCircuit(NewQC, After, TempMapping);
}

bool QuantumCircuit::insertCircuit(QuantumCircuit *NewQC,
                                   QuantumCircuit *After) {
  std::map<unsigned, QuantumOpParam> TempMapping;
  return this->insertCircuit(NewQC, After, TempMapping);
}

bool QuantumCircuit::insertCircuit(QuantumCircuit *NewQC) {
  return this->insertCircuit(NewQC, nullptr);
}

QuantumCircuit *QuantumCircuit::copyCircuit(std::string Name) {
  ValueToValueMapTy VMap;
  QuantumCircuit *QC = nullptr;
  if (isAFunction()) {
    // Clone the Function and create a new circuit.
    Function *F = dyn_cast<Function>(getBaseValue());
    Function *NewF = llvm::CloneFunction(F, VMap);
    QuantumKernel QK(*NewF);
    QC = new QuantumCircuit(*QM, QK);
  } else {
    // Clone the BasicBlock and create a new circuit.
    BasicBlock *BB = dyn_cast<BasicBlock>(getBaseValue());
    BasicBlock *NewBB = llvm::CloneBasicBlock(BB, VMap);
    QC = new QuantumCircuit(*QM, *NewBB);
  }

  QC->getBaseValue()->setName(Name);
  return QC;
}

bool QuantumCircuit::copyAndInsertCircuit(
    QuantumCircuit *ToCopy, QuantumOp &After,
    std::map<unsigned, QuantumOpParam> &Mapping) {
  QuantumCircuit *QC = ToCopy->copyCircuit();
  bool Worked = this->insertCircuit(QC, After, Mapping);
  if (QC->isAFunction()) {
    dyn_cast<Function>(QC->getBaseValue())->eraseFromParent();
    delete QC;
  }
  return Worked;
}

bool QuantumCircuit::copyAndInsertCircuit(
    QuantumCircuit *ToCopy, QuantumCircuit *After,
    std::map<unsigned, QuantumOpParam> &Mapping) {
  QuantumCircuit *QC = ToCopy->copyCircuit();
  bool Worked = this->insertCircuit(QC, After, Mapping);
  if (QC->isAFunction()) {
    dyn_cast<Function>(QC->getBaseValue())->eraseFromParent();
    delete QC;
  }
  return Worked;
}

bool QuantumCircuit::copyAndInsertCircuit(QuantumCircuit *ToCopy,
                                          QuantumOp &After) {
  std::map<unsigned, QuantumOpParam> TempMapping;
  return this->copyAndInsertCircuit(ToCopy, After, TempMapping);
}

bool QuantumCircuit::copyAndInsertCircuit(QuantumCircuit *ToCopy,
                                          QuantumCircuit *After) {
  std::map<unsigned, QuantumOpParam> TempMapping;
  return this->copyAndInsertCircuit(ToCopy, After, TempMapping);
}

bool QuantumCircuit::copyAndInsertCircuit(QuantumCircuit *ToCopy) {
  return this->copyAndInsertCircuit(ToCopy, nullptr);
}

bool QuantumCircuit::createLocalQubitRegister(unsigned Size,
                                              std::vector<QbitRef> &LocalQubits,
                                              std::string Name) {
  BasicBlock *ToAddTo = nullptr;
  if (isABasicBlock()) {
    // With a basic block, if there is a parent, we assign the allocas to the
    // entry block. If there is no parent function, we add it to the block
    // itself.
    Function *F = dyn_cast<BasicBlock>(this->getBaseValue())->getParent();
    if (F) {
      ToAddTo = &F->getEntryBlock();
    } else
      ToAddTo = dyn_cast<BasicBlock>(this->getBaseValue());
  } else {
    // If it is a function, we just add the new instructions to the entry block.
    ToAddTo = &dyn_cast<Function>(this->getBaseValue())->getEntryBlock();
  }

  // Insert the qubits into the designated block as an alloca.
  LLVMContext &C = QM->getBaseModule()->getContext();
  Value *V = ConstantInt::get(Type::getInt32Ty(C), Size, true);
  Instruction *Before = nullptr;
  if (ToAddTo->getParent())
    Before = &*ToAddTo->getFirstNonPHIOrDbgOrAlloca();
  else
    Before = &*ToAddTo->begin();
  AllocaInst *AI =
      new AllocaInst(Type::getInt16Ty(C), 0, V, Align(2), Name, Before);
  // Add local qubits to the list of qubits, even though they are not used.
  for (unsigned Idx = 0; Idx < Size; Idx++) {
    LocalQubits.emplace_back(AI, Idx);
    addQubit(LocalQubits.back());
  }
  return true;
}

bool QuantumCircuit::createGlobalQubitRegister(
    unsigned Size, std::vector<QbitRef> &GlobalQubits, std::string Name) {
  // Create new global variable, add it the static global variable list.
  LLVMContext &C = QM->getBaseModule()->getContext();
  GlobalVariable *NewGlobal = new GlobalVariable(
      *(QM->base),
      ArrayType::get(Type::getInt16Ty(QM->base->getContext()), Size), true,
      GlobalValue::ExternalLinkage, nullptr, Name);
  for (unsigned Idx = 0; Idx < Size; Idx++) {
    std::set<QbitRef, QubitCompare>::iterator It;
    bool Inserted = false;
    std::tie(It, Inserted) = getGlobalQubits().emplace(NewGlobal, Idx);
    addQubit(*It);
    GlobalQubits.push_back(*It);
  }
  return true;
}

QbitRef QuantumCircuit::getQubitInRegister(QbitRef &QR, unsigned Val) {
  Value *V = QR.getRegister();
  QbitRef Out;
  std::optional<unsigned> ArraySize = QR.getRegisterSize();
  // Do not create new QbitRef if it is larger than the array size.
  if (ArraySize.has_value() && Val >= ArraySize)
    return Out;
  Out = QbitRef(V, Val);
  if (Qubits.find(Out) == Qubits.end()) {
    addQubit(Out);
  }
  return Out;
}

void QuantumCircuit::collectModuleGlobalQubits(QuantumModule &QM) {
  // Collect global qubits across all circuits.
  for (auto &global : QM.base->globals()) {
    QbitRef temp(&global);
    if (!temp.isNull()) {
      unsigned sz = temp.getRegisterSize().value();
      for (int i = 0; i < sz; i++) {
        temp.setIndexResolved(i);
        GlobalQubits.insert(temp);
      }
    }
  }
}

std::set<QbitRef, QubitCompare> &QuantumCircuit::getGlobalQubits() {
  return getGlobalQubits(*QM);
}

std::set<QbitRef, QubitCompare> &
QuantumCircuit::getGlobalQubits(QuantumModule &QM) {
  if (GlobalQubits.size() == 0 && QM.base) {
    collectModuleGlobalQubits(QM);
  }
  return GlobalQubits;
}

bool QuantumCircuit::addQubit(const QbitRef &QR) {
  if (QR.getRegister() && isa<GlobalValue>(QR.getRegister())) {
    GlobalQubits.insert(QR);
  }
  return Qubits.insert(QR).second;
}

QbitRef QuantumCircuit::getQbitRefFromValue(CallBase *CB, unsigned Idx) {
  // Get the operand value.
  Value *V = CB->getArgOperand(Idx);
  QIter DummyIter;

  // Look for the value in the map of values to references.
  DenseMap<Value *, QbitRef>::iterator QIt = ValueToQbitRef.find(V);
  if (QIt != ValueToQbitRef.end())
    return QIt->second;

  // If it could not be found, use the QIter helper function and
  // insert it if a value could be constructed.
  QbitRef QR = DummyIter.qubit_backtrace_helper(CB, Idx, false);
  if (!QR.isNull())
    ValueToQbitRef.insert(std::make_pair(V, QR));

  return QR;
}

ParaRef QuantumCircuit::getParaRefFromValue(CallBase *CB, unsigned Idx) {
  // Get the operand value.
  Value *V = CB->getArgOperand(Idx);
  QIter DummyIter;

  // Look for the value in the map of values to references.
  DenseMap<Value *, ParaRef>::iterator PIt = ValueToParaRef.find(V);
  if (PIt != ValueToParaRef.end())
    return PIt->second;

  // If it could not be found, use the QIter helper function and
  // insert it if a value could be constructed.
  ParaRef PR = DummyIter.parametric_backtrace_helper(CB, Idx, false);
  if (!PR.isNull())
    ValueToParaRef.insert(std::make_pair(V, PR));

  return PR;
}

/// For the Value V, if it is an instruction, iterate over the operands, insert
/// it before InsertInst unless After is set to true.
static bool
recursiveAddOperands(QuantumOp &QO, Value *V, Instruction *InsertInst,
                     bool After, std::set<Value *> Visited,
                     std::map<unsigned, QuantumOpParam> &ArgMapping) {
  if (Visited.find(V) != Visited.end())
    return true;
  Instruction *I = dyn_cast<Instruction>(V);
  Argument *Arg = dyn_cast<Argument>(V);
  if (!I && !Arg)
    return true;
  Visited.insert(V);

  // If we have an argument, we need to make sure that we have the argument in
  // the new location. Check the mapping for the new value, and replace the uses
  // of the old value for the instructions in the new function we are currently
  // inserting into.
  if (Arg) {
    unsigned ArgLoc = Arg->getArgNo();
    std::map<unsigned, QuantumOpParam>::iterator It = ArgMapping.find(ArgLoc);
    if (Arg->getParent() == InsertInst->getParent()->getParent())
      return true;
    if (It == ArgMapping.end()) {
      errs() << " WARNING: Quantum SDK - Unable to insert operation "
             << QO.printPretty() << " due to unresolved operands for argument "
             << ArgLoc << ": ";
      Arg->print(errs());
      errs() << "\n";
      return false;
    }
    Value *NewVal = It->second.getValue(&QO);
    Arg->replaceUsesWithIf(NewVal, [InsertInst](Use &U) {
      if (Instruction *CI = dyn_cast<Instruction>(U.getUser())) {
        return CI->getParent() == InsertInst->getParent();
      }
      return false;
    });
    Instruction *NewAsInst = dyn_cast<Instruction>(NewVal);
    if (NewAsInst && !NewAsInst->getParent()) {
      NewAsInst->insertBefore(InsertInst);
    }
    QO.setParamsNeedUpdating();
    return true;
  }

  // If the instruction already has a parent, and the function matches the
  // function we are inserting into, don't add the operation.
  Instruction *NewToInsert = I;
  if (I->getParent() &&
      I->getParent()->getParent() == InsertInst->getParent()->getParent()) {
    return true;
  } else if (I->getParent()) {
    // If there is a parent, but it doesn't match, clone the new instruction
    // replace the uses in the new function, and use it to be inserted
    // in the next step.
    NewToInsert = I->clone();
    I->replaceUsesWithIf(NewToInsert, [InsertInst](Use &U) {
      if (Instruction *CI = dyn_cast<Instruction>(U.getUser())) {
        return CI->getParent() == InsertInst->getParent();
      }
      return false;
    });
    QO.setParamsNeedUpdating();
  }

  // Insert the instruction.
  Instruction *InsertBeforeInRecursion = NewToInsert;
  if (PHINode *PN = dyn_cast<PHINode>(NewToInsert)) {
    // If we have a PHINode, it must be inserted before the other
    // instructions in the block to follow the LLVM rules.
    PN->insertBefore(InsertInst->getParent()->getFirstNonPHI());
    InsertBeforeInRecursion = InsertInst;
  } else if (AllocaInst *AI = dyn_cast<AllocaInst>(NewToInsert)) {
    // If we have an Alloca, it must be inserted in the entry block.
    Function *F = InsertInst->getParent()->getParent();
    BasicBlock *EB = &F->getEntryBlock();
    NewToInsert->insertBefore(EB->getFirstNonPHIOrDbgOrAlloca());
  } else if (!After) {
    NewToInsert->insertBefore(InsertInst);
  } else {
    NewToInsert->insertAfter(InsertInst);
  }

  // Recursively check and insert each operation of V.
  bool Result = true;
  for (Value *V : NewToInsert->operand_values()) {
    Result = Result && recursiveAddOperands(QO, V, InsertBeforeInRecursion,
                                            false, Visited, ArgMapping);
  }

  return Result;
}

static bool
recursiveAddOperands(QuantumOp &QO, Value *V, Instruction *InsertInst,
                     bool After,
                     std::map<unsigned, QuantumOpParam> &ArgMapping) {
  std::set<Value *> Visited;
  return recursiveAddOperands(QO, V, InsertInst, After, Visited, ArgMapping);
}

static bool recursiveAddOperands(QuantumOp &QO, Value *V,
                                 Instruction *InsertInst, bool After = false) {
  std::map<unsigned, QuantumOpParam> TempArgMapping;
  return recursiveAddOperands(QO, V, InsertInst, After, TempArgMapping);
}

bool QuantumCircuit::appendGate(QuantumOp *QCO) {
  std::map<unsigned, QuantumOpParam> ArgMapping;
  return appendGate(QCO, ArgMapping);
}

bool QuantumCircuit::appendGate(
    QuantumOp *QCO, std::map<unsigned, QuantumOpParam> &ArgMapping) {
  // If the operation is just a pointer, exit.
  if (QCO == nullptr) {
    return false;
  }

  // If the circuit has sub-circuits, do not append the operation.
  if (CircuitList.size() > 0) {
    return false;
  }

  // If the quantum operation is already is a parent circuit, it should
  // not be added.
  if (QCO->getParentCircuit() != nullptr)
    return false;

  // Increase the number of the gates.
  addToGates();

  // Add the instruction to the end of the basic block.
  BasicBlock *BB = dyn_cast<BasicBlock>(getBaseValue());
  Instruction *ToAdd = dyn_cast<Instruction>(QCO->getBaseValue());
  Instruction *AfterInst = BB->getTerminator();
  if (!recursiveAddOperands(*QCO, ToAdd, AfterInst, false, ArgMapping))
    return false;
  QCO->allParametersFromValue();

  // Create set for the number of qubits in the operations.
  SmallVector<std::pair<QbitRef, unsigned>, 2> &Qubits = QCO->getQubits();
  std::set<QbitRef, QubitCompare> QubitSet;
  for (std::pair<QbitRef, unsigned> QPair : Qubits) {
    QubitSet.insert(QPair.first);

    // If the qubit is not already in the circuit, add it here and create
    // connections for the corresponding blocking operation.
    if (this->Qubits.find(QPair.first) == this->Qubits.end()) {
      this->addQubit(QPair.first);

      QuantumCircuitOp *Prev = StartOp;
      for (QuantumOp *QO : BlockingOps) {
        QuantumCircuitOp::createDependence(*Prev, *QO, QPair.first);
        Prev = QO;
      }
      if (Prev != StartOp) {
        Prev->addSuccessor(EndOp);
        QuantumCircuitOp::createDependence(*Prev, *EndOp, QPair.first);
      }
    }
  }

  // If there is one predecessor, and it is the starting operation,
  // then disconnect the starting operation from the ending operation
  // and add the new instruction between the Start and End operations.
  if (EndOp->getPredecessors().size() == 1 && EndOp->isSuccessorTo(StartOp)) {
    EndOp->removePredecessor(StartOp);
    EndOp->addPredecessor(QCO);
    StartOp->addSuccessor(QCO);
    QCO->setParentCircuit(this);
    return true;
  }

  // Get predecessors for the ending operation.
  std::vector<QuantumCircuitOp *> Preds;
  for (QuantumCircuitOp *PredOp : EndOp->getPredecessors()) {
    Preds.push_back(PredOp);
  }

  for (QuantumCircuitOp *PredOp : Preds) {
    // If this is an operation with no qubits, we act as if it is a barrier
    // operation for all qubits.
    if (Qubits.size() < 1) {
      // Get the qubit dependencies, unlink it form the end operation, and
      // create a dependence from the predecessor operation to the new blocking
      // operation.
      std::set<QbitRef, QubitCompare> DepQs = PredOp->getQubitDependence(EndOp);
      QuantumCircuitOp::unlinkOperations(*PredOp, *EndOp);
      PredOp->addSuccessor(QCO);
      for (QbitRef QR : DepQs) {
        QuantumCircuitOp *Dep = PredOp->getDependence(QR);
        QuantumCircuitOp::createDependence(*PredOp, *QCO, QR);
      }
      continue;
    }

    // For each predecessor check whether there is a dependence on a qubit in
    // the new operation.
    for (std::pair<QbitRef, unsigned> QPair : Qubits) {
      QuantumCircuitOp *Dep = PredOp->getDependence(QPair.first);

      // If there is dependence on the qubit, we can skip.
      if (Dep == nullptr || Dep == QCO) {
        continue;
      }

      // Erase the qubit as the dependency was found.
      QubitSet.erase(QPair.first);

      // Unlink the dependence on the current qubit from the Predecessor to End
      // operation. If there are no qubit dependencies, unlink the previous
      // operation and the end operation.
      QuantumCircuitOp::unlinkDependence(*PredOp, *EndOp, QPair.first);
      QuantumCircuitOp::unlinkOperationsIfNoDependence(*PredOp, *EndOp);

      // Add the new operation as a successor to the current predecessor.
      PredOp->addSuccessor(QCO);
    }

    // If all the qubits have been handled, we can continue.
    if (QubitSet.size() == 0)
      continue;
  }

  // If not all the qubits have been handled, the Start operation gets added as
  // a predecessor.
  if (QubitSet.size() > 0) {
    if (BlockingOps.size() > 0) {
      BlockingOps.back()->addSuccessor(QCO);
      for (const QbitRef &QR : QubitSet)
        QuantumCircuitOp::createDependence(*BlockingOps.back(), *QCO, QR);
    } else {
      StartOp->addSuccessor(QCO);
    }
  }

  // Add the ending operation as a successor create the necessary
  // dependencies for a blocking operation.
  if (Qubits.size() < 1) {
    for (const QbitRef &QR : this->Qubits) {
      QuantumCircuitOp::createDependence(*QCO, *EndOp, QR);
    }
    BlockingOps.push_back(QCO);
  }
  QCO->addSuccessor(EndOp);
  QCO->setParentCircuit(this);

  return true;
}

static void handleDependenceChange(
    QuantumCircuit *QC, QuantumOp *QCO, QbitRef &QR,
    std::map<QbitRef, QuantumCircuitOp *, QubitCompare> &LatestUsed) {
  QuantumCircuitOp *LatestOp = nullptr;

  // Check the previous operation. If there is no entry, it means that
  // there needs to be a relationship between the start operation and the
  // new operation for the current qubit. Mark the latest operation as the
  // start qubit in this case. Otherwise, get LatestOp.
  std::map<QbitRef, QuantumCircuitOp *, QubitCompare>::iterator UsedIt =
      LatestUsed.find(QR);
  if (UsedIt == LatestUsed.end()) {
    LatestOp = QC->getStartOp();
  } else {
    LatestOp = UsedIt->second;
  }

  // In the case that it is not the start operation, we can get a dependence
  // relationship for the current qubit and the previous operation using
  // this qubit to get the corresponding successor operation.  If this is the
  // case remove that qubit dependence, and remove the relationship between
  // the operations if no other qubit relationships exist.
  QuantumCircuitOp *Dependent = LatestOp->getDependence(QR);
  if (Dependent != nullptr) {
    QuantumCircuitOp::unlinkDependence(*LatestOp, *Dependent, QR);
    QuantumCircuitOp::unlinkOperationsIfNoDependence(*LatestOp, *Dependent);
  }

  // Ad a connection between the latest used operation and the new operation.
  LatestOp->addSuccessor(QCO);
  QCO->addPredecessor(LatestOp);

  if (Dependent == nullptr)
    return;

  // Add the dependent operation as a successor.
  QCO->addSuccessor(Dependent);
}

bool QuantumCircuit::insertGate(QuantumOp *QCO, QuantumCircuitOp *After) {
  std::map<unsigned, QuantumOpParam> TempArgMapping;
  return insertGate(QCO, After, TempArgMapping);
}

bool QuantumCircuit::insertGate(
    QuantumOp *QCO, QuantumCircuitOp *After,
    std::map<unsigned, QuantumOpParam> &ArgMapping) {
  // If the operation doesn't exist, just exit.
  if (QCO == nullptr) {
    return false;
  }

  // If there's no gate provided, just append it to the end of the parent
  // circuit.
  if (After == nullptr) {
    return this->appendGate(QCO);
  }

  // If there's no gate provided, just append it to the end of the parent
  // circuit.
  if (After == getEndOp()) {
    return false;
  }

  if (After->getParentCircuit() != this)
    return false;

  // If the quantum operation is already in a parent circuit, it should
  // not be added.
  if (QCO->getParentCircuit() != nullptr)
    return false;

  // Increase the number of gates.
  addToGates();

  // We need to figure out what the "state" of the circuit iteration is at
  // this point. That is, where have the qubits been used. So, iterating
  // from the start of the circuit, we find the operation pointed to by
  // After.

  // As we do this, we are going to keep track of when the qubits were last
  // used. This will let us set up the appropriate edge relationships for
  // that particular state of the circuit.
  std::map<QbitRef, QuantumCircuitOp *, QubitCompare> LatestUsed;
  unsigned BlockingIdx = 1;
  if (After != StartOp) {
    QuantumCircuitIterator CurrQCI = this->begin();
    for (QuantumCircuitIterator QCI : *this) {
      CurrQCI = QCI;

      // If a start operation is found, we should clear the latest used cache.
      if ((*QCI)->getKind() == OpKind::Start) {
        LatestUsed.clear();
        continue;
      }

      // Mark the latest used operation if it is any other operation.
      QuantumOp *QO = static_cast<QuantumOp *>(*QCI);
      for (QuantumCircuitOp *NextOp : QO->getSuccessors()) {
        for (const QbitRef &QR : QO->getQubitDependence(NextOp)) {
          LatestUsed[QR] = QO;
        }
      }
      if (QO->getQubits().size() == 0)
        BlockingIdx++;

      if (*QCI == After)
        break;
    }
  } else {
    if (After->getSuccessors().find(EndOp) != After->getSuccessors().end())
      After->removeSuccessor(EndOp);
  }

  // Add the operation to the basic block.
  BasicBlock *BB = dyn_cast<BasicBlock>(getBaseValue());
  Instruction *ToAdd = dyn_cast<Instruction>(QCO->getBaseValue());
  Instruction *AfterInst = nullptr;
  if (After == StartOp) {
    // Insert the operation before the first non debug instruction.
    AfterInst = BB->getFirstNonPHIOrDbgOrLifetime();
    if (!recursiveAddOperands(*QCO, ToAdd, AfterInst, false, ArgMapping))
      return false;
  } else {
    // Insert the operation after the instruction represented by the After
    // operation.
    AfterInst =
        dyn_cast<Instruction>(static_cast<QuantumOp *>(After)->getBaseValue());
    if (!recursiveAddOperands(*QCO, ToAdd, AfterInst, true, ArgMapping))
      return false;
  }
  QCO->allParametersFromValue();

  for (std::pair<QbitRef, unsigned> &QRPair : QCO->getQubits()) {
    // If the qubit is not already in the circuit, add it here and create
    // connections for the corresponding blocking operation.
    if (this->Qubits.find(QRPair.first) == this->Qubits.end()) {
      this->addQubit(QRPair.first);

      QuantumCircuitOp *Prev = StartOp;
      for (QuantumOp *QO : BlockingOps) {
        QuantumCircuitOp::createDependence(*Prev, *QO, QRPair.first);
        Prev = QO;
      }
      if (Prev != StartOp) {
        Prev->addSuccessor(EndOp);
        QuantumCircuitOp::createDependence(*Prev, *EndOp, QRPair.first);
      }

      if (BlockingIdx > 1) {
        LatestUsed[QRPair.first] = BlockingOps[BlockingIdx - 1];
      }
    }
  }

  // Iterate over the qubits for the current operation to determine
  // qubit relationships for predecessor and successor relationships with
  // respect to current state of the circuit. The After operation is captured by
  // the LatestUsed map, so we are able to use this iteration to set up those
  // qubit relationships as well.
  if (QCO->getQubits().size() > 0) {
    for (std::pair<QbitRef, unsigned> &QRPair : QCO->getQubits()) {
      handleDependenceChange(this, QCO, QRPair.first, LatestUsed);
    }
  } else {
    for (std::pair<QbitRef, QuantumCircuitOp *> QRPair : LatestUsed) {
      handleDependenceChange(this, QCO, QRPair.first, LatestUsed);
    }
    if (BlockingIdx > BlockingOps.size()) {
      BlockingOps.push_back(QCO);
    } else {
      BlockingOps.insert(BlockingOps.begin() + BlockingIdx, QCO);
    }
  }

  // Explicitly add After as a predecessor of the new operation. and set the
  // parent circuit if they we not previously defined. As everything is a set,
  // this should not affect the outcome.
  QCO->addPredecessor(After);
  QCO->setParentCircuit(this);

  // It is possible that the qubit had no dependencies from the start operation
  // and the new operation is "dangling". In this case, add the end operation as
  // a successor.
  if (QCO->getSuccessors().size() == 0) {
    QCO->addSuccessor(this->getEndOp());
  }

  return true;
}

void QuantumCircuit::iterateGates(std::vector<QuantumCircuitOp *> &Ops) {
  for (QuantumCircuitOp *Op : *this)
    Ops.push_back(Op);
}

void QuantumCircuit::iterateGatesLimitedQubits(
    std::vector<QuantumCircuitOp *> &Ops,
    std::set<QbitRef, QubitCompare> &LimitQubits) {
  QuantumCircuitIterator It = this->begin(LimitQubits);
  while (It != this->end()) {
    Ops.push_back(*It);
    It++;
  }
}

void QuantumCircuit::iterateGatesTimeSlices(
    std::vector<QuantumCircuitSlice> &Slices) {
  for (QuantumCircuit::slice_iterator SLIt = this->begin_slice();
       SLIt != this->end_slice(); SLIt++)
    Slices.emplace_back(*SLIt);
}

void QuantumCircuit::iterateGatesTimeSlicesLimitedQubits(
    std::vector<QuantumCircuitSlice> &Slices,
    std::set<QbitRef, QubitCompare> &LimitQubits) {
  for (QuantumCircuit::slice_iterator SLIt = this->begin_slice(LimitQubits);
       SLIt != this->end_slice(); SLIt++)
    Slices.emplace_back(*SLIt);
}

QuantumCircuit::QuantumCircuitIterator::QuantumCircuitIterator(
    QuantumCircuitOp *StartingOp) {
  Op = StartingOp;
  if (!Op)
    return;

  // We will only iterate over the smallest circuit possible
  // for a given operation.
  Circuit = Op->getParentCircuit();
  Frontier.push_back(Op);
  // If it is not the starting operation, we iterate over the circuit
  // until we reach the operation to ensure that we get the same ordering
  // of operations each time.
  if (Op == Circuit->getStartOp())
    advance();
  else
    pseudoAddVisited();
}

void QuantumCircuit::QuantumCircuitIterator::advance() {
  // Each call to this function implements a single step in
  // a graph traversal.
  bool NextOpFound = false;
  while (!NextOpFound && Frontier.size() > 0) {
    // We start by sorting the frontier in place so we consistently pull the
    // same operation.
    std::sort(Frontier.begin(), Frontier.end(),
              [](QuantumCircuitOp *A, QuantumCircuitOp *B) { return *A > *B; });

    // We get the last operation and pick off the element, this is more memory
    // efficient than iterating front to back.
    QuantumCircuitOp *Curr = Frontier.back();
    Frontier.pop_back();

    // If we have visited this operation, skip it.
    if (Visited.find(Curr) != Visited.end())
      continue;

    Visited.insert(Curr);
    if (Curr != Circuit->getStartOp() && Curr->getKind() != OpKind::End) {
      // If this is not the first node of the circuit, and not an ending node
      // check if we are looking for specific qubits only. If we are, we check
      // the qubits for whether they contain the specified qubit, and mark that
      // we have found a valid operation if it does.
      if (OnlyCertainQubits && Curr->getKind() == OpKind::Quantum) {
        QuantumOp *Temp = static_cast<QuantumOp *>(Curr);
        bool FoundAQubit = false;
        for (std::pair<QbitRef, unsigned> &QR : Temp->getQubits()) {
          FoundAQubit = LimitedQubits.find(QR.first) != LimitedQubits.end();
          if (FoundAQubit) {
            NextOpFound = true;
            break;
          }
        }
      } else {
        // If we are not looking for specific qubits, we can mark that we found
        // an operation.
        NextOpFound = true;
      }

      // Set the iterator operation to current operation if it is valid.
      if (NextOpFound)
        Op = Curr;
    }

    bool AllFound = true;

    // Examine the successors of the new node.
    for (QuantumCircuitOp *Succ : Curr->getSuccessors()) {
      // If it is the ending operation, we skip the value.
      if (Succ == Circuit->getEndOp()) {
        continue;
      }
      AllFound = true;
      // Check the predecessor values of this successor node.
      for (QuantumCircuitOp *Prev : Succ->getPredecessors()) {
        // Check whether the successor node comes before the previous
        // node in the graph.
        //    |
        //    V
        //  NodeA
        //    |
        //    V
        //  Node B <--|
        //    |       |
        //    V       |
        //  Node C ---|
        //
        // In this case, Node C is a successor and a predecessor to Node B.
        // However, if we are checking whether Node B can be visited after
        // Node A, we can. Node B can only be accessed from Node A or Node C,
        // since we must go through Node B to get to Node C, we can still
        // visit Node B.
        bool PrevIsEnd = Circuit->isAncestorTo(Succ, Prev);

        // If the successor node is not an ancestor, or predecessor has not been
        // visited, we mark that not all predecessor nodes have been found.
        AllFound =
            AllFound && ((Visited.find(Prev) != Visited.end()) || PrevIsEnd);
        if (!AllFound)
          break;
      }

      // If we found all the predecessors for this node, we can add it to the
      // frontier for later processing.
      if (AllFound) {
        Frontier.push_back(Succ);
      }
    }
  }

  // If there are no more operations to process, we set the operation to the
  // nullptr, indicating that we are done iterating over the circuit.
  if (!NextOpFound && Frontier.size() < 1) {
    Op = nullptr;
  }
}

void QuantumCircuit::QuantumCircuitIterator::pseudoAddVisited() {
  // Retain the original operation we want to start on.
  QuantumCircuitOp *HoldActualOp = Op;
  // Iterate over the circuit until we find the operation we want.
  Op = Circuit->getStartOp();
  while (Op != HoldActualOp)
    advance();
}

QuantumCircuit::QuantumCircuitSliceIterator::QuantumCircuitSliceIterator(
    QuantumCircuitOp *StartOp) {
  // This iterator follows the same general functionality as the
  // QuantumCircuitIterator. Please see the above for implementation details.
  if (!StartOp)
    return;

  StartingOp = StartOp;

  Circuit = StartingOp->getParentCircuit();
  if (StartingOp == Circuit->getStartOp()) {
    Frontier.push_back(StartingOp);
    advance();
  } else {
    Frontier.push_back(Circuit->getStartOp());
    pseudoAddVisited();
  }
}

QuantumCircuit::QuantumCircuitIterator::QuantumCircuitIterator(
    QuantumCircuitOp *StartOp, std::set<QbitRef, QubitCompare> &LimitQubits)
    : QuantumCircuitIterator(StartOp) {
  // If we have limited qubits specified, add them to the set, then continue
  // with the original construction.
  if (LimitQubits.size() > 0) {
    OnlyCertainQubits = true;
    for (const QbitRef &QR : LimitQubits)
      LimitedQubits.insert(QR);
  }
}

QuantumCircuit::QuantumCircuitSliceIterator::QuantumCircuitSliceIterator(
    QuantumCircuitOp *StartOp, std::set<QbitRef, QubitCompare> &LimitQubits)
    : QuantumCircuitSliceIterator(StartOp) {
  if (LimitQubits.size() > 0) {
    OnlyCertainQubits = true;
    for (const QbitRef &QR : LimitQubits)
      LimitedQubits.insert(QR);
  }
}

void QuantumCircuit::QuantumCircuitSliceIterator::pseudoAddVisited() {
  // Follow the same logic as pseudoAddVisited for QuantumCircuitIterator, but
  // check whether the operation is contained in the time slice.
  QuantumCircuitOp *TempOp = StartingOp;
  bool Contained = llvm::any_of(
      Slice.Ops, [&TempOp](QuantumCircuitOp *QCO) { return TempOp == QCO; });
  while (Contained) {
    advance();
    Contained = llvm::any_of(
        Slice.Ops, [&TempOp](QuantumCircuitOp *QCO) { return TempOp == QCO; });
  }
}

void QuantumCircuit::QuantumCircuitSliceIterator::advance() {
  // Each call to this function implements a single step in
  // a graph traversal collecting the entire frontier.
  bool NextSliceFound = false;

  std::vector<QuantumCircuitOp *> NewFrontier;
  // Clear the current slice.
  Slice.Ops.clear();
  QuantumCircuit *AddingFrom = nullptr;
  while (!NextSliceFound && Frontier.size() > 0) {
    // We start by sorting the frontier in place so we consistently pull the
    // same operation.
    std::sort(Frontier.begin(), Frontier.end(),
              [](QuantumCircuitOp *A, QuantumCircuitOp *B) { return *A > *B; });

    // Iterate over the entire frontier, collecting all operations that can
    // currently be executed.
    for (QuantumCircuitOp *Curr : Frontier) {
      // If we have visited this operation, skip it.
      if (Visited.find(Curr) != Visited.end())
        continue;

      // If we hav already added to the list, and this operation does not
      // belong, skip it for now, but keep it for the next frontier.
      if (AddingFrom && (AddingFrom != Curr->getParentCircuit())) {
        NewFrontier.push_back(Curr);
        continue;
      }

      if (StartProcessed && Curr->getKind() != OpKind::End) {
        // If the first node has been added, and not an ending node
        // check if we are looking for specific qubits only. If we are, we check
        // the qubits for whether they contain the specified qubit, and mark
        // that we have found a valid operation if it does.
        bool OpValid = false;
        if (OnlyCertainQubits && Curr->getKind() == OpKind::Quantum) {
          QuantumOp *Temp = static_cast<QuantumOp *>(Curr);
          bool FoundAQubit = false;
          for (std::pair<QbitRef, unsigned> &QR : Temp->getQubits()) {
            FoundAQubit = LimitedQubits.find(QR.first) != LimitedQubits.end();
            if (FoundAQubit) {
              OpValid = true;
              break;
            }
          }
        } else {
          OpValid = true;
        }

        if (OpValid) {
          if (!AddingFrom) {
            AddingFrom = Curr->getParentCircuit();
          }
          Slice.Ops.push_back(Curr);
          NextSliceFound = true;
        }
      }

      if (Curr == Circuit->getStartOp())
        StartProcessed = true;

      Visited.insert(Curr);
      bool AllFound = true;

      // Examine the successors of the new node.
      for (QuantumCircuitOp *Succ : Curr->getSuccessors()) {
        // If it is the ending operation, we skip the value.
        if (Succ == Circuit->getEndOp())
          continue;
        AllFound = true;
        // Check the predecessor values of this successor node
        for (QuantumCircuitOp *Prev : Succ->getPredecessors()) {
          // Check whether the successor node comes before the previous
          // node in the graph.
          bool PrevIsEnd = Circuit->isAncestorTo(Succ, Prev);

          // If the successor node is not an ancestor, or predecessor has not
          // been visited, we mark that not all predecessor nodes have been
          // found.
          AllFound =
              AllFound && ((Visited.find(Prev) != Visited.end()) || PrevIsEnd);
          if (!AllFound)
            break;
        }
        if (AllFound)
          NewFrontier.push_back(Succ);
      }
    }
    // Replace the frontier with the newly built list.
    Frontier = std::move(NewFrontier);
  }

  // If no slice found, and no more operations to analyze, clear the slice
  // as an empty slice is the end of the iteration.
  if (!NextSliceFound && Frontier.size() < 1) {
    Slice.Ops.clear();
  }
}

bool QuantumCircuit::isAncestorTo(QuantumCircuitOp *A, QuantumCircuitOp *B) {
  // Make sure that operations A and B are included in the circuit we are
  // searching.
  bool FoundA = A->getParentCircuit() == this;
  bool FoundB = B->getParentCircuit() == this;
  for (QuantumCircuit *QC : CircuitList) {
    FoundA = FoundA || (A->getParentCircuit() == QC);
    FoundB = FoundB || (B->getParentCircuit() == QC);
    if (FoundA && FoundB)
      break;
  }
  if (!FoundA || !FoundB)
    return false;

  if (A->getParentCircuit() != this) {
    // Make sure that A's parent circuit is actually included
    // in this circuit if we are not searching it.
    bool Found = false;
    for (QuantumCircuit *QC : CircuitList) {
      Found = Found || (A->getParentCircuit() == QC);
      if (Found)
        break;
    }
    if (!Found)
      return false;
  }
  // Create a frontier that starts at the given operation.
  std::vector<QuantumCircuitOp *> Frontier = {A};
  DenseSet<QuantumCircuitOp *> Visited;

  bool Found = false;
  // As long as there are operation to process and we have not found operation
  // B, continue iterating over the graph until we find B or get to the end of
  // the circuit.
  while (Frontier.size() > 0 && !Found) {
    QuantumCircuitOp *Curr = Frontier.back();
    Frontier.pop_back();
    Visited.insert(Curr);
    if (Curr == EndOp)
      continue;
    for (QuantumCircuitOp *Succ : Curr->getSuccessors()) {
      if (Succ == B) {
        Found = true;
        break;
      }

      if (Succ == EndOp)
        continue;

      if (Visited.find(Succ) != Visited.end())
        continue;

      Frontier.push_back(Succ);
    }
  }
  return Found;
}
