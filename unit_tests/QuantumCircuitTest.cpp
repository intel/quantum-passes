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

#include "IntelQuantumPasses/QuantumUtils/QuantumCircuit.h"
#include "IntelQuantumPasses/QuantumAnalysis/QuantumAnnotationsToJson.h"
#include "IntelQuantumPasses/QuantumAnalysis/QuantumCompilerLinkage.h"
#include "IntelQuantumPasses/QuantumAnalysis/QuantumGateIdentifiers.h"
#include "IntelQuantumPasses/QuantumAnalysis/QuantumIterator.h"
#include "IntelQuantumPasses/QuantumAnalysis/QuantumRefs.h"
#include "IntelQuantumPasses/QuantumAnalysis/QuantumSpinNativeToJson.h"

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

/// Testing if the start and end operation nodes are connecting when the basic
/// block the circuit is constructed from is empty.
TEST(QuantumCircuitTest, EmptyBasicBlock) {
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
  StartingOp *StartOp = QC.getStartOp();
  EndingOp *EndOp = QC.getEndOp();

  EXPECT_EQ(StartOp->getKind(), OpKind::Start);
  EXPECT_EQ(EndOp->getKind(), OpKind::End);

  EXPECT_EQ(StartOp->getPredecessors().size(), 0);
  EXPECT_EQ(StartOp->getSuccessors().size(), 1);
  EXPECT_EQ(EndOp->getSuccessors().size(), 0);
  EXPECT_EQ(EndOp->getPredecessors().size(), 1);
  EXPECT_TRUE(StartOp->isPredecessorTo(EndOp));
  EXPECT_TRUE(EndOp->isSuccessorTo(StartOp));
}

/// Testing if an empty kernel with an empty block successfully connects the
/// start node of the kernel to the start node of the block and the end node of
/// the block to the end node of the kernel.
TEST(QuantumCircuitTest, EmptyKernel) {
  LLVMContext C;
  std::unique_ptr<Module> M = parseIR(C, *&Code_Generated);
  ParaRef::setModule(*M);

  QuantumModule QM;
  runInitialPasses(*M, QM);

  QuantumKernel *EmptyFunction =
      FindKernelInModule(QM, "_Z12empty_kernelv.stub");

  QuantumCircuit &QC = *QuantumCircuit::Create(QM, *EmptyFunction);
  EXPECT_EQ(QC.getNumberGates(), 0);
  std::vector<QuantumCircuit *> CircuitList = QC.getCircuitList();
  EXPECT_EQ(CircuitList.size(), 1);
  StartingOp *MainStartOp = QC.getStartOp();
  EndingOp *MainEndOp = QC.getEndOp();
  QuantumCircuit *BlockQC = CircuitList[0];
  StartingOp *BlockStartOp = BlockQC->getStartOp();
  EndingOp *BlockEndOp = BlockQC->getEndOp();

  EXPECT_EQ(MainStartOp->getKind(), OpKind::Start);
  EXPECT_EQ(MainEndOp->getKind(), OpKind::End);

  EXPECT_EQ(BlockStartOp->getKind(), OpKind::Start);
  EXPECT_EQ(BlockEndOp->getKind(), OpKind::End);

  EXPECT_EQ(MainStartOp->getPredecessors().size(), 0);
  EXPECT_EQ(MainStartOp->getSuccessors().size(), 1);
  EXPECT_EQ(MainEndOp->getSuccessors().size(), 0);
  EXPECT_EQ(MainEndOp->getPredecessors().size(), 1);
  EXPECT_EQ(BlockStartOp->getSuccessors().size(), 1);
  EXPECT_EQ(BlockStartOp->getPredecessors().size(), 1);
  EXPECT_EQ(BlockEndOp->getSuccessors().size(), 1);
  EXPECT_EQ(BlockEndOp->getPredecessors().size(), 1);

  EXPECT_TRUE(MainStartOp->isPredecessorTo(BlockStartOp));
  EXPECT_TRUE(BlockStartOp->isSuccessorTo(MainStartOp));
  EXPECT_TRUE(BlockStartOp->isPredecessorTo(BlockEndOp));
  EXPECT_TRUE(BlockEndOp->isSuccessorTo(BlockStartOp));
  EXPECT_TRUE(BlockEndOp->isPredecessorTo(MainEndOp));
  EXPECT_TRUE(MainEndOp->isSuccessorTo(BlockEndOp));
}

/// Make sure intrinsic instructions get added to the QuantumCircuit.
TEST(QuantumCircuitTest, IntrinsicsOnlyBlock) {
  LLVMContext C;
  std::unique_ptr<Module> M = parseIR(C, *&Code_Generated);
  ParaRef::setModule(*M);

  QuantumModule QM;
  runInitialPasses(*M, QM);

  QuantumKernel *F = FindKernelInModule(QM, "_Z22simple_intrinsic_blockv.stub");
  BasicBlock *BB = FindBlockInFunction(*(F->getFunction()), "entry");

  QuantumCircuit &BlockQC = *QuantumCircuit::Create(QM, *BB);
  StartingOp *BlockStartOp = BlockQC.getStartOp();
  EndingOp *BlockEndOp = BlockQC.getEndOp();
  QuantumCircuitOp *QCO = *(BlockStartOp->getSuccessors().begin());
  EXPECT_EQ(BlockQC.getNumberGates(), 1);
  EXPECT_EQ(QCO->getKind(), OpKind::Quantum);

  EXPECT_EQ(BlockStartOp->getSuccessors().size(), 1);
  EXPECT_EQ(BlockStartOp->getPredecessors().size(), 0);
  EXPECT_EQ(QCO->getSuccessors().size(), 1);
  EXPECT_EQ(QCO->getPredecessors().size(), 1);
  EXPECT_EQ(BlockEndOp->getSuccessors().size(), 0);
  EXPECT_EQ(BlockEndOp->getPredecessors().size(), 1);

  EXPECT_TRUE(BlockStartOp->isPredecessorTo(QCO));
  EXPECT_TRUE(QCO->isSuccessorTo(BlockStartOp));
  EXPECT_TRUE(QCO->isPredecessorTo(BlockEndOp));
  EXPECT_TRUE(BlockEndOp->isSuccessorTo(QCO));
}

/// Make sure canonical instructions get added to the QuantumCircuit.
TEST(QuantumCircuitTest, CanonicalsBlock) {
  LLVMContext C;
  std::unique_ptr<Module> M = parseIR(C, *&Code_Generated);
  ParaRef::setModule(*M);

  QuantumModule QM;
  runInitialPasses(*M, QM);

  QuantumKernel *F = FindKernelInModule(QM, "_Z22simple_canonical_blockv.stub");
  BasicBlock *BB = FindBlockInFunction(*(F->getFunction()), "entry");

  QuantumCircuit &BlockQC = *QuantumCircuit::Create(QM, *BB);
  StartingOp *BlockStartOp = BlockQC.getStartOp();
  EndingOp *BlockEndOp = BlockQC.getEndOp();
  QuantumCircuitOp *QCO = *(BlockStartOp->getSuccessors().begin());
  EXPECT_EQ(BlockQC.getNumberGates(), 1);
  EXPECT_EQ(QCO->getKind(), OpKind::Quantum);

  EXPECT_EQ(BlockStartOp->getSuccessors().size(), 1);
  EXPECT_EQ(BlockStartOp->getPredecessors().size(), 0);
  EXPECT_EQ(QCO->getSuccessors().size(), 1);
  EXPECT_EQ(QCO->getPredecessors().size(), 1);
  EXPECT_EQ(BlockEndOp->getSuccessors().size(), 0);
  EXPECT_EQ(BlockEndOp->getPredecessors().size(), 1);

  EXPECT_TRUE(BlockStartOp->isPredecessorTo(QCO));
  EXPECT_TRUE(QCO->isSuccessorTo(BlockStartOp));
  EXPECT_TRUE(QCO->isPredecessorTo(BlockEndOp));
  EXPECT_TRUE(BlockEndOp->isSuccessorTo(QCO));
}

