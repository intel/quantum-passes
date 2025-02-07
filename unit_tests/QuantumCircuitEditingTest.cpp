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

/// Test moving gate from the middle of the circuit to the end by providing a
/// circuit.
TEST(QuantumCircuitTest, MoveGateToEndAppend) {
  LLVMContext C;
  std::unique_ptr<Module> M = parseIR(C, *&Code_Generated);
  ParaRef::setModule(*M);

  QuantumModule QM;
  runInitialPasses(*M, QM);

  QuantumKernel *F = FindKernelInModule(QM, "_Z18qbitref_dependencyv.stub");
  BasicBlock *BB = FindBlockInFunction(*(F->getFunction()), "entry");

  QuantumCircuit &BlockQC = *QuantumCircuit::Create(QM, *BB);
  StartingOp *BlockStartOp = BlockQC.getStartOp();
  EndingOp *BlockEndOp = BlockQC.getEndOp();
  QuantumCircuit::QuantumCircuitIterator It = BlockQC.begin();
  QuantumCircuitOp *CX1 = *It;
  It++;
  std::vector<QuantumCircuitOp *> Hs;
  for (unsigned Idx = 0; Idx < 2; Idx++) {
    Hs.push_back(*It);
    It++;
  }
  QuantumCircuitOp *CX2 = *It;
  QuantumOp *QOCX2 = static_cast<QuantumOp *>(CX2);

  QuantumOp *QOCX1 = static_cast<QuantumOp *>(CX1);
  bool Worked = QOCX1->moveGate(&BlockQC);
  ASSERT_TRUE(Worked);
  EXPECT_EQ(BlockQC.getNumberGates(), 4);

  ASSERT_TRUE(QOCX1->isPredecessorTo(BlockEndOp));
  ASSERT_TRUE(BlockEndOp->isSuccessorTo(QOCX1));
  ASSERT_TRUE(QOCX1->isSuccessorTo(CX2));
  ASSERT_TRUE(CX2->isPredecessorTo(QOCX1));

  EXPECT_TRUE(dyn_cast<Instruction>(QOCX2->getBaseValue())
                  ->comesBefore(dyn_cast<Instruction>(QOCX1->getBaseValue())));
}

/// Test moving gate from the middle of the circuit to the end by providing an
/// operation.
TEST(QuantumCircuitTest, MoveGateToEndAfterOp) {
  LLVMContext C;
  std::unique_ptr<Module> M = parseIR(C, *&Code_Generated);
  ParaRef::setModule(*M);

  QuantumModule QM;
  runInitialPasses(*M, QM);

  QuantumKernel *F = FindKernelInModule(QM, "_Z18qbitref_dependencyv.stub");
  BasicBlock *BB = FindBlockInFunction(*(F->getFunction()), "entry");

  QuantumCircuit &BlockQC = *QuantumCircuit::Create(QM, *BB);
  StartingOp *BlockStartOp = BlockQC.getStartOp();
  EndingOp *BlockEndOp = BlockQC.getEndOp();
  QuantumCircuit::QuantumCircuitIterator It = BlockQC.begin();
  QuantumCircuitOp *CX1 = *It;
  It++;
  std::vector<QuantumCircuitOp *> Hs;
  for (unsigned Idx = 0; Idx < 2; Idx++) {
    Hs.push_back(*It);
    It++;
  }
  QuantumCircuitOp *CX2 = *It;
  QuantumOp *QOCX2 = static_cast<QuantumOp *>(CX2);

  QuantumOp *QOCX1 = static_cast<QuantumOp *>(CX1);
  bool Worked = QOCX1->moveGate(*CX2);
  ASSERT_TRUE(Worked);
  EXPECT_EQ(BlockQC.getNumberGates(), 4);

  ASSERT_TRUE(QOCX1->isPredecessorTo(BlockEndOp));
  ASSERT_TRUE(BlockEndOp->isSuccessorTo(QOCX1));
  ASSERT_TRUE(QOCX1->isSuccessorTo(CX2));
  EXPECT_TRUE(CX2->isPredecessorTo(QOCX1));

  EXPECT_TRUE(dyn_cast<Instruction>(QOCX2->getBaseValue())
                  ->comesBefore(dyn_cast<Instruction>(QOCX1->getBaseValue())));
}

