//===- QuantumCircuitTest.cpp -----------------------------------*- C++ -*-===//
//
//===----------------------------------------------------------------------===//
// INTEL CONFIDENTIAL
//
// Copyright 2024 Intel Corporation.
//
// This software and the related documents are Intel copyrighted materials, and
// your use of them is governed by the express license under which they were
// provided to you ("License"). Unless the License provides otherwise, you may
// not use, modify, copy, publish, distribute, disclose or transmit this
// software or the related documents without Intel's prior written permission.
//
// This software and the related documents are provided as is, with no express
// or implied warranties, other than those that are expressly stated in the
// License.
//===----------------------------------------------------------------------===//
/// \file "QuantumCircuitTest.cpp"
///
/// This file provides a basic unit test to verify the functionality of the
/// Quantum Circuit Object.
///
//===----------------------------------------------------------------------===//

#include "IntelQuantumPasses/QuantumAnalysis/QuantumAnnotationsToJson.h"
#include "IntelQuantumPasses/QuantumAnalysis/QuantumCompilerLinkage.h"
#include "IntelQuantumPasses/QuantumAnalysis/QuantumGateIdentifiers.h"
#include "IntelQuantumPasses/QuantumAnalysis/QuantumIterator.h"
#include "IntelQuantumPasses/QuantumAnalysis/QuantumRefs.h"
#include "IntelQuantumPasses/QuantumAnalysis/QuantumSpinNativeToJson.h"
#include "IntelQuantumPasses/QuantumUtils/QuantumCircuit.h"

#include "llvm/AsmParser/Parser.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRPrintingPasses.h"
#include "llvm/IR/InstIterator.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/PassManager.h"
#include "llvm/IRPrinter/IRPrintingPasses.h"
#include "llvm/IRReader/IRReader.h"
#include "llvm/InitializePasses.h"
#include "llvm/Pass.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/SourceMgr.h"
#include "llvm/Support/raw_ostream.h"

#include "gtest/gtest.h"

#include "QuantumPasses/QuantumUtils/QuantumCircuitTest.h"

#include <map>

using namespace llvm;
using namespace aqcc;

static std::unique_ptr<Module> parseIR(LLVMContext &C, const char *IR) {
  SMDiagnostic Err;
  std::unique_ptr<Module> Mod = parseAssemblyString(IR, Err, C);
  if (!Mod)
    Err.print("QuantumCircuitTest", errs());
  return Mod;
}

static void runInitialPasses(Module &M, QuantumModule &QM) {
  QuantumCompilerLinkage QCL;
  QCL.doInitialization(M, QM);
  QCL.locateOrDeclareQRTFunctions(M, QM);
  QM.setSure();
  QCL.checkForConditioning(M, QM);
  QCL.checkForSeparation(M, QM);
  QCL.findMaxQSDAUsage(M, QM);
  QuantumAnnotationsToJson QAJ;
  QAJ.doInitialization(M, QM);

  QuantumSpinNativeToJson SNTJ = QuantumSpinNativeToJson();
  SNTJ.addSpinNativeFunctions(M, QM);
  updateCompilerLinkage(M, QM);

  QuantumCircuit::getGlobalQubits(QM).clear();
}

// Get the function value from the module if it exists as a quantum kernel.
static QuantumKernel *FindKernelInModule(QuantumModule &QM,
                                         std::string FuncName) {
  for (QuantumKernel &QK : QM) {
    if (QK.getFunction()->getName().str() == FuncName) {
      return &QK;
    }
  }
  return nullptr;
}

// Using the basic block name, extract the basic block value from the function
// if it exists.
static BasicBlock *FindBlockInFunction(Function &F, std::string BBName) {
  for (BasicBlock &BB : F) {
    if (BB.getName().str() == BBName) {
      return &BB;
    }
  }
  return nullptr;
}

/// Test creating a new local allocation of qubits for a function.
TEST(QuantumCircuitTest, CreateNewLocalQubitRegister) {
  LLVMContext C;
  std::unique_ptr<Module> M = parseIR(C, *&Code_Generated);
  ParaRef::setModule(*M);

  QuantumModule QM;
  runInitialPasses(*M, QM);

  QuantumKernel *EmptyFunction =
      FindKernelInModule(QM, "_Z12empty_kernelv.stub");
  BasicBlock *EmptyBB =
      FindBlockInFunction(*(EmptyFunction->getFunction()), "entry");

  QuantumCircuit &QC = *QuantumCircuit::Create(QM, *EmptyBB);
  EXPECT_EQ(QC.getNumberGates(), 0);
  EXPECT_EQ(QC.getQubits().size(), 0);

  std::vector<QbitRef> QRs;
  QC.createLocalQubitRegister(10, QRs, "test_reg");
  EXPECT_EQ(QRs.size(), 10);
  Instruction *I = &*EmptyBB->begin();
  AllocaInst *AI = dyn_cast<AllocaInst>(I);
  EXPECT_NE(AI, nullptr);

  EXPECT_EQ(QC.getQubits().size(), 10);
}

/// Test creating a new global allocation of qubits for the entire module.
TEST(QuantumCircuitTest, CreateNewGlobalQubitRegister) {
  LLVMContext C;
  std::unique_ptr<Module> M = parseIR(C, *&Code_Generated);
  ParaRef::setModule(*M);

  QuantumModule QM;
  runInitialPasses(*M, QM);

  QuantumKernel *EmptyFunction =
      FindKernelInModule(QM, "_Z12empty_kernelv.stub");
  BasicBlock *EmptyBB =
      FindBlockInFunction(*(EmptyFunction->getFunction()), "entry");

  QuantumCircuit &QC = *QuantumCircuit::Create(QM, *EmptyBB);
  EXPECT_EQ(QC.getNumberGates(), 0);
  EXPECT_EQ(QC.getQubits().size(), 0);
  EXPECT_EQ(QC.getGlobalQubits().size(), 3);

  std::vector<QbitRef> QRs;
  QC.createGlobalQubitRegister(10, QRs, "test_reg");
  EXPECT_EQ(QRs.size(), 10);
  EXPECT_EQ(QC.getQubits().size(), 10);
  EXPECT_EQ(QC.getGlobalQubits().size(), 13);
}