/// Test to make sure a quantum kernel gets added to the QuantumCircuit.
TEST(QuantumCircuitTest, KernelCallBlock) {
  LLVMContext C;
  std::unique_ptr<Module> M = parseIR(C, *&Code_Generated);
  ParaRef::setModule(*M);

  QuantumModule QM;
  runInitialPasses(*M, QM);

  QuantumKernel *F =
      FindKernelInModule(QM, "_Z24simple_kernel_call_blockv.stub");
  BasicBlock *BB = FindBlockInFunction(*(F->getFunction()), "entry");

  QuantumCircuit &BlockQC = *QuantumCircuit::Create(QM, *BB);
  StartingOp *BlockStartOp = BlockQC.getStartOp();
  EndingOp *BlockEndOp = BlockQC.getEndOp();
  QuantumCircuitOp *QCO = *(BlockStartOp->getSuccessors().begin());
  EXPECT_EQ(BlockQC.getNumberGates(), 1);
  EXPECT_EQ(QCO->getKind(), OpKind::Quantum);

  EXPECT_EQ(BlockStartOp->getSuccessors().size(), 1);
  EXPECT_EQ(BlockStartOp->getPredecessors().size(), 0);
  EXPECT_EQ(QCO->getSuccessors().size(), 1);
  EXPECT_EQ(QCO->getPredecessors().size(), 1);
  EXPECT_EQ(BlockEndOp->getSuccessors().size(), 0);
  EXPECT_EQ(BlockEndOp->getPredecessors().size(), 1);

  EXPECT_TRUE(BlockStartOp->isPredecessorTo(QCO));
  EXPECT_TRUE(QCO->isSuccessorTo(BlockStartOp));
  EXPECT_TRUE(QCO->isPredecessorTo(BlockEndOp));
  EXPECT_TRUE(BlockEndOp->isSuccessorTo(QCO));
}

/// Make sure that all nodes pass through the quantum kernel call when it is
/// embedded between several quantum calls, and when it is added, it becomes the
/// new start block for any qubit used afterwards.
TEST(QuantumCircuitTest, CanonicalAndKernelCallBlock) {
  LLVMContext C;
  std::unique_ptr<Module> M = parseIR(C, *&Code_Generated);
  ParaRef::setModule(*M);

  QuantumModule QM;
  runInitialPasses(*M, QM);

  QuantumKernel *F =
      FindKernelInModule(QM, "_Z34simple_kernel_call_canonical_blockv.stub");
  BasicBlock *BB = FindBlockInFunction(*(F->getFunction()), "entry");

  QuantumCircuit &BlockQC = *QuantumCircuit::Create(QM, *BB);
  StartingOp *BlockStartOp = BlockQC.getStartOp();
  EndingOp *BlockEndOp = BlockQC.getEndOp();
  QuantumCircuit::QuantumCircuitIterator It = BlockQC.begin();
  EXPECT_EQ(BlockQC.getNumberGates(), 6);
  std::vector<QuantumCircuitOp *> PrepZs;
  for (unsigned Idx = 0; Idx < 2; Idx++) {
    PrepZs.push_back(*It);
    It++;
  }
  QuantumCircuitOp *CanonicalCall = *(It);
  It++;
  std::vector<QuantumCircuitOp *> Hs;
  for (unsigned Idx = 0; Idx < 3; Idx++) {
    Hs.push_back(*It);
    It++;
  }

  EXPECT_EQ(BlockStartOp->getSuccessors().size(), 3);
  EXPECT_EQ(BlockStartOp->getPredecessors().size(), 0);
  for (QuantumCircuitOp *QCO : PrepZs) {
    EXPECT_EQ(QCO->getSuccessors().size(), 1);
    EXPECT_EQ(QCO->getPredecessors().size(), 1);
  }
  EXPECT_EQ(CanonicalCall->getSuccessors().size(), 3);
  EXPECT_EQ(CanonicalCall->getPredecessors().size(), 3);
  for (QuantumCircuitOp *QCO : Hs) {
    EXPECT_EQ(QCO->getSuccessors().size(), 1);
    EXPECT_EQ(QCO->getPredecessors().size(), 1);
  }
  EXPECT_EQ(BlockEndOp->getSuccessors().size(), 0);
  EXPECT_EQ(BlockEndOp->getPredecessors().size(), 3);

  for (QuantumCircuitOp *QCO : PrepZs) {
    EXPECT_TRUE(BlockStartOp->isPredecessorTo(QCO));
    EXPECT_TRUE(QCO->isSuccessorTo(BlockStartOp));
    EXPECT_TRUE(QCO->isPredecessorTo(CanonicalCall));
    EXPECT_TRUE(CanonicalCall->isSuccessorTo(QCO));
  }

  for (QuantumCircuitOp *QCO : Hs) {
    EXPECT_TRUE(CanonicalCall->isPredecessorTo(QCO));
    EXPECT_TRUE(QCO->isSuccessorTo(CanonicalCall));
    EXPECT_TRUE(QCO->isPredecessorTo(BlockEndOp));
    EXPECT_TRUE(BlockEndOp->isSuccessorTo(QCO));
  }
}

/// Test that we successfully create dependency of a two qubit gate, followed by
/// a one qubit gate on each qubit when using qubit references.
TEST(QuantumCircuitTest, TwoQubitOpQbitRefs) {
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
  EXPECT_EQ(BlockQC.getNumberGates(), 4);
  QuantumCircuitOp *CX1 = *It;
  It++;
  std::vector<QuantumCircuitOp *> Hs;
  for (unsigned Idx = 0; Idx < 2; Idx++) {
    Hs.push_back(*It);
    It++;
  }
  QuantumCircuitOp *CX2 = *It;

  EXPECT_EQ(BlockStartOp->getSuccessors().size(), 1);
  EXPECT_EQ(BlockStartOp->getPredecessors().size(), 0);
  EXPECT_EQ(CX1->getSuccessors().size(), 2);
  EXPECT_EQ(CX1->getPredecessors().size(), 1);
  for (QuantumCircuitOp *QCO : Hs) {
    EXPECT_EQ(QCO->getSuccessors().size(), 1);
    EXPECT_EQ(QCO->getPredecessors().size(), 1);
  }
  EXPECT_EQ(CX2->getSuccessors().size(), 1);
  EXPECT_EQ(CX2->getPredecessors().size(), 2);
  EXPECT_EQ(BlockEndOp->getSuccessors().size(), 0);
  EXPECT_EQ(BlockEndOp->getPredecessors().size(), 1);

  for (QuantumCircuitOp *QCO : Hs) {
    EXPECT_TRUE(CX1->isPredecessorTo(QCO));
    EXPECT_TRUE(QCO->isSuccessorTo(CX1));
    EXPECT_TRUE(QCO->isPredecessorTo(CX2));
    EXPECT_TRUE(CX2->isSuccessorTo(QCO));
  }
}