/// Test moving gate from the middle of the circuit from the end of the circuit.
TEST(QuantumCircuitTest, MoveGateToMiddle) {
  LLVMContext C;
  std::unique_ptr<Module> M = parseIR(C, *&Code_Generated);
  ParaRef::setModule(*M);

  QuantumModule QM;
  runInitialPasses(*M, QM);

  QuantumKernel *F = FindKernelInModule(QM, "_Z18qbitref_dependencyv.stub");
  BasicBlock *BB = FindBlockInFunction(*(F->getFunction()), "entry");

  QuantumCircuit &BlockQC = *QuantumCircuit::Create(QM, *BB);
  StartingOp *BlockStartOp = BlockQC.getStartOp();
  EndingOp *BlockEndOp = BlockQC.getEndOp();
  QuantumCircuit::QuantumCircuitIterator It = BlockQC.begin();
  QuantumCircuitOp *CX1 = *It;
  It++;
  std::vector<QuantumCircuitOp *> Hs;
  for (unsigned Idx = 0; Idx < 2; Idx++) {
    Hs.push_back(*It);
    It++;
  }
  QuantumCircuitOp *CX2 = *It;
  QuantumOp *QOCX2 = static_cast<QuantumOp *>(CX2);

  QuantumOp *QOCX1 = static_cast<QuantumOp *>(CX1);
  bool Worked = QOCX1->moveGate(*Hs[1]);
  ASSERT_TRUE(Worked);
  EXPECT_EQ(BlockQC.getNumberGates(), 4);

  EXPECT_TRUE(QOCX1->isPredecessorTo(QOCX2));
  ASSERT_TRUE(QOCX2->isSuccessorTo(QOCX1));
  ASSERT_TRUE(QOCX1->isSuccessorTo(Hs[0]));
  ASSERT_TRUE(QOCX1->isSuccessorTo(Hs[1]));
  ASSERT_TRUE(Hs[0]->isPredecessorTo(QOCX1));
  ASSERT_TRUE(Hs[1]->isPredecessorTo(QOCX1));

  QuantumOp *H1 = static_cast<QuantumOp *>(Hs[0]);
  QuantumOp *H2 = static_cast<QuantumOp *>(Hs[1]);

  EXPECT_TRUE(dyn_cast<Instruction>(QOCX1->getBaseValue())
                  ->comesBefore(dyn_cast<Instruction>(QOCX2->getBaseValue())));
  EXPECT_TRUE(dyn_cast<Instruction>(H1->getBaseValue())
                  ->comesBefore(dyn_cast<Instruction>(QOCX1->getBaseValue())));
  EXPECT_TRUE(dyn_cast<Instruction>(H2->getBaseValue())
                  ->comesBefore(dyn_cast<Instruction>(QOCX1->getBaseValue())));
}

/// Test moving gate after itself, expected to fail.
TEST(QuantumCircuitTest, MoveAfterMovedOp) {
  LLVMContext C;
  std::unique_ptr<Module> M = parseIR(C, *&Code_Generated);
  ParaRef::setModule(*M);

  QuantumModule QM;
  runInitialPasses(*M, QM);

  QuantumKernel *F = FindKernelInModule(QM, "_Z18qbitref_dependencyv.stub");
  BasicBlock *BB = FindBlockInFunction(*(F->getFunction()), "entry");

  QuantumCircuit &BlockQC = *QuantumCircuit::Create(QM, *BB);
  StartingOp *BlockStartOp = BlockQC.getStartOp();
  EndingOp *BlockEndOp = BlockQC.getEndOp();
  QuantumCircuit::QuantumCircuitIterator It = BlockQC.begin();
  QuantumCircuitOp *CX1 = *It;
  It++;
  std::vector<QuantumCircuitOp *> Hs;
  for (unsigned Idx = 0; Idx < 2; Idx++) {
    Hs.push_back(*It);
    It++;
  }
  QuantumCircuitOp *CX2 = *It;
  QuantumOp *QOCX2 = static_cast<QuantumOp *>(CX2);

  QuantumOp *QOCX1 = static_cast<QuantumOp *>(CX1);
  bool Worked = QOCX1->moveGate(*QOCX1);
  ASSERT_FALSE(Worked);
}

/// Test moving gate to different circuit when qubit arguments are
/// global.
TEST(QuantumCircuitTest, MoveGateToDifferentCircuitNoArgs) {
  LLVMContext C;
  std::unique_ptr<Module> M = parseIR(C, *&Code_Generated);
  ParaRef::setModule(*M);

  QuantumModule QM;
  runInitialPasses(*M, QM);

  QuantumKernel *F = FindKernelInModule(QM, "_Z18qbitref_dependencyv.stub");
  BasicBlock *BB = FindBlockInFunction(*(F->getFunction()), "entry");

  QuantumKernel *NewF =
      FindKernelInModule(QM, "_Z34simple_kernel_call_canonical_blockv.stub");
  BasicBlock *NewBB = FindBlockInFunction(*(NewF->getFunction()), "entry");

  QuantumCircuit &BlockQC = *QuantumCircuit::Create(QM, *BB);
  StartingOp *BlockStartOp = BlockQC.getStartOp();
  EndingOp *BlockEndOp = BlockQC.getEndOp();
  QuantumCircuit::QuantumCircuitIterator It = BlockQC.begin();
  QuantumCircuitOp *CX1 = *It;
  It++;
  std::vector<QuantumCircuitOp *> Hs;
  for (unsigned Idx = 0; Idx < 2; Idx++) {
    Hs.push_back(*It);
    It++;
  }
  QuantumCircuitOp *CX2 = *It;
  QuantumOp *QOCX2 = static_cast<QuantumOp *>(CX2);

  QuantumCircuit &NewBlockQC = *QuantumCircuit::Create(QM, *NewBB);
  StartingOp *NewBlockStartOp = NewBlockQC.getStartOp();
  EndingOp *NewBlockEndOp = NewBlockQC.getEndOp();
  It = NewBlockQC.begin();
  std::vector<QuantumOp *> NewPrepZs;
  for (unsigned Idx = 0; Idx < 2; Idx++) {
    NewPrepZs.push_back(static_cast<QuantumOp *>(*It));
    It++;
  }
  QuantumOp *NewCanonicalOp = static_cast<QuantumOp *>(*It);
  It++;
  std::vector<QuantumOp *> NewHs;
  for (unsigned Idx = 0; Idx < 3; Idx++) {
    NewHs.push_back(static_cast<QuantumOp *>(*It));
    It++;
  }

  QuantumOp *QOCX1 = static_cast<QuantumOp *>(CX1);
  bool Worked = QOCX1->moveGate(*NewCanonicalOp);
  ASSERT_TRUE(Worked);
  EXPECT_EQ(BlockQC.getNumberGates(), 3);
  EXPECT_EQ(NewBlockQC.getNumberGates(), 7);

  ASSERT_TRUE(QOCX1->isPredecessorTo(NewHs[0]));
  ASSERT_TRUE(QOCX1->isPredecessorTo(NewHs[1]));
  ASSERT_TRUE(NewHs[0]->isSuccessorTo(QOCX1));
  ASSERT_TRUE(NewHs[1]->isSuccessorTo(QOCX1));
  ASSERT_TRUE(QOCX1->isSuccessorTo(NewCanonicalOp));
  EXPECT_TRUE(NewCanonicalOp->isPredecessorTo(QOCX1));

  EXPECT_TRUE(
      dyn_cast<Instruction>(QOCX1->getBaseValue())
          ->comesBefore(dyn_cast<Instruction>(NewHs[0]->getBaseValue())));
  EXPECT_TRUE(
      dyn_cast<Instruction>(QOCX1->getBaseValue())
          ->comesBefore(dyn_cast<Instruction>(NewHs[0]->getBaseValue())));
  EXPECT_TRUE(dyn_cast<Instruction>(NewCanonicalOp->getBaseValue())
                  ->comesBefore(dyn_cast<Instruction>(QOCX1->getBaseValue())));
}

