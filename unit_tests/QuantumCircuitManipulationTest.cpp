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

/// Test appending a gate into an empty circuit.
TEST(QuantumCircuitTest, AppendIntoEmptyBB) {
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

  std::string S = "QubitReg";
  StringRef SR(S);
  GlobalValue *GV = M->getNamedValue(SR);
  ASSERT_NE(GV, nullptr);
  QbitRef NewQ(GV, (unsigned)0);
  std::vector<QbitRef> Qubits = {NewQ};
  std::vector<ParaRef> Params;

  QuantumOp *QCO = QuantumOp::Create("H", QM, Qubits, Params);
  ASSERT_NE(QCO, nullptr);
  EXPECT_TRUE(QC.appendGate(QCO));

  EXPECT_EQ(QC.getNumberGates(), 1);
  EXPECT_EQ(QC.getQubits().size(), 1);
  EXPECT_TRUE(QCO->isSuccessorTo(StartOp));
  EXPECT_TRUE(QCO->isPredecessorTo(EndOp));
  EXPECT_FALSE(StartOp->isPredecessorTo(EndOp));
  EXPECT_FALSE(EndOp->isSuccessorTo(StartOp));

  EXPECT_EQ(dyn_cast<Instruction>(QCO->getBaseValue())->getParent(), EmptyBB);
}

/// Test appending a gate into an empty circuit, but explicitly inserting after
/// the start operation.
TEST(QuantumCircuitTest, InsertIntoEmptyBB) {
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

  std::string S = "QubitReg";
  StringRef SR(S);
  GlobalValue *GV = M->getNamedValue(SR);
  ASSERT_NE(GV, nullptr);
  QbitRef NewQ(GV, (unsigned)0);
  std::vector<QbitRef> Qubits = {NewQ};
  std::vector<ParaRef> Params;

  QuantumOp *QCO = QuantumOp::Create("H", QM, Qubits, Params);
  ASSERT_NE(QCO, nullptr);
  EXPECT_TRUE(QC.insertGate(QCO, StartOp));

  EXPECT_EQ(QC.getNumberGates(), 1);
  EXPECT_EQ(QC.getQubits().size(), 1);
  EXPECT_TRUE(QCO->isSuccessorTo(StartOp));
  EXPECT_TRUE(QCO->isPredecessorTo(EndOp));
  EXPECT_FALSE(StartOp->isPredecessorTo(EndOp));
  EXPECT_FALSE(EndOp->isSuccessorTo(StartOp));

  EXPECT_EQ(dyn_cast<Instruction>(QCO->getBaseValue())->getParent(), EmptyBB);
}

/// Test appending a gate into an empty circuit, but explicitly inserting after
/// the End operation. This should fail, but not crash.
TEST(QuantumCircuitTest, InsertAfterEnd) {
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

  std::string S = "QubitReg";
  StringRef SR(S);
  GlobalValue *GV = M->getNamedValue(SR);
  ASSERT_NE(GV, nullptr);
  QbitRef NewQ(GV, (unsigned)0);
  std::vector<QbitRef> Qubits = {NewQ};
  std::vector<ParaRef> Params;

  QuantumOp *QCO = QuantumOp::Create("H", QM, Qubits, Params);
  ASSERT_NE(QCO, nullptr);
  EXPECT_FALSE(QC.insertGate(QCO, EndOp));

  EXPECT_EQ(QC.getNumberGates(), 0);
  EXPECT_EQ(QC.getQubits().size(), 0);
  EXPECT_FALSE(QCO->isSuccessorTo(StartOp));
  EXPECT_FALSE(QCO->isPredecessorTo(EndOp));
  EXPECT_TRUE(StartOp->isPredecessorTo(EndOp));
  EXPECT_TRUE(EndOp->isSuccessorTo(StartOp));
}