/// Test that we successfully create a dependency from a two qubit gate,
/// followed by a one qubit gate one each qubit when using QIDs.
TEST(QuantumCircuitTest, TwoQubitOpQIDs) {
  LLVMContext C;
  std::unique_ptr<Module> M = parseIR(C, *&Code_Generated);
  ParaRef::setModule(*M);

  QuantumModule QM;
  runInitialPasses(*M, QM);

  QuantumKernel *F = FindKernelInModule(QM, "_Z14qid_dependencyv.stub");
  BasicBlock *BB = FindBlockInFunction(*(F->getFunction()), "entry");

  QuantumCircuit &BlockQC = *QuantumCircuit::Create(QM, *BB);
  StartingOp *BlockStartOp = BlockQC.getStartOp();
  EndingOp *BlockEndOp = BlockQC.getEndOp();
  QuantumCircuit::QuantumCircuitIterator It = BlockQC.begin();
  EXPECT_EQ(BlockQC.getNumberGates(), 4);
  QuantumCircuitOp *CPhase1 = *It;
  It++;
  std::vector<QuantumCircuitOp *> Rxys;
  for (unsigned Idx = 0; Idx < 2; Idx++) {
    Rxys.push_back(*It);
    It++;
  }
  QuantumCircuitOp *CPhase2 = *It;

  EXPECT_EQ(BlockStartOp->getSuccessors().size(), 1);
  EXPECT_EQ(BlockStartOp->getPredecessors().size(), 0);
  EXPECT_EQ(CPhase1->getSuccessors().size(), 2);
  EXPECT_EQ(CPhase1->getPredecessors().size(), 1);
  for (QuantumCircuitOp *QCO : Rxys) {
    EXPECT_EQ(QCO->getSuccessors().size(), 1);
    EXPECT_EQ(QCO->getPredecessors().size(), 1);
  }
  EXPECT_EQ(CPhase2->getSuccessors().size(), 1);
  EXPECT_EQ(CPhase2->getPredecessors().size(), 2);
  EXPECT_EQ(BlockEndOp->getSuccessors().size(), 0);
  EXPECT_EQ(BlockEndOp->getPredecessors().size(), 1);

  for (QuantumCircuitOp *QCO : Rxys) {
    EXPECT_TRUE(CPhase1->isPredecessorTo(QCO));
    EXPECT_TRUE(QCO->isSuccessorTo(CPhase1));
    EXPECT_TRUE(QCO->isPredecessorTo(CPhase2));
    EXPECT_TRUE(CPhase2->isSuccessorTo(QCO));
  }
}

/// Check that we correctly identify ancestors in the operation graph for
/// straight line operations.
TEST(QuantumCircuitTest, BasicAncestorTest) {
  LLVMContext C;
  std::unique_ptr<Module> M = parseIR(C, *&Code_Generated);
  ParaRef::setModule(*M);

  QuantumModule QM;
  runInitialPasses(*M, QM);

  QuantumKernel *F = FindKernelInModule(QM, "_Z10basic_linev.stub");
  BasicBlock *BodyBB = FindBlockInFunction(*(F->getFunction()), "body");
  BasicBlock *EntryBB = FindBlockInFunction(*(F->getFunction()), "entry");
  BasicBlock *EndBB = FindBlockInFunction(*(F->getFunction()), "end_block");

  QuantumCircuit &KernelQC = *QuantumCircuit::Create(QM, *F);
  StartingOp *KernelStartOp = KernelQC.getStartOp();
  EndingOp *KernelEndOp = KernelQC.getEndOp();

  std::vector<QuantumCircuit *> QCList = KernelQC.getCircuitList();
  EXPECT_EQ(QCList.size(), 3);
  EXPECT_EQ(KernelQC.getNumberGates(), 2);
  std::vector<QuantumCircuit *> RefCircuits = {nullptr, nullptr, nullptr};
  for (QuantumCircuit *QC : QCList) {
    if (QC->getBaseValue() == BodyBB)
      RefCircuits[1] = QC;
    else if (QC->getBaseValue() == EntryBB)
      RefCircuits[0] = QC;
    else if (QC->getBaseValue() == EndBB)
      RefCircuits[2] = QC;
  }

  for (unsigned Idx = 0; Idx < RefCircuits.size(); Idx++) {
    StartingOp *Q1Start = RefCircuits[Idx]->getStartOp();
    EXPECT_TRUE(KernelQC.isAncestorTo(KernelStartOp, Q1Start));
    EXPECT_FALSE(KernelQC.isAncestorTo(Q1Start, KernelStartOp));
    EXPECT_TRUE(KernelQC.isAncestorTo(Q1Start, KernelEndOp));
  }

  for (unsigned Idx = 0; Idx < RefCircuits.size() - 1; Idx++) {
    StartingOp *Q1Start = RefCircuits[Idx]->getStartOp();
    EndingOp *Q1End = RefCircuits[Idx]->getEndOp();
    StartingOp *Q2Start = RefCircuits[Idx + 1]->getStartOp();
    EndingOp *Q2End = RefCircuits[Idx + 1]->getEndOp();
    EXPECT_TRUE(Q1End->isPredecessorTo(Q2Start));
    EXPECT_TRUE(Q2Start->isSuccessorTo(Q1End));
    EXPECT_FALSE(RefCircuits[Idx]->isAncestorTo(Q1Start, Q2Start));
    EXPECT_TRUE(RefCircuits[Idx]->isAncestorTo(Q1Start, Q1End));
    EXPECT_TRUE(KernelQC.isAncestorTo(Q1Start, Q2Start));
    EXPECT_FALSE(KernelQC.isAncestorTo(Q2Start, Q1Start));
  }

  QuantumCircuit::QuantumCircuitIterator It = RefCircuits[1]->begin();
  QuantumCircuitOp *Op1 = (*It);
  It++;
  QuantumCircuitOp *Op2 = (*It);
  EXPECT_TRUE(RefCircuits[1]->isAncestorTo(Op1, Op2));
  EXPECT_TRUE(KernelQC.isAncestorTo(Op1, Op2));
  EXPECT_FALSE(RefCircuits[1]->isAncestorTo(Op2, Op1));
  EXPECT_FALSE(KernelQC.isAncestorTo(Op2, Op1));
  EXPECT_FALSE(RefCircuits[0]->isAncestorTo(Op1, Op2));
  EXPECT_FALSE(RefCircuits[2]->isAncestorTo(Op1, Op2));
}

/// Check that we correctly identify ancestors in the operation graph when there
/// is a loop of a single basic block.
TEST(QuantumCircuitTest, SingleBlockLoopAncestorTest) {
  LLVMContext C;
  std::unique_ptr<Module> M = parseIR(C, *&Code_Generated);
  ParaRef::setModule(*M);

  QuantumModule QM;
  runInitialPasses(*M, QM);

  QuantumKernel *F = FindKernelInModule(QM, "_Z10basic_loopv.stub");
  BasicBlock *BodyBB = FindBlockInFunction(*(F->getFunction()), "for.body");
  BasicBlock *EntryBB = FindBlockInFunction(*(F->getFunction()), "entry");
  BasicBlock *EndBB =
      FindBlockInFunction(*(F->getFunction()), "for.cond.cleanup");

  QuantumCircuit &KernelQC = *QuantumCircuit::Create(QM, *F);
  StartingOp *KernelStartOp = KernelQC.getStartOp();
  EndingOp *KernelEndOp = KernelQC.getEndOp();

  std::vector<QuantumCircuit *> QCList = KernelQC.getCircuitList();
  EXPECT_EQ(QCList.size(), 3);
  EXPECT_EQ(KernelQC.getNumberGates(), 2);
  std::vector<QuantumCircuit *> RefCircuits = {nullptr, nullptr, nullptr};
  for (QuantumCircuit *QC : QCList) {
    if (QC->getBaseValue() == BodyBB)
      RefCircuits[1] = QC;
    else if (QC->getBaseValue() == EntryBB)
      RefCircuits[0] = QC;
    else if (QC->getBaseValue() == EndBB)
      RefCircuits[2] = QC;
  }

  for (unsigned Idx = 0; Idx < RefCircuits.size(); Idx++) {
    StartingOp *Q1Start = RefCircuits[Idx]->getStartOp();
    EXPECT_TRUE(KernelQC.isAncestorTo(KernelStartOp, Q1Start));
    EXPECT_FALSE(KernelQC.isAncestorTo(Q1Start, KernelStartOp));
    EXPECT_TRUE(KernelQC.isAncestorTo(Q1Start, KernelEndOp));
  }

  for (unsigned Idx = 0; Idx < RefCircuits.size() - 1; Idx++) {
    StartingOp *Q1Start = RefCircuits[Idx]->getStartOp();
    EndingOp *Q1End = RefCircuits[Idx]->getEndOp();
    StartingOp *Q2Start = RefCircuits[Idx + 1]->getStartOp();
    EndingOp *Q2End = RefCircuits[Idx + 1]->getEndOp();
    EXPECT_TRUE(Q1End->isPredecessorTo(Q2Start));
    EXPECT_TRUE(Q2Start->isSuccessorTo(Q1End));
    EXPECT_FALSE(RefCircuits[Idx]->isAncestorTo(Q1Start, Q2Start));
    EXPECT_TRUE(RefCircuits[Idx]->isAncestorTo(Q1Start, Q1End));
    EXPECT_TRUE(KernelQC.isAncestorTo(Q1Start, Q2Start));
    EXPECT_FALSE(KernelQC.isAncestorTo(Q2Start, Q1Start));

    if (Idx == 1) {
      EXPECT_EQ(Q1End->getSuccessors().size(), 2);
      EXPECT_EQ(Q1Start->getPredecessors().size(), 2);

      EXPECT_FALSE(RefCircuits[Idx]->isAncestorTo(Q1Start, Q1Start));
      EXPECT_TRUE(KernelQC.isAncestorTo(Q1Start, Q1Start));
      EXPECT_FALSE(RefCircuits[Idx]->isAncestorTo(Q1End, Q1Start));
      EXPECT_TRUE(KernelQC.isAncestorTo(Q1End, Q1Start));
    }
  }

  QuantumCircuit::QuantumCircuitIterator It = RefCircuits[1]->begin();
  QuantumCircuitOp *Op1 = (*It);
  It++;
  QuantumCircuitOp *Op2 = (*It);
  EXPECT_TRUE(RefCircuits[1]->isAncestorTo(Op1, Op2));
  EXPECT_TRUE(KernelQC.isAncestorTo(Op1, Op2));
  EXPECT_FALSE(RefCircuits[1]->isAncestorTo(Op2, Op1));
  EXPECT_TRUE(KernelQC.isAncestorTo(Op2, Op1));
  EXPECT_FALSE(RefCircuits[0]->isAncestorTo(Op1, Op2));
  EXPECT_FALSE(RefCircuits[2]->isAncestorTo(Op1, Op2));
}

