//===- QuantumInitPlacement.cpp -------------------------------------------===//
//
// Copyright (C) 2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//
//===----------------------------------------------------------------------===//
//
//
// Implementing functions declared in QuantumInitPlacement.h
//
//===----------------------------------------------------------------------===//

#include "IntelQuantumPasses/QuantumAnalysis/QuantumInitPlacement.h"
#include "IntelQuantumPasses/InitializeQuantumPasses.h"
#include "IntelQuantumPasses/QuantumUtils/QuantumGeneralUtils.h"
#include "IntelQuantumPasses/QuantumUtils/QuantumPlacementUtils.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/Debug.h"
#include <algorithm>
#include <iterator>
#include <random>
#include <set>
#include <unordered_set>
#include <utility>

#define DEBUG_TYPE "initPlacement"

using namespace aqcc;

extern cl::list<std::string> quantum_debug;

namespace llvm {

cl::opt<std::string> PlacementMethod("placement-method",
                                     cl::desc("Placement method"),
                                     cl::init("none"));
static cl::opt<size_t>
    num_iterations("iterations",
                   cl::desc("Number of iterations of local search"),
                   cl::init(100000));
static cl::opt<size_t> num_resets("resets",
                                  cl::desc("Number of resets for local search"),
                                  cl::init(1));

std::string QuantumInitPlacement::placement_method_ = "trivial";

int partition(std::vector<unsigned> &v, int start, int end) {

  int pivot = end;
  int j = start;
  for (int i = start; i < end; ++i) {
    if (v[i] < v[pivot]) {
      std::swap(v[i], v[j]);
      ++j;
    }
  }
  std::swap(v[j], v[pivot]);
  return j;
}

void quicksort(std::vector<unsigned> &v, int start, int end) {

  if (start < end) {
    int p = partition(v, start, end);
    quicksort(v, start, p - 1);
    quicksort(v, p + 1, end);
  }
}

///////////////////////////////// Mapping methods
void QuantumInitPlacement::validateCustomSubset(CustomMap &custom_subset_,
                                                int num_phys_qubits_) {

  std::vector<bool> present(num_phys_qubits_, false);
  for (auto &pair : custom_subset_) {
    for (auto qid : pair.second) {
      if (qid != scheduler::MapProgToPhysQubits::GetVirtualValue())
        if (present[qid]) {
          std::string ErrMsg = "User-defined qubit mapping has "
                               "duplicate indices.\n";
          if (qid == 0)
            ErrMsg += "This may be due to an insufficient number of qubit "
                      "identifiers.\n If you wish some qubits to be mapped "
                      "by the compiler, use -1 as a placeholder.\n";
          displayErrorAndExit("Quantum Init Placement Pass", ErrMsg);
        } else
          present[(unsigned)qid] = true;
    }
  }

  return;
}

int QuantumInitPlacement::getCustomSubset(QuantumModule &QM,
                                          CustomMap &custom_subset_) {
  int num_phys_qubits_ = QM.global_qubit_map->getNumPhysQubits();

  // Get the user defined placement for the global qubits only
  for (auto &global : QM.base->globals()) {
    QbitRef qreg(&global);
    if (!qreg.isNull()) {
      auto to_add = std::make_pair(qreg, std::vector<unsigned>());
      Constant *g_const = global.getInitializer();

      unsigned size = qreg.getRegisterSize().value();

      unsigned num_zeros = 0; // this will be used to check for zeroinitialized
      unsigned is_aggregate = g_const->getType()->isAggregateType();
      for (unsigned i = 0; i < size; i++) {
        Constant *g_elem = g_const;

        if (is_aggregate)
          g_elem = g_const->getAggregateElement(i);

        if (const ConstantInt *CI = dyn_cast<ConstantInt>(g_elem)) {
          int idx = CI->getSExtValue();
          if (idx == 0)
            num_zeros++;

          if (idx > num_phys_qubits_ - 1) {
            std::string err_msg = "User-defined mapping used qubit index > "
                                  "possible physical qubits at ";
            std::string g_name = global.getName().data();
            err_msg += (g_name + "[" + std::to_string(i) + "]\n");
            displayErrorAndExit("Quantum Init Placement Pass", err_msg);
            return EXIT_FAILURE;
          } else if (idx < 0)
            to_add.second.push_back(
                scheduler::MapProgToPhysQubits::GetVirtualValue());
          else
            to_add.second.push_back((unsigned)idx);
        }
      }
      // check for zero initialized
      if (num_zeros < size)
        custom_subset_.push_back(to_add);
    }
  }

  // validate the user-defined subset
  validateCustomSubset(custom_subset_, num_phys_qubits_);

  return EXIT_SUCCESS;
}

int QuantumInitPlacement::setCustomSetToPhyMap(QuantumModule &QM) {

  aqcc::initializeGlobalQubitMap(QM);

  // Update custom_subset from user code
  CustomMap custom_subset_;
  // only allow global qubits to take in custom set
  unsigned status =
      getCustomSubset(QM, custom_subset_); // only contains global qubits
  if (status == EXIT_FAILURE) {
    return status;
  }

  // place the custom subset
  auto global_map = QM.global_qubit_map;
  unsigned glob_total = global_map->getNumProgQubits();
  for (auto &pair : custom_subset_) {
    auto range = global_map->getRangeFromProgQbitRef(pair.first);
    global_map->setCustomProgToPhysMap(pair.second, range.first, range.second);
  }

  // Densely place any unmapped qubits
  auto mapped_set = global_map->getMappedPhysQubits();
  findBestDenseSet(QM, glob_total, mapped_set);

  if (!global_map->setCustomProgToPhysMap(mapped_set)) {
    displayErrorAndExit("Quantum Init Placement Pass",
                        "Some global qubits were not properly placed.");
  }

  // Trivially place any remaining qubits
  if (!global_map->setTrivialProgToPhysMap()) {
    displayErrorAndExit("Quantum Init Placement Pass",
                        "Some global qubits were not properly placed.");
  }

  for (auto &QK : QM) {
    for (auto &QBB_scc : QK.qbb_sccs()) {
      bool keep_release = QBB_scc.second;
      auto qubit_map = initializeQBBQubitMap(QBB_scc.first, QM, keep_release);
      unsigned total = qubit_map->getNumProgQubits();
      if (QM.has_platform) {
        if (total > QM.getNumPlatformQubits())
          displayErrorAndExit("Quantum Init Placement Pass",
                              "program has more program qubits than available "
                              "physical qubits.");
        qubit_map->setNumPhysQubits(QM.getNumPlatformQubits());
      } else
        qubit_map->setNumPhysQubits(total);

      // Densely place any unmapped qubits
      auto mapped_set = qubit_map->getMappedPhysQubits();
      findBestDenseSet(QM, total, mapped_set);

      if (!qubit_map->setCustomProgToPhysMap(mapped_set)) {
        displayErrorAndExit(
            "Quantum Init Placement Pass",
            "Some qubits were not properly placed in function " +
                DemangleQuantumKernelName(QK.getFunction()->getName()));
      }

      // Trivially place any remaining qubits
      if (!qubit_map->setTrivialProgToPhysMap()) {
        displayErrorAndExit(
            "Quantum Init Placement Pass",
            "Some qubits were not properly placed in function " +
                DemangleQuantumKernelName(QK.getFunction()->getName()));
      }

      // copy the outcome map to all members of the scc (initial and final)
      for (int i = 0; i < QBB_scc.first.size(); i++) {
        QM.qubit_maps[QBB_scc.first[i]] = qubit_map;
        QM.final_qubit_maps[QBB_scc.first[i]] =
            std::make_shared<scheduler::MapProgToPhysQubits>(*qubit_map);
      }

      if (!qubit_map->isConsistent())
        displayErrorAndExit(
            "Quantum Init Placement Pass",
            "maps are inconsistent. This is potentially a bug.");
    }
  }
  return EXIT_SUCCESS;
}

int QuantumInitPlacement::setDenseProgToPhysMap(QuantumModule &QM) {

  aqcc::initializeGlobalQubitMap(QM);

  // if there are global qubits declared
  // use dense method on the global qubits
  unsigned num_global = QM.global_qubit_map->getNumProgQubits();

  // get the best dense subset
  std::set<unsigned> best_subset;
  findBestDenseSet(QM, num_global, best_subset);

  if (best_subset.size() < num_global) {
    errs() << "WARNING: Quantum SDK - [InitPlacement] Failed to obtain a "
              "physical qubit subset using Dense method. Switching to "
              "trivial placement\n";
    placement_method_ = "trivial";
    placeProgToPhysQubits(QM);
  } else {
    if (!QM.global_qubit_map->setCustomProgToPhysMap(best_subset)) {
      return EXIT_FAILURE;
    }
  }

  bool out = true;
  // Now assign best dense subset to each QBB, where the
  // "setCustomProgToPhysMap" will handle cases where elements
  // of that subset have already been assigned
  for (auto &QK : QM) {
    for (auto &QBB_scc : QK.qbb_sccs()) {
      bool keep_release = QBB_scc.second;
      auto qubit_map = initializeQBBQubitMap(QBB_scc.first, QM, keep_release);
      unsigned total = qubit_map->getNumProgQubits();
      unsigned to_place = qubit_map->getNumMappedQubits();

      if (total == 0)
        continue;
      if (QM.has_platform) {
        if (total > QM.getNumPlatformQubits())
          displayErrorAndExit("Quantum Init Placement Pass",
                              "program has more program qubits than available "
                              "physical qubits.");
        qubit_map->setNumPhysQubits(QM.getNumPlatformQubits());
      } else
        qubit_map->setNumPhysQubits(total);

      // Densely place any unmapped qubits
      auto mapped_set = qubit_map->getMappedPhysQubits();
      findBestDenseSet(QM, total, mapped_set);

      if (!qubit_map->setCustomProgToPhysMap(mapped_set)) {
        displayErrorAndExit(
            "Quantum Init Placement Pass",
            "Some qubits were not properly placed in function " +
                DemangleQuantumKernelName(QK.getFunction()->getName()));
      }

      // Trivially place any remaining qubits
      if (!qubit_map->setTrivialProgToPhysMap()) {
        displayErrorAndExit(
            "Quantum Init Placement Pass",
            "Some qubits were not properly placed in function " +
                DemangleQuantumKernelName(QK.getFunction()->getName()));
      }

      // copy the outcome map to all members of the scc (initial and final)
      for (int i = 0; i < QBB_scc.first.size(); i++) {
        QM.qubit_maps[QBB_scc.first[i]] = qubit_map;
        QM.final_qubit_maps[QBB_scc.first[i]] =
            std::make_shared<scheduler::MapProgToPhysQubits>(*qubit_map);
      }

      if (!qubit_map->isConsistent())
        displayErrorAndExit(
            "Quantum Init Placement Pass",
            "maps are inconsistent. This is potentially a bug.");
    }
  }
  return EXIT_SUCCESS;
}

int QuantumInitPlacement::setTrivialProgToPhysMap(QuantumModule &QM) {
  if (!addTrivialQubitPlacement(QM))
    return EXIT_FAILURE;
  return EXIT_SUCCESS;
}

int QuantumInitPlacement::setLocalProgToPhysMap(QuantumModule &QM) {
  aqcc::initializeGlobalQubitMap(QM);
  aqcc::initializeQBBQubitMap(QM);

  std::unordered_map<unsigned, unsigned> local_placement;

  // Important assumptions
  // qbits of local scope have larger program qbit id then all qbits of global
  // scope
  // local qbits can be reused for different QBBs

  unsigned total_global_qbits = QM.global_qubit_map->getNumProgQubits();
  unsigned total_prog_qbits = QM.global_qubit_map->getNumProgQubits();
  unsigned total_phys_qbits = QM.getNumPlatformQubits();

  // ASKME: Should this be done once in the Quantum Module, independent of the
  // passes?
  for (auto &QK : QM) {
    for (auto &QBB : QK) {
      // This is global qbits + local qubits of QBB
      unsigned num_qbb_qbits = QM.qubit_maps[&QBB]->getNumProgQubits();
      if (num_qbb_qbits > total_prog_qbits) {
        total_prog_qbits = num_qbb_qbits;
      }
    }
  }

  LocalSearch local_search;
  ApplicationGraph application_graph;
  local_search.setNumIterations(num_iterations);
  local_search.setNumResets(num_resets);

  std::vector<unsigned> all_phys_qbits(total_phys_qbits);
  std::iota(all_phys_qbits.begin(), all_phys_qbits.end(), 0);

  if (total_global_qbits > 0) {
    application_graph.constructApplicationGraph(QM);
    std::vector<unsigned> program_qubits_to_map(total_prog_qbits);
    std::iota(program_qubits_to_map.begin(), program_qubits_to_map.end(), 0);
    std::unordered_map<unsigned, unsigned> fixed_qubits;
    std::vector<unsigned> starting_qubits;

    local_placement = local_search.localSearch(
        program_qubits_to_map, all_phys_qbits, fixed_qubits, starting_qubits,
        application_graph, QM);
    std::unordered_map<unsigned, unsigned> global_map;

    // Assumes global qubit ids are lower than local qubit ids
    for (size_t global_qbit = 0; global_qbit < total_global_qbits;
         ++global_qbit) {
      global_map[global_qbit] = local_placement[global_qbit];
    }

    if (!QM.global_qubit_map->setCustomProgToPhysMap(global_map)) {
      return EXIT_FAILURE;
    }
  }

  // Reset QBB maps
  for (auto &QK : QM) {
    for (auto &QBB : QK) {
      QM.qubit_maps[&QBB] = nullptr;
    }
  }

  for (auto &QK : QM) {
    for (auto &QBB_scc : QK.qbb_sccs()) {
      bool keep_release = QBB_scc.second;
      auto qubit_map = initializeQBBQubitMap(QBB_scc.first, QM, keep_release);
      unsigned total = qubit_map->getNumProgQubits();

      if (total == 0)
        continue;
      if (QM.has_platform) {
        if (total > QM.getNumPlatformQubits())
          displayErrorAndExit("Quantum Init Placement Pass",
                              "program has more program qubits than available "
                              "physical qubits.");
        qubit_map->setNumPhysQubits(QM.getNumPlatformQubits());
      } else
        qubit_map->setNumPhysQubits(total);

      auto mapped_set = qubit_map->getMappedPhysQubits();
      unsigned num_mapped_qubits = qubit_map->getNumMappedQubits();

      std::vector<unsigned> program_qubits_to_place;
      std::vector<unsigned> phys_qubits_to_use;
      std::unordered_map<unsigned, unsigned> fixed_qubits;

      for (auto &qubit : all_phys_qbits) {
        if (mapped_set.count(qubit) > 0) {
          unsigned prog_qubit = qubit_map->getPhysToProg(qubit);
          fixed_qubits.insert(std::make_pair(prog_qubit, qubit));
        } else {
          phys_qubits_to_use.push_back(qubit);
        }
      }

      for (unsigned prog_qubit = 0; prog_qubit < total; ++prog_qubit) {
        if (fixed_qubits.count(prog_qubit) == 0) {
          program_qubits_to_place.push_back(prog_qubit);
        }
      }

      local_placement.clear();
      local_search.localSearchReset();
      application_graph.graph.clear();

      for (auto &QBB_ptr : QBB_scc.first) {
        QM.qubit_maps[QBB_ptr] = qubit_map;
        application_graph.addQBBToGraph(*QBB_ptr, QM);
      }

      std::vector<unsigned> starting_qubits;
      auto placement = local_search.localSearch(
          program_qubits_to_place, phys_qubits_to_use, fixed_qubits,
          starting_qubits, application_graph, QM);

      if (!qubit_map->setCustomProgToPhysMap(placement)) {
        displayErrorAndExit(
            "Quantum Init Placement Pass",
            "Some qubits were not properly placed in function " +
                DemangleQuantumKernelName(QK.getFunction()->getName()));
      }

      for (auto &QBB_ptr : QBB_scc.first) {
        QM.qubit_maps[QBB_ptr] = qubit_map;
        QM.final_qubit_maps[QBB_ptr] =
            std::make_shared<scheduler::MapProgToPhysQubits>(*qubit_map);
      }
    }
  }

  return EXIT_SUCCESS;
}

int QuantumInitPlacement::placeProgToPhysQubits(QuantumModule &QM) {

  int status;
  if (placement_method_ == "trivial") {
    status = QuantumInitPlacement::setTrivialProgToPhysMap(QM);
  } else if (placement_method_ == "dense") {
    status = setDenseProgToPhysMap(QM);
  } else if (placement_method_ == "custom") {
    status = setCustomSetToPhyMap(QM);
  } else if (placement_method_ == "greedy") {
    displayErrorAndExit("Quantum Init Placement Pass",
                        "unsupported qubit placement method.");
  } else if (placement_method_ == "local") {
    status = setLocalProgToPhysMap(QM);
  } else {
    displayErrorAndExit("Quantum Init Placement Pass",
                        "unknown qubit placement method.");
  }

  bool debug_print = false;
  if (!quantum_debug.empty()) {
    for (std::string &DebugPass : quantum_debug) {
      debug_print = (DebugPass == "initPlacement");
    }
  }

  for (auto &QK : QM) {
    for (auto &QBB : QK) {
      DEBUG_WITH_TYPE("initPlacement", dbgs() << "prog to phy mapping\n");
      if (debug_print)
        errs() << "prog to phy mapping\n";
      std::vector<unsigned> progToPhy = QM.qubit_maps[&QBB]->progToPhysMap();
      for (int i = 0; i < QM.qubit_maps[&QBB]->getNumProgQubits(); i++) {
        DEBUG_WITH_TYPE("initPlacement", dbgs() << "prog qbit " << i
                                                << " placed at phy location "
                                                << progToPhy[i] << "\n");
        if (debug_print) {
          errs() << "prog qbit " << i << " placed at phy location "
                 << progToPhy[i] << "\n";
        }
      }

      DEBUG_WITH_TYPE("initPlacement", dbgs() << "phy to prog mapping\n");
      if (debug_print)
        errs() << "phy to prog mapping\n";
      std::vector<unsigned> phyToProg = QM.qubit_maps[&QBB]->physToProgMap();
      for (int i = 0; i < phyToProg.size(); i++) {
        DEBUG_WITH_TYPE("initPlacement", dbgs() << "phy location " << i
                                                << " has prog qbit "
                                                << phyToProg[i] << "\n");
        if (debug_print) {
          errs() << "phy location " << i << " has prog qbit " << phyToProg[i]
                 << "\n";
        }
      }
    }
  }

  return status;
}

bool QuantumInitPlacement::run(Module &M, QuantumModule &QM) {
  // This pass only works if a platform config is provided
  if (!QM.has_platform)
    displayErrorAndExit("Quantum Init Placement",
                        "Please provide a platform config to proceed.");

  // set the placement-method
  if (PlacementMethod != "none")
    setPlacementMethod(PlacementMethod);

  // When needed, one can print information on the platform/machine using:
  LLVM_DEBUG(QM.MachinePrint());

  QM.qubit_maps.clear();
  int status = placeProgToPhysQubits(QM);

  if (status != EXIT_SUCCESS) {
    displayErrorAndExit("Quantum Init Placement Pass",
                        "Unable to map program qubits to physical qubits.");
  }

  return false;
} // runOnModule

} // namespace llvm