// Test inserting at the end with qubit shared with a predecessor to the end
// operation.
TEST(QuantumCircuitTest, InsertAtEndOnePred) {
  LLVMContext C;
  std::unique_ptr<Module> M = parseIR(C, *&Code_Generated);
  ParaRef::setModule(*M);

  QuantumModule QM;
  runInitialPasses(*M, QM);

  QuantumKernel *Function =
      FindKernelInModule(QM, "_Z34simple_kernel_call_canonical_blockv.stub");
  BasicBlock *SeveralGateBB =
      FindBlockInFunction(*(Function->getFunction()), "entry");

  QuantumCircuit &QC = *QuantumCircuit::Create(QM, *SeveralGateBB);
  EXPECT_EQ(QC.getNumberGates(), 6);
  StartingOp *StartOp = QC.getStartOp();
  EndingOp *EndOp = QC.getEndOp();
  QuantumCircuit::QuantumCircuitIterator It = QC.begin();
  for (unsigned Idx = 0; Idx < 4; Idx++) {
    It++;
  }
  QuantumCircuitOp *QCO1 = *It;
  QuantumOp *QO1 = static_cast<QuantumOp *>(QCO1);

  std::string S = "QubitReg";
  StringRef SR(S);
  GlobalValue *GV = M->getNamedValue(SR);
  ASSERT_NE(GV, nullptr);
  QbitRef NewQ1(GV, (unsigned)1);
  std::vector<QbitRef> Qubits = {NewQ1};
  std::vector<ParaRef> Params;

  QuantumOp *QCO = QuantumOp::Create("H", QM, Qubits, Params);
  ASSERT_NE(QCO, nullptr);
  EXPECT_TRUE(QC.insertGate(QCO, QCO1));

  EXPECT_EQ(QC.getNumberGates(), 7);
  EXPECT_EQ(QC.getQubits().size(), 3);

  EXPECT_TRUE(QCO->isSuccessorTo(QCO1));
  EXPECT_TRUE(QCO->isPredecessorTo(EndOp));
  EXPECT_FALSE(QCO1->isPredecessorTo(EndOp));

  EXPECT_EQ(dyn_cast<Instruction>(QCO->getBaseValue())->getParent(),
            SeveralGateBB);
  EXPECT_TRUE(dyn_cast<Instruction>(QO1->getBaseValue())
                  ->comesBefore(dyn_cast<Instruction>(QCO->getBaseValue())));
}

// Test append to the circuit with a qubit shared with a predecessor to the end
// operation.
TEST(QuantumCircuitTest, AppendOnePred) {
  LLVMContext C;
  std::unique_ptr<Module> M = parseIR(C, *&Code_Generated);
  ParaRef::setModule(*M);

  QuantumModule QM;
  runInitialPasses(*M, QM);

  QuantumKernel *Function =
      FindKernelInModule(QM, "_Z34simple_kernel_call_canonical_blockv.stub");
  BasicBlock *SeveralGateBB =
      FindBlockInFunction(*(Function->getFunction()), "entry");

  QuantumCircuit &QC = *QuantumCircuit::Create(QM, *SeveralGateBB);
  EXPECT_EQ(QC.getNumberGates(), 6);
  StartingOp *StartOp = QC.getStartOp();
  EndingOp *EndOp = QC.getEndOp();
  QuantumCircuit::QuantumCircuitIterator It = QC.begin();
  for (unsigned Idx = 0; Idx < 3; Idx++) {
    It++;
  }
  QuantumCircuitOp *QCO1 = *It;
  QuantumOp *QO1 = static_cast<QuantumOp *>(QCO1);

  std::string S = "QubitReg";
  StringRef SR(S);
  GlobalValue *GV = M->getNamedValue(SR);
  ASSERT_NE(GV, nullptr);
  QbitRef NewQ1(GV, (unsigned)0);
  std::vector<QbitRef> Qubits = {NewQ1};
  std::vector<ParaRef> Params;

  QuantumOp *QCO = QuantumOp::Create("H", QM, Qubits, Params);
  ASSERT_NE(QCO, nullptr);
  EXPECT_TRUE(QC.appendGate(QCO));

  EXPECT_EQ(QC.getNumberGates(), 7);
  EXPECT_EQ(QC.getQubits().size(), 3);

  EXPECT_TRUE(QCO->isSuccessorTo(QCO1));
  EXPECT_TRUE(QCO->isPredecessorTo(EndOp));
  EXPECT_FALSE(QCO1->isPredecessorTo(EndOp));

  EXPECT_EQ(dyn_cast<Instruction>(QCO->getBaseValue())->getParent(),
            SeveralGateBB);
  EXPECT_TRUE(dyn_cast<Instruction>(QO1->getBaseValue())
                  ->comesBefore(dyn_cast<Instruction>(QCO->getBaseValue())));
}

