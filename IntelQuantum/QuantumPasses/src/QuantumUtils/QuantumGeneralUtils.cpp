//===-- QuantumGeneralUtils.cpp ------------------------------------------====//
//
// Copyright (C) 2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//
//===----------------------------------------------------------------------===//
//
// This file implements a set of common utility functions for the quantum
// compiler.
//
//===----------------------------------------------------------------------===//

#include "IntelQuantumPasses/QuantumUtils/QuantumGeneralUtils.h"
#include "IntelQuantumPasses/QuantumAnalysis/QuantumModule.h"
#include "IntelQuantumPasses/QuantumUtils/QuantumMapProgToPhysQubits.hpp"
#include "llvm/ADT/StringRef.h"
#include "llvm/Demangle/Demangle.h"
#include "llvm/IR/Intrinsics.h"

#include <map>

namespace llvm {
namespace aqcc {

// create a new quantum kernel in M with name and returns the function ptr
Function *createEmptyQKernel(std::string name, Module &M, BasicBlock *BB,
                             bool mangle) {
  SmallVector<Type *> TempType;
  return createEmptyQKernel(name, M, TempType, BB, mangle);
}

// create a new quantum kernel in M with name and returns the function ptr
Function *createEmptyQKernel(std::string name, Module &M,
                             SmallVector<Type *> &TypeArray, BasicBlock *BB,
                             bool mangle) {

  FunctionType *VdTy =
      FunctionType::get(Type::getVoidTy(M.getContext()), TypeArray, false);
  std::string mangle_name;
  if (mangle) {
    mangle_name = "_Z";
    mangle_name += name + "v.stub";
  } else {
    mangle_name = name;
  }

  Function *F =
      Function::Create(VdTy, GlobalValue::ExternalLinkage, mangle_name, &M);

  if (!F) {
    return nullptr;
  }

  F->setSection(QBBS_TEXT_SECTION);

  if (BB) {
    BB->removeFromParent();
    BB->insertInto(F);
    return F;
  }

  BasicBlock *entryBB = BasicBlock::Create(F->getContext(), QBB_TWINE, F);

  if (!(ReturnInst::Create(F->getContext(), nullptr, entryBB))) {
    return nullptr;
  }

  return F;
}

Value *isLoadFromQSDA(ParaRef &para) {

  if (LoadInst *LI = dyn_cast<LoadInst>(para.getValue())) {
    if (GEPOperator *gep = dyn_cast<GEPOperator>(LI->getPointerOperand())) {
      if (GlobalVariable *arry =
              dyn_cast<GlobalVariable>(gep->getPointerOperand())) {
        if (arry->hasAttribute(QSDA_ATTR)) {
          return dyn_cast<Value>(gep);
        }
      }
    }
  }
  return nullptr;
}

int getUUIDforQBBFunction(Function &F) {

  StringRef F_name = F.getName();
  if (!F_name.contains("QBB"))
    return -1;
  const long unsigned int start = F_name.find("QBB__", 0) + 5;
  const long unsigned int end = F_name.find("_", start);

  if (start == StringRef::npos || end == StringRef::npos)
    return -1;

  return std::stoi(F_name.substr(start, end).str());
}

std::string DemangleQuantumKernelName(StringRef mangled_name) {
  return llvm::demangle(mangled_name.rtrim(".stub").str());
}

void moveAllAttachedInstOutOfQBB(Instruction *I, Instruction *before) {
  // if 'before' is in a QBB, we can't move out of a QBB
  if (isQBasicBlock(*(before->getParent())))
    return;
  // if 'I' is not in a QBB, no need to move
  if (I->getParent()) {
    if (isQBasicBlock(*(I->getParent())))
      I->moveBefore(before);

    // move any attached instructions recursively
    for (auto op : I->operand_values()) {
      if (Instruction *I_op = dyn_cast<Instruction>(op))
        if (isQBasicBlock(*(I_op->getParent())))
          moveAllAttachedInstOutOfQBB(I_op, I);
    }
  }
}

Value *createNewQSDArray(Module &M, StringRef name, unsigned num) {

  ArrayType *ArrayTy = ArrayType::get(Type::getDoubleTy(M.getContext()), num);
  Value *out = dyn_cast<Value>(M.getOrInsertGlobal(name, ArrayTy));
  if (nullptr == out)
    displayErrorAndExit("QuantumGeneralUtils",
                        "Unable to create global array of quantum parameters.");
  dyn_cast<GlobalVariable>(out)->addAttribute(QSDA_ATTR);
  dyn_cast<GlobalVariable>(out)->setInitializer(
      Constant::getNullValue(ArrayTy));

  return out;
}

bool verifyLinearChain(
    BasicBlock *BB, std::set<BasicBlock *> &visited,
    std::map<std::pair<BasicBlock *, Instruction *>, bool> &Cache,
    std::vector<QBBIter>::reverse_iterator qbb_iter,
    std::vector<QBBIter>::reverse_iterator qbb_end) {
  if (qbb_iter == qbb_end)
    return true;

  // if we are inside a loop (find a previously visited BB)
  // return true to exit loop without failing verification
  if (visited.find(BB) != visited.end())
    return true;
  else
    visited.insert(BB);
  if (qbb_iter->getBasicBlock() == BB) {
    ++qbb_iter;
    if (qbb_iter == qbb_end)
      return true;
  }
  auto succ = succ_begin(BB), end = succ_end(BB);
  if ((succ == end))
    return false;

  std::map<std::pair<BasicBlock *, Instruction *>, bool>::iterator It;
  for (; succ != end; ++succ) {
    std::set<BasicBlock *> br_visited = visited;
    It = Cache.find(std::make_pair(*succ, &**qbb_iter));
    if (It != Cache.end()) {
      return It->second;
    }
    if (!verifyLinearChain(*succ, br_visited, Cache, qbb_iter, qbb_end)) {
      Cache.insert(std::make_pair(std::make_pair(*succ, &**qbb_iter), false));
      return false;
    } else {
      Cache.insert(std::make_pair(std::make_pair(*succ, &**qbb_iter), true));
    }
  }

  return true;
}

int validateQuantumKernel(Function &F, std::vector<QBBIter> &F_gates) {

  std::vector<BasicBlock *> LastSCC;
  BasicBlock *contains_release;

  std::string func_name = F.getName().str();

  // look for a release call and flag BB

  int IntrID =
      Function::lookupIntrinsicID("llvm.quantum.release.quantum.state");
  Function *release = Intrinsic::getDeclaration(F.getParent(), IntrID);
  // Move through the user list to add the the release_calls map
  for (auto user : release->users()) {
    if (CallBase *CB = dyn_cast<CallBase>(user)) {
      if (CB->getParent()->getParent() == &F)
        contains_release = CB->getParent();
    }
  }

  // recall scc_iter is in "reverse_order" so F_gates will be backward
  for (scc_iterator<Function *> I = scc_begin(&F), IE = scc_end(&F); I != IE;
       ++I) {

    std::vector<BasicBlock *> SCC = *I;

    if (I.hasCycle()) {
      for (size_t i = 0; i < SCC.size(); i++) {
        QBBIter temp(*SCC[i]);
        // test for condition (3)
        if (!temp.isEnd() || SCC[i] == contains_release) {
          F_gates.clear();
          return -1;
        }
      };
    } else {
      // by def, if I has no loops, then SCC has one element
      QBBIter temp(*SCC[0]);
      if (!temp.isEnd() || SCC[0] == contains_release) {
        F_gates.push_back(temp);
      }
    }
  };

  // test for condition (2) which can be rephrased as
  // for a dfs, every branch passes through every BB containing gates
  // in (reverse) order of F_gates
  std::set<BasicBlock *> visited;
  std::map<std::pair<BasicBlock *, Instruction *>, bool> Cache;
  if (!verifyLinearChain(&F.getEntryBlock(), visited, Cache, F_gates.rbegin(),
                         F_gates.rend())) {
    F_gates.clear();
    return -2;
  }

  // test for condition (1)
  for (size_t i = 0; i < F_gates.size(); i++) {
    for (QBBIter gate = F_gates[i]; !gate.isEnd(); ++gate) {

      std::vector<QbitRef> qubits = gate.getQubitOperands();
      // test for condition (1)
      for (unsigned j = 0; j < qubits.size(); j++) {
        if (!qubits[j].isResolved() || qubits[j].isArgument()) {
          // F_gates.clear(); //Needed for FLEQ, since qbit args are okay
          return -3;
        }
        for (unsigned long k = j + 1; k < qubits.size(); k++) {
          if (qubits[j].isEqual(qubits[k])) {
            // F_gates.clear();
            return -4;
          }
        }
      };
    };
  };
  // If we got here, q kernel is valid

  if (!isQKernel(F))
    return 0;

  return 1;
}

bool passesQbitsByValue(Function &F) {
  for (auto arg = F.arg_begin(); arg != F.arg_end(); ++arg) {
    if (IntegerType *ITy = dyn_cast<IntegerType>(arg->getType())) {
      if (ITy->getBitWidth() == 16) {
        for (User *user : arg->users()) {
          if (StoreInst *SI = dyn_cast<StoreInst>(user)) {
            return true;
          }
        }
      }
    }
  }
  return false;
}

// TODO: Move ScheduleBase to this repository.
/*void populateGateDurationsInScheduleBase(QuantumModule &QM,
                                         popr::ScheduleBase<unsigned> *schedule,
                                         BasicBlock *QBB, bool use_qid) {

  // Assumptions is that the unsigned in this case is the intermediary
  // MapProgToPhysQubits id, NOT THE QID!

  if (!QM.has_platform)
    return;

  scheduler::MapProgToPhysQubits *qubit_map = &*QM.qubit_maps[QBB];
  if (!qubit_map)
    qubit_map = &*QM.global_qubit_map;
  if (!qubit_map)
    return;

  unsigned num_qids = qubit_map->getNumPhysQubits();

  if (num_qids > QM.getNumPlatformQubits())
    return;

  if (num_qids == 0 && !use_qid)
    return;

  if (num_qids == 0)
    num_qids = QM.getNumPlatformQubits();

  QuantumGates all_gates = QM.gates();

  // get an average gate time to set as a default
  unsigned all_gate_durations = 0;
  unsigned num_gates_instances = 0;

  for (auto gate_iter = all_gates.begin(); gate_iter != all_gates.end();
       ++gate_iter) {

    int id = getGateIdentifier(gate_iter);
    int id_native = getBaseVersionForSpinNative(id);
    unsigned num_qubit_arg = getGateNumQubitOperands(gate_iter);
    if (id_native >= kSpinBegin && id_native < kSpinEnd) {

      if (num_qubit_arg == 1) {
        for (int qid = 0; qid < num_qids; qid++) {

          if (!use_qid && qubit_map->isPhysicalQubitFree(qid))
            continue;

          std::vector<unsigned> idxs;
          std::vector<unsigned> qids;
          if (use_qid) {
            idxs.push_back(qid);
          } else {
            idxs.push_back(qubit_map->getPhysToProg(qid));
          }
          qids.push_back(qid);

          int dur = QM.getGateDurationInClockCycles(id_native, qids);

          if (dur > 0) {
            schedule->setGateDuration(id, idxs, dur);
            all_gate_durations += dur;
            num_gates_instances++;
          }
        }
      } else if (num_qubit_arg == 2) {

        for (int qid1 = 0; qid1 < num_qids - 1; qid1++) {

          if (!use_qid && qubit_map->isPhysicalQubitFree(qid1))
            continue;

          std::vector<unsigned> idxs;
          std::vector<unsigned> qids;
          if (use_qid) {
            idxs.push_back(qid1);
            idxs.push_back(qid1);
          } else {
            idxs.push_back(qubit_map->getPhysToProg(qid1));
            idxs.push_back(qubit_map->getPhysToProg(qid1));
          }
          qids.push_back(qid1);
          qids.push_back(qid1);

          for (int qid2 = qid1 + 1; qid2 < num_qids; qid2++) {

            if (!use_qid && qubit_map->isPhysicalQubitFree(qid2))
              continue;

            if (use_qid) {
              idxs[1] = qid2;
            } else {
              idxs[1] = qubit_map->getPhysToProg(qid2);
            }

            qids[1] = qid2;

            int dur = QM.getGateDurationInClockCycles(id_native, qids);
            if (dur > 0) {
              schedule->setGateDuration(id, idxs, dur);
              all_gate_durations += dur;
              num_gates_instances++;
            }
          }
        }
      } // qubit number
    }   // maps to a spin native
  }     // loop over gates

  // set default
  unsigned dur_def =
      (num_gates_instances == 0 ? 1 : all_gate_durations / num_gates_instances);
  dur_def = std::max<unsigned>(dur_def, 1);
  schedule->setGateDurationDefault(dur_def);
}*/

bool addSimpleGlobalQubitMap(QuantumModule &QM) {

  QM.global_qubit_map = std::make_shared<scheduler::MapProgToPhysQubits>();
  unsigned total = 0;
  for (auto &global : QM.base->globals()) {
    QbitRef temp(&global);
    if (!temp.isNull()) {
      unsigned sz = temp.getRegisterSize().value();
      for (int i = 0; i < sz; i++) {
        temp.setIndexResolved(i);
        QM.global_qubit_map->addProgQubit(temp);
        total++;
      }
    }
  }

  if (QM.has_platform)
    QM.global_qubit_map->setNumPhysQubits(QM.getNumPlatformQubits());
  else
    QM.global_qubit_map->setNumPhysQubits(total);

  bool out = QM.global_qubit_map->setTrivialProgToPhysMap();

  // copyGlobalQubitMapToQBB(QM);

  return out;
}

void copyGlobalQubitMapToQBB(QuantumModule &QM) {
  for (auto &QK : QM) {
    for (auto &QBB : QK) {
      // Create a fresh copy for each QBB of both the initial and final maps.
      QM.qubit_maps[&QBB] = std::make_shared<scheduler::MapProgToPhysQubits>(
          *QM.global_qubit_map);
      QM.final_qubit_maps[&QBB] =
          std::make_shared<scheduler::MapProgToPhysQubits>(
              *QM.global_qubit_map);
    }
  }
}

void updateWithPredecessorQubitMap(BasicBlock *BB, QuantumModule &QM,
                                   bool keep_release) {
  std::set<BasicBlock *> visited = {BB};
  auto qubit_map = QM.qubit_maps[BB];
  // use temporary map to avoid  possibly overwriting the current map
  scheduler::MapProgToPhysQubits temp;
  BasicBlock *pred = BB->getUniquePredecessor();
  if (!pred)
    // there is no predecessor so nothing to do
    return;
  bool check =
      updateWithPredecessorQubitMap(pred, temp, visited, QM, keep_release);
  qubit_map->join(temp);
  assert(qubit_map->isConsistent() && check);
}

bool updateWithPredecessorQubitMap(BasicBlock *BB,
                                   scheduler::MapProgToPhysQubits &passed,
                                   std::set<BasicBlock *> &visited,
                                   QuantumModule &QM, bool keep_release) {

  assert(BB);

  // visited is used to avoid infinite regression from loops in the control flow
  if (visited.find(BB) != visited.end()) {
    return false;
  }
  visited.insert(BB);

  if (isHoldQBB(*BB)) {
    auto qubit_map = QM.qubit_maps[BB];
    if (qubit_map) {
      passed.join(*qubit_map);
      return true;
    }
  } else if (isReleaseQBB(*BB)) {
    if (keep_release) {
      auto qubit_map = QM.qubit_maps[BB];
      if (qubit_map) {
        passed.join(*qubit_map);
        return true;
      }
    } else {
      passed.removeAllProgQubits();
      return true;
    }
  }

  auto pred_it = pred_begin(BB);
  auto end = pred_end(BB);
  if (pred_it == end) {
    passed.join(*QM.global_qubit_map);
    return true;
  }
  bool out = false;
  while (pred_it != end) {
    scheduler::MapProgToPhysQubits temp;
    if (updateWithPredecessorQubitMap(*pred_it, temp, visited, QM,
                                      keep_release)) {
      if (out) {
        passed.intersect(temp);
      } else {
        passed.join(temp);
        out = true;
      }
    }
    ++pred_it;
  }
  return out;
}

void initializeGlobalQubitMap(QuantumModule &QM) {
  // Add the global qubits if there are any
  QM.global_qubit_map = std::make_shared<scheduler::MapProgToPhysQubits>();
  int num_global_prog_qbits = 0;
  for (auto &global : QM.base->globals()) {
    QbitRef temp(&global);
    if (!temp.isNull()) {
      unsigned sz = temp.getRegisterSize().value();
      for (int i = 0; i < sz; i++) {
        temp.setIndexResolved(i);
        QM.global_qubit_map->addProgQubit(temp);
        num_global_prog_qbits++;
      }
    }
  }

  if (QM.has_platform) {
    if (QM.global_qubit_map->getNumProgQubits() > QM.getNumPlatformQubits()) {
      std::string ErrMsg =
          "Number of declared global qubits (" +
          std::to_string(QM.global_qubit_map->getNumProgQubits()) +
          ") exceeds the number of qubits (" +
          std::to_string(QM.getNumPlatformQubits()) +
          ") in the configuration file";
      displayErrorAndExit("QuantumGeneralUtils", ErrMsg);
    }
    QM.global_qubit_map->setNumPhysQubits(QM.getNumPlatformQubits());
  } else
    QM.global_qubit_map->setNumPhysQubits(num_global_prog_qbits);

} // end of initializeGlobalQubitMap

void initializeQBBQubitMap(QuantumModule &QM) {
  int total_global = QM.global_qubit_map->getNumProgQubits();
  bool global_qubits_exist = (total_global > 0) ? true : false;

  for (auto &QK : QM) {
    for (auto &QBB : QK) {
      auto &qubit_map = QM.qubit_maps[&QBB];
      // sync this map with its predecessor qubit map
      qubit_map = std::make_shared<scheduler::MapProgToPhysQubits>();

      // if there are any global qubit, copy the global qubits into the QBB's
      // qubit map
      if (global_qubits_exist) {
        for (int i = 0; i < total_global; i++) {
          aqcc::QbitRef global_qubit = QM.global_qubit_map->getQRef(i);
          QM.qubit_maps[&QBB]->addProgQubit(global_qubit);
        }
      }

      aqcc::QBBIter qi(QBB); // iterator to step through each QBB

      // Add any additional local qubits
      for (qi.gotoBegin(); !qi.isEnd(); ++qi) {
        std::vector<aqcc::QbitRef> wires = qi.getQubitOperands();
        for (std::vector<aqcc::QbitRef>::iterator it = wires.begin();
             it != wires.end(); ++it) {
          aqcc::QbitRef gate_qubit = *it;
          // this is a local qubit
          if (gate_qubit.isLocalAlloc()) {
            QM.qubit_maps[&QBB]->addProgQubit(gate_qubit);
          }
        }
      } // end of for qi
    }
  }
}

std::shared_ptr<scheduler::MapProgToPhysQubits>
initializeQBBQubitMap(std::vector<BasicBlock *> QBBs, QuantumModule &QM,
                      bool keep_release) {

  assert(QBBs.size() > 0);
  BasicBlock *QBB0 = QBBs.front();
  auto &qubit_map = QM.qubit_maps[QBB0];
  // sync this map with its predecessor qubit map
  if (!qubit_map)
    qubit_map = std::make_shared<scheduler::MapProgToPhysQubits>();
  else
    return qubit_map;

  updateWithPredecessorQubitMap(QBB0, QM, keep_release);

  // Add any additional qubits
  for (auto QBB : QBBs) {
    for (aqcc::QBBIter qi(*QBB); !qi.isEnd(); ++qi) {
      std::vector<aqcc::QbitRef> wires = qi.getQubitOperands();
      for (std::vector<aqcc::QbitRef>::iterator it = wires.begin();
           it != wires.end(); ++it) {
        aqcc::QbitRef gate_qubit = *it;
        // is qubit not in the predecessor, then add
        if (!(qubit_map->isAProgQubit(gate_qubit)) && !gate_qubit.isQID()) {
          qubit_map->addProgQubit(gate_qubit);
        }
      }
    } // end of for qi
  }
  return qubit_map;
}

bool addTrivialQubitPlacement(QuantumModule &QM) {

  aqcc::initializeGlobalQubitMap(QM);

  // Add a trivial placement to the global qubits
  bool out = QM.global_qubit_map->setTrivialProgToPhysMap();

  // add a trivial placement for each QBB
  for (auto &QK : QM) {
    for (auto &QBB_scc : QK.qbb_sccs()) {
      bool keep_release = QBB_scc.second;
      auto qubit_map = initializeQBBQubitMap(QBB_scc.first, QM, keep_release);
      unsigned total = qubit_map->getNumProgQubits();
      if (total == 0)
        continue;

      if (QM.has_platform) {
        if (total > QM.getNumPlatformQubits()) {
          std::string ErrMsg = "Number of declared global and local qubits (" +
                               std::to_string(total) +
                               ") exceeds the number of qubits (" +
                               std::to_string(QM.getNumPlatformQubits()) +
                               ") in the configuration file";
          displayErrorAndExit("QuantumGeneralUtils", ErrMsg);
        }
        qubit_map->setNumPhysQubits(QM.getNumPlatformQubits());
      } else
        qubit_map->setNumPhysQubits(total);

      out = out && qubit_map->setTrivialProgToPhysMap();

      // exit if any one QBB cannot be mapped
      if (!out)
        return out;

      // copy the outcome map to all members of the scc (initial and final)
      for (int i = 0; i < QBB_scc.first.size(); i++) {
        QM.qubit_maps[QBB_scc.first[i]] = qubit_map;
        QM.final_qubit_maps[QBB_scc.first[i]] =
            std::make_shared<scheduler::MapProgToPhysQubits>(*qubit_map);
      }

    } // end of for QBB
  }   // end of for QK

  return out;
} // end of addTrivialQubitPlacement

void setSingleSliceBeginAndEnd(BasicBlock &BB, bool CheckEnd) {

  aqcc::QBBIter Qit(BB);
  std::vector<aqcc::ParaRef> Paras;
  Qit.gotoBegin();

  if (!Qit.isEnd()) {
    Paras = Qit.getParametricOperands();

    // ASSUMPTION!: the last argument is always the slice ID

    // set begin slice
    if (!CheckEnd ||
        (Paras.size() > 0 && Paras.back().getType()->isIntegerTy(32))) {
      Paras.back() =
          aqcc::ParaRef(SLICE_BEG, Type::getInt32Ty(BB.getContext()));
      Qit.setParametricOperands(Paras);
    }
  }

  // set end slice
  Qit.gotoEnd();
  --Qit;
  if (!Qit.isBegin() && !Qit.isEnd()) {
    Paras = Qit.getParametricOperands();
    if (!CheckEnd ||
        (Paras.size() > 0 && Paras.back().getType()->isIntegerTy(32))) {
      Paras.back() =
          aqcc::ParaRef(SLICE_END, Type::getInt32Ty(BB.getContext()));
      Qit.setParametricOperands(Paras);
    }
  }
}

void setSerializedSlicing(BasicBlock &BB) {

  for (QBBIter Qit(BB); !Qit.isEnd(); ++Qit) {
    auto Paras = Qit.getParametricOperands();

    // ASSUMPTION!: the last argument is always the slice ID

    // set all slice ids to Begin which is all gates in a single slice
    Paras.back() = aqcc::ParaRef(SLICE_BEG, Type::getInt32Ty(BB.getContext()));
    Qit.setParametricOperands(Paras);
  }
}

std::unique_ptr<Module> separateQuantumFromClassicalModule(Module &M) {

  // create New Module
  // Adapted from llvm 'CloneModule.cpp'

  std::string old_ID = M.getModuleIdentifier();
  if (old_ID == "<stdin>")
    old_ID = "a.ll";
  unsigned ext_pos = old_ID.find(".ll");
  std::string C_ID = old_ID;
  C_ID.insert(ext_pos, "_classical");
  std::string Q_ID = old_ID;
  Q_ID.insert(ext_pos, "_quantum");

  std::unique_ptr<Module> New = std::make_unique<Module>(Q_ID, M.getContext());
  New->setSourceFileName(M.getSourceFileName());
  New->setDataLayout(M.getDataLayout());
  New->setTargetTriple(M.getTargetTriple());

  M.setModuleIdentifier(C_ID);

  // loop over Functions of M, and for each QBB:
  // -make an exact copy in New
  // -delete its body in M
  for (auto &F : M) {
    if (isQKernel(F)) {
      // create new Q Kernel in New
      createEmptyQKernel(F.getName().str(), *New, &F.getEntryBlock(), false);
      F.deleteBody();
    }
  };

  // move intrinsic declarations to New
  for (int id = kSpinBegin; id != kSpinIMM_IDXEnd; id++) {

    // Add the Intrinsic declaration to IR

    int IntrID = getInstrinsicIDFromGateIdentifier(id);
    Function *native_gate = Intrinsic::getDeclaration(&M, IntrID);

    native_gate->dropAllReferences();
    native_gate->removeFromParent();
    if (id >= kSpinEnd)
      Intrinsic::getDeclaration(&*New, IntrID);
  }

  return New;
}

void displayErrorAndExit(std::string PassName, std::string ErrMsg) {
  errs() << "ERROR: Quantum SDK - " << PassName << " says: " << ErrMsg << "\n";
  report_fatal_error("Cannot process further. Exiting...\n");
}

void recursiveMoveBefore(Instruction *I, Instruction *Before, unsigned BaseNum,
                         std::map<Instruction *, unsigned> &ValToNumber,
                         std::map<Instruction *, unsigned> &MovedBefore) {
  // If it's not an instruction, it's defined in scope (Arguments, Globals,
  // constants).
  if (!I)
    return;

  // If they don't have the same parent we cannot consider their relationship.
  if (ValToNumber.find(I) == ValToNumber.end() ||
      ValToNumber.find(Before) == ValToNumber.end())
    return;

  // If the location of the current value is greater than where we need to
  // insert before, we may need to move this operation.
  if (ValToNumber[I] > BaseNum) {
    // Check to see if the value has been previously moved.
    std::map<Instruction *, unsigned>::iterator It = MovedBefore.find(I);
    bool Move = true;
    // If it has, and it was moved before BaseNum, do not move it again.
    if (It != MovedBefore.end()) {
      if (It->second < BaseNum)
        Move = false;
    }

    // If it should be moved, perform it here, and update the movement map.
    if (Move) {
      I->moveBefore(Before);
      MovedBefore[I] = BaseNum;
    }
  }

  // Now check all the operands of this instruction. We will check if the
  // operand comes before each of it's uses in the function, and move it before
  // the current instruction here after if it does not already come before
  // the current operation.
  for (unsigned OpIdx = 0, OpEnd = I->getNumOperands(); OpIdx < OpEnd;
       OpIdx++) {
    Value *V = I->getOperand(OpIdx);
    recursiveMoveBefore(dyn_cast<Instruction>(V), I, BaseNum, ValToNumber,
                        MovedBefore);
  }
}

void updateGatesForBB(BasicBlock &BB, std::vector<Instruction *> &ValsToCheck) {
  std::vector<BasicBlock *> BBs = {&BB};
  updateGatesForBBs(BBs, ValsToCheck);
}

void updateGatesForBBs(std::vector<BasicBlock *> &BBs,
                       std::vector<Instruction *> &ValsToCheck) {
  if (ValsToCheck.size() < 1)
    return;

  // Create mappings from the instruction values to the original ordering in the
  // block.
  std::map<Instruction *, unsigned> ValToNumber;
  unsigned Idx = 0;
  std::set<BasicBlock *> BBSet;
  for (BasicBlock *BB : BBs) {
    BBSet.insert(BB);
    for (Instruction &IRef : *BB) {
      Instruction *I = &IRef;
      ValToNumber[I] = Idx++;
    }
  }

  // Create mappings from the instruction values to where they are moved in the
  // block.
  std::map<Instruction *, unsigned> MovedBefore;

  // For each value, we need to see if it is used in an operation before it is
  // instantiated in the function.
  std::set<Instruction *> Checked;
  for (Instruction *I : ValsToCheck) {
    if (Checked.find(I) != Checked.end())
      continue;

    Checked.insert(I);
    if (BBSet.find(I->getParent()) == BBSet.end())
      continue;

    // Find the earliest use in the function using the previously defined maps.
    Instruction *Earliest = nullptr;
    unsigned EarliestNum = -1;
    for (User *U : I->users()) {
      Instruction *UserI = dyn_cast<Instruction>(U);
      if (!UserI)
        continue;
      if (ValToNumber.find(UserI) == ValToNumber.end())
        continue;
      unsigned CurrUserNum = ValToNumber[UserI];
      if (!Earliest) {
        Earliest = UserI;
        EarliestNum = CurrUserNum;
      } else if (EarliestNum > CurrUserNum) {
        EarliestNum = CurrUserNum;
        Earliest = UserI;
      }
    }

    if (!Earliest)
      continue;

    // We need to check the instruction and its arguments to make sure they
    // are instantiated before the Earliest Instruction's use.  It will never
    // hurt the ordering to move an operation earlier than it currently is, as
    // long as all the other operands to that function are moved earlier as
    // well.
    recursiveMoveBefore(I, Earliest, EarliestNum, ValToNumber, MovedBefore);
  }
  ValsToCheck.clear();
}

} // namespace aqcc
} // namespace llvm
