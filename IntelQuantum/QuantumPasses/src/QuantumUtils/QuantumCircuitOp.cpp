//===- QuantumCircuitOp.cpp ---------------------------------------------====//
//
// Copyright (C) 2024 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//
//===----------------------------------------------------------------------===//
//
// This file is the implementation file for QuantumCircuitOp class and
// subclasses.
//
//===----------------------------------------------------------------------===//

#include "llvm/Demangle/Demangle.h"
#include "llvm/IR/Dominators.h"

#include "IntelQuantumPasses/QuantumAnalysis/QuantumIterator.h"
#include "IntelQuantumPasses/QuantumUtils/QuantumCircuit.h"
#include "IntelQuantumPasses/QuantumUtils/QuantumGeneralUtils.h"

#include <map>
#include <set>

QuantumCircuitOp::QuantumCircuitOp() { GateId = QuantumCircuit::getGateID(); }

QuantumCircuitOp::QuantumCircuitOp(QuantumCircuit &QC) {
  Parent = &QC;
  GateId = QC.getGateID();
}

QuantumOp::QuantumOp(Value *V) : QuantumCircuitOp() {
  OriginalValue = V;
  Parent = nullptr;
  Kind = OpKind::Quantum;
  if (!V)
    return;
  SentryNode = false;
  if (CallBase *CB = dyn_cast<CallBase>(OriginalValue)) {
    CalledVal = CB->getCalledFunction();
  }
  allParametersFromValue();
}

QuantumOp::QuantumOp(Value *V, QuantumCircuit &QC) : QuantumCircuitOp(QC) {
  OriginalValue = V;
  Parent = &QC;
  Kind = OpKind::Quantum;
  if (!V)
    return;
  SentryNode = false;
  if (CallBase *CB = dyn_cast<CallBase>(OriginalValue)) {
    CalledVal = CB->getCalledFunction();
  }
  allParametersFromValue();
}

QuantumOp::QuantumOp(Value *V, std::vector<QbitRef> &InQubits,
                     QuantumCircuit &QC)
    : QuantumCircuitOp(QC) {
  OriginalValue = V;
  Parent = &QC;
  unsigned Idx = 0;
  Kind = OpKind::Quantum;
  SentryNode = false;
  if (CallBase *CB = dyn_cast<CallBase>(OriginalValue)) {
    CalledVal = CB->getCalledFunction();
  }
  for (QbitRef QR : InQubits)
    Qubits.push_back(std::make_pair(QR, Idx++));
}

QuantumOp::QuantumOp(Value *V, std::vector<QbitRef> &InQubits,
                     std::vector<ParaRef> &InResolvedClassicalParameters,
                     QuantumCircuit &QC)
    : QuantumCircuitOp(QC) {
  OriginalValue = V;
  unsigned Idx = 0;
  Kind = OpKind::Quantum;
  SentryNode = false;
  if (CallBase *CB = dyn_cast<CallBase>(OriginalValue)) {
    CalledVal = CB->getCalledFunction();
  }
  for (QbitRef QR : InQubits)
    Qubits.push_back(std::make_pair(QR, Idx++));
  for (ParaRef PR : InResolvedClassicalParameters)
    ResolvedClassicalParameters.push_back(std::make_pair(PR, Idx++));
}

QuantumOp::QuantumOp(
    Value *V, std::vector<std::pair<QbitRef, unsigned>> &InQubits,
    std::vector<std::pair<ParaRef, unsigned>> &InResolvedClassicalParameters,
    std::vector<std::pair<Value *, unsigned>> &InOtherParameters,
    QuantumCircuit &QC)
    : QuantumCircuitOp(QC) {
  OriginalValue = V;
  Kind = OpKind::Quantum;
  SentryNode = false;
  if (CallBase *CB = dyn_cast<CallBase>(OriginalValue)) {
    CalledVal = CB->getCalledFunction();
  }
  for (std::pair<QbitRef, unsigned> QR : InQubits)
    Qubits.push_back(QR);
  for (std::pair<ParaRef, unsigned> CP : InResolvedClassicalParameters)
    ResolvedClassicalParameters.push_back(CP);
  for (std::pair<Value *, unsigned> OP : InOtherParameters)
    OtherParameters.push_back(OP);
}

QuantumOp::QuantumOp(Value *V, std::vector<QbitRef> &InQubits,
                     std::vector<ParaRef> &InResolvedClassicalParameters)
    : QuantumCircuitOp() {
  OriginalValue = V;
  unsigned Idx = 0;
  Kind = OpKind::Quantum;
  SentryNode = false;
  if (CallBase *CB = dyn_cast<CallBase>(OriginalValue)) {
    CalledVal = CB->getCalledFunction();
  }
  for (QbitRef QR : InQubits)
    Qubits.push_back(std::make_pair(QR, Idx++));
  for (ParaRef PR : InResolvedClassicalParameters)
    ResolvedClassicalParameters.push_back(std::make_pair(PR, Idx++));
}

