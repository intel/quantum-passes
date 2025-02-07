//===- quantum-circuit-object.cpp ---------------------------*- C++ -*-===//
//
//===----------------------------------------------------------------------===//
//
// Copyright (C) 2024 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//
//===----------------------------------------------------------------------===//
/// \file "quantum-circuit-object.cpp"
///
/// This tool takes in a quantum module and iterates over each quantum kernel
/// and outputs the gates contained through a consistent iteration order.
///
//===----------------------------------------------------------------------===//

#include "llvm/Bitcode/BitcodeWriter.h"
#include "llvm/Demangle/Demangle.h"
#include "llvm/IR/IRPrintingPasses.h"
#include "llvm/IRReader/IRReader.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/InitLLVM.h"
#include "llvm/Support/SourceMgr.h"
#include "llvm/Support/ToolOutputFile.h"
#include "llvm/Support/raw_ostream.h"

#include "IntelQuantumPasses/QuantumAnalysis/QuantumAnnotationsToJson.h"
#include "IntelQuantumPasses/QuantumAnalysis/QuantumCompilerLinkage.h"
#include "IntelQuantumPasses/QuantumAnalysis/QuantumModule.h"
#include "IntelQuantumPasses/QuantumAnalysis/QuantumSpinNativeToJson.h"

#include "IntelQuantumPasses/QuantumUtils/QuantumCircuit.h"
#include "IntelQuantumPasses/QuantumUtils/QuantumGeneralUtils.h"

#include <iostream>

static cl::opt<std::string> InputSourceFile(cl::Positional,
                                            cl::desc("<Source file>"),
                                            cl::init("-"),
                                            cl::value_desc("filename"));

static cl::opt<int> CheckStage("s", cl::desc("<stage>"), cl::init(-1));

/// Pick a circuit from a list to view or edit.  Circuits are selected
/// in the form of [parent id]-[child id].
QuantumCircuit *
pickCircuitToChange(std::vector<QuantumCircuit *> &CircuitList) {
  unsigned TopLevelIdx = 0;
  for (QuantumCircuit *TopQC : CircuitList) {
    unsigned SecondLevelIdx = 0;
    std::cout << TopLevelIdx << ": " << TopQC->getBaseValue()->getName().str()
              << ":\n";
    for (QuantumCircuit *SecondQC : TopQC->getCircuitList()) {
      std::cout
          << "  " << TopLevelIdx << "-" << SecondLevelIdx << ": "
          << dyn_cast<BasicBlock>(SecondQC->getBaseValue())->getName().str()
          << ":\n";
      SecondLevelIdx++;
    }
    TopLevelIdx++;
  }
  std::cout << "Select Circuit: ";
  std::string CircuitCode;
  std::cin >> CircuitCode;
  size_t Loc = CircuitCode.find("-");
  if (Loc == std::string::npos) {
    int First = std::stoi(CircuitCode);
    return CircuitList[First];
  }
  int First = std::stoi(CircuitCode.substr(0, Loc));
  int Second = std::stoi(CircuitCode.substr(Loc + 1));
  return CircuitList[First]->getCircuitList()[Second];
}

/// Processing operation deletion, simply print the operations
/// and corresponding IDs, then remove it using the included
/// gate.
void deleteOp(QuantumCircuit *QC) {
  std::vector<QuantumCircuitOp *> OpList;
  QC->iterateGates(OpList);
  std::cout << "Operations:\n";
  unsigned Idx = 0;
  for (QuantumCircuitOp *QCO : OpList)
    std::cout << Idx++ << ": " << QCO->printPretty() << "\n";
  std::string Selected;
  std::cout << "Selected Op: ";
  std::cin >> Selected;
  int SelectedOp = std::stoi(Selected);
  static_cast<QuantumOp *>(OpList[SelectedOp])->deleteGate();
}