// Test inserting at the end with qubits shared with two predecessors to the end
// operation.
TEST(QuantumCircuitTest, AppendTwoPreds) {
  LLVMContext C;
  std::unique_ptr<Module> M = parseIR(C, *&Code_Generated);
  ParaRef::setModule(*M);

  QuantumModule QM;
  runInitialPasses(*M, QM);

  QuantumKernel *Function =
      FindKernelInModule(QM, "_Z34simple_kernel_call_canonical_blockv.stub");
  BasicBlock *SeveralGateBB =
      FindBlockInFunction(*(Function->getFunction()), "entry");

  QuantumCircuit &QC = *QuantumCircuit::Create(QM, *SeveralGateBB);
  EXPECT_EQ(QC.getNumberGates(), 6);
  StartingOp *StartOp = QC.getStartOp();
  EndingOp *EndOp = QC.getEndOp();
  QuantumCircuit::QuantumCircuitIterator It = QC.begin();
  for (unsigned Idx = 0; Idx < 4; Idx++) {
    It++;
  }
  QuantumCircuitOp *QCO1 = *It;
  QuantumOp *QO1 = static_cast<QuantumOp *>(QCO1);
  It++;
  QuantumCircuitOp *QCO2 = *It;
  QuantumOp *QO2 = static_cast<QuantumOp *>(QCO2);

  std::string S = "QubitReg";
  StringRef SR(S);
  GlobalValue *GV = M->getNamedValue(SR);
  ASSERT_NE(GV, nullptr);
  QbitRef NewQ1(GV, (unsigned)1);
  QbitRef NewQ2(GV, (unsigned)2);
  std::vector<QbitRef> Qubits = {NewQ1, NewQ2};
  std::vector<ParaRef> Params;

  QuantumOp *QCO = QuantumOp::Create("CNOT", QM, Qubits, Params);
  ASSERT_NE(QCO, nullptr);
  EXPECT_TRUE(QC.appendGate(QCO));

  EXPECT_EQ(QC.getNumberGates(), 7);
  EXPECT_EQ(QC.getQubits().size(), 3);

  EXPECT_TRUE(QCO->isSuccessorTo(QCO1));
  EXPECT_TRUE(QCO->isSuccessorTo(QCO2));
  EXPECT_TRUE(QCO->isPredecessorTo(EndOp));
  EXPECT_FALSE(QCO1->isPredecessorTo(EndOp));
  EXPECT_FALSE(QCO2->isPredecessorTo(EndOp));

  EXPECT_EQ(dyn_cast<Instruction>(QCO->getBaseValue())->getParent(),
            SeveralGateBB);
  EXPECT_TRUE(dyn_cast<Instruction>(QO1->getBaseValue())
                  ->comesBefore(dyn_cast<Instruction>(QCO->getBaseValue())));
  EXPECT_TRUE(dyn_cast<Instruction>(QO2->getBaseValue())
                  ->comesBefore(dyn_cast<Instruction>(QCO->getBaseValue())));
}

// Test failure of inserting a nullptr operation to the circuit.
TEST(QuantumCircuitTest, InsertNullptr) {
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

  EXPECT_FALSE(QC.appendGate(nullptr));
  EXPECT_FALSE(QC.insertGate(nullptr, StartOp));

  EXPECT_EQ(QC.getNumberGates(), 0);
  EXPECT_EQ(QC.getQubits().size(), 0);
  EXPECT_TRUE(StartOp->isPredecessorTo(EndOp));
  EXPECT_TRUE(EndOp->isSuccessorTo(StartOp));
}