QuantumOp::QuantumOp(
    Value *V, std::vector<std::pair<QbitRef, unsigned>> &InQubits,
    std::vector<std::pair<ParaRef, unsigned>> &InResolvedClassicalParameters,
    std::vector<std::pair<Value *, unsigned>> &InOtherParameters)
    : QuantumCircuitOp() {
  OriginalValue = V;
  Kind = OpKind::Quantum;
  SentryNode = false;
  if (CallBase *CB = dyn_cast<CallBase>(OriginalValue)) {
    CalledVal = CB->getCalledFunction();
  }
  for (std::pair<QbitRef, unsigned> QR : InQubits)
    Qubits.push_back(QR);
  for (std::pair<ParaRef, unsigned> CP : InResolvedClassicalParameters)
    ResolvedClassicalParameters.push_back(CP);
  for (std::pair<Value *, unsigned> OP : InOtherParameters)
    OtherParameters.push_back(OP);
}

static void collectToDelete(Value *V, std::vector<Value *> &ToDelete) {
  Instruction *I = dyn_cast<Instruction>(V);
  if (!I)
    return;

  if (I->getParent())
    return;

  ToDelete.push_back(I);
  for (Value *NextV : I->operands()) {
    collectToDelete(NextV, ToDelete);
  }
}

QuantumOp::~QuantumOp() {
  std::vector<Value *> ToDelete;
  collectToDelete(OriginalValue, ToDelete);
  for (Value *V : ToDelete) {
    V->deleteValue();
  }
}

QuantumOp *QuantumOp::Create(Function *F, QuantumModule &QM,
                             std::vector<QbitRef> &Qubits,
                             std::vector<ParaRef> &Params,
                             QuantumCircuitOp *After, std::string Name) {
  FunctionType *FTy = F->getFunctionType();
  std::vector<Value *> Args;
  for (QbitRef &QR : Qubits) {
    Args.push_back(QR.createValue(nullptr, false, true));
  }

  for (ParaRef &PR : Params) {
    Args.push_back(PR.getValue());
  }

  CallInst *CI = CallInst::Create(FTy, F, Args, Name);
  if (!After) {
    QuantumOp *NewOp = new QuantumOp(CI, Qubits, Params);
    NewOp->setQuantumModule(QM);
    QM.CreatedQCOs.insert(NewOp);
    return NewOp;
  }

  QuantumCircuit *QC = After->getParentCircuit();
  QuantumOp *QCO = new QuantumOp(CI, Qubits, Params, *QC);
  QCO->setQuantumModule(QM);
  QC->insertGate(QCO, After);

  return QCO;
}

QuantumOp *QuantumOp::Create(Function *F, QuantumModule &QM,
                             std::vector<QuantumOpParam> &OperationParams,
                             QuantumCircuitOp *After, std::string Name) {
  FunctionType *FTy = F->getFunctionType();
  std::vector<Value *> Args;
  std::vector<std::pair<QbitRef, unsigned>> Qubits;
  std::vector<std::pair<ParaRef, unsigned>> Params;
  std::vector<std::pair<Value *, unsigned>> Other;
  for (QuantumOpParam &QOP : OperationParams) {
    switch (QOP.ParamType) {
    case OpParamTy::QUBIT: {
      Qubits.push_back(std::make_pair(QOP.QR, Args.size()));
      Args.push_back(QOP.QR.createValue(nullptr, false, true));
      break;
    }
    case OpParamTy::QUBITPTR: {
      Qubits.push_back(std::make_pair(QOP.QR, Args.size()));
      Args.push_back(QOP.QR.createValue(nullptr, false, true));
      break;
    }
    case OpParamTy::GATEPARAM: {
      Params.push_back(std::make_pair(QOP.PR, Args.size()));
      Args.push_back(QOP.PR.getValue());
      break;
    }
    case OpParamTy::DOUBLE: {
      Value *NewValue = ConstantFP::get(
          Type::getDoubleTy(QM.getBaseModule()->getContext()), QOP.OtherDouble);
      Other.push_back(std::make_pair(NewValue, Args.size()));
      Args.push_back(NewValue);
      break;
    }
    case OpParamTy::INT: {
      Value *NewValue =
          ConstantInt::get(FTy->getParamType(Args.size()), QOP.OtherInt);
      Other.push_back(std::make_pair(NewValue, Args.size()));
      Args.push_back(NewValue);
      break;
    }
    case OpParamTy::OTHER: {
      Other.push_back(std::make_pair(QOP.OtherValue, Args.size()));
      Args.push_back(QOP.OtherValue);
      break;
    }
    }
  }

  CallInst *CI = CallInst::Create(FTy, F, Args, Name);
  if (!After) {
    QuantumOp *QCO = new QuantumOp(CI, Qubits, Params, Other);
    QCO->setQuantumModule(QM);
    QM.CreatedQCOs.insert(QCO);

    return QCO;
  }

  QuantumCircuit *QC = After->getParentCircuit();
  QuantumOp *QCO = new QuantumOp(CI, Qubits, Params, Other, *QC);
  QCO->setQuantumModule(QM);
  QC->insertGate(QCO, After);

  return QCO;
}