/// Process the users input to add a gate to a circuit.
void processInputToAddGate(QuantumCircuit *QC, QuantumModule &QM) {
  std::vector<QuantumCircuitOp *> OpList;
  // Get all the gates and assign IDs to the qubits.
  QC->iterateGates(OpList);
  std::map<int, QbitRef> QubitMap;
  unsigned Idx = 0;
  for (const QbitRef &QR : QC->getQubits()) {
    QubitMap.insert(std::make_pair(Idx++, QR));
  }

  for (const QbitRef &QR : QuantumCircuit::getGlobalQubits(QM)) {
    QubitMap.insert(std::make_pair(Idx++, QR));
  }
  Idx = 0;
  std::string OperationName;
  std::string QubitList;
  std::string ParamList;
  std::cout << "\nValid Names: ";
  for (const std::pair<std::string, Function *> &Pair :
       QM.HumanNameToFunction) {
    std::cout << Pair.first << ", ";
  }
  std::cout << "\nOperation to add: ";
  std::cin >> OperationName;

  // Output all the qubits available and their IDs.
  std::cout << "\nQubits:\n";
  for (std::pair<int, QbitRef> QubitInt : QubitMap) {
    std::cout << "\t" << QubitInt.first << ": "
              << getPrettyName(QubitInt.second) << "\n";
  }
  std::cout << "Qubits to Apply to (comma-separated list, with no spaces, use "
               "- for none): ";
  std::cin >> QubitList;
  std::string RemainingList = QubitList;

  // Separate by commas, and create a list of the QbitRefs.
  std::vector<QbitRef> QubitVector;
  if (RemainingList == "-")
    RemainingList = "";
  while (RemainingList.size() > 0) {
    size_t CommaLoc = RemainingList.find(",");
    int ToUse = std::stoi(RemainingList.substr(0, CommaLoc));
    QubitVector.push_back(QubitMap[ToUse]);
    if (CommaLoc == std::string::npos) {
      RemainingList = "";
      continue;
    }
    RemainingList = RemainingList.substr(CommaLoc + 1);
    errs() << "\"" << RemainingList << "\"\n";
  }

  // Do the same with the parameters.
  std::cout << "\nParams to use (comma-separated list, Use - for none): ";
  std::cin >> ParamList;
  RemainingList = ParamList;
  if (RemainingList == "-")
    RemainingList = "";
  std::vector<ParaRef> ParamVector;
  size_t CurrentStart = 0;
  while (RemainingList.size() > 0) {
    size_t CommaLoc = RemainingList.find(",");
    double ToUse = std::stod(RemainingList.substr(0, CommaLoc));
    ParamVector.push_back(ParaRef(ToUse));
    if (CommaLoc == std::string::npos) {
      RemainingList = "";
      continue;
    }
    RemainingList = RemainingList.substr(CommaLoc + 1);
  }

  // Create the operation
  QuantumOp *NewOp =
      QuantumOp::Create(OperationName, QM, QubitVector, ParamVector);

  // To put after an operation, print the operations out, giving IDs based
  // on the order in the operation list.
  bool OpDone = false;
  while (!OpDone) {
    // Get input about whether to input at the end of the circuit or after a
    // particular operation.
    std::string AppendOrAfter;
    std::cout << "\nAppend at end (e, default), start(s), or insert after "
                 "operation (i): ";
    std::cin >> AppendOrAfter;
    OpDone = true;
    if (AppendOrAfter == "i") {
      std::cout << "Operations:\n";
      for (QuantumCircuitOp *QCO : OpList)
        std::cout << Idx++ << ": " << QCO->printPretty() << "\n";
      std::string Selected;
      std::cout << "Selected Op: ";
      std::cin >> Selected;
      std::cout << "\n";
      int SelectedOp = std::stoi(Selected);
      QC->insertGate(NewOp, OpList[SelectedOp]);
    } else if (AppendOrAfter == "s") {
      QC->insertGate(NewOp, QC->getStartOp());
    } else if (AppendOrAfter == "e") {
      QC->appendGate(NewOp);
    } else {
      OpDone = false;
      std::cout << "Not a valid option\n";
    }
  }
}