/// Test adding a new index to an existing qbitref.
TEST(QuantumCircuitTest, CreateNewQbitRefIndex) {
  LLVMContext C;
  std::unique_ptr<Module> M = parseIR(C, *&Code_Generated);
  ParaRef::setModule(*M);

  QuantumModule QM;
  runInitialPasses(*M, QM);

  QuantumKernel *EmptyFunction =
      FindKernelInModule(QM, "_Z12empty_kernelv.stub");
  BasicBlock *EmptyBB =
      FindBlockInFunction(*(EmptyFunction->getFunction()), "entry");

  QuantumCircuit &QC = *QuantumCircuit::Create(QM, *EmptyBB);

  std::string S = "QubitReg";
  StringRef SR(S);
  GlobalValue *GV = M->getNamedValue(SR);
  ASSERT_NE(GV, nullptr);
  QbitRef NewQ(GV, (unsigned)0);

  QbitRef NewReg = QC.getQubitInRegister(NewQ, 3);
  ASSERT_TRUE(NewReg.isNull());

  NewReg = QC.getQubitInRegister(NewQ, 1);
  ASSERT_FALSE(NewReg.isNull());

  EXPECT_EQ(QC.getQubits().size(), 1);
  EXPECT_EQ(QC.getGlobalQubits().size(), 1);
}

/// Test creating a new circuit representing an empty kernel.
TEST(QuantumCircuitTest, CreateNewCircuitQK) {
  LLVMContext C;
  std::unique_ptr<Module> M = parseIR(C, *&Code_Generated);
  ParaRef::setModule(*M);

  QuantumModule QM;
  runInitialPasses(*M, QM);

  QuantumCircuit *QC = QuantumCircuit::Create(QM, "test_circuit_1", true);
  EXPECT_TRUE(isa<Function>(QC->getBaseValue()));
  EXPECT_EQ(QC->getCircuitList().size(), 1);
  EXPECT_EQ(QC->getQubits().size(), 0);
}

/// Test creating a new circuit representing an empty kernel, but with
/// arguments.
TEST(QuantumCircuitTest, CreateNewCircuitQKArgs) {
  LLVMContext C;
  std::unique_ptr<Module> M = parseIR(C, *&Code_Generated);
  ParaRef::setModule(*M);

  QuantumModule QM;
  runInitialPasses(*M, QM);

  std::vector<QuantumOpParam> Args;
  Args.emplace_back(OpParamTy::QUBITPTR);
  Args.emplace_back(OpParamTy::GATEPARAM);
  QuantumCircuit *QC = QuantumCircuit::Create(QM, Args, "test_circuit_2");
  EXPECT_TRUE(isa<Function>(QC->getBaseValue()));
  EXPECT_EQ(QC->getCircuitList().size(), 1);
  EXPECT_EQ(QC->getQubits().size(), 1);

  EXPECT_EQ(QC->getGlobalQubits().size(), 3);
}

/// Test adding a gate into a circuit based on new basic block.
TEST(QuantumCircuitTest, CreateNewCircuitQBB) {
  LLVMContext C;
  std::unique_ptr<Module> M = parseIR(C, *&Code_Generated);
  ParaRef::setModule(*M);

  QuantumModule QM;
  runInitialPasses(*M, QM);

  QuantumCircuit *QC = QuantumCircuit::Create(QM, "test_circuit", false);
  EXPECT_TRUE(isa<BasicBlock>(QC->getBaseValue()));
  EXPECT_EQ(QC->getCircuitList().size(), 0);
  EXPECT_EQ(QC->getQubits().size(), 0);

  EXPECT_EQ(QC->getGlobalQubits().size(), 3);
}

/// Test adding a new local allocation of qubits and a gate on a qubit from that
/// allocation in a new QBB circuit.
TEST(QuantumCircuitTest, AddQubitGateNewQBB) {
  LLVMContext C;
  std::unique_ptr<Module> M = parseIR(C, *&Code_Generated);
  ParaRef::setModule(*M);

  QuantumModule QM;
  runInitialPasses(*M, QM);

  QuantumCircuit *QC = QuantumCircuit::Create(QM, "test_circuit", false);
  EXPECT_TRUE(isa<BasicBlock>(QC->getBaseValue()));
  EXPECT_EQ(QC->getCircuitList().size(), 0);
  EXPECT_EQ(QC->getQubits().size(), 0);

  std::vector<QbitRef> QRs;
  QC->createLocalQubitRegister(10, QRs, "test_reg");
  EXPECT_EQ(QRs.size(), 10);
  BasicBlock *EmptyBB = dyn_cast<BasicBlock>(QC->getBaseValue());
  Instruction *I = &*EmptyBB->begin();
  AllocaInst *AI = dyn_cast<AllocaInst>(I);
  EXPECT_NE(AI, nullptr);

  EXPECT_EQ(QC->getQubits().size(), 10);
  std::vector<QbitRef> Qbits;
  Qbits.push_back(QRs[5]);
  std::vector<ParaRef> Params = {2.0};
  QuantumOp *RXGate = QuantumOp::Create("RX", QM, Qbits, Params);
  EXPECT_TRUE(QC->appendGate(RXGate));
  EXPECT_EQ(QC->getNumberGates(), 1);

  EXPECT_EQ(dyn_cast<Instruction>(RXGate->getBaseValue())->getParent(),
            EmptyBB);
}