QuantumOp *QuantumOp::Create(int GateID, QuantumModule &QM,
                             std::vector<QbitRef> &Qubits,
                             std::vector<ParaRef> &Params,
                             QuantumCircuitOp *After, std::string Name) {
  if (!isAGate(GateID))
    return nullptr;
  Function *F = getGateFunction(GateID, QM.base);
  return QuantumOp::Create(F, QM, Qubits, Params, After);
}

QuantumOp *QuantumOp::Create(std::string GateName, QuantumModule &QM,
                             std::vector<QbitRef> &Qubits,
                             std::vector<ParaRef> &Params,
                             QuantumCircuitOp *After) {
  std::map<std::string, Function *>::iterator It;
  It = QM.HumanNameToFunction.find(GateName);
  if (It == QM.HumanNameToFunction.end())
    return nullptr;
  return QuantumOp::Create(It->second, QM, Qubits, Params, After);
}

QuantumOp *QuantumOp::Create(int GateID, QuantumModule &QM,
                             std::vector<QbitRef> &Qubits,
                             std::vector<ParaRef> &Params,
                             QuantumCircuit &InsertAtEnd) {
  if (!isAGate(GateID))
    return nullptr;
  Function *F = getGateFunction(GateID, QM.base);
  QuantumOp *NewOp = QuantumOp::Create(F, QM, Qubits, Params);
  InsertAtEnd.appendGate(NewOp);
  return NewOp;
}

QuantumOp *QuantumOp::Create(std::string GateName, QuantumModule &QM,
                             std::vector<QbitRef> &Qubits,
                             std::vector<ParaRef> &Params,
                             QuantumCircuit &InsertAtEnd) {
  std::map<std::string, Function *>::iterator It;
  It = QM.HumanNameToFunction.find(GateName);
  if (It == QM.HumanNameToFunction.end())
    return nullptr;
  QuantumOp *NewOp = QuantumOp::Create(It->second, QM, Qubits, Params);
  InsertAtEnd.appendGate(NewOp);
  return NewOp;
}

QuantumOp *QuantumOp::Create(std::string GateName, QuantumModule &QM,
                             std::vector<QuantumOpParam> &OperationParams,
                             QuantumCircuit &InsertAtEnd) {
  std::map<std::string, Function *>::iterator It;
  It = QM.HumanNameToFunction.find(GateName);
  if (It == QM.HumanNameToFunction.end())
    return nullptr;
  QuantumOp *NewOp = QuantumOp::Create(It->second, QM, OperationParams);
  InsertAtEnd.appendGate(NewOp);
  return NewOp;
}

QuantumOp *QuantumOp::Create(std::string GateName, QuantumModule &QM,
                             std::vector<QuantumOpParam> &OperationParams,
                             QuantumCircuitOp *After) {
  std::map<std::string, Function *>::iterator It;
  It = QM.HumanNameToFunction.find(GateName);
  if (It == QM.HumanNameToFunction.end())
    return nullptr;
  QuantumOp *NewOp = QuantumOp::Create(It->second, QM, OperationParams, After);
  return NewOp;
}

QbitRef QuantumOp::getQbitRefFromValue(CallBase *CB, unsigned Idx) {
  // Get the operand value.
  Value *V = CB->getArgOperand(Idx);
  QIter DummyIter;

  // If it could not be found, use the QIter helper function and
  // insert it if a value could be constructed.
  QbitRef QR = DummyIter.qubit_backtrace_helper(CB, Idx, false);

  return QR;
}

ParaRef QuantumOp::getParaRefFromValue(CallBase *CB, unsigned Idx) {
  // Get the operand value.
  Value *V = CB->getArgOperand(Idx);
  QIter DummyIter;

  // If it could not be found, use the QIter helper function and
  // insert it if a value could be constructed.
  ParaRef PR = DummyIter.parametric_backtrace_helper(CB, Idx, false);

  return PR;
}