void opsToDefine(Value *V, std::set<unsigned> &Vals,
                 std::set<Value *> &Visited) {
  if (Visited.find(V) != Visited.end())
    return;

  Instruction *I = dyn_cast<Instruction>(V);
  Argument *Arg = dyn_cast<Argument>(V);
  Visited.insert(V);
  if (!I && !Arg)
    return;

  if (Arg) {
    unsigned ArgLoc = Arg->getArgNo();
    Vals.insert(ArgLoc);
    return;
  }

  for (Value *V : I->operand_values()) {
    opsToDefine(V, Vals, Visited);
  }
}

void processInputToMoveGate(QuantumCircuit *QC,
                            std::vector<QuantumCircuit *> &CircuitList,
                            QuantumModule &QM, bool Copy = false) {
  std::vector<QuantumCircuitOp *> OpList;
  QC->iterateGates(OpList);
  std::cout << "Operations:\n";
  unsigned Idx = 0;
  for (QuantumCircuitOp *QCO : OpList)
    std::cout << Idx++ << ": " << QCO->printPretty() << "\n";
  std::string Selected;
  std::string MoveOrCopy = "move";
  if (Copy)
    MoveOrCopy = "copy";
  std::cout << "Select Operation to " << MoveOrCopy << ": ";
  std::cin >> Selected;
  int SelectedOp = std::stoi(Selected);
  QuantumOp *ToMove = nullptr;
  ToMove = static_cast<QuantumOp *>(OpList[SelectedOp]);

  std::cout << "Choose circuit to " << MoveOrCopy << " "
            << ToMove->printPretty() << "to\n";
  QuantumCircuit *NewQC = pickCircuitToChange(CircuitList);

  std::vector<QuantumCircuitOp *> NewOpList;
  OpList.clear();
  NewQC->iterateGates(OpList);
  std::cout << "Operations:\n";
  Idx = 0;
  for (QuantumCircuitOp *QCO : OpList)
    std::cout << Idx++ << ": " << QCO->printPretty() << "\n";
  std::cout
      << "Select Operation to " << MoveOrCopy << " " << ToMove->printPretty()
      << " after, input - to append the operation to the end of circuit: ";
  std::cin >> Selected;
  QuantumOp *ToMoveAfter = nullptr;
  if (Selected != "-") {
    int NewSelectedOp = std::stoi(Selected);
    ToMoveAfter = static_cast<QuantumOp *>(OpList[NewSelectedOp]);
  }

  std::map<unsigned, QuantumOpParam> ArgMapping;
  if (NewQC != QC) {
    std::set<unsigned> Vals;
    std::set<Value *> Visited;
    opsToDefine(ToMove->getBaseValue(), Vals, Visited);
    Function *F = nullptr;
    if (QC->isABasicBlock()) {
      F = dyn_cast<BasicBlock>(QC->getBaseValue())->getParent();
    } else {
      F = dyn_cast<Function>(QC->getBaseValue());
    }
    std::cout << "For function: " << F->getName().str()
              << " with function type ";
    F->getFunctionType()->print(errs());
    std::cout << "\n";
    for (unsigned V : Vals) {
      std::cout << "Define argument " << V
                << " type Int (i), Double (d), Qubit (q), Parameter (p): ";
      std::string ParamTypeString;
      std::cin >> ParamTypeString;
      if (ParamTypeString == "q") {
        std::map<int, QbitRef> QubitMap;
        unsigned Idx = 0;
        for (const QbitRef &QR : NewQC->getQubits()) {
          QubitMap.insert(std::make_pair(Idx++, QR));
        }
        std::cout << "\nQubits:\n";
        for (std::pair<int, QbitRef> QubitInt : QubitMap) {
          std::cout << "\t" << QubitInt.first << ": "
                    << getPrettyName(QubitInt.second) << "\n";
        }
        std::string QubitString;
        std::cout << "\nSelect which qubit to use:\n";
        std::cin >> QubitString;
        int ToUse = std::stoi(QubitString);
        if (QubitMap[ToUse].getIndexResolved().has_value())
          ArgMapping.emplace(V,
                             QuantumOpParam(OpParamTy::QUBIT, QubitMap[ToUse]));
        else
          ArgMapping.emplace(
              V, QuantumOpParam(OpParamTy::QUBITPTR, QubitMap[ToUse]));
      } else if (ParamTypeString == "i") {
        std::string IntString;
        std::cin >> IntString;
        ArgMapping.emplace(
            V, QuantumOpParam(OpParamTy::INT, std::stoi(IntString)));
      } else if (ParamTypeString == "d") {
        std::string DoubleString;
        std::cin >> DoubleString;
        ArgMapping.emplace(
            V, QuantumOpParam(OpParamTy::DOUBLE, std::stod(DoubleString)));
      } else if (ParamTypeString == "p") {
        std::string DoubleString;
        std::cin >> DoubleString;
        ArgMapping.emplace(
            V, QuantumOpParam(OpParamTy::GATEPARAM, std::stod(DoubleString)));
      }
    }
  }

  if (ToMoveAfter) {
    if (Copy)
      ToMove->copyGate(*ToMoveAfter, ArgMapping);
    else
      errs() << ToMove->moveGate(*ToMoveAfter, ArgMapping) << "\n";
  } else {
    if (Copy)
      ToMove->copyGate(NewQC, ArgMapping);
    else
      ToMove->moveGate(NewQC, ArgMapping);
  }
}