/// Check that we correctly identify ancestors in the operation graph when there
/// is a loop of several basic blocks.
TEST(QuantumCircuitTest, SeparatedBlockLoopAncestorTest) {
  LLVMContext C;
  std::unique_ptr<Module> M = parseIR(C, *&Code_Generated);
  ParaRef::setModule(*M);

  QuantumModule QM;
  runInitialPasses(*M, QM);

  QuantumKernel *F = FindKernelInModule(QM, "_Z16multi_block_loopv.stub");
  BasicBlock *BodyBB = FindBlockInFunction(*(F->getFunction()), "for.body");
  BasicBlock *EntryBB = FindBlockInFunction(*(F->getFunction()), "entry");
  BasicBlock *EndBB =
      FindBlockInFunction(*(F->getFunction()), "for.cond.cleanup");
  BasicBlock *Body1BB = FindBlockInFunction(*(F->getFunction()), "body1");

  QuantumCircuit &KernelQC = *QuantumCircuit::Create(QM, *F);
  StartingOp *KernelStartOp = KernelQC.getStartOp();
  EndingOp *KernelEndOp = KernelQC.getEndOp();

  std::vector<QuantumCircuit *> QCList = KernelQC.getCircuitList();
  EXPECT_EQ(QCList.size(), 4);
  EXPECT_EQ(KernelQC.getNumberGates(), 2);
  std::vector<QuantumCircuit *> RefCircuits = {nullptr, nullptr, nullptr,
                                               nullptr};
  for (QuantumCircuit *QC : QCList) {
    if (QC->getBaseValue() == BodyBB)
      RefCircuits[1] = QC;
    else if (QC->getBaseValue() == EntryBB)
      RefCircuits[0] = QC;
    else if (QC->getBaseValue() == EndBB)
      RefCircuits[3] = QC;
    else if (QC->getBaseValue() == Body1BB)
      RefCircuits[2] = QC;
  }

  for (unsigned Idx = 0; Idx < RefCircuits.size(); Idx++) {
    StartingOp *Q1Start = RefCircuits[Idx]->getStartOp();
    EXPECT_TRUE(KernelQC.isAncestorTo(KernelStartOp, Q1Start));
    EXPECT_FALSE(KernelQC.isAncestorTo(Q1Start, KernelStartOp));
    EXPECT_TRUE(KernelQC.isAncestorTo(Q1Start, KernelEndOp));
  }

  for (unsigned Idx = 0; Idx < RefCircuits.size() - 1; Idx++) {
    StartingOp *Q1Start = RefCircuits[Idx]->getStartOp();
    EndingOp *Q1End = RefCircuits[Idx]->getEndOp();
    StartingOp *Q2Start = RefCircuits[Idx + 1]->getStartOp();
    EndingOp *Q2End = RefCircuits[Idx + 1]->getEndOp();
    EXPECT_TRUE(Q1End->isPredecessorTo(Q2Start));
    EXPECT_TRUE(Q2Start->isSuccessorTo(Q1End));
    EXPECT_FALSE(RefCircuits[Idx]->isAncestorTo(Q1Start, Q2Start));
    EXPECT_TRUE(RefCircuits[Idx]->isAncestorTo(Q1Start, Q1End));
    EXPECT_TRUE(KernelQC.isAncestorTo(Q1Start, Q2Start));

    if (Idx == 1) {
      EXPECT_FALSE(Q1End->isPredecessorTo(Q1Start));
      EXPECT_FALSE(Q1Start->isPredecessorTo(Q1End));
      EXPECT_FALSE(RefCircuits[Idx]->isAncestorTo(Q1Start, Q1Start));
      EXPECT_TRUE(KernelQC.isAncestorTo(Q1Start, Q1Start));
      EXPECT_FALSE(RefCircuits[Idx]->isAncestorTo(Q1End, Q1Start));
      EXPECT_TRUE(KernelQC.isAncestorTo(Q1End, Q1Start));

      EXPECT_EQ(Q1End->getSuccessors().size(), 1);
      EXPECT_EQ(Q1Start->getPredecessors().size(), 2);

      EXPECT_TRUE(KernelQC.isAncestorTo(Q2Start, Q1Start));
      EXPECT_TRUE(KernelQC.isAncestorTo(Q2End, Q1Start));
    } else if (Idx == 2) {
      EXPECT_EQ(Q1End->getSuccessors().size(), 2);
      EXPECT_EQ(Q1Start->getPredecessors().size(), 1);
      EXPECT_FALSE(KernelQC.isAncestorTo(Q2Start, Q1Start));
    } else {
      EXPECT_EQ(Q1End->getSuccessors().size(), 1);
      EXPECT_EQ(Q1Start->getPredecessors().size(), 1);
      EXPECT_FALSE(KernelQC.isAncestorTo(Q2Start, Q1Start));
    }
  }

  QuantumCircuit::QuantumCircuitIterator It = RefCircuits[1]->begin();
  QuantumCircuitOp *Op1 = (*It);
  It++;
  QuantumCircuitOp *Op2 = (*It);
  EXPECT_TRUE(RefCircuits[1]->isAncestorTo(Op1, Op2));
  EXPECT_TRUE(KernelQC.isAncestorTo(Op1, Op2));
  EXPECT_FALSE(RefCircuits[1]->isAncestorTo(Op2, Op1));
  EXPECT_TRUE(KernelQC.isAncestorTo(Op2, Op1));
  EXPECT_FALSE(RefCircuits[0]->isAncestorTo(Op1, Op2));
  EXPECT_FALSE(RefCircuits[2]->isAncestorTo(Op1, Op2));
}