/// Test appending a qubit and a gate to a circuit representing a new quantum
/// kernel.
TEST(QuantumCircuitTest, AddQubitGateNewQK) {
  LLVMContext C;
  std::unique_ptr<Module> M = parseIR(C, *&Code_Generated);
  ParaRef::setModule(*M);

  QuantumModule QM;
  runInitialPasses(*M, QM);

  QuantumCircuit *QC = QuantumCircuit::Create(QM, "test_circuit_1", true);
  EXPECT_TRUE(isa<Function>(QC->getBaseValue()));
  EXPECT_EQ(QC->getCircuitList().size(), 1);
  EXPECT_EQ(QC->getQubits().size(), 0);

  std::vector<QbitRef> QRs;
  QC->createLocalQubitRegister(10, QRs, "test_reg");
  EXPECT_EQ(QRs.size(), 10);
  BasicBlock *EmptyBB =
      &dyn_cast<Function>(QC->getBaseValue())->getEntryBlock();
  Instruction *I = &*EmptyBB->begin();
  AllocaInst *AI = dyn_cast<AllocaInst>(I);
  EXPECT_NE(AI, nullptr);

  std::vector<QbitRef> Qbits;
  Qbits.push_back(QRs[5]);
  std::vector<ParaRef> Params = {2.0};
  QuantumOp *RXGate = QuantumOp::Create("RX", QM, Qbits, Params);
  EXPECT_TRUE(QC->getCircuitList()[0]->appendGate(RXGate));
  EXPECT_EQ(QC->getNumberGates(), 1);
  EXPECT_EQ(dyn_cast<Instruction>(RXGate->getBaseValue())->getParent(),
            EmptyBB);
}

/// Test inserting a circuit representing a quantum kernel into the end of a
/// different circuit.
TEST(QuantumCircuitTest, InsertNewQKCircuitEnd) {
  LLVMContext C;
  std::unique_ptr<Module> M = parseIR(C, *&Code_Generated);
  ParaRef::setModule(*M);

  QuantumModule QM;
  runInitialPasses(*M, QM);

  QuantumKernel *FuncToInsert =
      FindKernelInModule(QM, "_Z16basic_line_limitv.stub");

  QuantumCircuit &InsertQC = *QuantumCircuit::Create(QM, *FuncToInsert);

  QuantumKernel *NewFunction =
      FindKernelInModule(QM, "_Z18kernel_with_doubled.stub");
  BasicBlock *LastBlock =
      FindBlockInFunction(*(NewFunction->getFunction()), "aqcc.meas.move.end");

  QuantumCircuit &InsertIntoQC = *QuantumCircuit::Create(QM, *NewFunction);
  EXPECT_TRUE(InsertIntoQC.insertCircuit(&InsertQC));

  EXPECT_EQ(InsertIntoQC.getCircuitList().size(), 4);
  EXPECT_EQ(InsertQC.getCircuitList().size(), 0);
  EXPECT_EQ(InsertIntoQC.getCircuitList().size(), 4);
  EXPECT_EQ(InsertIntoQC.getNumberGates(), 4);

  QuantumCircuit *LastQC = InsertIntoQC.getCircuitList()[2];
  QuantumCircuit *AddedQC = InsertIntoQC.getCircuitList()[3];
  BasicBlock *AddedBlock = dyn_cast<BasicBlock>(AddedQC->getBaseValue());
  EXPECT_EQ(AddedBlock->getSinglePredecessor(), LastBlock);
  BranchInst *BI = dyn_cast<BranchInst>(LastBlock->getTerminator());
  ASSERT_NE(BI, nullptr);
  EXPECT_EQ(BI->getSuccessor(0), AddedBlock);

  EXPECT_NE(LastQC->getEndOp()->getSuccessors().find(AddedQC->getStartOp()),
            LastQC->getEndOp()->getSuccessors().end());
  EXPECT_EQ(LastQC->getEndOp()->getSuccessors().find(InsertIntoQC.getEndOp()),
            LastQC->getEndOp()->getSuccessors().end());
}

/// Test inserting a circuit representing a quantum kernel into the end of a
/// different circuit where there are many predecessors to the end block.
TEST(QuantumCircuitTest, InsertNewQKCircuitManyBlocksEnd) {
  LLVMContext C;
  std::unique_ptr<Module> M = parseIR(C, *&Code_Generated);
  ParaRef::setModule(*M);

  QuantumModule QM;
  runInitialPasses(*M, QM);

  QuantumKernel *FuncToInsert =
      FindKernelInModule(QM, "_Z18kernel_with_no_args.stub");

  QuantumCircuit &InsertQC = *QuantumCircuit::Create(QM, *FuncToInsert);

  QuantumKernel *NewFunction =
      FindKernelInModule(QM, "_Z18kernel_with_doubled.stub");
  BasicBlock *LastBlock =
      FindBlockInFunction(*(NewFunction->getFunction()), "aqcc.meas.move.end");

  QuantumCircuit &InsertIntoQC = *QuantumCircuit::Create(QM, *NewFunction);
  EXPECT_TRUE(InsertIntoQC.insertCircuit(&InsertQC));

  EXPECT_EQ(InsertQC.getCircuitList().size(), 0);
  EXPECT_EQ(InsertIntoQC.getCircuitList().size(), 6);
  EXPECT_EQ(InsertIntoQC.getNumberGates(), 2);

  QuantumCircuit *LastQC = InsertIntoQC.getCircuitList()[2];
  QuantumCircuit *AddedQC = InsertIntoQC.getCircuitList()[3];
  BasicBlock *AddedBlock = dyn_cast<BasicBlock>(AddedQC->getBaseValue());
  EXPECT_EQ(AddedBlock->getSinglePredecessor(), LastBlock);
  BranchInst *BI = dyn_cast<BranchInst>(LastBlock->getTerminator());
  ASSERT_NE(BI, nullptr);
  EXPECT_EQ(BI->getSuccessor(0), AddedBlock);

  EXPECT_NE(LastQC->getEndOp()->getSuccessors().find(AddedQC->getStartOp()),
            LastQC->getEndOp()->getSuccessors().end());
  EXPECT_EQ(LastQC->getEndOp()->getSuccessors().find(InsertIntoQC.getEndOp()),
            LastQC->getEndOp()->getSuccessors().end());
}