Type *QuantumOpParam::getType(LLVMContext &C) {
  Type *OutType = nullptr;
  switch (ParamType) {
  case (OpParamTy::QUBIT):
    OutType = Type::getInt16Ty(C);
    break;
  case (OpParamTy::QUBITPTR):
    OutType = Type::getInt16Ty(C)->getPointerTo();
    break;
  case (OpParamTy::GATEPARAM):
    OutType = Type::getDoubleTy(C);
    break;
  case (OpParamTy::INT):
    OutType = Type::getInt32Ty(C);
    break;
  case (OpParamTy::DOUBLE):
    OutType = Type::getDoubleTy(C);
    break;
  case (OpParamTy::OTHER):
    OutType = T;
    break;
  default:
    break;
  }

  return OutType;
}

Value *QuantumOpParam::getValue(LLVMContext &C) {
  Value *OutVal = nullptr;
  switch (ParamType) {
  case (OpParamTy::QUBIT):
    OutVal = QR.createValue(nullptr, false, true);
    break;
  case (OpParamTy::QUBITPTR):
    OutVal = QR.getRegister();
    break;
  case (OpParamTy::GATEPARAM):
    OutVal = PR.getValue();
    break;
  case (OpParamTy::INT):
    OutVal = ConstantInt::get(Type::getInt32Ty(C), OtherInt, true);
    break;
  case (OpParamTy::DOUBLE):
    OutVal = ConstantFP::get(Type::getDoubleTy(C), OtherDouble);
    break;
  case (OpParamTy::OTHER):
    OutVal = OtherValue;
    break;
  default:
    break;
  }

  return OutVal;
}

Value *QuantumOpParam::getValue(QuantumCircuit *QC) {
  if (!QC)
    return nullptr;
  return getValue(QC->getQuantumModule()->getBaseModule()->getContext());
}

Value *QuantumOpParam::getValue(QuantumOp *QO) {
  if (!QO)
    return nullptr;
  return getValue(QO->getQuantumModule()->getBaseModule()->getContext());
}

void QuantumOp::allParametersFromValue() {
  CallBase *CB = dyn_cast<CallBase>(OriginalValue);
  if (!CB)
    return;
  unsigned NumArgs = CB->arg_size();
  if (!ParamsChanged)
    return;
  Qubits.clear();
  ResolvedClassicalParameters.clear();
  OtherParameters.clear();
  for (unsigned Idx = 0; Idx < NumArgs; Idx++) {
    Value *V = CB->getArgOperand(Idx);

    QbitRef QR;
    if (Parent)
      QR = Parent->getQbitRefFromValue(CB, Idx);
    else
      QR = getQbitRefFromValue(CB, Idx);
    if (!QR.isNull()) {
      Qubits.push_back(std::make_pair(QR, Idx));
      continue;
    }

    ParaRef PR;
    if (Parent)
      PR = Parent->getParaRefFromValue(CB, Idx);
    else
      PR = getParaRefFromValue(CB, Idx);
    if (!PR.isNull()) {
      ResolvedClassicalParameters.push_back(std::make_pair(PR, Idx));
      continue;
    }

    OtherParameters.push_back(std::make_pair(V, Idx));
  }

  ParamsChanged = false;
}

void QuantumCircuitOp::setParentCircuit(QuantumCircuit *QC) {
  if (!QM && QC)
    setQuantumModule(*QC->getQuantumModule());
  Parent = QC;
}