void processInputToInsertCircuit(std::vector<QuantumCircuit *> &QCList,
                                 QuantumModule &QM, bool CopyCircuit = false) {
  std::string CircuitIdxStr;
  unsigned CircuitIdx;
  std::cout << "Quantum Circuits: \n";
  for (unsigned Idx = 0; Idx < QCList.size(); Idx++) {
    std::cout << Idx << " " << QCList[Idx]->getBaseValue()->getName().str()
              << "\n";
    unsigned SecondLevelIdx = 0;
    for (QuantumCircuit *SecondQC : QCList[Idx]->getCircuitList()) {
      std::cout
          << "  " << Idx << "-" << SecondLevelIdx << ": "
          << dyn_cast<BasicBlock>(SecondQC->getBaseValue())->getName().str()
          << ":\n";
      SecondLevelIdx++;
    }
  }

  std::string CopyOrMove = CopyCircuit ? "copy" : "insert";
  std::cout << "\nPick circuit to " << CopyOrMove << ": ";
  std::cin >> CircuitIdxStr;

  CircuitIdx = std::stoi(CircuitIdxStr);

  QuantumCircuit *ToInsert = QCList[CircuitIdx];

  std::cout << "\nPick circuit to " << CopyOrMove << " after, or end of: ";
  std::cin >> CircuitIdxStr;

  QuantumCircuit *InsertInto = nullptr;
  QuantumCircuit *InsertAfter = nullptr;
  size_t Loc = CircuitIdxStr.find("-");
  if (Loc == std::string::npos) {
    int First = std::stoi(CircuitIdxStr);
    InsertInto = QCList[First];
  } else {
    int First = std::stoi(CircuitIdxStr.substr(0, Loc));
    int Second = std::stoi(CircuitIdxStr.substr(Loc + 1));
    InsertAfter = QCList[First]->getCircuitList()[Second];
    InsertInto = QCList[First];
  }

  std::map<unsigned, QuantumOpParam> Mapping;
  if (ToInsert->getOpParams().size() > 0) {
    std::cout << "Handle Parameters: ";
    unsigned ArgIdx = 0;
    for (QuantumOpParam &QOP : ToInsert->getOpParams()) {
      switch (QOP.ParamType) {
      case OpParamTy::QUBITPTR: {
        std::map<int, QbitRef> QubitMap;
        unsigned Idx = 0;
        for (const QbitRef &QR : InsertInto->getQubits()) {
          QubitMap.insert(std::make_pair(Idx++, QR));
        }
        std::cout << "\nQubits:\n";
        for (std::pair<int, QbitRef> QubitInt : QubitMap) {
          std::cout << "\t" << QubitInt.first << ": "
                    << getPrettyName(QubitInt.second) << "\n";
        }
        std::string QubitString;
        std::cout << "\nSelect which qubit to use:\n";
        std::cin >> QubitString;
        int ToUse = std::stoi(QubitString);
        Mapping.emplace(Idx,
                        QuantumOpParam(OpParamTy::QUBITPTR, QubitMap[ToUse]));
        break;
      }
      case OpParamTy::GATEPARAM: {
        std::vector<QuantumOp *> QOs;
        bool FoundCircuit = false;
        for (QuantumCircuitOp *QCO : *InsertInto) {
          if (QCO->getKind() == OpKind::Quantum) {
            QOs.push_back(static_cast<QuantumOp *>(QCO));
            std::cout << QOs.size() << ": " << QCO->printPretty() << "\n";
          }
          if (QCO->getKind() == OpKind::Start) {
            if (QCO->getParentCircuit() == InsertAfter) {
              FoundCircuit = true;
            } else if (FoundCircuit) {
              break;
            }
          }
        }
        std::string OpID;
        std::cout << "Select Op with param: ";
        std::cin >> OpID;
        unsigned OpIDVal = std::stoi(OpID);

        QuantumOp *QO = QOs[OpIDVal];
        unsigned CurrentIdx = 0;
        for (std::pair<ParaRef, unsigned> &P : QO->getResolvedParams()) {
          std::cout << CurrentIdx++ << " " << getPrettyName(P.first) << "\n";
        }

        std::cout << "Select Param: ";
        std::cin >> OpID;
        OpIDVal = std::stoi(OpID);
        ParaRef P = QO->getResolvedParams()[OpIDVal].first;
        Mapping.emplace(ArgIdx, QuantumOpParam(OpParamTy::GATEPARAM, P));
        break;
      }
      case OpParamTy::DOUBLE: {
        std::string DoubleString;
        std::cin >> DoubleString;
        Mapping.emplace(
            ArgIdx, QuantumOpParam(OpParamTy::DOUBLE, std::stod(DoubleString)));
        break;
      }
      case OpParamTy::INT: {
        std::string IntString;
        std::cin >> IntString;
        Mapping.emplace(ArgIdx,
                        QuantumOpParam(OpParamTy::INT, std::stoi(IntString)));
        break;
      }
      }
      ArgIdx++;
    }
  }

  if (CopyCircuit)
    InsertInto->copyAndInsertCircuit(ToInsert, InsertAfter, Mapping);
  else
    InsertInto->insertCircuit(ToInsert, InsertAfter, Mapping);
}