/// Test inserting a circuit representing a QBB into the end of a
/// different circuit.
TEST(QuantumCircuitTest, InsertNewQBBCircuitEnd) {
  LLVMContext C;
  std::unique_ptr<Module> M = parseIR(C, *&Code_Generated);
  ParaRef::setModule(*M);

  QuantumModule QM;
  runInitialPasses(*M, QM);

  QuantumKernel *FuncToInsert =
      FindKernelInModule(QM, "_Z16basic_line_limitv.stub");
  BasicBlock *BB = FindBlockInFunction(*(FuncToInsert->getFunction()), "entry");
  BB->removeFromParent();

  QuantumCircuit &InsertQC = *QuantumCircuit::Create(QM, *BB);

  QuantumKernel *NewFunction =
      FindKernelInModule(QM, "_Z18kernel_with_doubled.stub");
  BasicBlock *LastBlock =
      FindBlockInFunction(*(NewFunction->getFunction()), "aqcc.meas.move.end");

  QuantumCircuit &InsertIntoQC = *QuantumCircuit::Create(QM, *NewFunction);
  EXPECT_TRUE(InsertIntoQC.insertCircuit(&InsertQC));
  EXPECT_EQ(InsertIntoQC.getCircuitList().size(), 4);
  EXPECT_EQ(InsertQC.getCircuitList().size(), 0);
  EXPECT_EQ(InsertIntoQC.getCircuitList().size(), 4);
  EXPECT_EQ(InsertIntoQC.getNumberGates(), 4);

  QuantumCircuit *LastQC = InsertIntoQC.getCircuitList()[2];
  QuantumCircuit *AddedQC = InsertIntoQC.getCircuitList()[3];
  BasicBlock *AddedBlock = dyn_cast<BasicBlock>(AddedQC->getBaseValue());
  EXPECT_EQ(AddedBlock->getSinglePredecessor(), LastBlock);
  BranchInst *BI = dyn_cast<BranchInst>(LastBlock->getTerminator());
  ASSERT_NE(BI, nullptr);
  EXPECT_EQ(BI->getSuccessor(0), AddedBlock);

  EXPECT_NE(LastQC->getEndOp()->getSuccessors().find(AddedQC->getStartOp()),
            LastQC->getEndOp()->getSuccessors().end());
  EXPECT_EQ(LastQC->getEndOp()->getSuccessors().find(InsertIntoQC.getEndOp()),
            LastQC->getEndOp()->getSuccessors().end());
}

/// Test failing to add a circuit representing a QBB already in a circuit into
/// the end of a different circuit.
TEST(QuantumCircuitTest, InsertExistingQBBCircuitEnd) {
  LLVMContext C;
  std::unique_ptr<Module> M = parseIR(C, *&Code_Generated);
  ParaRef::setModule(*M);

  QuantumModule QM;
  runInitialPasses(*M, QM);

  QuantumKernel *FuncToInsert =
      FindKernelInModule(QM, "_Z16basic_line_limitv.stub");
  BasicBlock *BB = FindBlockInFunction(*(FuncToInsert->getFunction()), "entry");

  QuantumCircuit &InsertQC = *QuantumCircuit::Create(QM, *BB);

  QuantumKernel *NewFunction =
      FindKernelInModule(QM, "_Z18kernel_with_doubled.stub");

  QuantumCircuit &InsertIntoQC = *QuantumCircuit::Create(QM, *NewFunction);
  EXPECT_FALSE(InsertIntoQC.insertCircuit(&InsertQC));
}

