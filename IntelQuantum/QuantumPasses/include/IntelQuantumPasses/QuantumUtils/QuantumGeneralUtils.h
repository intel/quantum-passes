//===- QuantumGeneralUtils.h ---------------------------------------------====//
//
// Copyright (C) 2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//
//===----------------------------------------------------------------------===//
//
// This file defines a set of common utility functions for the quantum compiler.
//
//===----------------------------------------------------------------------===//
#ifndef QUANTUM_GENERAL_UTILS_H
#define QUANTUM_GENERAL_UTILS_H

#include "IntelQuantumPasses/QuantumAnalysis/QuantumAnnotationGetters.h"
#include "IntelQuantumPasses/QuantumAnalysis/QuantumIterator.h"
#include "IntelQuantumPasses/QuantumAnalysis/QuantumModule.h"
#include "llvm/ADT/SCCIterator.h"
#include "llvm/Analysis/CFG.h"
#include "llvm/IR/CFG.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/GlobalValue.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Operator.h"
#include "llvm/Pass.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/raw_ostream.h"

#include <map>

namespace llvm {
namespace aqcc {

class QuantumModule;

////////////////////////////////////////////////////////////////////////////////
// relevant constants across quantum compiler

const std::string QBBS_TEXT_SECTION = ".qbbs_text";
const std::string QKSTUB = "stub";
const std::string QBBS_SECTION = ".qbbs";
const std::string QSDA_ATTR = "quantum_shared_var";
const std::string QBB_TWINE = "aqcc.quantum";
const std::string QBB_TWINE_RELEASE = QBB_TWINE + ".release";
const std::string MEAS_MOVE_TWINE = "aqcc.meas.move.end";
// QSDA reserved positions are for common constants in the QSDA
// Currently there are four positions for:
// qsda[0] = 0.;
// qsda[1] = Pi/2;
// qsda[2] = Pi;
// qsda[3] = 3 Pi/2;
const unsigned QSDA_RESERVED_POSITIONS = 4;

// ID for instruction slicing
const unsigned SLICE_BEG = 1;
const unsigned SLICE_MID = 0;
const unsigned SLICE_END = 2;

// Frontend Instrinsic Names
const std::string RELEASE_ALL_FRONT = "release_quantum_state";

///////////////////////////////////////////////////////////////////////////////

/// create a new quantum kernel in M with name and return the Function pointer
/// the BasicBlock argument is used when your want to move the basic block of
/// one quantum kernel into the new one. Use this with caution as it assumes the
/// BB passed has a return instruction, and typically invalidates the quantum
/// kernel it is coming from.
Function *createEmptyQKernel(std::string name, Module &M,
                             SmallVector<Type *> &TypeArray,
                             BasicBlock * = nullptr, bool mangle = true);

Function *createEmptyQKernel(std::string name, Module &M,
                             BasicBlock * = nullptr, bool mangle = true);

/// returns pointer to constexp GEP if the ParaRef points to a load from QSDA
/// returns nullptr otherwise
Value *isLoadFromQSDA(ParaRef &para);

/// returns the uuid for Function F if is a simple QBB or -1 if it is not a
/// simple QBB
int getUUIDforQBBFunction(Function &F);

void moveAllAttachedInstOutOfQBB(Instruction *I, Instruction *before);

Value *createNewQSDArray(Module &M, StringRef name, unsigned num);

/// simple demangling function
std::string DemangleQuantumKernelName(StringRef mangled_name);

/// Returns a code as to whether the passed Function is a valid quantum kernel
/// for formating into quantum basic blocks (QBBs). Currently (3/2022), this
/// means it satisfies the following three conditions:
///       1) if an SCC contains a loop, all BBs in the SCC contain no quantum
///          instructions or return = -1.
///       2) The strongly connected components (SCC) of the BB control flow
///          form an unbranching tree (a line basically) or return = -2.
///       3) All qubit gate operands are fully resolved or return = -3 and all
///       qubits for a given multi-qubit gate are distinct or return = -4.
///  If all three pass, then the function returns 1 and the BB_gates list
///  returns QBBIters into each BB which contains quantum gates IN REVERSE
///  ORDER. If the Function passed is not a quantum kernel, then the function
///  return 0.
int validateQuantumKernel(Function &F, std::vector<QBBIter> &BB_gates);

/// Check arguments of the function to verify if they are qbits, they are passed
/// by reference. Returns false if all qubit arguments are passed by reference
/// and true otherwise
bool passesQbitsByValue(Function &F);

/// Performs a recursive DFS to verify that every branch passes through elements
/// between first iterator and exclsively the last iterator returns true if
/// verification pass, and false otherwise.
bool verifyLinearChain(
    BasicBlock *, std::set<BasicBlock *> &,
    std::map<std::pair<BasicBlock *, Instruction *>, bool> &Cache,
    std::vector<QBBIter>::reverse_iterator,
    std::vector<QBBIter>::reverse_iterator);

// Is the function a quantum kernel?
inline bool isQBBSText(const Function &F) {
  return F.getSection() == QBBS_TEXT_SECTION;
}
inline bool isQKernel(const Function &F) {
  return F.getSection() == QBBS_TEXT_SECTION && F.getName().contains(QKSTUB);
}
inline bool isQBasicBlock(const BasicBlock &BB) {
  return BB.getName().contains(QBB_TWINE);
}
inline bool isReleaseQBB(const BasicBlock &BB) {
  return BB.getName().contains(QBB_TWINE_RELEASE);
}
inline bool isHoldQBB(const BasicBlock &BB) {
  return isQBasicBlock(BB) && !isReleaseQBB(BB);
}

/// Method primarily used by QM to add a simple qubit placement to its
/// global_qubit_map If QBBs exist (post-conditioned) this maps pointer is
/// copied to each QBB. returns true if their were enough physical qubits in the
/// platform to represent all
///  program qubits are false otherwise.
bool addSimpleGlobalQubitMap(QuantumModule &QM);

/// Create a fresh copy for each QBB by copying from global_qubit_map
/// Need to perform this to initialize qubit_maps
void copyGlobalQubitMapToQBB(QuantumModule &QM);

/// return the qubit map for the closest predecessor hold QBB of the passed
/// basic block or the global map if there is no such predecessor
void updateWithPredecessorQubitMap(BasicBlock *BB, QuantumModule &QM,
                                   bool keep_release = false);
bool updateWithPredecessorQubitMap(BasicBlock *BB,
                                   scheduler::MapProgToPhysQubits &passed,
                                   std::set<BasicBlock *> &visited,
                                   QuantumModule &QM,
                                   bool keep_release = false);

/// Method primarily used by QM to add a trivial qubit placement to each QBB
/// returns true if there were enough physical qubits in the platform to
/// represent all program qubits are false otherwise.
bool addTrivialQubitPlacement(QuantumModule &QM);

/// Must be run before initializeQBBQubitMap
/// Goes through the list of global qubits, if  there are any, and add them to
/// global_qubit_map
void initializeGlobalQubitMap(QuantumModule &QM);

/// Holds the unique QBB qubit map and its placement for each QBB
/// If there are global qubits declared, regardless of if the global qubits
/// are used in a QBB, these global qubits will be added to the QBB qubit map
void initializeQBBQubitMap(QuantumModule &QM);

// same as above, but only for the passed QBB and starts with the predecessor
// only adds additional qubits for this QBB
// returns pointer to new entry
std::shared_ptr<scheduler::MapProgToPhysQubits>
initializeQBBQubitMap(std::vector<BasicBlock *>, QuantumModule &QM,
                      bool keep_release = false);

/*
void populateGateDurationsInScheduleBase(QuantumModule &QM,
                                         popr::ScheduleBase<unsigned> *schedule,
                                         BasicBlock *QBB = nullptr,
                                         bool use_qid = false);
*/

/// sets the slice index for the begining and the end quantum native intrinsic
/// such that the entire QBB is a single slice
void setSingleSliceBeginAndEnd(BasicBlock &BB, bool CheckEnd = false);

/// sets the slice index for every quantum native intrinsic such that every
/// instruction is in its own slice
void setSerializedSlicing(BasicBlock &BB);

std::unique_ptr<Module> separateQuantumFromClassicalModule(Module &M);

void displayErrorAndExit(std::string PassName, std::string ErrMsg);

void updateGatesForBBs(std::vector<BasicBlock *> &BBs,
                       std::vector<Instruction *> &ValsToCheck);

void updateGatesForBB(BasicBlock &BB, std::vector<Instruction *> &ValsToCheck);

} // namespace aqcc
} // namespace llvm

#endif