/// Check for correct iteration across two basic blocks with an unconditional
/// branch.
TEST(QuantumCircuitTest, BranchingCircuits) {
  LLVMContext C;
  std::unique_ptr<Module> M = parseIR(C, *&Code_Generated);
  ParaRef::setModule(*M);

  QuantumModule QM;
  runInitialPasses(*M, QM);

  QuantumKernel *F = FindKernelInModule(QM, "_Z12basic_branchb.stub");
  BasicBlock *BodyBB = FindBlockInFunction(*(F->getFunction()), "body");
  BasicBlock *EntryBB = FindBlockInFunction(*(F->getFunction()), "entry");
  BasicBlock *EndBB = FindBlockInFunction(*(F->getFunction()), "if.end");
  BasicBlock *Branch1BB = FindBlockInFunction(*(F->getFunction()), "if.then");
  BasicBlock *Branch2BB = FindBlockInFunction(*(F->getFunction()), "if.else");

  QuantumCircuit &KernelQC = *QuantumCircuit::Create(QM, *F);
  StartingOp *KernelStartOp = KernelQC.getStartOp();
  EndingOp *KernelEndOp = KernelQC.getEndOp();

  std::vector<QuantumCircuit *> QCList = KernelQC.getCircuitList();
  EXPECT_EQ(QCList.size(), 5);
  EXPECT_EQ(KernelQC.getNumberGates(), 4);
  std::vector<QuantumCircuit *> RefCircuits = {nullptr, nullptr, nullptr,
                                               nullptr, nullptr};
  for (QuantumCircuit *QC : QCList) {
    if (QC->getBaseValue() == BodyBB)
      RefCircuits[1] = QC;
    else if (QC->getBaseValue() == EntryBB)
      RefCircuits[0] = QC;
    else if (QC->getBaseValue() == EndBB)
      RefCircuits[4] = QC;
    else if (QC->getBaseValue() == Branch1BB)
      RefCircuits[2] = QC;
    else if (QC->getBaseValue() == Branch2BB)
      RefCircuits[3] = QC;
  }

  for (unsigned Idx = 0; Idx < 1; Idx++) {
    StartingOp *Q1Start = RefCircuits[Idx]->getStartOp();
    EndingOp *Q1End = RefCircuits[Idx]->getEndOp();
    StartingOp *Q2Start = RefCircuits[Idx + 1]->getStartOp();
    EndingOp *Q2End = RefCircuits[Idx + 1]->getEndOp();
    EXPECT_TRUE(Q1End->isPredecessorTo(Q2Start));
    EXPECT_TRUE(Q2Start->isSuccessorTo(Q1End));
  }

  EXPECT_EQ(RefCircuits[1]->getEndOp()->getSuccessors().size(), 2);
  EXPECT_EQ(RefCircuits[1]->getNumberGates(), 2);
  EXPECT_EQ(RefCircuits[1]->getStartOp()->getPredecessors().size(), 1);
  EXPECT_EQ(RefCircuits[4]->getStartOp()->getPredecessors().size(), 2);
  for (unsigned Idx = 2; Idx < 4; Idx++) {
    EXPECT_EQ(RefCircuits[Idx]->getNumberGates(), 1);
    StartingOp *Q1Start = RefCircuits[Idx]->getStartOp();
    EndingOp *Q1End = RefCircuits[Idx]->getEndOp();
    StartingOp *Q2Start = RefCircuits[4]->getStartOp();
    EndingOp *Q0End = RefCircuits[1]->getEndOp();
    EXPECT_EQ(Q1Start->getPredecessors().size(), 1);
    EXPECT_EQ(Q1End->getSuccessors().size(), 1);
    EXPECT_TRUE(Q1Start->isSuccessorTo(Q0End));
    EXPECT_TRUE(Q0End->isPredecessorTo(Q1Start));
    EXPECT_TRUE(Q1End->isPredecessorTo(Q2Start));
    EXPECT_TRUE(Q2Start->isSuccessorTo(Q1End));
  }
}

/// Check for correct iteration across two basic blocks with an unconditional
/// branch.
TEST(QuantumCircuitTest, BasicIterationTest) {
  LLVMContext C;
  std::unique_ptr<Module> M = parseIR(C, *&Code_Generated);
  ParaRef::setModule(*M);

  QuantumModule QM;
  runInitialPasses(*M, QM);

  QuantumKernel *F = FindKernelInModule(QM, "_Z10basic_linev.stub");
  BasicBlock *BodyBB = FindBlockInFunction(*(F->getFunction()), "body");
  BasicBlock *EntryBB = FindBlockInFunction(*(F->getFunction()), "entry");
  BasicBlock *EndBB = FindBlockInFunction(*(F->getFunction()), "end_block");

  QuantumCircuit &KernelQC = *QuantumCircuit::Create(QM, *F);
  StartingOp *KernelStartOp = KernelQC.getStartOp();
  EndingOp *KernelEndOp = KernelQC.getEndOp();

  std::vector<std::tuple<BasicBlock *, std::string, OpKind>> OpPairs = {
      {EntryBB, "", OpKind::Start},
      {BodyBB, "", OpKind::Start},
      {BodyBB, "_Z5PrepZRt", OpKind::Quantum},
      {BodyBB, "_Z1HRt", OpKind::Quantum},
      {EndBB, "", OpKind::Start}};

  std::vector<QuantumCircuitOp *> Ops;
  KernelQC.iterateGates(Ops);
  QuantumCircuit::QuantumCircuitIterator It = KernelQC.begin();
  EXPECT_EQ(OpPairs.size(), Ops.size());
  for (unsigned Idx = 0; Idx < Ops.size(); Idx++) {
    EXPECT_EQ(Ops[Idx]->getParentCircuit()->getBaseValue(),
              std::get<0>(OpPairs[Idx]));
    EXPECT_EQ(Ops[Idx]->getKind(), std::get<2>(OpPairs[Idx]));

    EXPECT_EQ((*It)->getParentCircuit()->getBaseValue(),
              std::get<0>(OpPairs[Idx]));
    EXPECT_EQ((*It)->getKind(), std::get<2>(OpPairs[Idx]));
    if (Idx == 2 || Idx == 3) {
      QuantumOp *Op = static_cast<QuantumOp *>(*It);
      EXPECT_EQ(Op->getCalledValue()->getName(), std::get<1>(OpPairs[Idx]));

      Op = static_cast<QuantumOp *>(Ops[Idx]);
      EXPECT_EQ(Op->getCalledValue()->getName(), std::get<1>(OpPairs[Idx]));
    }
    It++;
  }
}

/// Check for correct iteration when one block has a loop.
TEST(QuantumCircuitTest, LoopIterationTest) {
  LLVMContext C;
  std::unique_ptr<Module> M = parseIR(C, *&Code_Generated);
  ParaRef::setModule(*M);

  QuantumModule QM;
  runInitialPasses(*M, QM);

  QuantumKernel *F = FindKernelInModule(QM, "_Z10basic_loopv.stub");
  BasicBlock *BodyBB = FindBlockInFunction(*(F->getFunction()), "for.body");
  BasicBlock *EntryBB = FindBlockInFunction(*(F->getFunction()), "entry");
  BasicBlock *EndBB =
      FindBlockInFunction(*(F->getFunction()), "for.cond.cleanup");

  QuantumCircuit &KernelQC = *QuantumCircuit::Create(QM, *F);
  StartingOp *KernelStartOp = KernelQC.getStartOp();
  EndingOp *KernelEndOp = KernelQC.getEndOp();

  std::vector<std::tuple<BasicBlock *, std::string, OpKind>> OpPairs = {
      {EntryBB, "", OpKind::Start},
      {BodyBB, "", OpKind::Start},
      {BodyBB, "_Z5PrepZRt", OpKind::Quantum},
      {BodyBB, "_Z1HRt", OpKind::Quantum},
      {EndBB, "", OpKind::Start}};

  std::vector<QuantumCircuitOp *> Ops;
  KernelQC.iterateGates(Ops);
  QuantumCircuit::QuantumCircuitIterator It = KernelQC.begin();
  EXPECT_EQ(OpPairs.size(), Ops.size());
  for (unsigned Idx = 0; Idx < Ops.size(); Idx++) {
    EXPECT_EQ(Ops[Idx]->getParentCircuit()->getBaseValue(),
              std::get<0>(OpPairs[Idx]));
    EXPECT_EQ(Ops[Idx]->getKind(), std::get<2>(OpPairs[Idx]));

    EXPECT_EQ((*It)->getParentCircuit()->getBaseValue(),
              std::get<0>(OpPairs[Idx]));
    EXPECT_EQ((*It)->getKind(), std::get<2>(OpPairs[Idx]));
    if (Idx == 2 || Idx == 3) {
      QuantumOp *Op = static_cast<QuantumOp *>(*It);
      EXPECT_EQ(Op->getCalledValue()->getName(), std::get<1>(OpPairs[Idx]));

      Op = static_cast<QuantumOp *>(Ops[Idx]);
      EXPECT_EQ(Op->getCalledValue()->getName(), std::get<1>(OpPairs[Idx]));
    }
    It++;
  }
}