/// Test inserting a circuit representing a quantum kernel after another
/// circuit.
TEST(QuantumCircuitTest, InsertNewQKCircuitAfter) {
  LLVMContext C;
  std::unique_ptr<Module> M = parseIR(C, *&Code_Generated);
  ParaRef::setModule(*M);

  QuantumModule QM;
  runInitialPasses(*M, QM);

  QuantumKernel *FuncToInsert =
      FindKernelInModule(QM, "_Z18kernel_with_no_args.stub");

  QuantumCircuit &InsertQC = *QuantumCircuit::Create(QM, *FuncToInsert);

  QuantumKernel *NewFunction =
      FindKernelInModule(QM, "_Z18kernel_with_doubled.stub");
  BasicBlock *BlockAfter =
      FindBlockInFunction(*(NewFunction->getFunction()), "aqcc.meas.move.end");
  BasicBlock *BlockBefore =
      FindBlockInFunction(*(NewFunction->getFunction()), "aqcc.quantum");

  QuantumCircuit &InsertIntoQC = *QuantumCircuit::Create(QM, *NewFunction);
  EXPECT_TRUE(
      InsertIntoQC.insertCircuit(&InsertQC, InsertIntoQC.getCircuitList()[1]));

  EXPECT_EQ(InsertQC.getCircuitList().size(), 0);
  EXPECT_EQ(InsertIntoQC.getCircuitList().size(), 6);
  EXPECT_EQ(InsertIntoQC.getNumberGates(), 2);

  QuantumCircuit *PrevQC = InsertIntoQC.getCircuitList()[1];
  QuantumCircuit *AfterQC = InsertIntoQC.getCircuitList()[2];
  QuantumCircuit *AddedQCBegin = InsertIntoQC.getCircuitList()[3];
  QuantumCircuit *AddedQCEnd = InsertIntoQC.getCircuitList()[5];
  BasicBlock *AddedBlockBegin =
      dyn_cast<BasicBlock>(AddedQCBegin->getBaseValue());
  BasicBlock *AddedBlockEnd = dyn_cast<BasicBlock>(AddedQCEnd->getBaseValue());
  EXPECT_EQ(AddedBlockBegin->getSinglePredecessor(), BlockBefore);
  BranchInst *BI = dyn_cast<BranchInst>(BlockBefore->getTerminator());
  ASSERT_NE(BI, nullptr);
  EXPECT_EQ(BI->getSuccessor(0), AddedBlockBegin);

  EXPECT_EQ(BlockAfter->getSinglePredecessor(), AddedBlockEnd);
  BI = dyn_cast<BranchInst>(AddedBlockEnd->getTerminator());
  ASSERT_NE(BI, nullptr);
  EXPECT_EQ(BI->getSuccessor(0), BlockAfter);

  EXPECT_NE(
      PrevQC->getEndOp()->getSuccessors().find(AddedQCBegin->getStartOp()),
      PrevQC->getEndOp()->getSuccessors().end());
  EXPECT_EQ(PrevQC->getEndOp()->getSuccessors().find(AfterQC->getStartOp()),
            PrevQC->getEndOp()->getSuccessors().end());

  EXPECT_NE(AddedQCEnd->getEndOp()->getSuccessors().find(AfterQC->getStartOp()),
            AddedQCEnd->getEndOp()->getSuccessors().end());
}

/// Test inserting a circuit representing a quantum kernel after another
/// circuit that has multiple successors.
TEST(QuantumCircuitTest, InsertNewQKCircuitAfterMultiBranch) {
  LLVMContext C;
  std::unique_ptr<Module> M = parseIR(C, *&Code_Generated);
  ParaRef::setModule(*M);

  QuantumModule QM;
  runInitialPasses(*M, QM);

  QuantumKernel *FuncToInsert =
      FindKernelInModule(QM, "_Z18kernel_with_no_args.stub");

  QuantumCircuit &InsertQC = *QuantumCircuit::Create(QM, *FuncToInsert);

  QuantumKernel *NewFunction = FindKernelInModule(QM, "_Z12basic_branchb.stub");
  BasicBlock *BlockAfter1 =
      FindBlockInFunction(*(NewFunction->getFunction()), "if.then");
  BasicBlock *BlockAfter2 =
      FindBlockInFunction(*(NewFunction->getFunction()), "if.else");
  BasicBlock *BlockBefore =
      FindBlockInFunction(*(NewFunction->getFunction()), "body");

  QuantumCircuit &InsertIntoQC = *QuantumCircuit::Create(QM, *NewFunction);
  EXPECT_TRUE(
      InsertIntoQC.insertCircuit(&InsertQC, InsertIntoQC.getCircuitList()[1]));

  EXPECT_EQ(InsertQC.getCircuitList().size(), 0);
  EXPECT_EQ(InsertIntoQC.getCircuitList().size(), 8);
  EXPECT_EQ(InsertIntoQC.getNumberGates(), 5);

  QuantumCircuit *PrevQC = InsertIntoQC.getCircuitList()[1];
  QuantumCircuit *AfterQC1 = InsertIntoQC.getCircuitList()[2];
  QuantumCircuit *AfterQC2 = InsertIntoQC.getCircuitList()[3];
  QuantumCircuit *AddedQCBegin = InsertIntoQC.getCircuitList()[5];
  QuantumCircuit *AddedQCEnd = InsertIntoQC.getCircuitList()[7];
  BasicBlock *AddedBlockBegin =
      dyn_cast<BasicBlock>(AddedQCBegin->getBaseValue());
  BasicBlock *AddedBlockEnd = dyn_cast<BasicBlock>(AddedQCEnd->getBaseValue());
  EXPECT_EQ(AddedBlockBegin->getSinglePredecessor(), BlockBefore);
  BranchInst *BI = dyn_cast<BranchInst>(BlockBefore->getTerminator());
  ASSERT_NE(BI, nullptr);
  EXPECT_EQ(BI->getSuccessor(0), AddedBlockBegin);

  EXPECT_EQ(BlockAfter1->getSinglePredecessor(), AddedBlockEnd);
  BI = dyn_cast<BranchInst>(AddedBlockEnd->getTerminator());
  ASSERT_NE(BI, nullptr);
  EXPECT_EQ(BI->getSuccessor(0), BlockAfter1);

  EXPECT_EQ(BlockAfter2->getSinglePredecessor(), AddedBlockEnd);
  BI = dyn_cast<BranchInst>(AddedBlockEnd->getTerminator());
  ASSERT_NE(BI, nullptr);
  EXPECT_EQ(BI->getSuccessor(1), BlockAfter2);

  EXPECT_NE(
      PrevQC->getEndOp()->getSuccessors().find(AddedQCBegin->getStartOp()),
      PrevQC->getEndOp()->getSuccessors().end());
  EXPECT_EQ(PrevQC->getEndOp()->getSuccessors().find(AfterQC1->getStartOp()),
            PrevQC->getEndOp()->getSuccessors().end());
  EXPECT_EQ(PrevQC->getEndOp()->getSuccessors().find(AfterQC2->getStartOp()),
            PrevQC->getEndOp()->getSuccessors().end());

  EXPECT_NE(
      AddedQCEnd->getEndOp()->getSuccessors().find(AfterQC1->getStartOp()),
      AddedQCEnd->getEndOp()->getSuccessors().end());
  EXPECT_NE(
      AddedQCEnd->getEndOp()->getSuccessors().find(AfterQC2->getStartOp()),
      AddedQCEnd->getEndOp()->getSuccessors().end());
}