void processInputToAddQubit(QuantumCircuit *QC, QuantumModule &QM) {
  std::string ExpandExisting;
  std::cout << "Add new qubit for existing qubit register? (y/n): ";
  std::cin >> ExpandExisting;

  while (ExpandExisting == "y") {
    std::map<int, QbitRef> QubitMap;
    unsigned Idx = 0;
    for (const QbitRef &QR : QC->getQubits()) {
      QubitMap.insert(std::make_pair(Idx++, QR));
    }
    std::cout << "\nQubits:\n";
    for (std::pair<int, QbitRef> QubitInt : QubitMap) {
      std::cout << "\t" << QubitInt.first << ": "
                << getPrettyName(QubitInt.second) << "\n";
    }
    std::cout << "Get qubit in register holding: ";
    std::string QubitString;
    std::cin >> QubitString;
    int ToUse = std::stoi(QubitString);

    std::cout << "New Index: ";
    std::string NewIndex;
    std::cin >> NewIndex;
    int NewIndexInt = std::stoi(NewIndex);
    QC->getQubitInRegister(QubitMap[ToUse], NewIndexInt);

    std::cout << "keep adding? (y/n): ";
    std::cin >> ExpandExisting;
  }

  std::cout << "Add local or global? (y/n): ";
  std::cin >> ExpandExisting;
  if (ExpandExisting == "n")
    return;

  std::cout << "Local (l) Register of Qubits or Global (g): ";
  std::string LocalOrGlobal;
  std::cin >> LocalOrGlobal;

  std::cout << "Number of qubits: ";
  std::string NumQubitsStr;
  std::cin >> NumQubitsStr;
  unsigned NumQubits = std::stoi(NumQubitsStr);

  std::cout << "Name of Register: ";
  std::string Name;
  std::cin >> Name;

  std::vector<QbitRef> NewQubits;
  if (LocalOrGlobal == "l")
    QC->createLocalQubitRegister(NumQubits, NewQubits, Name);
  else
    QC->createGlobalQubitRegister(NumQubits, NewQubits, Name);
}