/// Check for correct iteration when there is a branch instruction to different
/// blocks.
TEST(QuantumCircuitTest, BranchIterationTest) {
  LLVMContext C;
  std::unique_ptr<Module> M = parseIR(C, *&Code_Generated);
  ParaRef::setModule(*M);

  QuantumModule QM;
  runInitialPasses(*M, QM);

  QuantumKernel *F = FindKernelInModule(QM, "_Z12basic_branchb.stub");
  BasicBlock *BodyBB = FindBlockInFunction(*(F->getFunction()), "body");
  BasicBlock *EntryBB = FindBlockInFunction(*(F->getFunction()), "entry");
  BasicBlock *EndBB = FindBlockInFunction(*(F->getFunction()), "if.end");
  BasicBlock *Branch1BB = FindBlockInFunction(*(F->getFunction()), "if.then");
  BasicBlock *Branch2BB = FindBlockInFunction(*(F->getFunction()), "if.else");

  QuantumCircuit &KernelQC = *QuantumCircuit::Create(QM, *F);
  StartingOp *KernelStartOp = KernelQC.getStartOp();
  EndingOp *KernelEndOp = KernelQC.getEndOp();

  std::vector<std::tuple<BasicBlock *, std::string, OpKind>> OpPairs = {
      {EntryBB, "", OpKind::Start},
      {BodyBB, "", OpKind::Start},
      {BodyBB, "_Z5PrepZRt", OpKind::Quantum},
      {BodyBB, "_Z1HRt", OpKind::Quantum},
      {Branch1BB, "", OpKind::Start},
      {Branch1BB, "_Z1ZRt", OpKind::Quantum},
      {Branch2BB, "", OpKind::Start},
      {Branch2BB, "_Z1XRt", OpKind::Quantum},
      {EndBB, "", OpKind::Start}};

  std::vector<QuantumCircuitOp *> Ops;
  KernelQC.iterateGates(Ops);
  QuantumCircuit::QuantumCircuitIterator It = KernelQC.begin();
  EXPECT_EQ(OpPairs.size(), Ops.size());
  for (unsigned Idx = 0; Idx < Ops.size(); Idx++) {
    EXPECT_EQ(Ops[Idx]->getParentCircuit()->getBaseValue(),
              std::get<0>(OpPairs[Idx]));
    EXPECT_EQ(Ops[Idx]->getKind(), std::get<2>(OpPairs[Idx]));

    EXPECT_EQ((*It)->getParentCircuit()->getBaseValue(),
              std::get<0>(OpPairs[Idx]));
    EXPECT_EQ((*It)->getKind(), std::get<2>(OpPairs[Idx]));
    if (Idx == 2 || Idx == 3 || Idx == 5 || Idx == 7) {
      QuantumOp *Op = static_cast<QuantumOp *>(*It);
      EXPECT_EQ(Op->getCalledValue()->getName(), std::get<1>(OpPairs[Idx]));

      Op = static_cast<QuantumOp *>(Ops[Idx]);
      EXPECT_EQ(Op->getCalledValue()->getName(), std::get<1>(OpPairs[Idx]));
    }
    It++;
  }
}

/// Check for correct iteration, except we use time slices instead.
TEST(QuantumCircuitTest, BasicTimeSliceTest) {
  LLVMContext C;
  std::unique_ptr<Module> M = parseIR(C, *&Code_Generated);
  ParaRef::setModule(*M);

  QuantumModule QM;
  runInitialPasses(*M, QM);

  QuantumKernel *F = FindKernelInModule(QM, "_Z19basic_line_parallelv.stub");
  BasicBlock *BodyBB = FindBlockInFunction(*(F->getFunction()), "body");
  BasicBlock *EntryBB = FindBlockInFunction(*(F->getFunction()), "entry");
  BasicBlock *EndBB = FindBlockInFunction(*(F->getFunction()), "end_block");

  QuantumCircuit &KernelQC = *QuantumCircuit::Create(QM, *F);
  StartingOp *KernelStartOp = KernelQC.getStartOp();
  EndingOp *KernelEndOp = KernelQC.getEndOp();

  QuantumCircuit::QuantumCircuitIterator CIt =
      KernelQC.getCircuitList()[1]->begin();
  QbitRef Q1 = static_cast<QuantumOp *>(*CIt)->getQubits()[0].first;
  CIt++;
  CIt++;
  QbitRef Q2 = static_cast<QuantumOp *>(*CIt)->getQubits()[0].first;
  QbitRef NoVal;

  std::vector<
      std::vector<std::tuple<BasicBlock *, std::string, QbitRef, OpKind>>>
      OpSlicePairs;
  OpSlicePairs.emplace_back();
  OpSlicePairs.back().push_back({EntryBB, "", NoVal, OpKind::Start});
  OpSlicePairs.emplace_back();
  OpSlicePairs.back().push_back({BodyBB, "", NoVal, OpKind::Start});
  OpSlicePairs.emplace_back();
  OpSlicePairs.back().push_back({BodyBB, "_Z5PrepZRt", Q2, OpKind::Quantum});
  OpSlicePairs.back().push_back({BodyBB, "_Z5PrepZRt", Q1, OpKind::Quantum});
  OpSlicePairs.emplace_back();
  OpSlicePairs.back().push_back({BodyBB, "_Z1HRt", Q1, OpKind::Quantum});
  OpSlicePairs.emplace_back();
  OpSlicePairs.back().push_back({EndBB, "", NoVal, OpKind::Start});

  std::vector<QuantumCircuitSlice> Slices;
  KernelQC.iterateGatesTimeSlices(Slices);
  QuantumCircuit::QuantumCircuitSliceIterator It = KernelQC.begin_slice();
  EXPECT_EQ(OpSlicePairs.size(), Slices.size());
  for (unsigned Idx = 0; Idx < Slices.size(); Idx++) {
    for (unsigned OIdx = 0; OIdx < Slices[Idx].Ops.size(); OIdx++) {
      EXPECT_EQ(Slices[Idx].Ops[OIdx]->getParentCircuit()->getBaseValue(),
                std::get<0>(OpSlicePairs[Idx][OIdx]));
      EXPECT_EQ(Slices[Idx].Ops[OIdx]->getKind(),
                std::get<3>(OpSlicePairs[Idx][OIdx]));

      EXPECT_EQ((*It).Ops[OIdx]->getParentCircuit()->getBaseValue(),
                std::get<0>(OpSlicePairs[Idx][OIdx]));
      EXPECT_EQ((*It).Ops[OIdx]->getKind(),
                std::get<3>(OpSlicePairs[Idx][OIdx]));
      if (Idx == 2 || Idx == 3) {
        QuantumOp *Op1 = static_cast<QuantumOp *>((*It).Ops[OIdx]);
        EXPECT_EQ(Op1->getCalledValue()->getName(),
                  std::get<1>(OpSlicePairs[Idx][OIdx]));

        QuantumOp *Op2 = static_cast<QuantumOp *>(Slices[Idx].Ops[OIdx]);
        EXPECT_EQ(Op2->getCalledValue()->getName(),
                  std::get<1>(OpSlicePairs[Idx][OIdx]));

        QbitRef Q1 = Op1->getQubits()[0].first;
        EXPECT_TRUE(Q1.isEqual(std::get<2>(OpSlicePairs[Idx][OIdx])));
        QbitRef Q2 = Op2->getQubits()[0].first;
        EXPECT_TRUE(Q2.isEqual(std::get<2>(OpSlicePairs[Idx][OIdx])));
      }
    }
    It++;
  }
}