/// Test inserting a circuit representing a QBB after another
/// circuit.
TEST(QuantumCircuitTest, InsertNewQBBCircuitAfter) {
  LLVMContext C;
  std::unique_ptr<Module> M = parseIR(C, *&Code_Generated);
  ParaRef::setModule(*M);

  QuantumModule QM;
  runInitialPasses(*M, QM);

  QuantumKernel *FuncToInsert =
      FindKernelInModule(QM, "_Z16basic_line_limitv.stub");
  BasicBlock *BB = FindBlockInFunction(*(FuncToInsert->getFunction()), "entry");
  BB->removeFromParent();

  QuantumCircuit &InsertQC = *QuantumCircuit::Create(QM, *BB);

  QuantumKernel *NewFunction =
      FindKernelInModule(QM, "_Z18kernel_with_doubled.stub");
  BasicBlock *BlockAfter =
      FindBlockInFunction(*(NewFunction->getFunction()), "aqcc.meas.move.end");
  BasicBlock *BlockBefore =
      FindBlockInFunction(*(NewFunction->getFunction()), "aqcc.quantum");

  QuantumCircuit &InsertIntoQC = *QuantumCircuit::Create(QM, *NewFunction);
  EXPECT_TRUE(
      InsertIntoQC.insertCircuit(&InsertQC, InsertIntoQC.getCircuitList()[1]));

  EXPECT_EQ(InsertIntoQC.getCircuitList().size(), 4);
  EXPECT_EQ(InsertIntoQC.getNumberGates(), 4);

  QuantumCircuit *PrevQC = InsertIntoQC.getCircuitList()[1];
  QuantumCircuit *AfterQC = InsertIntoQC.getCircuitList()[2];
  QuantumCircuit *AddedQCBegin = InsertIntoQC.getCircuitList()[3];
  BasicBlock *AddedBlockBegin =
      dyn_cast<BasicBlock>(AddedQCBegin->getBaseValue());
  EXPECT_EQ(AddedBlockBegin->getSinglePredecessor(), BlockBefore);
  BranchInst *BI = dyn_cast<BranchInst>(BlockBefore->getTerminator());
  ASSERT_NE(BI, nullptr);
  EXPECT_EQ(BI->getSuccessor(0), AddedBlockBegin);

  EXPECT_EQ(BlockAfter->getSinglePredecessor(), AddedBlockBegin);
  BI = dyn_cast<BranchInst>(AddedBlockBegin->getTerminator());
  ASSERT_NE(BI, nullptr);
  EXPECT_EQ(BI->getSuccessor(0), BlockAfter);

  EXPECT_NE(
      PrevQC->getEndOp()->getSuccessors().find(AddedQCBegin->getStartOp()),
      PrevQC->getEndOp()->getSuccessors().end());
  EXPECT_EQ(PrevQC->getEndOp()->getSuccessors().find(AfterQC->getStartOp()),
            PrevQC->getEndOp()->getSuccessors().end());

  EXPECT_NE(
      AddedQCBegin->getEndOp()->getSuccessors().find(AfterQC->getStartOp()),
      AddedQCBegin->getEndOp()->getSuccessors().end());
}

/// Test failing to insert a circuit representing a QBB after another
/// circuit that is not in the circuit we are adding to.
TEST(QuantumCircuitTest, InsertNewQBBCircuitAfterWrongParent) {
  LLVMContext C;
  std::unique_ptr<Module> M = parseIR(C, *&Code_Generated);
  ParaRef::setModule(*M);

  QuantumModule QM;
  runInitialPasses(*M, QM);

  QuantumKernel *FuncToInsert =
      FindKernelInModule(QM, "_Z16basic_line_limitv.stub");
  BasicBlock *BB = FindBlockInFunction(*(FuncToInsert->getFunction()), "entry");
  BB->removeFromParent();

  QuantumCircuit &InsertQC = *QuantumCircuit::Create(QM, *BB);

  QuantumKernel *NewFunction =
      FindKernelInModule(QM, "_Z18kernel_with_doubled.stub");

  QuantumKernel *OtherFunction =
      FindKernelInModule(QM, "_Z18kernel_with_no_args.stub");

  QuantumCircuit &InsertIntoQC = *QuantumCircuit::Create(QM, *NewFunction);
  QuantumCircuit &WrongQC = *QuantumCircuit::Create(QM, *OtherFunction);
  EXPECT_FALSE(
      InsertIntoQC.insertCircuit(&InsertQC, WrongQC.getCircuitList()[1]));
}