/// Test moving gate to new circuit, but qubit is a function argument
/// and must be provided the argument mapping to work.
TEST(QuantumCircuitTest, MoveGateToDifferentCircuitQubitArgs) {
  LLVMContext C;
  std::unique_ptr<Module> M = parseIR(C, *&Code_Generated);
  ParaRef::setModule(*M);

  QuantumModule QM;
  runInitialPasses(*M, QM);

  QuantumKernel *F =
      FindKernelInModule(QM, "_Z23kernel_with_qubit_arg_1Pt.stub");
  BasicBlock *BB = FindBlockInFunction(*(F->getFunction()), "entry");

  QuantumKernel *NewF =
      FindKernelInModule(QM, "_Z23kernel_with_qubit_arg_2Pt.stub");
  BasicBlock *NewBB = FindBlockInFunction(*(NewF->getFunction()), "entry");

  QuantumCircuit &BlockQC = *QuantumCircuit::Create(QM, *BB);
  StartingOp *BlockStartOp = BlockQC.getStartOp();
  EndingOp *BlockEndOp = BlockQC.getEndOp();
  QuantumCircuit::QuantumCircuitIterator It = BlockQC.begin();
  QuantumOp *RZ = static_cast<QuantumOp *>(*It);

  QuantumCircuit &NewBlockQC = *QuantumCircuit::Create(QM, *NewBB);
  StartingOp *NewBlockStartOp = NewBlockQC.getStartOp();
  EndingOp *NewBlockEndOp = NewBlockQC.getEndOp();
  It = NewBlockQC.begin();
  QuantumOp *RX = static_cast<QuantumOp *>(*It);

  std::map<unsigned, QuantumOpParam> Mapping;
  QbitRef QR(NewF->getFunction()->getArg(0));
  Mapping.emplace(0, QuantumOpParam(OpParamTy::QUBITPTR, QR));
  bool Worked = RZ->moveGate(*RX, Mapping);
  ASSERT_TRUE(Worked);
  EXPECT_EQ(BlockQC.getNumberGates(), 0);
  EXPECT_EQ(NewBlockQC.getNumberGates(), 2);

  SmallVector<std::pair<QbitRef, unsigned>, 2> &Qubits = RX->getQubits();
  ASSERT_EQ(Qubits[0].first.getRegister(), NewF->getFunction()->getArg(0));

  EXPECT_TRUE(RX->isPredecessorTo(RZ));
  ASSERT_TRUE(RZ->isSuccessorTo(RX));
  EXPECT_TRUE(RZ->isPredecessorTo(NewBlockEndOp));
  EXPECT_TRUE(BlockStartOp->isPredecessorTo(BlockEndOp));
  ASSERT_TRUE(BlockEndOp->isSuccessorTo(BlockStartOp));

  EXPECT_TRUE(dyn_cast<Instruction>(RX->getBaseValue())
                  ->comesBefore(dyn_cast<Instruction>(RZ->getBaseValue())));
}