void QuantumCircuitOp::resolveDependence(QuantumCircuitOp &Pred,
                                         QuantumCircuitOp &Succ) {
  // If there is not an entry for the successor in the mapping from operation
  // to qubit set for the Predecessor map, add it here for use later.
  if (Pred.SuccessorDependence.find(&Succ) == Pred.SuccessorDependence.end())
    Pred.SuccessorDependence.emplace(&Succ, std::set<QbitRef, QubitCompare>());

  // If the successor operation is a quantum operation, we can handle it in
  // a fairly standard way.
  if (Succ.getKind() == Quantum) {
    QuantumOp *QO = static_cast<QuantumOp *>(&Succ);
    // Check the qubits for the successor.
    for (std::pair<QbitRef, unsigned> &Pair : QO->getQubits()) {
      // If there is already a qubit dependency, do not replace it. Continue
      // checking qubits.
      if (Pred.SuccessorDependency.find(Pair.first) !=
          Pred.SuccessorDependency.end()) {
        continue;
      }
      if (Pred.getKind() == Start) {
        // If the previous operation is the start operation, add it
        // automatically to both maps.
        Pred.SuccessorDependence[&Succ].insert(Pair.first);
        Pred.SuccessorDependency[Pair.first] = &Succ;
      } else {
        // Otherwise, get the quantum operation, and check the qubits for
        // matches. Add it to the dependency for each match.
        QuantumOp *CurrQO = static_cast<QuantumOp *>(&Pred);
        for (std::pair<QbitRef, unsigned> &CurrPair : CurrQO->getQubits()) {
          if (CurrPair.first.isEqual(Pair.first) == 1) {
            Pred.SuccessorDependence[&Succ].insert(Pair.first);
            Pred.SuccessorDependency[Pair.first] = &Succ;
          }
        }
      }
    }
  } else if (Succ.getKind() == End) {
    // If the successor is the ending operation, and the predecessor is a
    // quantum operation, set up a qubit dependency for each qubit.
    if (Pred.getKind() == Quantum) {
      QuantumOp *CurrQO = static_cast<QuantumOp *>(&Pred);
      for (std::pair<QbitRef, unsigned> &CurrPair : CurrQO->getQubits()) {
        if (Pred.SuccessorDependency.find(CurrPair.first) ==
            Pred.SuccessorDependency.end()) {
          Pred.SuccessorDependence[&Succ].insert(CurrPair.first);
          Pred.SuccessorDependency[CurrPair.first] = &Succ;
        }
      }
    }
  }
}

void QuantumCircuitOp::createDependence(QuantumCircuitOp &Pred,
                                        QuantumCircuitOp &Succ,
                                        const QbitRef &QR) {
  // If there is not an entry for the successor in the mapping from operation
  // to qubit set for the Predecessor map, add it here for use later.
  if (Pred.SuccessorDependence.find(&Succ) == Pred.SuccessorDependence.end())
    Pred.SuccessorDependence.emplace(&Succ, std::set<QbitRef, QubitCompare>());
  Pred.SuccessorDependence[&Succ].insert(QR);
  Pred.SuccessorDependency[QR] = &Succ;
}

void QuantumCircuitOp::unlinkDependence(QuantumCircuitOp &Pred,
                                        QuantumCircuitOp &Succ, QbitRef &QR) {
  // Remove the qubit dependency for QR between Predecessor and Successor.
  if (Pred.getDependence(QR) != &Succ)
    return;
  Pred.SuccessorDependency.erase(QR);
  Pred.SuccessorDependence[&Succ].erase(QR);
}

void QuantumCircuitOp::unlinkOperationsIfNoDependence(QuantumCircuitOp &Pred,
                                                      QuantumCircuitOp &Succ) {
  // Get the qubit set between the predecessor and successor.
  std::map<QuantumCircuitOp *, std::set<QbitRef, QubitCompare>>::iterator It =
      Pred.SuccessorDependence.find(&Succ);
  bool CanRemove = false;
  // If none was found, or the set is empty we can remove.
  if (It == Pred.SuccessorDependence.end())
    CanRemove = true;
  else if (It->second.size() == 0) {
    Pred.SuccessorDependence.erase(&Succ);
    CanRemove = true;
  }
  // Remove if possible.
  if (CanRemove)
    Pred.removeSuccessor(&Succ);
}

void QuantumCircuitOp::unlinkOperations(QuantumCircuitOp &Pred,
                                        QuantumCircuitOp &Succ) {
  // Get the qubit set between the predecessor and successor.
  std::map<QuantumCircuitOp *, std::set<QbitRef, QubitCompare>>::iterator It =
      Pred.SuccessorDependence.find(&Succ);

  if (It != Pred.SuccessorDependence.end()) {
    // Remove any dependencies.
    for (const QbitRef &QR : It->second)
      if (Pred.SuccessorDependency.find(QR) != Pred.SuccessorDependency.end())
        Pred.SuccessorDependency.erase(QR);
    Pred.SuccessorDependence.erase(&Succ);
  }

  // Remove the predecessor and successor relationship.
  if (Pred.Successors.find(&Succ) != Pred.Successors.end())
    Pred.removeSuccessor(&Succ);
}

bool QuantumCircuitOp::hasDependence(QbitRef &QR) {
  return SuccessorDependency.find(QR) != SuccessorDependency.end();
}

QuantumCircuitOp *QuantumCircuitOp::getDependence(QbitRef &QR) {
  std::map<QbitRef, QuantumCircuitOp *, QubitCompare>::iterator It =
      SuccessorDependency.find(QR);
  if (It == SuccessorDependency.end())
    return nullptr;
  return It->second;
}

