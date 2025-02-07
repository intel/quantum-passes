//===- QuantumKernelStatsPrint.cpp - Print approx depth to screen ---------===//
//
// Copyright (C) 2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//
//===----------------------------------------------------------------------===//
//
// This file implements a pass which prints an approximate depth to screen.
//
//===----------------------------------------------------------------------===//

#include "IntelQuantumPasses/QuantumCompile/QuantumKernelStatsPrint.h"
#include "IntelQuantumPasses/QuantumAnalysis/QuantumAnnotationGetters.h"
#include "IntelQuantumPasses/QuantumAnalysis/QuantumGateIdentifiers.h"
#include "IntelQuantumPasses/QuantumAnalysis/QuantumIterator.h"
#include "IntelQuantumPasses/QuantumAnalysis/QuantumModule.h"
#include "IntelQuantumPasses/QuantumAnalysis/QuantumSpinNativeToJson.h"
#include "IntelQuantumPasses/QuantumUtils/QuantumGeneralUtils.h"

#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/InstIterator.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Intrinsics.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/PassManager.h"
#include "llvm/IRReader/IRReader.h"
#include "llvm/InitializePasses.h"
#include "llvm/Pass.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/SourceMgr.h"
#include "llvm/Support/raw_ostream.h"

#include <cassert>
#include <fstream>
#include <iostream>

using namespace llvm;
using namespace aqcc;

static void getQBBInfo(QuantumModule &QM, BasicBlock &QBB,
                       std::map<int, unsigned> &gate_count_map,
                       unsigned &tot_num_gates, unsigned &depth,
                       unsigned &unit) {

  // program qubits are mapped to their qubit_map id in the schedule
  // ScheduleBase<unsigned> schedule;
  // bool is_separated = QM.isSimpleQBBSplit();
  // populateGateDurationsInScheduleBase(QM, &schedule, &QBB, is_separated);

  for (QBBIter qit(QBB); !qit.isEnd(); ++qit) {
    std::vector<aqcc::QbitRef> qubits = qit.getQubitOperands();
    std::vector<unsigned> ids;
    for (int i = 0; i < qubits.size(); i++) {
      // TODO: Recall that QM.qubit_maps correspond to the qubit placements at
      // the beginning of the QBBs while QM.final_qubit_maps correspond to
      // those at the end.
      if (qubits[i].isQID())
        ids.push_back(qubits[i].getIndexResolved().value());
      else {
        QM.qubit_maps[&QBB]->addProgQubit(qubits[i]);
        ids.push_back(QM.qubit_maps[&QBB]->getIdFromProgQbitRef(qubits[i]));
      }
    }
    int op = getNoIMMVersionForAll(qit.getIdentifier());
    // schedule.add_gate_to_schedule(op, ids);
    tot_num_gates++;
    gate_count_map[op]++;
  };

  // depth = schedule.getDepth();
  // unit = schedule.getGateDurationDefault();
}

static void printGateStats(std::string name, unsigned tot_count,
                           std::map<int, unsigned> &count_map, unsigned depth,
                           unsigned unit) {

  errs() << "\n Stats for QBB in quantum_kernel - '" << name << "'\n\n";
  errs() << "\tTotal number of gates: " << tot_count << "\n";

  for (auto &pair : count_map) {
    errs() << "\t\tTotal number of "
           << llvm::GetNameFromGateIdentifier(pair.first)
           << " gates: " << pair.second << "\n";
  }

  errs() << "\n\tApproximate depth: " << depth << " time units"
         << "\n\t\t(with an average gate time of " << unit
         << " time units)\n\n";
}

static bool QuantumKernelStatsPrint(QuantumModule &QM) {
  // We will change the how we print based on where we are in the compilation
  QuantumModule::QCompPosition pos = QM.getCompilationPosition();

  if (pos < QuantumModule::kConditioned)
    errs() << "WARNING: Quantum SDK - cannot print circuit stats this early "
              "in the compilation: "
           << pos << "\n";
  else if (pos < QuantumModule::kSimpleQBBSplit) {

    // before QBBs are split out of the QKs, we can use the usual iteration
    // over the QM
    /// ASKME: Do we need the counter?
    // unsigned qbb_cnt = 0;

    for (auto &QK : QM) {
      for (auto &QBB : QK) {

        std::map<int, unsigned> gate_count_map;
        unsigned tot_num_gates = 0;
        unsigned depth = 0;
        unsigned unit = 0;

        getQBBInfo(QM, QBB, gate_count_map, tot_num_gates, depth, unit);
        printGateStats(DemangleQuantumKernelName(QK.getFunction()->getName()),
                       tot_num_gates, gate_count_map, depth, unit);

        // // qbb_cnt++;
      }
    }
  } else if (pos < QuantumModule::kModSplit) {

    // after QBBs are split out of the QKs, we'll have to run over all
    // functions to find the simple QBBs which will get flaged by isQKernel
    // method

    for (auto &F : *QM.getBaseModule()) {
      if (isQKernel(F)) {

        std::map<int, unsigned> gate_count_map;
        unsigned tot_num_gates = 0;
        unsigned depth = 0;
        unsigned unit = 0;

        getQBBInfo(QM, F.getEntryBlock(), gate_count_map, tot_num_gates, depth,
                   unit);
        std::string name = DemangleQuantumKernelName(F.getName());
        // // name = name.substr(0, name.size()-2);
        printGateStats(name, tot_num_gates, gate_count_map, depth, unit);
      }
    }

  } else
    errs() << "WARNING: Quantum SDK - cannot print circuit stats this late in "
              "the compilation: "
           << pos << "\n";
  return false;
}

namespace {

////////////////////////////////////////////////////////////////////////////////
class QuantumKernelStatsPrintLegacyPass : public ModulePass {

public:
  static char ID;

  QuantumModule &QM = QuantumCompilerLinkageLegacyPass::QM;

  /// Minimal constructor.
  QuantumKernelStatsPrintLegacyPass() : ModulePass(ID) {}
  /// Destructor.
  ~QuantumKernelStatsPrintLegacyPass(){};

  bool doInitialization(Module &M) override { return false; }

  void getAnalysisUsage(AnalysisUsage &AU) const override {

    AU.setPreservesAll();
    AU.addRequired<QuantumSpinNativeToJsonWrapperPass>();
  }

  bool runOnModule(Module &M) override {
    QuantumKernelStatsPrint(QM);

    return false;
  }
};
} // namespace

char QuantumKernelStatsPrintLegacyPass::ID = 0;

INITIALIZE_PASS_BEGIN(QuantumKernelStatsPrintLegacyPass, "q-stats-print",
                      "QuantumKernelStatsPrint Pass", false, false)
INITIALIZE_PASS_END(QuantumKernelStatsPrintLegacyPass, "q-stats-print",
                    "QuantumKernelStatsPrint Pass", false, false)

PreservedAnalyses QuantumKernelStatsPrintPass::run(Module &M,
                                                   ModuleAnalysisManager &MAM) {
  QuantumModuleProxy &QMP = MAM.getResult<QuantumCompilerLinkageAnalysis>(M);
  QuantumKernelStatsPrint(*QMP.QM);

  return PreservedAnalyses::all();
}