namespace {
struct QuantumInitPlacementLegacyPass : public ModulePass {

  static char ID; // Pass identification, replacement for typeid
  QuantumModule &QM = QuantumCompilerLinkageLegacyPass::QM;

  QuantumInitPlacementLegacyPass() : ModulePass(ID) {}

  ~QuantumInitPlacementLegacyPass() {}

  void getAnalysisUsage(AnalysisUsage &AU) const override {

    AU.setPreservesAll();
    // AU.addRequired<QuantumAnnotationsToJsonWrapperPass>();
    // AU.addRequired<QuantumSpinNativeToJsonWrapperPass>();
  }

  virtual bool doInitialization(Module &M) override { return false; }

  bool runOnModule(Module &M) override {
    QuantumInitPlacement QIP;
    return QIP.run(M, QM);
  }
};

class QuantumInitPlacementLegacyWrapperPass : public ModulePass {
  QuantumInitPlacementLegacyPass QIP;

public:
  static char ID; // Pass identification, replacement for typeid

  QuantumInitPlacementLegacyWrapperPass() : ModulePass(ID) {
    initializeQuantumInitPlacementLegacyWrapperPassPass(
        *PassRegistry::getPassRegistry());
  }

  QuantumInitPlacementLegacyPass &getQuantumInitPlacementPass() { return QIP; }