// Test inserting after an operation with a shared qubit into the middle of a
// circuit.
TEST(QuantumCircuitTest, InsertAfterSameQubit) {
  LLVMContext C;
  std::unique_ptr<Module> M = parseIR(C, *&Code_Generated);
  ParaRef::setModule(*M);

  QuantumModule QM;
  runInitialPasses(*M, QM);

  QuantumKernel *Function =
      FindKernelInModule(QM, "_Z34simple_kernel_call_canonical_blockv.stub");
  BasicBlock *SeveralGateBB =
      FindBlockInFunction(*(Function->getFunction()), "entry");

  QuantumCircuit &QC = *QuantumCircuit::Create(QM, *SeveralGateBB);
  EXPECT_EQ(QC.getNumberGates(), 6);
  StartingOp *StartOp = QC.getStartOp();
  EndingOp *EndOp = QC.getEndOp();
  QuantumCircuit::QuantumCircuitIterator It = QC.begin();
  QuantumCircuitOp *Op = *It;
  QuantumOp *OpQuantum = static_cast<QuantumOp *>(Op);
  It++;
  It++;
  QuantumCircuitOp *FollowOp = *It;
  QuantumOp *FollowOpQuantum = static_cast<QuantumOp *>(FollowOp);

  std::string S = "QubitReg";
  StringRef SR(S);
  GlobalValue *GV = M->getNamedValue(SR);
  ASSERT_NE(GV, nullptr);
  QbitRef NewQ1(GV, (unsigned)0);
  std::vector<QbitRef> Qubits = {NewQ1};
  std::vector<ParaRef> Params;

  QuantumOp *QCO = QuantumOp::Create("H", QM, Qubits, Params);
  ASSERT_NE(QCO, nullptr);
  EXPECT_TRUE(QC.insertGate(QCO, Op));

  EXPECT_EQ(QC.getNumberGates(), 7);
  EXPECT_EQ(QC.getQubits().size(), 3);

  EXPECT_TRUE(QCO->isSuccessorTo(Op));
  EXPECT_TRUE(QCO->isPredecessorTo(FollowOp));
  EXPECT_FALSE(Op->isPredecessorTo(FollowOp));
  EXPECT_FALSE(FollowOp->isSuccessorTo(Op));

  EXPECT_EQ(dyn_cast<Instruction>(QCO->getBaseValue())->getParent(),
            SeveralGateBB);
  EXPECT_TRUE(dyn_cast<Instruction>(OpQuantum->getBaseValue())
                  ->comesBefore(dyn_cast<Instruction>(QCO->getBaseValue())));
  EXPECT_TRUE(dyn_cast<Instruction>(QCO->getBaseValue())
                  ->comesBefore(
                      dyn_cast<Instruction>(FollowOpQuantum->getBaseValue())));
}

// Test inserting into the middle of a circuit after an operation with a
// different qubit, but after the qubit has been used elsewhere in the circuit.
// This creates an explicit dependence.
TEST(QuantumCircuitTest, InsertAfterDifferentQubit) {
  LLVMContext C;
  std::unique_ptr<Module> M = parseIR(C, *&Code_Generated);
  ParaRef::setModule(*M);

  QuantumModule QM;
  runInitialPasses(*M, QM);

  QuantumKernel *Function =
      FindKernelInModule(QM, "_Z34simple_kernel_call_canonical_blockv.stub");
  BasicBlock *SeveralGateBB =
      FindBlockInFunction(*(Function->getFunction()), "entry");

  QuantumCircuit &QC = *QuantumCircuit::Create(QM, *SeveralGateBB);
  EXPECT_EQ(QC.getNumberGates(), 6);
  StartingOp *StartOp = QC.getStartOp();
  EndingOp *EndOp = QC.getEndOp();
  QuantumCircuit::QuantumCircuitIterator It = QC.begin();
  QuantumCircuitOp *MatchingQOp = *It;
  It++;
  QuantumCircuitOp *DifferentQOp = *It;
  It++;
  QuantumCircuitOp *FollowOp = *It;

  std::string S = "QubitReg";
  StringRef SR(S);
  GlobalValue *GV = M->getNamedValue(SR);
  ASSERT_NE(GV, nullptr);
  QbitRef NewQ1(GV, (unsigned)0);
  std::vector<QbitRef> Qubits = {NewQ1};
  std::vector<ParaRef> Params;

  QuantumOp *QCO = QuantumOp::Create("H", QM, Qubits, Params);
  ASSERT_NE(QCO, nullptr);
  EXPECT_TRUE(QC.insertGate(QCO, DifferentQOp));

  EXPECT_EQ(QC.getNumberGates(), 7);
  EXPECT_EQ(QC.getQubits().size(), 3);

  EXPECT_TRUE(QCO->isSuccessorTo(MatchingQOp));
  EXPECT_TRUE(QCO->isSuccessorTo(DifferentQOp));
  EXPECT_TRUE(QCO->isPredecessorTo(FollowOp));
  EXPECT_FALSE(MatchingQOp->isPredecessorTo(FollowOp));
  EXPECT_TRUE(DifferentQOp->isPredecessorTo(FollowOp));
  EXPECT_FALSE(FollowOp->isSuccessorTo(MatchingQOp));
  EXPECT_TRUE(FollowOp->isSuccessorTo(DifferentQOp));

  EXPECT_EQ(dyn_cast<Instruction>(QCO->getBaseValue())->getParent(),
            SeveralGateBB);
}