std::set<QbitRef, QubitCompare>
QuantumCircuitOp::getQubitDependence(QuantumCircuitOp *QCO) {
  std::map<QuantumCircuitOp *, std::set<QbitRef, QubitCompare>>::iterator It =
      SuccessorDependence.find(QCO);
  if (It == SuccessorDependence.end())
    return std::set<QbitRef, QubitCompare>();
  return It->second;
}

void QuantumCircuitOp::addSuccessor(QuantumCircuitOp *Out) {
  resolveDependence(*this, *Out);
  Successors.insert(Out);
  Out->Predecessors.insert(this);
}

void QuantumCircuitOp::removeSuccessor(QuantumCircuitOp *Out) {
  Successors.erase(Out);
  Out->Predecessors.erase(this);
}

void QuantumCircuitOp::addSuccessors(std::set<QuantumCircuitOp *> &Outs) {
  for (QuantumCircuitOp *Out : Outs)
    addSuccessor(Out);
}

void QuantumCircuitOp::addPredecessor(QuantumCircuitOp *In) {
  resolveDependence(*In, *this);
  Predecessors.insert(In);
  In->Successors.insert(this);
}

void QuantumCircuitOp::removePredecessor(QuantumCircuitOp *In) {
  Predecessors.erase(In);
  In->Successors.erase(this);
}

void QuantumCircuitOp::addPredecessors(std::set<QuantumCircuitOp *> &Ins) {
  for (QuantumCircuitOp *In : Ins)
    addPredecessor(In);
}

std::string QuantumCircuitOp::printPretty() {
  std::string ReturnString =
      std::to_string(getGateID()) + ": Generic Operation";

  return ReturnString;
}

std::string QuantumOp::printPretty() {
  std::string ReturnString = std::to_string(getGateID()) + ": ";

  std::string CalledName = CalledVal->getName().str();

  std::string::size_type StubLoc = CalledName.find(".stub");
  if (StubLoc != std::string::npos) {
    ReturnString += demangle(CalledName.substr(0, StubLoc));
  } else
    ReturnString += demangle(CalledName);
  for (std::pair<QbitRef, unsigned> &QR : Qubits) {
    ReturnString += ", " + getPrettyName(QR.first);
  }

  for (std::pair<ParaRef, unsigned> &PR : ResolvedClassicalParameters) {
    ReturnString += ", " + getPrettyName(PR.first);
  }

  return ReturnString;
}

std::string QuantumOp::getOpName() {
  std::string ReturnString = std::to_string(getGateID()) + ": ";

  std::string CalledName = CalledVal->getName().str();

  return CalledName;
}

bool QuantumOp::moveGate(QuantumCircuitOp &After) {
  std::map<unsigned, QuantumOpParam> ArgMapping;
  return moveGate(After, ArgMapping);
}

static void fixForMoveCopy(QuantumCircuit *ParentQC, QuantumCircuit *CurrentQC,
                           QuantumOp *QO) {
  // Collect the operands used by QO if it is an instruction.
  std::vector<Instruction *> V;
  for (Value *Operand :
       dyn_cast<Instruction>(QO->getBaseValue())->operand_values()) {
    if (Instruction *I = dyn_cast<Instruction>(Operand))
      V.push_back(I);
  }

  // Get the original block of the circuit we are inserting into and
  // the circuit we are inserting from.
  BasicBlock *ParentBB = dyn_cast<BasicBlock>(ParentQC->getBaseValue());
  BasicBlock *CurrentBB = dyn_cast<BasicBlock>(CurrentQC->getBaseValue());

  // If there is no shared parent function, or it does not exists, we only
  // collect the new parent block. Otherwise, we add both in execution order to
  // the list of basic blocks.
  QuantumCircuit *SharedParent = ParentQC->getParentCircuit();
  std::vector<BasicBlock *> BBs;
  if (!ParentBB->getParent() ||
      ParentBB->getParent() != CurrentBB->getParent()) {
    BBs.push_back(ParentBB);
  } else {
    DominatorTree DT(*ParentBB->getParent());
    if (DT.dominates(ParentBB, *CurrentBB->begin()->getOperandList())) {
      BBs.push_back(ParentBB);
      BBs.push_back(CurrentBB);
    } else {
      BBs.push_back(CurrentBB);
      BBs.push_back(ParentBB);
    }
  }
  updateGatesForBBs(BBs, V);
}