QuantumCircuit *createNewCircuit(QuantumModule &QM) {
  std::cout << "Parent (p) or subcircuit (s): ";
  std::string ParentOrSub;
  std::cin >> ParentOrSub;

  std::cout << "Name of circuit: ";
  std::string Name;
  std::cin >> Name;

  std::vector<QuantumOpParam> Args;
  if (ParentOrSub == "p") {
    bool AddParam = true;
    std::string ParamType;
    while (AddParam) {
      std::cout << "Add parameter? Qubit pointer (q), integer (i), double (d), "
                   "classical parameter (p) or end (n): ";
      std::cin >> ParamType;

      if (ParamType == "n")
        AddParam = false;
      if (ParamType == "q")
        Args.emplace_back(OpParamTy::QUBITPTR);
      else if (ParamType == "d")
        Args.emplace_back(OpParamTy::DOUBLE);
      else if (ParamType == "i")
        Args.emplace_back(OpParamTy::INT);
      else if (ParamType == "p")
        Args.emplace_back(OpParamTy::GATEPARAM);
    }

    return QuantumCircuit::Create(QM, Args, Name);
  }

  return QuantumCircuit::Create(QM, Name);
}

int main(int argc, const char *argv[]) {
  InitLLVM X(argc, argv);

  cl::ParseCommandLineOptions(argc, argv, "Intel Quantum Circuit Processor\n");

  LLVMContext CurrContext;
  SMDiagnostic Err;
  std::unique_ptr<Module> ModuleToAnalyze =
      parseIRFile(InputSourceFile, Err, CurrContext);
  if (!ModuleToAnalyze) {
    Err.print(argv[0], errs());
    return -1;
  }

  // Intialize the module and determine the stage.
  QuantumCompilerLinkage QCL;
  QuantumSpinNativeToJson QSNJ;
  QuantumModule QM;
  QuantumAnnotationsToJson QAJ;
  QAJ.doInitialization(*ModuleToAnalyze, QM);
  QSNJ.addSpinNativeFunctions(*ModuleToAnalyze, QM);
  try {
    llvm::updateCompilerLinkage(*ModuleToAnalyze, QM, true);
  } catch (...) {
    displayErrorAndExit("Intel Quantum Circuit Processor",
                        "Unable to parse JSON files or Module passed in.");
  }

  std::error_code EC;
  std::unique_ptr<ToolOutputFile> Out(
      new ToolOutputFile("-", EC, sys::fs::OF_None));
  if (EC) {
    errs() << argv[0] << ": " << EC.message() << '\n';
    return -1;
  }

  QuantumCircuit *QC = nullptr;

  std::vector<QuantumCircuit *> CircuitList;
  for (QuantumKernel &QK : QM) {
    QC = QuantumCircuit::Create(QM, QK);
    if (QC->getNumberGates() > 0)
      CircuitList.push_back(QC);
    else
      delete QC;
  }

  // This block just outputs the circuits in different forms. Mostly
  // just for debugging.
  for (QuantumCircuit *QC : CircuitList) {
    errs() << "---------------------------------------\n";
    errs() << "New Function: ";
    std::string CalledName = QC->getBaseValue()->getName().str();
    std::string::size_type StubLoc = CalledName.find(".stub");
    if (StubLoc != std::string::npos) {
      errs() << demangle(CalledName.substr(0, StubLoc)) << "\n";
    } else
      errs() << demangle(CalledName) << "\n";

    errs() << "\n\n---------------\n";
    errs() << "As Iterator\n";
    for (QuantumCircuitOp *QCO : *QC) {
      errs() << static_cast<QuantumOp *>(QCO)->printPretty() << "\n";
    }

    errs() << "\n\n\n---------------\n";
    errs() << "As a complete list\n";
    std::vector<QuantumCircuitOp *> QCOs;
    QC->iterateGates(QCOs);
    errs() << QCOs.size() << "\n";
    for (QuantumCircuitOp *QCO : QCOs) {
      errs() << static_cast<QuantumOp *>(QCO)->printPretty() << "\n";
    }

    errs() << "\n\n\n---------------\n";
    errs() << "As Time Slice Iterator\n";
    unsigned TimeSliceIdx = 0;
    for (QuantumCircuit::slice_iterator SLIt = QC->begin_slice();
         SLIt != QC->end_slice(); SLIt++) {
      errs() << "-----";
      errs() << "Slice " << TimeSliceIdx++ << ":\n";
      for (QuantumCircuitOp *QCO : (*SLIt).Ops) {
        errs() << static_cast<QuantumOp *>(QCO)->printPretty() << "\n";
      }
    }

    errs() << "\n\n\n---------------\n";
    errs() << "As Time Slice List\n";
    TimeSliceIdx = 0;
    std::vector<QuantumCircuitSlice> QCSs;
    QC->iterateGatesTimeSlices(QCSs);
    for (QuantumCircuitSlice &Slice : QCSs) {
      errs() << "-----";
      errs() << "Slice " << TimeSliceIdx++ << ":\n";
      for (QuantumCircuitOp *QCO : Slice.Ops) {
        errs() << static_cast<QuantumOp *>(QCO)->printPretty() << "\n";
      }
    }
  }

  // Continuous loop to look at and edit quantum circuit.
  bool Editing = true;
  while (Editing) {
    std::string Response;
    std::cout << "Edit circuit (e), move circuit (m), copy circuit (c), print "
                 "circuit (p), output circuit (o), add circuit (a) or end "
                 "program (n)? (e/m/p/o/a/n): ";
    std::cin >> Response;
    std::cout << "\n";

    if ("n" == Response) {
      // Change the state of the loop.
      Editing = false;
      continue;
    } else if ("o" == Response) {
      // Get the path for the file, create a stream to output it to, and do so.
      // Exit if something fails.
      std::cout << "File path to output to: ";
      std::string NewFileName;
      std::cin >> NewFileName;
      raw_fd_ostream outputFile(NewFileName, EC, sys::fs::OF_None);
      if (EC) {
        errs() << "Error opening file for writing: " << EC.message() << "\n";
        return 1;
      }

      ModuleToAnalyze->print(outputFile, nullptr);
      continue;
    } else if ("a" == Response) {
      QuantumCircuit *NewQC = createNewCircuit(QM);
      CircuitList.push_back(NewQC);
      continue;
    } else if ("c" == Response) {
      processInputToInsertCircuit(CircuitList, QM, true);
      continue;
    } else if ("m" == Response) {
      processInputToInsertCircuit(CircuitList, QM);
      continue;
    }

    QuantumCircuit *QC = pickCircuitToChange(CircuitList);
    if ("p" == Response) {
      for (QuantumCircuitOp *QCO : *QC) {
        errs() << static_cast<QuantumOp *>(QCO)->printPretty() << "\n";
      }
      errs() << "As IR: \n";
      llvm::raw_fd_ostream os(1, false);
      os << *QC->getBaseValue() << "\n";
      continue;
    }
    std::cout << "Add (a), Move (m), Copy (c), Add Qubits (q) or Delete (d): ";
    std::cin >> Response;
    bool AddOrDelete = Response == "a";
    if (Response == "a")
      processInputToAddGate(QC, QM);
    else if (Response == "m")
      processInputToMoveGate(QC, CircuitList, QM);
    else if (Response == "c")
      processInputToMoveGate(QC, CircuitList, QM, true);
    else if (Response == "q")
      processInputToAddQubit(QC, QM);
    else
      deleteOp(QC);
  }

  for (QuantumCircuit *QC : CircuitList) {
    delete QC;
  }
}