/// Test moving gate to new circuit, but qubit is a function argument
/// and must be provided the argument mapping to work. This tests without the
/// mapping, which is expected to fail.
TEST(QuantumCircuitTest, MoveGateToDifferentCircuitQubitArgsNoMapping) {
  LLVMContext C;
  std::unique_ptr<Module> M = parseIR(C, *&Code_Generated);
  ParaRef::setModule(*M);

  QuantumModule QM;
  runInitialPasses(*M, QM);

  QuantumKernel *F =
      FindKernelInModule(QM, "_Z23kernel_with_qubit_arg_1Pt.stub");
  BasicBlock *BB = FindBlockInFunction(*(F->getFunction()), "entry");

  QuantumKernel *NewF =
      FindKernelInModule(QM, "_Z23kernel_with_qubit_arg_2Pt.stub");
  BasicBlock *NewBB = FindBlockInFunction(*(NewF->getFunction()), "entry");

  QuantumCircuit &BlockQC = *QuantumCircuit::Create(QM, *BB);
  StartingOp *BlockStartOp = BlockQC.getStartOp();
  EndingOp *BlockEndOp = BlockQC.getEndOp();
  QuantumCircuit::QuantumCircuitIterator It = BlockQC.begin();
  QuantumOp *RZ = static_cast<QuantumOp *>(*It);

  QuantumCircuit &NewBlockQC = *QuantumCircuit::Create(QM, *NewBB);
  StartingOp *NewBlockStartOp = NewBlockQC.getStartOp();
  EndingOp *NewBlockEndOp = NewBlockQC.getEndOp();
  It = NewBlockQC.begin();
  QuantumOp *RX = static_cast<QuantumOp *>(*It);

  std::map<unsigned, QuantumOpParam> Mapping;
  QbitRef QR(NewF->getFunction()->getArg(0));
  Mapping.emplace(0, QuantumOpParam(OpParamTy::QUBITPTR, QR));
  bool Worked = RZ->moveGate(*RX);
  ASSERT_FALSE(Worked);
}

/// Test moving gate to new circuit, but parameter is a function argument
/// and must be provided the argument mapping to work.
TEST(QuantumCircuitTest, MoveGateToDifferentCircuitParamArgs) {
  LLVMContext C;
  std::unique_ptr<Module> M = parseIR(C, *&Code_Generated);
  ParaRef::setModule(*M);

  QuantumModule QM;
  runInitialPasses(*M, QM);

  QuantumKernel *F = FindKernelInModule(QM, "_Z18kernel_with_doubled.stub");
  BasicBlock *BB = FindBlockInFunction(*(F->getFunction()), "aqcc.quantum");

  QuantumKernel *NewF =
      FindKernelInModule(QM, "_Z26kernel_with_int_and_doubleid.stub");
  BasicBlock *NewBB =
      FindBlockInFunction(*(NewF->getFunction()), "aqcc.quantum");

  QuantumCircuit &BlockQC = *QuantumCircuit::Create(QM, *BB);
  StartingOp *BlockStartOp = BlockQC.getStartOp();
  EndingOp *BlockEndOp = BlockQC.getEndOp();
  QuantumCircuit::QuantumCircuitIterator It = BlockQC.begin();
  QuantumOp *RZ1 = static_cast<QuantumOp *>(*It);

  QuantumCircuit &NewBlockQC = *QuantumCircuit::Create(QM, *NewBB);
  StartingOp *NewBlockStartOp = NewBlockQC.getStartOp();
  EndingOp *NewBlockEndOp = NewBlockQC.getEndOp();
  It = NewBlockQC.begin();
  QuantumOp *RZ2 = static_cast<QuantumOp *>(*It);

  std::map<unsigned, QuantumOpParam> Mapping;
  ParaRef PR(NewF->getFunction()->getArg(1));
  Mapping.emplace(0, QuantumOpParam(OpParamTy::GATEPARAM, PR));
  bool Worked = RZ1->moveGate(*RZ2, Mapping);
  ASSERT_TRUE(Worked);
  EXPECT_EQ(BlockQC.getNumberGates(), 0);
  EXPECT_EQ(NewBlockQC.getNumberGates(), 2);

  SmallVector<std::pair<ParaRef, unsigned>, 2> &Params =
      RZ2->getResolvedParams();
  ASSERT_EQ(Params[0].first.getValue(), NewF->getFunction()->getArg(1));

  EXPECT_TRUE(RZ2->isPredecessorTo(RZ1));
  ASSERT_TRUE(RZ1->isSuccessorTo(RZ2));
  EXPECT_TRUE(RZ1->isPredecessorTo(NewBlockEndOp));
  EXPECT_TRUE(BlockStartOp->isPredecessorTo(BlockEndOp));
  ASSERT_TRUE(BlockEndOp->isSuccessorTo(BlockStartOp));

  EXPECT_TRUE(dyn_cast<Instruction>(RZ2->getBaseValue())
                  ->comesBefore(dyn_cast<Instruction>(RZ1->getBaseValue())));
}