// Add gate after the start operation, and insert before corresponding operation
// for the used qubit.
TEST(QuantumCircuitTest, InsertAtStart) {
  LLVMContext C;
  std::unique_ptr<Module> M = parseIR(C, *&Code_Generated);
  ParaRef::setModule(*M);

  QuantumModule QM;
  runInitialPasses(*M, QM);

  QuantumKernel *Function =
      FindKernelInModule(QM, "_Z34simple_kernel_call_canonical_blockv.stub");
  BasicBlock *SeveralGateBB =
      FindBlockInFunction(*(Function->getFunction()), "entry");

  QuantumCircuit &QC = *QuantumCircuit::Create(QM, *SeveralGateBB);
  EXPECT_EQ(QC.getNumberGates(), 6);
  StartingOp *StartOp = QC.getStartOp();
  EndingOp *EndOp = QC.getEndOp();
  QuantumCircuit::QuantumCircuitIterator It = QC.begin();
  QuantumCircuitOp *Op = *It;
  QuantumOp *QCO1 = static_cast<QuantumOp *>(Op);

  std::string S = "QubitReg";
  StringRef SR(S);
  GlobalValue *GV = M->getNamedValue(SR);
  ASSERT_NE(GV, nullptr);
  QbitRef NewQ1(GV, (unsigned)0);
  std::vector<QbitRef> Qubits = {NewQ1};
  std::vector<ParaRef> Params;

  QuantumOp *QCO = QuantumOp::Create("H", QM, Qubits, Params);
  ASSERT_NE(QCO, nullptr);
  EXPECT_TRUE(QC.insertGate(QCO, StartOp));

  EXPECT_EQ(QC.getNumberGates(), 7);
  EXPECT_EQ(QC.getQubits().size(), 3);

  EXPECT_TRUE(QCO->isSuccessorTo(StartOp));
  EXPECT_TRUE(QCO->isPredecessorTo(Op));
  EXPECT_FALSE(Op->isSuccessorTo(StartOp));
  EXPECT_FALSE(StartOp->isPredecessorTo(Op));

  EXPECT_EQ(dyn_cast<Instruction>(QCO->getBaseValue())->getParent(),
            SeveralGateBB);

  EXPECT_TRUE(dyn_cast<Instruction>(QCO->getBaseValue())
                  ->comesBefore(dyn_cast<Instruction>(QCO1->getBaseValue())));
}

// Fail when adding gate after operation that is in different parent circuit.
TEST(QuantumCircuitTest, InsertDifferentParent) {
  LLVMContext C;
  std::unique_ptr<Module> M = parseIR(C, *&Code_Generated);
  ParaRef::setModule(*M);

  QuantumModule QM;
  runInitialPasses(*M, QM);

  QuantumKernel *EmptyFunction =
      FindKernelInModule(QM, "_Z12empty_kernelv.stub");
  BasicBlock *EmptyBB =
      FindBlockInFunction(*(EmptyFunction->getFunction()), "entry");

  QuantumCircuit &QCP = *QuantumCircuit::Create(QM, *EmptyFunction);
  QuantumCircuit *QC = QCP.getCircuitList()[0];
  EXPECT_EQ(QC->getNumberGates(), 0);
  StartingOp *StartOp = QC->getStartOp();
  EndingOp *EndOp = QC->getEndOp();

  std::string S = "QubitReg";
  StringRef SR(S);
  GlobalValue *GV = M->getNamedValue(SR);
  ASSERT_NE(GV, nullptr);
  QbitRef NewQ1(GV, (unsigned)0);
  std::vector<QbitRef> Qubits = {NewQ1};
  std::vector<ParaRef> Params;

  QuantumOp *QCO = QuantumOp::Create("H", QM, Qubits, Params);
  ASSERT_NE(QCO, nullptr);
  EXPECT_FALSE(QCP.appendGate(QCO));
  EXPECT_FALSE(QCP.insertGate(QCO, StartOp));

  EXPECT_EQ(QC->getNumberGates(), 0);
  EXPECT_EQ(QC->getQubits().size(), 0);
  EXPECT_TRUE(StartOp->isPredecessorTo(EndOp));
  EXPECT_TRUE(EndOp->isSuccessorTo(StartOp));
}