  const QuantumInitPlacementLegacyPass &getQuantumInitPlacementPass() const {
    return QIP;
  }

  virtual bool doInitialization(Module &M) override {
    return QIP.doInitialization(M);
  };

  bool runOnModule(Module &M) override { return QIP.runOnModule(M); }
};
} // namespace

PreservedAnalyses QuantumInitPlacementPass::run(Module &M,
                                                ModuleAnalysisManager &MAM) {
  QuantumModuleProxy &QMP = MAM.getResult<QuantumCompilerLinkageAnalysis>(M);
  QuantumInitPlacement QIP;

  QIP.run(M, *QMP.QM);

  return PreservedAnalyses::all();
}

char QuantumInitPlacementLegacyPass::ID = 0;
char QuantumInitPlacementLegacyWrapperPass::ID = 0;

INITIALIZE_PASS_BEGIN(QuantumInitPlacementLegacyPass, "q-init-placement",
                      "Get LLVM Quantum Initial Placement.", false, true)
INITIALIZE_PASS_END(QuantumInitPlacementLegacyPass, "q-init-placement",
                    "Get LLVM Quantum Initial Placement.", false, true)

INITIALIZE_PASS_BEGIN(QuantumInitPlacementLegacyWrapperPass,
                      "q-init-placement-wrap",
                      "Quantum Initial Placement wrapper", true, true)
INITIALIZE_PASS_END(QuantumInitPlacementLegacyWrapperPass,
                    "q-init-placement-wrap",
                    "Quantum Initial Placement wrapper", true, true)

#undef DEBUG_TYPE