/// Test inserting a circuit representing a QBB after a particular instruction.
TEST(QuantumCircuitTest, InsertNewCircuitAfterInst) {
  LLVMContext C;
  std::unique_ptr<Module> M = parseIR(C, *&Code_Generated);
  ParaRef::setModule(*M);

  QuantumModule QM;
  runInitialPasses(*M, QM);

  QuantumKernel *FuncToInsert =
      FindKernelInModule(QM, "_Z16basic_line_limitv.stub");
  BasicBlock *BB = FindBlockInFunction(*(FuncToInsert->getFunction()), "entry");
  BB->removeFromParent();

  QuantumCircuit &InsertQC = *QuantumCircuit::Create(QM, *BB);

  QuantumKernel *NewFunction =
      FindKernelInModule(QM, "_Z19basic_line_parallelv.stub");

  QuantumCircuit &InsertIntoQC = *QuantumCircuit::Create(QM, *NewFunction);
  QuantumCircuit::QuantumCircuitIterator It =
      InsertIntoQC.getCircuitList()[1]->begin();
  QuantumOp *QO = static_cast<QuantumOp *>(*It);
  EXPECT_TRUE(InsertIntoQC.insertCircuit(&InsertQC, *QO));

  EXPECT_EQ(InsertIntoQC.getCircuitList().size(), 5);
  EXPECT_EQ(InsertIntoQC.getNumberGates(), 6);

  QuantumCircuit *PrevQC = InsertIntoQC.getCircuitList()[1];
  BasicBlock *BlockBefore = dyn_cast<BasicBlock>(PrevQC->getBaseValue());
  QuantumCircuit *AfterQC = InsertIntoQC.getCircuitList()[3];
  BasicBlock *BlockAfter = dyn_cast<BasicBlock>(AfterQC->getBaseValue());
  QuantumCircuit *AddedQCBegin = InsertIntoQC.getCircuitList()[4];
  BasicBlock *AddedBlockBegin =
      dyn_cast<BasicBlock>(AddedQCBegin->getBaseValue());
  EXPECT_EQ(AddedBlockBegin->getSinglePredecessor(), BlockBefore);
  BranchInst *BI = dyn_cast<BranchInst>(BlockBefore->getTerminator());
  ASSERT_NE(BI, nullptr);
  EXPECT_EQ(BI->getSuccessor(0), AddedBlockBegin);

  EXPECT_EQ(BlockAfter->getSinglePredecessor(), AddedBlockBegin);
  BI = dyn_cast<BranchInst>(AddedBlockBegin->getTerminator());
  ASSERT_NE(BI, nullptr);
  EXPECT_EQ(BI->getSuccessor(0), BlockAfter);

  EXPECT_NE(
      PrevQC->getEndOp()->getSuccessors().find(AddedQCBegin->getStartOp()),
      PrevQC->getEndOp()->getSuccessors().end());
  EXPECT_EQ(PrevQC->getEndOp()->getSuccessors().find(AfterQC->getStartOp()),
            PrevQC->getEndOp()->getSuccessors().end());

  EXPECT_NE(
      AddedQCBegin->getEndOp()->getSuccessors().find(AfterQC->getStartOp()),
      AddedQCBegin->getEndOp()->getSuccessors().end());
}

/// Test inserting a circuit representing a quantum kernel with an argument, and
/// mapping specified for that mapping.
TEST(QuantumCircuitTest, InsertNewQKCircuitWithMapping) {
  LLVMContext C;
  std::unique_ptr<Module> M = parseIR(C, *&Code_Generated);
  ParaRef::setModule(*M);

  QuantumModule QM;
  runInitialPasses(*M, QM);

  QuantumKernel *FuncToInsert =
      FindKernelInModule(QM, "_Z26kernel_with_int_and_doubleid.stub");

  QuantumCircuit &InsertQC = *QuantumCircuit::Create(QM, *FuncToInsert);

  QuantumKernel *NewFunction =
      FindKernelInModule(QM, "_Z18kernel_with_doubled.stub");
  BasicBlock *BlockAfter =
      FindBlockInFunction(*(NewFunction->getFunction()), "aqcc.meas.move.end");
  BasicBlock *BlockBefore =
      FindBlockInFunction(*(NewFunction->getFunction()), "aqcc.quantum");

  QuantumCircuit &InsertIntoQC = *QuantumCircuit::Create(QM, *NewFunction);
  std::map<unsigned, QuantumOpParam> InsertMapping;
  InsertMapping.insert(std::make_pair(0, QuantumOpParam(OpParamTy::INT, 2)));
  InsertMapping.insert(std::make_pair(1, InsertIntoQC.getOpParams()[0]));

  EXPECT_TRUE(InsertIntoQC.insertCircuit(
      &InsertQC, InsertIntoQC.getCircuitList()[1], InsertMapping));

  EXPECT_EQ(InsertQC.getCircuitList().size(), 0);
  EXPECT_EQ(InsertIntoQC.getCircuitList().size(), 6);
  EXPECT_EQ(InsertIntoQC.getNumberGates(), 2);

  QuantumCircuit *PrevQC = InsertIntoQC.getCircuitList()[1];
  QuantumCircuit *AfterQC = InsertIntoQC.getCircuitList()[2];
  QuantumCircuit *AddedQCBegin = InsertIntoQC.getCircuitList()[3];
  QuantumCircuit *AddedQCEnd = InsertIntoQC.getCircuitList()[5];
  BasicBlock *AddedBlockBegin =
      dyn_cast<BasicBlock>(AddedQCBegin->getBaseValue());
  BasicBlock *AddedBlockEnd = dyn_cast<BasicBlock>(AddedQCEnd->getBaseValue());
  EXPECT_EQ(AddedBlockBegin->getSinglePredecessor(), BlockBefore);
  BranchInst *BI = dyn_cast<BranchInst>(BlockBefore->getTerminator());
  ASSERT_NE(BI, nullptr);
  EXPECT_EQ(BI->getSuccessor(0), AddedBlockBegin);

  EXPECT_EQ(BlockAfter->getSinglePredecessor(), AddedBlockEnd);
  BI = dyn_cast<BranchInst>(AddedBlockEnd->getTerminator());
  ASSERT_NE(BI, nullptr);
  EXPECT_EQ(BI->getSuccessor(0), BlockAfter);

  EXPECT_NE(
      PrevQC->getEndOp()->getSuccessors().find(AddedQCBegin->getStartOp()),
      PrevQC->getEndOp()->getSuccessors().end());
  EXPECT_EQ(PrevQC->getEndOp()->getSuccessors().find(AfterQC->getStartOp()),
            PrevQC->getEndOp()->getSuccessors().end());

  EXPECT_NE(AddedQCEnd->getEndOp()->getSuccessors().find(AfterQC->getStartOp()),
            AddedQCEnd->getEndOp()->getSuccessors().end());
}