// Create an operation with an ID number.
TEST(QuantumCircuitTest, CreateOpWithID) {
  LLVMContext C;
  std::unique_ptr<Module> M = parseIR(C, *&Code_Generated);
  ParaRef::setModule(*M);

  QuantumModule QM;
  runInitialPasses(*M, QM);

  std::string S = "QubitReg";
  StringRef SR(S);
  GlobalValue *GV = M->getNamedValue(SR);
  ASSERT_NE(GV, nullptr);
  QbitRef NewQ1(GV, (unsigned)0);
  std::vector<QbitRef> Qubits = {NewQ1};
  std::vector<ParaRef> Params;

  QuantumOp *QCO = QuantumOp::Create(3, QM, Qubits, Params);
  ASSERT_NE(QCO, nullptr);
}

// Fail to create an operation with a wrong ID number.
TEST(QuantumCircuitTest, CreateOpWithWrongID) {
  LLVMContext C;
  std::unique_ptr<Module> M = parseIR(C, *&Code_Generated);
  ParaRef::setModule(*M);

  QuantumModule QM;
  runInitialPasses(*M, QM);

  std::string S = "QubitReg";
  StringRef SR(S);
  GlobalValue *GV = M->getNamedValue(SR);
  ASSERT_NE(GV, nullptr);
  QbitRef NewQ1(GV, (unsigned)0);
  std::vector<QbitRef> Qubits = {NewQ1};
  std::vector<ParaRef> Params;

  QuantumOp *QCO = QuantumOp::Create(1000, QM, Qubits, Params);
  ASSERT_EQ(QCO, nullptr);
}

// Create an operation with canonical gate name.
TEST(QuantumCircuitTest, CreateOpWithGateString) {
  LLVMContext C;
  std::unique_ptr<Module> M = parseIR(C, *&Code_Generated);
  ParaRef::setModule(*M);

  QuantumModule QM;
  runInitialPasses(*M, QM);

  std::string S = "QubitReg";
  StringRef SR(S);
  GlobalValue *GV = M->getNamedValue(SR);
  ASSERT_NE(GV, nullptr);
  QbitRef NewQ1(GV, (unsigned)0);
  std::vector<QbitRef> Qubits = {NewQ1};
  std::vector<ParaRef> Params;

  QuantumOp *QCO = QuantumOp::Create("H", QM, Qubits, Params);
  ASSERT_NE(QCO, nullptr);
}

// Fail to create an operation with nonexistent name.
TEST(QuantumCircuitTest, CreateOpWithNonExistentString) {
  LLVMContext C;
  std::unique_ptr<Module> M = parseIR(C, *&Code_Generated);
  ParaRef::setModule(*M);

  QuantumModule QM;
  runInitialPasses(*M, QM);

  std::string S = "QubitReg";
  StringRef SR(S);
  GlobalValue *GV = M->getNamedValue(SR);
  ASSERT_NE(GV, nullptr);
  QbitRef NewQ1(GV, (unsigned)0);
  std::vector<QbitRef> Qubits = {NewQ1};
  std::vector<ParaRef> Params;

  QuantumOp *QCO = QuantumOp::Create("WrongGate", QM, Qubits, Params);
  ASSERT_EQ(QCO, nullptr);
}

// Create an operation with a non-builtin name.
TEST(QuantumCircuitTest, CreateOpCustomString) {
  LLVMContext C;
  std::unique_ptr<Module> M = parseIR(C, *&Code_Generated);
  ParaRef::setModule(*M);

  QuantumModule QM;
  runInitialPasses(*M, QM);

  std::vector<QbitRef> Qubits;
  std::vector<ParaRef> Params;

  QuantumOp *QCO = QuantumOp::Create("empty_kernel", QM, Qubits, Params);
  ASSERT_NE(QCO, nullptr);
}