/// Check for correct loop iteration, except we use time slices instead.
TEST(QuantumCircuitTest, LoopTimeSliceTest) {
  LLVMContext C;
  std::unique_ptr<Module> M = parseIR(C, *&Code_Generated);
  ParaRef::setModule(*M);

  QuantumModule QM;
  runInitialPasses(*M, QM);

  QuantumKernel *F = FindKernelInModule(QM, "_Z19basic_loop_parallelv.stub");
  BasicBlock *BodyBB = FindBlockInFunction(*(F->getFunction()), "for.body");
  BasicBlock *EntryBB = FindBlockInFunction(*(F->getFunction()), "entry");
  BasicBlock *EndBB =
      FindBlockInFunction(*(F->getFunction()), "for.cond.cleanup");

  QuantumCircuit &KernelQC = *QuantumCircuit::Create(QM, *F);
  StartingOp *KernelStartOp = KernelQC.getStartOp();
  EndingOp *KernelEndOp = KernelQC.getEndOp();

  QuantumCircuit::QuantumCircuitIterator CIt =
      KernelQC.getCircuitList()[2]->begin();
  QbitRef Q1 = static_cast<QuantumOp *>(*CIt)->getQubits()[0].first;
  CIt++;
  CIt++;
  QbitRef Q2 = static_cast<QuantumOp *>(*CIt)->getQubits()[0].first;
  QbitRef NoVal;

  std::vector<
      std::vector<std::tuple<BasicBlock *, std::string, QbitRef, OpKind>>>
      OpSlicePairs;
  OpSlicePairs.emplace_back();
  OpSlicePairs.back().push_back({EntryBB, "", NoVal, OpKind::Start});
  OpSlicePairs.emplace_back();
  OpSlicePairs.back().push_back({BodyBB, "", NoVal, OpKind::Start});
  OpSlicePairs.emplace_back();
  OpSlicePairs.back().push_back({BodyBB, "_Z5PrepZRt", Q2, OpKind::Quantum});
  OpSlicePairs.back().push_back({BodyBB, "_Z5PrepZRt", Q1, OpKind::Quantum});
  OpSlicePairs.emplace_back();
  OpSlicePairs.back().push_back({BodyBB, "_Z1HRt", Q1, OpKind::Quantum});
  OpSlicePairs.emplace_back();
  OpSlicePairs.back().push_back({EndBB, "", NoVal, OpKind::Start});

  std::vector<QuantumCircuitSlice> Slices;
  KernelQC.iterateGatesTimeSlices(Slices);
  QuantumCircuit::QuantumCircuitSliceIterator It = KernelQC.begin_slice();
  EXPECT_EQ(OpSlicePairs.size(), Slices.size());
  for (unsigned Idx = 0; Idx < Slices.size(); Idx++) {
    for (unsigned OIdx = 0; OIdx < Slices[Idx].Ops.size(); OIdx++) {
      EXPECT_EQ(Slices[Idx].Ops[OIdx]->getParentCircuit()->getBaseValue(),
                std::get<0>(OpSlicePairs[Idx][OIdx]));
      EXPECT_EQ(Slices[Idx].Ops[OIdx]->getKind(),
                std::get<3>(OpSlicePairs[Idx][OIdx]));

      EXPECT_EQ((*It).Ops[OIdx]->getParentCircuit()->getBaseValue(),
                std::get<0>(OpSlicePairs[Idx][OIdx]));
      EXPECT_EQ((*It).Ops[OIdx]->getKind(),
                std::get<3>(OpSlicePairs[Idx][OIdx]));
      if (Idx == 2 || Idx == 3) {
        QuantumOp *Op1 = static_cast<QuantumOp *>((*It).Ops[OIdx]);
        EXPECT_EQ(Op1->getCalledValue()->getName(),
                  std::get<1>(OpSlicePairs[Idx][OIdx]));

        QuantumOp *Op2 = static_cast<QuantumOp *>(Slices[Idx].Ops[OIdx]);
        EXPECT_EQ(Op2->getCalledValue()->getName(),
                  std::get<1>(OpSlicePairs[Idx][OIdx]));

        QbitRef Q1 = Op1->getQubits()[0].first;
        EXPECT_TRUE(Q1.isEqual(std::get<2>(OpSlicePairs[Idx][OIdx])));
        QbitRef Q2 = Op2->getQubits()[0].first;
        EXPECT_TRUE(Q2.isEqual(std::get<2>(OpSlicePairs[Idx][OIdx])));
      }
    }
    It++;
  }
}

/// Check for correct branch iteration, except we use time slices instead.
TEST(QuantumCircuitTest, BranchTimeSliceTest) {
  LLVMContext C;
  std::unique_ptr<Module> M = parseIR(C, *&Code_Generated);
  ParaRef::setModule(*M);

  QuantumModule QM;
  runInitialPasses(*M, QM);

  QuantumKernel *F = FindKernelInModule(QM, "_Z21basic_branch_parallelb.stub");
  BasicBlock *BodyBB = FindBlockInFunction(*(F->getFunction()), "branch");
  BasicBlock *EntryBB = FindBlockInFunction(*(F->getFunction()), "entry");
  BasicBlock *EndBB = FindBlockInFunction(*(F->getFunction()), "if.end");
  BasicBlock *Branch1BB = FindBlockInFunction(*(F->getFunction()), "if.then");
  BasicBlock *Branch2BB = FindBlockInFunction(*(F->getFunction()), "if.else");

  QuantumCircuit &KernelQC = *QuantumCircuit::Create(QM, *F);
  StartingOp *KernelStartOp = KernelQC.getStartOp();
  EndingOp *KernelEndOp = KernelQC.getEndOp();

  QuantumCircuit::QuantumCircuitIterator CIt =
      KernelQC.getCircuitList()[1]->begin();
  QbitRef Q1 = static_cast<QuantumOp *>(*CIt)->getQubits()[0].first;
  CIt++;
  CIt++;
  QbitRef Q2 = static_cast<QuantumOp *>(*CIt)->getQubits()[0].first;
  QbitRef NoVal;

  std::vector<
      std::vector<std::tuple<BasicBlock *, std::string, QbitRef, OpKind>>>
      OpSlicePairs;
  OpSlicePairs.emplace_back();
  OpSlicePairs.back().push_back({EntryBB, "", NoVal, OpKind::Start});
  OpSlicePairs.emplace_back();
  OpSlicePairs.back().push_back({BodyBB, "", NoVal, OpKind::Start});
  OpSlicePairs.emplace_back();
  OpSlicePairs.back().push_back({BodyBB, "_Z5PrepZRt", Q2, OpKind::Quantum});
  OpSlicePairs.back().push_back({BodyBB, "_Z5PrepZRt", Q1, OpKind::Quantum});
  OpSlicePairs.emplace_back();
  OpSlicePairs.back().push_back({BodyBB, "_Z1HRt", Q1, OpKind::Quantum});
  OpSlicePairs.emplace_back();
  OpSlicePairs.back().push_back({Branch2BB, "", NoVal, OpKind::Start});
  OpSlicePairs.emplace_back();
  OpSlicePairs.back().push_back({Branch2BB, "_Z1ZRt", Q2, OpKind::Quantum});
  OpSlicePairs.back().push_back({Branch2BB, "_Z1XRt", Q1, OpKind::Quantum});
  OpSlicePairs.emplace_back();
  OpSlicePairs.back().push_back({Branch1BB, "", NoVal, OpKind::Start});
  OpSlicePairs.emplace_back();
  OpSlicePairs.back().push_back({Branch1BB, "_Z1ZRt", Q1, OpKind::Quantum});
  OpSlicePairs.emplace_back();
  OpSlicePairs.back().push_back({Branch1BB, "_Z1XRt", Q1, OpKind::Quantum});
  OpSlicePairs.emplace_back();
  OpSlicePairs.back().push_back({EndBB, "", NoVal, OpKind::Start});

  std::vector<QuantumCircuitSlice> Slices;
  KernelQC.iterateGatesTimeSlices(Slices);
  QuantumCircuit::QuantumCircuitSliceIterator It = KernelQC.begin_slice();
  EXPECT_EQ(OpSlicePairs.size(), Slices.size());
  for (unsigned Idx = 0; Idx < Slices.size(); Idx++) {
    for (unsigned OIdx = 0; OIdx < Slices[Idx].Ops.size(); OIdx++) {
      EXPECT_EQ(Slices[Idx].Ops[OIdx]->getParentCircuit()->getBaseValue(),
                std::get<0>(OpSlicePairs[Idx][OIdx]));
      EXPECT_EQ(Slices[Idx].Ops[OIdx]->getKind(),
                std::get<3>(OpSlicePairs[Idx][OIdx]));

      EXPECT_EQ((*It).Ops[OIdx]->getParentCircuit()->getBaseValue(),
                std::get<0>(OpSlicePairs[Idx][OIdx]));
      EXPECT_EQ((*It).Ops[OIdx]->getKind(),
                std::get<3>(OpSlicePairs[Idx][OIdx]));
      if (Idx == 2 || Idx == 3 || Idx == 5 || Idx == 7 || Idx == 8) {
        QuantumOp *Op1 = static_cast<QuantumOp *>((*It).Ops[OIdx]);
        EXPECT_EQ(Op1->getCalledValue()->getName(),
                  std::get<1>(OpSlicePairs[Idx][OIdx]));

        QuantumOp *Op2 = static_cast<QuantumOp *>(Slices[Idx].Ops[OIdx]);
        EXPECT_EQ(Op2->getCalledValue()->getName(),
                  std::get<1>(OpSlicePairs[Idx][OIdx]));

        QbitRef Q1 = Op1->getQubits()[0].first;
        EXPECT_TRUE(Q1.isEqual(std::get<2>(OpSlicePairs[Idx][OIdx])));
        QbitRef Q2 = Op2->getQubits()[0].first;
        EXPECT_TRUE(Q2.isEqual(std::get<2>(OpSlicePairs[Idx][OIdx])));
      }
    }
    It++;
  }
}