bool QuantumOp::moveGate(QuantumCircuitOp &After,
                         std::map<unsigned, QuantumOpParam> &ArgMapping) {
  if (&After == this)
    return false;

  QuantumCircuit *CurrentQC = getParentCircuit();
  QuantumCircuit *ParentQC = After.getParentCircuit();
  if (!ParentQC)
    return false;

  bool WasRemoved = removeGate();
  if (!WasRemoved)
    return WasRemoved;

  bool WasInserted = ParentQC->insertGate(this, &After, ArgMapping);

  if (!WasInserted)
    return WasInserted;

  fixForMoveCopy(ParentQC, CurrentQC, this);

  return WasInserted;
}

bool QuantumOp::moveGate(QuantumCircuit *QC) {
  std::map<unsigned, QuantumOpParam> ArgMapping;
  bool Result = moveGate(QC, ArgMapping);
  return Result;
}

bool QuantumOp::moveGate(QuantumCircuit *QC,
                         std::map<unsigned, QuantumOpParam> &ArgMapping) {
  QuantumCircuit *ParentQC = QC;
  QuantumCircuit *CurrentQC = getParentCircuit();
  if (!ParentQC)
    return false;

  bool WasRemoved = removeGate();
  if (!WasRemoved)
    return WasRemoved;

  bool Result = ParentQC->appendGate(this, ArgMapping);
  fixForMoveCopy(ParentQC, CurrentQC, this);

  return Result;
}

QuantumOp *QuantumOp::copyGate(QuantumCircuitOp &After,
                               std::map<unsigned, QuantumOpParam> &ArgMapping) {
  QuantumCircuit *CurrentQC = getParentCircuit();
  QuantumCircuit *ParentQC = After.getParentCircuit();
  if (!ParentQC)
    return nullptr;

  Instruction *NewInst = dyn_cast<Instruction>(getBaseValue())->clone();
  QuantumOp *QO = new QuantumOp(NewInst);
  QO->setQuantumModule(*this->QM);

  bool Worked = ParentQC->insertGate(QO, &After, ArgMapping);
  if (!Worked)
    return QO;

  fixForMoveCopy(ParentQC, CurrentQC, this);
  return QO;
}

QuantumOp *QuantumOp::copyGate(QuantumCircuitOp &After) {
  std::map<unsigned, QuantumOpParam> ArgMapping;
  return copyGate(After, ArgMapping);
}

QuantumOp *QuantumOp::copyGate(QuantumCircuit *QC,
                               std::map<unsigned, QuantumOpParam> &ArgMapping) {
  QuantumCircuit *CurrentQC = getParentCircuit();
  QuantumCircuit *ParentQC = QC;
  if (!ParentQC)
    return nullptr;

  Instruction *NewInst = dyn_cast<Instruction>(getBaseValue())->clone();
  QuantumOp *QO = new QuantumOp(NewInst);
  QO->setQuantumModule(*this->QM);

  ParentQC->appendGate(QO, ArgMapping);
  fixForMoveCopy(ParentQC, CurrentQC, QO);
  return QO;
}

QuantumOp *QuantumOp::copyGate(QuantumCircuit *QC) {
  std::map<unsigned, QuantumOpParam> ArgMapping;
  return copyGate(QC, ArgMapping);
}

QuantumOp *QuantumOp::copyGate() {
  Instruction *NewInst = dyn_cast<Instruction>(getBaseValue())->clone();
  QuantumOp *QO = new QuantumOp(NewInst);
  QO->setParentCircuit(nullptr);
  QO->setQuantumModule(*this->QM);
  return QO;
}

bool QuantumOp::deleteGate() {
  bool WasRemoved = removeGate();
  if (!WasRemoved)
    return WasRemoved;

  // Erase the operation from memory.
  if (QM->CreatedQCOs.find(this) != QM->CreatedQCOs.end())
    QM->CreatedQCOs.erase(this);
  Value *TempBaseValue = getBaseValue();
  // Free the memory for the quantum operation.
  delete this;

  return true;
}