// Create an operation with a non-builtin name and other parameters.
TEST(QuantumCircuitTest, CreateOpWithDoubleParam) {
  LLVMContext C;
  std::unique_ptr<Module> M = parseIR(C, *&Code_Generated);
  ParaRef::setModule(*M);

  QuantumModule QM;
  runInitialPasses(*M, QM);

  std::vector<QuantumOpParam> Params;
  Params.emplace_back(OpParamTy::DOUBLE, 2.0);

  QuantumOp *QCO = QuantumOp::Create("kernel_with_double", QM, Params);
  ASSERT_NE(QCO, nullptr);
}

// Create an operation with a non-builtin name and mixed type parameters.
TEST(QuantumCircuitTest, CreateOpWithMixedParams) {
  LLVMContext C;
  std::unique_ptr<Module> M = parseIR(C, *&Code_Generated);
  ParaRef::setModule(*M);

  QuantumModule QM;
  runInitialPasses(*M, QM);

  std::vector<QuantumOpParam> Params;
  Params.emplace_back(OpParamTy::INT, 2);
  Params.emplace_back(OpParamTy::DOUBLE, 2.0);

  QuantumOp *QCO = QuantumOp::Create("kernel_with_int_and_double", QM, Params);
  ASSERT_NE(QCO, nullptr);
}

/// Test inserting a custom gate into the circuit.
TEST(QuantumCircuitTest, InsertCustomOpNoQubits) {
  LLVMContext C;
  std::unique_ptr<Module> M = parseIR(C, *&Code_Generated);
  ParaRef::setModule(*M);

  QuantumModule QM;
  runInitialPasses(*M, QM);

  QuantumKernel *Function =
      FindKernelInModule(QM, "_Z34simple_kernel_call_canonical_blockv.stub");
  BasicBlock *SeveralGateBB =
      FindBlockInFunction(*(Function->getFunction()), "entry");

  QuantumCircuit &QC = *QuantumCircuit::Create(QM, *SeveralGateBB);
  EXPECT_EQ(QC.getNumberGates(), 6);
  StartingOp *StartOp = QC.getStartOp();
  EndingOp *EndOp = QC.getEndOp();
  QuantumCircuit::QuantumCircuitIterator It = QC.begin();
  QuantumCircuitOp *Op = *It;
  QuantumOp *OpQuantum = static_cast<QuantumOp *>(Op);
  It++;
  It++;
  QuantumCircuitOp *FollowOp = *It;
  QuantumOp *FollowOpQuantum = static_cast<QuantumOp *>(FollowOp);

  std::vector<QuantumOpParam> Params;

  QuantumOp *QCO = QuantumOp::Create("empty_kernel", QM, Params);
  ASSERT_NE(QCO, nullptr);
  ASSERT_TRUE(QC.insertGate(QCO, Op));

  EXPECT_EQ(dyn_cast<Instruction>(QCO->getBaseValue())->getParent(),
            SeveralGateBB);
  EXPECT_TRUE(dyn_cast<Instruction>(OpQuantum->getBaseValue())
                  ->comesBefore(dyn_cast<Instruction>(QCO->getBaseValue())));
  EXPECT_TRUE(dyn_cast<Instruction>(QCO->getBaseValue())
                  ->comesBefore(
                      dyn_cast<Instruction>(FollowOpQuantum->getBaseValue())));
}

/// Test that we successfully create dependency of a two qubit gate, followed by
/// a one qubit gate on each qubit when using qubit references.
TEST(QuantumCircuitTest, RemoveOneQubitGate) {
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

  bool Worked = static_cast<QuantumOp *>(Hs[0])->removeGate();
  EXPECT_TRUE(Worked);

  EXPECT_EQ(BlockQC.getNumberGates(), 3);
  EXPECT_TRUE(CX1->isPredecessorTo(Hs[1]));
  EXPECT_FALSE(CX1->isPredecessorTo(Hs[0]));
  EXPECT_TRUE(CX1->isPredecessorTo(CX2));
  EXPECT_TRUE(CX2->isSuccessorTo(Hs[1]));
  EXPECT_FALSE(CX2->isSuccessorTo(Hs[0]));
  EXPECT_TRUE(CX2->isSuccessorTo(CX1));
  EXPECT_EQ(Hs[0]->getPredecessors().size(), 0);
  EXPECT_EQ(Hs[0]->getSuccessors().size(), 0);
  EXPECT_EQ(
      dyn_cast<Instruction>(static_cast<QuantumOp *>(Hs[0])->getBaseValue())
          ->getParent(),
      nullptr);
}