/// Test moving gate to new circuit, but parameter is a local qubit,
/// so checks that the alloca instruction is also copied.
TEST(QuantumCircuitTest, MoveGateToDifferentCircuitLocalQubit) {
  LLVMContext C;
  std::unique_ptr<Module> M = parseIR(C, *&Code_Generated);
  ParaRef::setModule(*M);

  QuantumModule QM;
  runInitialPasses(*M, QM);

  QuantumKernel *F =
      FindKernelInModule(QM, "_Z23kernel_with_local_qubitd.stub");
  BasicBlock *BB = FindBlockInFunction(*(F->getFunction()), "aqcc.quantum");

  QuantumKernel *NewF =
      FindKernelInModule(QM, "_Z26kernel_with_int_and_doubleid.stub");
  BasicBlock *NewBB =
      FindBlockInFunction(*(NewF->getFunction()), "aqcc.quantum");
  BasicBlock *NewEntryBB = FindBlockInFunction(*(NewF->getFunction()), "entry");

  QuantumCircuit &BlockQC = *QuantumCircuit::Create(QM, *BB);
  StartingOp *BlockStartOp = BlockQC.getStartOp();
  EndingOp *BlockEndOp = BlockQC.getEndOp();
  QuantumCircuit::QuantumCircuitIterator It = BlockQC.begin();
  QuantumOp *RXY = static_cast<QuantumOp *>(*It);

  QuantumCircuit &NewBlockQC = *QuantumCircuit::Create(QM, *NewBB);
  StartingOp *NewBlockStartOp = NewBlockQC.getStartOp();
  EndingOp *NewBlockEndOp = NewBlockQC.getEndOp();
  It = NewBlockQC.begin();
  QuantumOp *RZ = static_cast<QuantumOp *>(*It);

  bool Worked = RXY->moveGate(&NewBlockQC);
  ASSERT_TRUE(Worked);
  EXPECT_EQ(BlockQC.getNumberGates(), 1);
  EXPECT_EQ(NewBlockQC.getNumberGates(), 2);

  EXPECT_TRUE(RZ->isPredecessorTo(NewBlockEndOp));
  ASSERT_FALSE(RZ->isPredecessorTo(RXY));
  ASSERT_TRUE(RXY->isSuccessorTo(NewBlockStartOp));
  EXPECT_TRUE(RXY->isPredecessorTo(NewBlockEndOp));

  EXPECT_TRUE(dyn_cast<Instruction>(RZ->getBaseValue())
                  ->comesBefore(dyn_cast<Instruction>(RXY->getBaseValue())));
  EXPECT_TRUE(isa<AllocaInst>(&*NewEntryBB->begin()));
}

/// All copy tests follow the same structure as their move counterparts,
/// but checks that the original gate was not moved as well.

TEST(QuantumCircuitTest, CopyGateToEndAppend) {
  LLVMContext C;
  std::unique_ptr<Module> M = parseIR(C, *&Code_Generated);
  ParaRef::setModule(*M);

  QuantumModule QM;
  runInitialPasses(*M, QM);

  QuantumKernel *F = FindKernelInModule(QM, "_Z18qbitref_dependencyv.stub");
  BasicBlock *BB = FindBlockInFunction(*(F->getFunction()), "entry");

  QuantumCircuit &BlockQC = *QuantumCircuit::Create(QM, *BB);
  StartingOp *BlockStartOp = BlockQC.getStartOp();
  EndingOp *BlockEndOp = BlockQC.getEndOp();
  QuantumCircuit::QuantumCircuitIterator It = BlockQC.begin();
  QuantumCircuitOp *CX1 = *It;
  It++;
  std::vector<QuantumCircuitOp *> Hs;
  for (unsigned Idx = 0; Idx < 2; Idx++) {
    Hs.push_back(*It);
    It++;
  }
  QuantumCircuitOp *CX2 = *It;
  QuantumOp *QOCX2 = static_cast<QuantumOp *>(CX2);

  QuantumOp *QOCX1 = static_cast<QuantumOp *>(CX1);
  QuantumOp *Copied = QOCX1->copyGate(&BlockQC);
  ASSERT_NE(Copied, nullptr);
  EXPECT_EQ(BlockQC.getNumberGates(), 5);

  EXPECT_TRUE(Copied->isPredecessorTo(BlockEndOp));
  ASSERT_TRUE(BlockEndOp->isSuccessorTo(Copied));
  ASSERT_TRUE(Copied->isSuccessorTo(CX2));
  EXPECT_TRUE(CX2->isPredecessorTo(Copied));

  EXPECT_TRUE(dyn_cast<Instruction>(QOCX2->getBaseValue())
                  ->comesBefore(dyn_cast<Instruction>(Copied->getBaseValue())));
  EXPECT_TRUE(dyn_cast<Instruction>(QOCX1->getBaseValue())
                  ->comesBefore(dyn_cast<Instruction>(QOCX2->getBaseValue())));
}

TEST(QuantumCircuitTest, CopyGateToEndAfterOp) {
  LLVMContext C;
  std::unique_ptr<Module> M = parseIR(C, *&Code_Generated);
  ParaRef::setModule(*M);

  QuantumModule QM;
  runInitialPasses(*M, QM);

  QuantumKernel *F = FindKernelInModule(QM, "_Z18qbitref_dependencyv.stub");
  BasicBlock *BB = FindBlockInFunction(*(F->getFunction()), "entry");

  QuantumCircuit &BlockQC = *QuantumCircuit::Create(QM, *BB);
  StartingOp *BlockStartOp = BlockQC.getStartOp();
  EndingOp *BlockEndOp = BlockQC.getEndOp();
  QuantumCircuit::QuantumCircuitIterator It = BlockQC.begin();
  QuantumCircuitOp *CX1 = *It;
  It++;
  std::vector<QuantumCircuitOp *> Hs;
  for (unsigned Idx = 0; Idx < 2; Idx++) {
    Hs.push_back(*It);
    It++;
  }
  QuantumCircuitOp *CX2 = *It;
  QuantumOp *QOCX2 = static_cast<QuantumOp *>(CX2);

  QuantumOp *QOCX1 = static_cast<QuantumOp *>(CX1);
  QuantumOp *Copied = QOCX1->copyGate(*CX2);
  ASSERT_NE(Copied, nullptr);
  EXPECT_EQ(BlockQC.getNumberGates(), 5);

  EXPECT_TRUE(Copied->isPredecessorTo(BlockEndOp));
  ASSERT_TRUE(BlockEndOp->isSuccessorTo(Copied));
  ASSERT_TRUE(Copied->isSuccessorTo(CX2));
  EXPECT_TRUE(CX2->isPredecessorTo(Copied));

  EXPECT_TRUE(dyn_cast<Instruction>(QOCX2->getBaseValue())
                  ->comesBefore(dyn_cast<Instruction>(Copied->getBaseValue())));
  EXPECT_TRUE(dyn_cast<Instruction>(QOCX1->getBaseValue())
                  ->comesBefore(dyn_cast<Instruction>(QOCX2->getBaseValue())));
}