/// Test creating a copy of a circuit representing a quantum kernel, then
/// inserting that circuit into an existing circuit.
TEST(QuantumCircuitTest, CopyInsertNewQKCircuit) {
  LLVMContext C;
  std::unique_ptr<Module> M = parseIR(C, *&Code_Generated);
  ParaRef::setModule(*M);

  QuantumModule QM;
  runInitialPasses(*M, QM);

  QuantumKernel *FuncToInsert =
      FindKernelInModule(QM, "_Z16basic_line_limitv.stub");

  QuantumCircuit &InsertQC = *QuantumCircuit::Create(QM, *FuncToInsert);

  QuantumKernel *NewFunction =
      FindKernelInModule(QM, "_Z18kernel_with_doubled.stub");
  BasicBlock *LastBlock =
      FindBlockInFunction(*(NewFunction->getFunction()), "aqcc.meas.move.end");

  QuantumCircuit &InsertIntoQC = *QuantumCircuit::Create(QM, *NewFunction);
  EXPECT_TRUE(InsertIntoQC.copyAndInsertCircuit(&InsertQC));

  EXPECT_EQ(InsertIntoQC.getCircuitList().size(), 4);
  EXPECT_EQ(InsertQC.getCircuitList().size(), 1);
  EXPECT_EQ(InsertQC.getNumberGates(), 3);
  EXPECT_EQ(InsertIntoQC.getCircuitList().size(), 4);
  EXPECT_EQ(InsertIntoQC.getNumberGates(), 4);

  QuantumCircuit *LastQC = InsertIntoQC.getCircuitList()[2];
  QuantumCircuit *AddedQC = InsertIntoQC.getCircuitList()[3];
  BasicBlock *AddedBlock = dyn_cast<BasicBlock>(AddedQC->getBaseValue());
  EXPECT_EQ(AddedBlock->getSinglePredecessor(), LastBlock);
  BranchInst *BI = dyn_cast<BranchInst>(LastBlock->getTerminator());
  ASSERT_NE(BI, nullptr);
  EXPECT_EQ(BI->getSuccessor(0), AddedBlock);

  EXPECT_NE(LastQC->getEndOp()->getSuccessors().find(AddedQC->getStartOp()),
            LastQC->getEndOp()->getSuccessors().end());
  EXPECT_EQ(LastQC->getEndOp()->getSuccessors().find(InsertIntoQC.getEndOp()),
            LastQC->getEndOp()->getSuccessors().end());
}

/// Test creating a copy of a circuit representing a QBB, then
/// inserting that circuit into an existing circuit.
TEST(QuantumCircuitTest, CopyInsertNewQBBCircuit) {
  LLVMContext C;
  std::unique_ptr<Module> M = parseIR(C, *&Code_Generated);
  ParaRef::setModule(*M);

  QuantumModule QM;
  runInitialPasses(*M, QM);

  QuantumKernel *FuncToInsert =
      FindKernelInModule(QM, "_Z16basic_line_limitv.stub");
  BasicBlock *BB = FindBlockInFunction(*(FuncToInsert->getFunction()), "entry");
  BB->removeFromParent();

  QuantumCircuit &InsertQC = *QuantumCircuit::Create(QM, *BB);

  QuantumKernel *NewFunction =
      FindKernelInModule(QM, "_Z18kernel_with_doubled.stub");
  BasicBlock *LastBlock =
      FindBlockInFunction(*(NewFunction->getFunction()), "aqcc.meas.move.end");

  QuantumCircuit &InsertIntoQC = *QuantumCircuit::Create(QM, *NewFunction);
  EXPECT_TRUE(InsertIntoQC.copyAndInsertCircuit(&InsertQC));
  EXPECT_EQ(InsertQC.getNumberGates(), 3);
  EXPECT_EQ(InsertIntoQC.getCircuitList().size(), 4);
  EXPECT_EQ(InsertIntoQC.getNumberGates(), 4);

  QuantumCircuit *LastQC = InsertIntoQC.getCircuitList()[2];
  QuantumCircuit *AddedQC = InsertIntoQC.getCircuitList()[3];
  BasicBlock *AddedBlock = dyn_cast<BasicBlock>(AddedQC->getBaseValue());
  EXPECT_EQ(AddedBlock->getSinglePredecessor(), LastBlock);
  BranchInst *BI = dyn_cast<BranchInst>(LastBlock->getTerminator());
  ASSERT_NE(BI, nullptr);
  EXPECT_EQ(BI->getSuccessor(0), AddedBlock);

  EXPECT_NE(LastQC->getEndOp()->getSuccessors().find(AddedQC->getStartOp()),
            LastQC->getEndOp()->getSuccessors().end());
  EXPECT_EQ(LastQC->getEndOp()->getSuccessors().find(InsertIntoQC.getEndOp()),
            LastQC->getEndOp()->getSuccessors().end());
}