/// Test that we successfully create dependency of a two qubit gate, followed by
/// a one qubit gate on each qubit when using qubit references.
TEST(QuantumCircuitTest, RemoveTwoQubitGate) {
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

  bool Worked = static_cast<QuantumOp *>(CX1)->removeGate();
  EXPECT_TRUE(Worked);

  EXPECT_EQ(BlockQC.getNumberGates(), 3);
  EXPECT_TRUE(BlockStartOp->isPredecessorTo(Hs[1]));
  EXPECT_TRUE(BlockStartOp->isPredecessorTo(Hs[0]));
  EXPECT_TRUE(Hs[0]->isSuccessorTo(BlockStartOp));
  EXPECT_TRUE(Hs[1]->isSuccessorTo(BlockStartOp));
  EXPECT_TRUE(Hs[0]->isPredecessorTo(CX2));
  EXPECT_TRUE(Hs[1]->isPredecessorTo(CX2));
  EXPECT_EQ(CX1->getPredecessors().size(), 0);
  EXPECT_EQ(CX1->getSuccessors().size(), 0);
  EXPECT_EQ(dyn_cast<Instruction>(static_cast<QuantumOp *>(CX1)->getBaseValue())
                ->getParent(),
            nullptr);
}

/// Test that we successfully create dependency of a two qubit gate, followed by
/// a one qubit gate on each qubit when using qubit references.
TEST(QuantumCircuitTest, DeleteBlockingOp) {
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

  QuantumOp *CanonicalQuantum = static_cast<QuantumOp *>(CanonicalCall);
  bool Worked = CanonicalQuantum->removeGate();
  EXPECT_TRUE(Worked);

  EXPECT_EQ(BlockQC.getNumberGates(), 5);
  for (unsigned Idx = 0; Idx < 2; Idx++) {
    QuantumCircuitOp *QCO1 = PrepZs[Idx];
    QuantumCircuitOp *QCO2 = Hs[Idx];
    EXPECT_TRUE(QCO2->isSuccessorTo(QCO1));
    EXPECT_TRUE(QCO1->isPredecessorTo(QCO2));
  }

  QuantumCircuitOp *QCO = Hs[2];
  EXPECT_TRUE(QCO->isSuccessorTo(BlockStartOp));
  EXPECT_TRUE(BlockStartOp->isPredecessorTo(QCO));
  EXPECT_EQ(CanonicalQuantum->getPredecessors().size(), 0);
  EXPECT_EQ(CanonicalQuantum->getSuccessors().size(), 0);
  EXPECT_EQ(
      dyn_cast<Instruction>(CanonicalQuantum->getBaseValue())->getParent(),
      nullptr);
}

/// Test that we successfully create dependency of a two qubit gate, followed by
/// a one qubit gate on each qubit when using qubit references.
TEST(QuantumCircuitTest, DeleteWithNoParent) {
  LLVMContext C;
  std::unique_ptr<Module> M = parseIR(C, *&Code_Generated);
  ParaRef::setModule(*M);

  QuantumModule QM;
  runInitialPasses(*M, QM);

  QuantumKernel *F = FindKernelInModule(QM, "_Z18qbitref_dependencyv.stub");
  BasicBlock *BB = FindBlockInFunction(*(F->getFunction()), "entry");

  std::string S = "QubitReg";
  StringRef SR(S);
  GlobalValue *GV = M->getNamedValue(SR);
  ASSERT_NE(GV, nullptr);
  QbitRef NewQ1(GV, (unsigned)1);
  std::vector<QbitRef> Qubits = {NewQ1};
  std::vector<ParaRef> Params;

  QuantumOp *QCO = QuantumOp::Create("H", QM, Qubits, Params);
  ASSERT_NE(QCO, nullptr);

  ASSERT_FALSE(QCO->deleteGate());
}