TEST(QuantumCircuitTest, CopyGateToMiddle) {
  LLVMContext C;
  std::unique_ptr<Module> M = parseIR(C, *&Code_Generated);
  ParaRef::setModule(*M);

  QuantumModule QM;
  runInitialPasses(*M, QM);

  QuantumKernel *F = FindKernelInModule(QM, "_Z18qbitref_dependencyv.stub");
  BasicBlock *BB = FindBlockInFunction(*(F->getFunction()), "entry");

  QuantumCircuit &BlockQC = *QuantumCircuit::Create(QM, *BB);
  StartingOp *BlockStartOp = BlockQC.getStartOp();
  EndingOp *BlockEndOp = BlockQC.getEndOp();
  QuantumCircuit::QuantumCircuitIterator It = BlockQC.begin();
  QuantumCircuitOp *CX1 = *It;
  It++;
  std::vector<QuantumCircuitOp *> Hs;
  for (unsigned Idx = 0; Idx < 2; Idx++) {
    Hs.push_back(*It);
    It++;
  }
  QuantumCircuitOp *CX2 = *It;
  QuantumOp *QOCX2 = static_cast<QuantumOp *>(CX2);

  QuantumOp *QOCX1 = static_cast<QuantumOp *>(CX1);
  QuantumOp *NewGate = QOCX1->copyGate(*Hs[1]);
  ASSERT_NE(NewGate, nullptr);
  EXPECT_EQ(BlockQC.getNumberGates(), 5);

  EXPECT_TRUE(NewGate->isPredecessorTo(QOCX2));
  ASSERT_TRUE(QOCX2->isSuccessorTo(NewGate));
  ASSERT_TRUE(NewGate->isSuccessorTo(Hs[0]));
  ASSERT_TRUE(NewGate->isSuccessorTo(Hs[1]));
  ASSERT_TRUE(Hs[0]->isPredecessorTo(NewGate));
  ASSERT_TRUE(Hs[1]->isPredecessorTo(NewGate));

  QuantumOp *H1 = static_cast<QuantumOp *>(Hs[0]);
  QuantumOp *H2 = static_cast<QuantumOp *>(Hs[1]);

  EXPECT_TRUE(dyn_cast<Instruction>(NewGate->getBaseValue())
                  ->comesBefore(dyn_cast<Instruction>(QOCX2->getBaseValue())));
  EXPECT_TRUE(
      dyn_cast<Instruction>(H1->getBaseValue())
          ->comesBefore(dyn_cast<Instruction>(NewGate->getBaseValue())));
  EXPECT_TRUE(
      dyn_cast<Instruction>(H2->getBaseValue())
          ->comesBefore(dyn_cast<Instruction>(NewGate->getBaseValue())));
  EXPECT_TRUE(dyn_cast<Instruction>(QOCX1->getBaseValue())
                  ->comesBefore(dyn_cast<Instruction>(H1->getBaseValue())));
  EXPECT_TRUE(dyn_cast<Instruction>(QOCX1->getBaseValue())
                  ->comesBefore(dyn_cast<Instruction>(H2->getBaseValue())));
}