/// Check for correct iteration across two basic blocks with an unconditional
/// branch, only allowing for instructions involving a particular qubit.
TEST(QuantumCircuitTest, BasicIterationLimitQubitsTest) {
  LLVMContext C;
  std::unique_ptr<Module> M = parseIR(C, *&Code_Generated);
  ParaRef::setModule(*M);

  QuantumModule QM;
  runInitialPasses(*M, QM);

  QuantumKernel *F = FindKernelInModule(QM, "_Z16basic_line_limitv.stub");
  BasicBlock *EntryBB = FindBlockInFunction(*(F->getFunction()), "entry");

  QuantumCircuit &KernelQC = *QuantumCircuit::Create(QM, *F);
  StartingOp *KernelStartOp = KernelQC.getStartOp();
  EndingOp *KernelEndOp = KernelQC.getEndOp();

  QuantumCircuit::QuantumCircuitIterator CIt =
      KernelQC.getCircuitList()[0]->begin();
  CIt++;
  CIt++;
  QbitRef Q2 = static_cast<QuantumOp *>(*CIt)->getQubits()[1].first;
  QbitRef NoVal;

  std::vector<std::tuple<BasicBlock *, std::string, OpKind>> OpPairs = {
      {EntryBB, "", OpKind::Start},
      {EntryBB, "_Z5PrepZRt", OpKind::Quantum},
      {EntryBB, "_Z4CNOTRtS_", OpKind::Quantum}};

  std::vector<QuantumCircuitOp *> Ops;
  std::set<QbitRef, QubitCompare> Limit = {Q2};
  KernelQC.iterateGatesLimitedQubits(Ops, Limit);
  QuantumCircuit::QuantumCircuitIterator It = KernelQC.begin(Limit);
  EXPECT_EQ(OpPairs.size(), Ops.size());
  for (unsigned Idx = 0; Idx < Ops.size(); Idx++) {
    EXPECT_EQ(Ops[Idx]->getParentCircuit()->getBaseValue(),
              std::get<0>(OpPairs[Idx]));
    EXPECT_EQ(Ops[Idx]->getKind(), std::get<2>(OpPairs[Idx]));

    EXPECT_EQ((*It)->getParentCircuit()->getBaseValue(),
              std::get<0>(OpPairs[Idx]));
    EXPECT_EQ((*It)->getKind(), std::get<2>(OpPairs[Idx]));
    if (Idx == 1 || Idx == 2) {
      QuantumOp *Op = static_cast<QuantumOp *>(*It);
      EXPECT_EQ(Op->getCalledValue()->getName(), std::get<1>(OpPairs[Idx]));

      Op = static_cast<QuantumOp *>(Ops[Idx]);
      EXPECT_EQ(Op->getCalledValue()->getName(), std::get<1>(OpPairs[Idx]));
    }
    It++;
  }
}

/// Check for correct iteration, except we use time slices instead, only
/// allowing for instructions involving a particular qubit.
TEST(QuantumCircuitTest, BasicTimeSliceLimitQubitsTest) {
  LLVMContext C;
  std::unique_ptr<Module> M = parseIR(C, *&Code_Generated);
  ParaRef::setModule(*M);

  QuantumModule QM;
  runInitialPasses(*M, QM);

  QuantumKernel *F = FindKernelInModule(QM, "_Z16basic_line_limitv.stub");
  BasicBlock *EntryBB = FindBlockInFunction(*(F->getFunction()), "entry");

  QuantumCircuit &KernelQC = *QuantumCircuit::Create(QM, *F);
  StartingOp *KernelStartOp = KernelQC.getStartOp();
  EndingOp *KernelEndOp = KernelQC.getEndOp();

  QuantumCircuit::QuantumCircuitIterator CIt =
      KernelQC.getCircuitList()[0]->begin();
  CIt++;
  CIt++;
  QbitRef Q2 = static_cast<QuantumOp *>(*CIt)->getQubits()[0].first;
  QbitRef NoVal;

  std::vector<std::vector<std::tuple<BasicBlock *, std::string, OpKind>>>
      OpSlicePairs;
  OpSlicePairs.emplace_back();
  OpSlicePairs.back().push_back({EntryBB, "", OpKind::Start});
  OpSlicePairs.emplace_back();
  OpSlicePairs.back().push_back({EntryBB, "_Z5PrepZRt", OpKind::Quantum});
  OpSlicePairs.emplace_back();
  OpSlicePairs.back().push_back({EntryBB, "_Z4CNOTRtS_", OpKind::Quantum});

  std::vector<QuantumCircuitSlice> Slices;
  std::set<QbitRef, QubitCompare> Limit = {Q2};
  KernelQC.iterateGatesTimeSlicesLimitedQubits(Slices, Limit);
  QuantumCircuit::QuantumCircuitSliceIterator It = KernelQC.begin_slice(Limit);
  EXPECT_EQ(OpSlicePairs.size(), Slices.size());
  for (unsigned Idx = 0; Idx < Slices.size(); Idx++) {
    for (unsigned OIdx = 0; OIdx < Slices[Idx].Ops.size(); OIdx++) {
      EXPECT_EQ(Slices[Idx].Ops[OIdx]->getParentCircuit()->getBaseValue(),
                std::get<0>(OpSlicePairs[Idx][OIdx]));
      EXPECT_EQ(Slices[Idx].Ops[OIdx]->getKind(),
                std::get<2>(OpSlicePairs[Idx][OIdx]));

      EXPECT_EQ((*It).Ops[OIdx]->getParentCircuit()->getBaseValue(),
                std::get<0>(OpSlicePairs[Idx][OIdx]));
      EXPECT_EQ((*It).Ops[OIdx]->getKind(),
                std::get<2>(OpSlicePairs[Idx][OIdx]));
      if (Idx == 2 || Idx == 3) {
        QuantumOp *Op1 = static_cast<QuantumOp *>((*It).Ops[OIdx]);
        EXPECT_EQ(Op1->getCalledValue()->getName(),
                  std::get<1>(OpSlicePairs[Idx][OIdx]));

        QuantumOp *Op2 = static_cast<QuantumOp *>(Slices[Idx].Ops[OIdx]);
        EXPECT_EQ(Op2->getCalledValue()->getName(),
                  std::get<1>(OpSlicePairs[Idx][OIdx]));
      }
    }
    It++;
  }
}