bool QuantumOp::removeGate() {
  // If ToRemove is not in a circuit, we can't do anything about this.
  if (!getParentCircuit()) {
    return false;
  }

  // Structures to hold mapping of qubit to dependencies.
  std::map<QbitRef, QuantumCircuitOp *, QubitCompare> LostDependenciesSuccs;
  std::map<QbitRef, QuantumCircuitOp *, QubitCompare> LostDependenciesPreds;

  // Structures to hold the successors and predecessors that were explicitly
  // defined so that the explicit ordering can be maintained.
  std::set<QuantumCircuitOp *> SuccsNoQbitDepends;
  std::set<QuantumCircuitOp *> PredsNoQbitDepends;

  // Collect the necessary information about the successors. We create a new
  // list here since we will be manipulating the base lists in the ToRemove
  // object, which cases issues during iteration.
  std::set<QuantumCircuitOp *> Succs;
  for (QuantumCircuitOp *Succ : getSuccessors()) {
    // Check for qubit dependencies between the operation to delete and the
    // successor operation.
    const std::set<QbitRef, QubitCompare> &QubitSet = getQubitDependence(Succ);
    // If there is none, the relationship was manual, we will nee to enforce it
    // later on. Mark it as such.
    if (QubitSet.size() == 0) {
      SuccsNoQbitDepends.insert(Succ);
    }

    // For the qubits that determine a dependency, mark it in the appropriate
    // mapping.
    for (const QbitRef &QR : QubitSet) {
      LostDependenciesSuccs.insert(std::make_pair(QR, Succ));
    }

    Succs.insert(Succ);
  }

  // Do the same as above, but for the predecessors.
  std::set<QuantumCircuitOp *> Preds;
  for (QuantumCircuitOp *Pred : getPredecessors()) {
    const std::set<QbitRef, QubitCompare> &QubitSet =
        Pred->getQubitDependence(this);

    if (QubitSet.size() == 0) {
      PredsNoQbitDepends.insert(Pred);
    }
    for (const QbitRef &QR : QubitSet) {
      LostDependenciesPreds.insert(std::make_pair(QR, Pred));
    }

    Preds.insert(Pred);
  }

  // Explicitly unlink the successors from the operation we are about to remove.
  for (QuantumCircuitOp *QCO : Succs) {
    QuantumCircuitOp::unlinkOperations(*this, *QCO);
  }

  // Explicitly unlink the predecessors from the operation we are about to
  // remove.
  for (QuantumCircuitOp *QCO : Preds) {
    QuantumCircuitOp::unlinkOperations(*QCO, *this);
  }

  // Mark the reduction in gate count.
  getParentCircuit()->reduceGates();

  // Iterate over the removed dependencies for the predecessors.
  for (std::pair<QbitRef, QuantumCircuitOp *> QRToQCOPair :
       LostDependenciesPreds) {
    // Find the corresponding successor for the qubit.
    std::map<QbitRef, QuantumCircuitOp *, QubitCompare>::iterator It;
    It = LostDependenciesSuccs.find(QRToQCOPair.first);
    assert(It != LostDependenciesSuccs.end());

    // Add the successor as a dependent of the predecessor based on the given
    // qubit.
    QRToQCOPair.second->addSuccessor(It->second);
  }

  // In the case that there were explicit dependencies, connect that predecessor
  // to all successors. This ensures the ordering.
  for (QuantumCircuitOp *QCO : PredsNoQbitDepends) {
    for (QuantumCircuitOp *Succ : Succs)
      QCO->addSuccessor(Succ);
  }

  // Do the same but for the explicit successors.
  for (QuantumCircuitOp *QCO : SuccsNoQbitDepends) {
    for (QuantumCircuitOp *Pred : Preds)
      Pred->addSuccessor(QCO);
  }

  setParentCircuit(nullptr);

  // Remove the operation from the IR.
  dyn_cast<Instruction>(getBaseValue())->removeFromParent();

  QM->CreatedQCOs.insert(this);

  return true;
}

std::string StartingOp::printPretty() {
  return "Start Circuit: " + Parent->getName();
}

std::string EndingOp::printPretty() {
  return "End Circuit: " + Parent->getName();
}

bool QuantumCircuitOp::operator<(QuantumCircuitOp &Other) {
  // First check the parent circuit starting ID. Then check
  // by the gate ID within the circuit.
  if (this->getParentCircuit()->getStartOp()->getGateID() !=
      Other.getParentCircuit()->getStartOp()->getGateID())
    return this->getParentCircuit()->getStartOp()->getGateID() <
           Other.getParentCircuit()->getStartOp()->getGateID();
  return this->getGateID() < Other.getGateID();
}

bool QuantumCircuitOp::operator>(QuantumCircuitOp &Other) {
  // First check the parent circuit starting ID. Then check
  // by the gate ID within the circuit.
  if (this->getParentCircuit()->getStartOp()->getGateID() !=
      Other.getParentCircuit()->getStartOp()->getGateID())
    return this->getParentCircuit()->getStartOp()->getGateID() >
           Other.getParentCircuit()->getStartOp()->getGateID();
  return this->getGateID() > Other.getGateID();
}

bool QuantumCircuitOp::operator==(QuantumCircuitOp &Other) {
  // First check the parent circuit starting ID. Then check
  // by the gate ID within the circuit.
  if (this->getParentCircuit()->getStartOp()->getGateID() !=
      Other.getParentCircuit()->getStartOp()->getGateID())
    return false;
  return this->getGateID() == Other.getGateID();
}