TEST(QuantumCircuitTest, CopyGateToDifferentCircuitNoArgs) {
  LLVMContext C;
  std::unique_ptr<Module> M = parseIR(C, *&Code_Generated);
  ParaRef::setModule(*M);

  QuantumModule QM;
  runInitialPasses(*M, QM);

  QuantumKernel *F = FindKernelInModule(QM, "_Z18qbitref_dependencyv.stub");
  BasicBlock *BB = FindBlockInFunction(*(F->getFunction()), "entry");

  QuantumKernel *NewF =
      FindKernelInModule(QM, "_Z34simple_kernel_call_canonical_blockv.stub");
  BasicBlock *NewBB = FindBlockInFunction(*(NewF->getFunction()), "entry");

  QuantumCircuit &BlockQC = *QuantumCircuit::Create(QM, *BB);
  StartingOp *BlockStartOp = BlockQC.getStartOp();
  EndingOp *BlockEndOp = BlockQC.getEndOp();
  QuantumCircuit::QuantumCircuitIterator It = BlockQC.begin();
  QuantumCircuitOp *CX1 = *It;
  It++;
  std::vector<QuantumCircuitOp *> Hs;
  for (unsigned Idx = 0; Idx < 2; Idx++) {
    Hs.push_back(*It);
    It++;
  }
  QuantumCircuitOp *CX2 = *It;
  QuantumOp *QOCX2 = static_cast<QuantumOp *>(CX2);

  QuantumCircuit &NewBlockQC = *QuantumCircuit::Create(QM, *NewBB);
  StartingOp *NewBlockStartOp = NewBlockQC.getStartOp();
  EndingOp *NewBlockEndOp = NewBlockQC.getEndOp();
  It = NewBlockQC.begin();
  std::vector<QuantumOp *> NewPrepZs;
  for (unsigned Idx = 0; Idx < 2; Idx++) {
    NewPrepZs.push_back(static_cast<QuantumOp *>(*It));
    It++;
  }
  QuantumOp *NewCanonicalOp = static_cast<QuantumOp *>(*It);
  It++;
  std::vector<QuantumOp *> NewHs;
  for (unsigned Idx = 0; Idx < 3; Idx++) {
    NewHs.push_back(static_cast<QuantumOp *>(*It));
    It++;
  }

  QuantumOp *QOCX1 = static_cast<QuantumOp *>(CX1);
  QuantumOp *NewGate = QOCX1->copyGate(*NewCanonicalOp);
  ASSERT_NE(NewGate, nullptr);
  EXPECT_EQ(BlockQC.getNumberGates(), 4);
  EXPECT_EQ(NewBlockQC.getNumberGates(), 7);

  ASSERT_TRUE(NewGate->isPredecessorTo(NewHs[0]));
  ASSERT_TRUE(NewGate->isPredecessorTo(NewHs[1]));
  ASSERT_TRUE(NewHs[0]->isSuccessorTo(NewGate));
  ASSERT_TRUE(NewHs[1]->isSuccessorTo(NewGate));
  ASSERT_TRUE(NewGate->isSuccessorTo(NewCanonicalOp));
  EXPECT_TRUE(NewCanonicalOp->isPredecessorTo(NewGate));

  EXPECT_TRUE(
      dyn_cast<Instruction>(NewGate->getBaseValue())
          ->comesBefore(dyn_cast<Instruction>(NewHs[0]->getBaseValue())));
  EXPECT_TRUE(
      dyn_cast<Instruction>(NewGate->getBaseValue())
          ->comesBefore(dyn_cast<Instruction>(NewHs[0]->getBaseValue())));
  EXPECT_TRUE(
      dyn_cast<Instruction>(NewCanonicalOp->getBaseValue())
          ->comesBefore(dyn_cast<Instruction>(NewGate->getBaseValue())));
}

TEST(QuantumCircuitTest, CopyGateToDifferentCircuitQubitArgs) {
  LLVMContext C;
  std::unique_ptr<Module> M = parseIR(C, *&Code_Generated);
  ParaRef::setModule(*M);

  QuantumModule QM;
  runInitialPasses(*M, QM);

  QuantumKernel *F =
      FindKernelInModule(QM, "_Z23kernel_with_qubit_arg_1Pt.stub");
  BasicBlock *BB = FindBlockInFunction(*(F->getFunction()), "entry");

  QuantumKernel *NewF =
      FindKernelInModule(QM, "_Z23kernel_with_qubit_arg_2Pt.stub");
  BasicBlock *NewBB = FindBlockInFunction(*(NewF->getFunction()), "entry");

  QuantumCircuit &BlockQC = *QuantumCircuit::Create(QM, *BB);
  StartingOp *BlockStartOp = BlockQC.getStartOp();
  EndingOp *BlockEndOp = BlockQC.getEndOp();
  QuantumCircuit::QuantumCircuitIterator It = BlockQC.begin();
  QuantumOp *RZ = static_cast<QuantumOp *>(*It);

  QuantumCircuit &NewBlockQC = *QuantumCircuit::Create(QM, *NewBB);
  StartingOp *NewBlockStartOp = NewBlockQC.getStartOp();
  EndingOp *NewBlockEndOp = NewBlockQC.getEndOp();
  It = NewBlockQC.begin();
  QuantumOp *RX = static_cast<QuantumOp *>(*It);

  std::map<unsigned, QuantumOpParam> Mapping;
  QbitRef QR(NewF->getFunction()->getArg(0));
  Mapping.emplace(0, QuantumOpParam(OpParamTy::QUBITPTR, QR));
  QuantumOp *NewGate = RZ->copyGate(*RX, Mapping);
  ASSERT_NE(NewGate, nullptr);
  EXPECT_EQ(BlockQC.getNumberGates(), 1);
  EXPECT_EQ(NewBlockQC.getNumberGates(), 2);

  SmallVector<std::pair<QbitRef, unsigned>, 2> &Qubits = RX->getQubits();
  ASSERT_EQ(Qubits[0].first.getRegister(), NewF->getFunction()->getArg(0));

  EXPECT_TRUE(RX->isPredecessorTo(NewGate));
  ASSERT_TRUE(NewGate->isSuccessorTo(RX));
  EXPECT_TRUE(NewGate->isPredecessorTo(NewBlockEndOp));
  EXPECT_TRUE(BlockStartOp->isPredecessorTo(RZ));
  ASSERT_TRUE(BlockEndOp->isSuccessorTo(RZ));

  EXPECT_TRUE(
      dyn_cast<Instruction>(RX->getBaseValue())
          ->comesBefore(dyn_cast<Instruction>(NewGate->getBaseValue())));
}

TEST(QuantumCircuitTest, CopyGateToDifferentCircuitParamArgs) {
  LLVMContext C;
  std::unique_ptr<Module> M = parseIR(C, *&Code_Generated);
  ParaRef::setModule(*M);

  QuantumModule QM;
  runInitialPasses(*M, QM);

  QuantumKernel *F = FindKernelInModule(QM, "_Z18kernel_with_doubled.stub");
  BasicBlock *BB = FindBlockInFunction(*(F->getFunction()), "aqcc.quantum");

  QuantumKernel *NewF =
      FindKernelInModule(QM, "_Z26kernel_with_int_and_doubleid.stub");
  BasicBlock *NewBB =
      FindBlockInFunction(*(NewF->getFunction()), "aqcc.quantum");

  QuantumCircuit &BlockQC = *QuantumCircuit::Create(QM, *BB);
  StartingOp *BlockStartOp = BlockQC.getStartOp();
  EndingOp *BlockEndOp = BlockQC.getEndOp();
  QuantumCircuit::QuantumCircuitIterator It = BlockQC.begin();
  QuantumOp *RZ1 = static_cast<QuantumOp *>(*It);

  QuantumCircuit &NewBlockQC = *QuantumCircuit::Create(QM, *NewBB);
  StartingOp *NewBlockStartOp = NewBlockQC.getStartOp();
  EndingOp *NewBlockEndOp = NewBlockQC.getEndOp();
  It = NewBlockQC.begin();
  QuantumOp *RZ2 = static_cast<QuantumOp *>(*It);

  std::map<unsigned, QuantumOpParam> Mapping;
  ParaRef PR(NewF->getFunction()->getArg(1));
  Mapping.emplace(0, QuantumOpParam(OpParamTy::GATEPARAM, PR));
  QuantumOp *NewGate = RZ1->copyGate(*RZ2, Mapping);
  ASSERT_NE(NewGate, nullptr);
  EXPECT_EQ(BlockQC.getNumberGates(), 1);
  EXPECT_EQ(NewBlockQC.getNumberGates(), 2);

  SmallVector<std::pair<ParaRef, unsigned>, 2> &Params =
      RZ2->getResolvedParams();
  ASSERT_EQ(Params[0].first.getValue(), NewF->getFunction()->getArg(1));

  EXPECT_TRUE(RZ2->isPredecessorTo(NewGate));
  ASSERT_TRUE(NewGate->isSuccessorTo(RZ2));
  EXPECT_TRUE(NewGate->isPredecessorTo(NewBlockEndOp));
  EXPECT_TRUE(BlockStartOp->isPredecessorTo(RZ1));
  ASSERT_TRUE(BlockEndOp->isSuccessorTo(RZ1));

  EXPECT_TRUE(
      dyn_cast<Instruction>(RZ2->getBaseValue())
          ->comesBefore(dyn_cast<Instruction>(NewGate->getBaseValue())));
}

TEST(QuantumCircuitTest, CopyGateToDifferentCircuitLocalQubit) {
  LLVMContext C;
  std::unique_ptr<Module> M = parseIR(C, *&Code_Generated);
  ParaRef::setModule(*M);

  QuantumModule QM;
  runInitialPasses(*M, QM);

  QuantumKernel *F =
      FindKernelInModule(QM, "_Z23kernel_with_local_qubitd.stub");
  BasicBlock *BB = FindBlockInFunction(*(F->getFunction()), "aqcc.quantum");

  QuantumKernel *NewF =
      FindKernelInModule(QM, "_Z26kernel_with_int_and_doubleid.stub");
  BasicBlock *NewBB =
      FindBlockInFunction(*(NewF->getFunction()), "aqcc.quantum");
  BasicBlock *NewEntryBB = FindBlockInFunction(*(NewF->getFunction()), "entry");

  QuantumCircuit &BlockQC = *QuantumCircuit::Create(QM, *BB);
  StartingOp *BlockStartOp = BlockQC.getStartOp();
  EndingOp *BlockEndOp = BlockQC.getEndOp();
  QuantumCircuit::QuantumCircuitIterator It = BlockQC.begin();
  QuantumOp *RXY = static_cast<QuantumOp *>(*It);

  QuantumCircuit &NewBlockQC = *QuantumCircuit::Create(QM, *NewBB);
  StartingOp *NewBlockStartOp = NewBlockQC.getStartOp();
  EndingOp *NewBlockEndOp = NewBlockQC.getEndOp();
  It = NewBlockQC.begin();
  QuantumOp *RZ = static_cast<QuantumOp *>(*It);

  QuantumOp *NewGate = RXY->copyGate(&NewBlockQC);
  ASSERT_NE(NewGate, nullptr);
  EXPECT_EQ(BlockQC.getNumberGates(), 2);
  EXPECT_EQ(NewBlockQC.getNumberGates(), 2);

  EXPECT_TRUE(RZ->isPredecessorTo(NewBlockEndOp));
  ASSERT_FALSE(RZ->isPredecessorTo(NewGate));
  ASSERT_TRUE(NewGate->isSuccessorTo(NewBlockStartOp));
  EXPECT_TRUE(NewGate->isPredecessorTo(NewBlockEndOp));

  EXPECT_TRUE(
      dyn_cast<Instruction>(RZ->getBaseValue())
          ->comesBefore(dyn_cast<Instruction>(NewGate->getBaseValue())));
  EXPECT_TRUE(isa<AllocaInst>(&*NewEntryBB->begin()));
}