//===- XToHZHPass.cpp -----------------------------------------------------===//
// INTEL CONFIDENTIAL
//
// Copyright (C) 2020 - 2023 Intel Corporation
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
//
// This is an example pass which converts all X gates with a sequence of HZH
// gates.
//
//===----------------------------------------------------------------------===//

#include "ToffoliWithCircuitPass.h"
#include "IntelQuantumPasses/QuantumAnalysis/QuantumCompilerLinkage.h"
#include "IntelQuantumPasses/QuantumAnalysis/QuantumGateIdentifiers.h"
#include "IntelQuantumPasses/QuantumAnalysis/QuantumIterator.h"
#include "IntelQuantumPasses/QuantumAnalysis/QuantumModule.h"
#include "IntelQuantumPasses/QuantumUtils/QuantumCircuit.h"
#include "IntelQuantumPasses/QuantumUtils/QuantumGeneralUtils.h"
#include "llvm/IR/Function.h"
#include "llvm/Pass.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

#define DEBUG_TYPE "test_circuit"

static QuantumCircuit *createToffoli(QuantumModule &QM) {
  // Create a new Circuit
  std::vector<QuantumOpParam> Params;
  Params.push_back(QuantumOpParam(OpParamTy::QUBITPTR));
  Params.push_back(QuantumOpParam(OpParamTy::QUBITPTR));
  Params.push_back(QuantumOpParam(OpParamTy::QUBITPTR));
  QuantumCircuit *ToffoliCircuit =
      QuantumCircuit::Create(QM, Params, "ToffoliGateDecomp");
  QuantumCircuit *SubToffoliCircuit = ToffoliCircuit->getCircuitList()[0];
  std::vector<QuantumOpParam> DefinedParams = ToffoliCircuit->getOpParams();

  Params = {DefinedParams[2]};
  errs() << DefinedParams[0].ParamType << "\n";
  QuantumOp::Create("H", QM, Params, *SubToffoliCircuit);
  Params = {DefinedParams[1], DefinedParams[2]};
  QuantumOp::Create("CNOT", QM, Params, *SubToffoliCircuit);
  Params = {DefinedParams[2]};
  QuantumOp::Create("Tdag", QM, Params, *SubToffoliCircuit);
  Params = {DefinedParams[0], DefinedParams[2]};
  QuantumOp::Create("CNOT", QM, Params, *SubToffoliCircuit);
  Params = {DefinedParams[2]};
  QuantumOp::Create("T", QM, Params, *SubToffoliCircuit);
  Params = {DefinedParams[1], DefinedParams[2]};
  QuantumOp::Create("CNOT", QM, Params, *SubToffoliCircuit);
  Params = {DefinedParams[2]};
  QuantumOp::Create("Tdag", QM, Params, *SubToffoliCircuit);
  Params = {DefinedParams[0], DefinedParams[2]};
  QuantumOp::Create("CNOT", QM, Params, *SubToffoliCircuit);
  Params = {DefinedParams[1]};
  QuantumOp::Create("T", QM, Params, *SubToffoliCircuit);
  Params = {DefinedParams[2]};
  QuantumOp::Create("T", QM, Params, *SubToffoliCircuit);
  Params = {DefinedParams[2]};
  QuantumOp::Create("H", QM, Params, *SubToffoliCircuit);
  Params = {DefinedParams[0], DefinedParams[1]};
  QuantumOp::Create("CNOT", QM, Params, *SubToffoliCircuit);
  Params = {DefinedParams[1]};
  QuantumOp::Create("T", QM, Params, *SubToffoliCircuit);
  Params = {DefinedParams[2]};
  QuantumOp::Create("Tdag", QM, Params, *SubToffoliCircuit);
  Params = {DefinedParams[0], DefinedParams[1]};
  QuantumOp::Create("CNOT", QM, Params, *SubToffoliCircuit);

  return ToffoliCircuit;
}

static void replaceToffolis(QuantumModule &QM) {

  std::vector<QuantumCircuit *> QCs;
  for (QuantumKernel &QK : QM) {
    QuantumCircuit *ExistingQC = QuantumCircuit::Create(QM, QK);
    QCs.push_back(ExistingQC);
  }

  // Create a new Circuit
  QuantumCircuit *ToffoliCircuit = createToffoli(QM);
  ToffoliCircuit->getBaseValue()->dump();
  for (QuantumCircuit *QC : QCs) {
    errs() << "Original:\n";
    QC->getBaseValue()->dump();
    std::vector<QuantumOp *> ToReplace;
    for (QuantumCircuitOp *QCO : *QC) {
      if (QCO->getKind() != OpKind::Quantum)
        continue;
      QuantumOp *QO = static_cast<QuantumOp *>(QCO);
      if (QO->getOpName().find("Toffoli") != std::string::npos)
        ToReplace.push_back(QO);
    }

    for (QuantumOp *QO : ToReplace) {
      std::map<unsigned, QuantumOpParam> Mapping;
      for (std::pair<QbitRef, unsigned> &Pair : QO->getQubits())
        Mapping.insert(std::make_pair(
            Pair.second, QuantumOpParam(OpParamTy::QUBIT, Pair.first)));
      QC->copyAndInsertCircuit(ToffoliCircuit, *QO, Mapping);
      QO->removeGate();
    }
    errs() << "------------\nAfter:\n";
    QC->getBaseValue()->dump();
  }
}

PreservedAnalyses DecomposeToffoliCircuitPass::run(Module &M,
                                                   ModuleAnalysisManager &MAM) {
  llvm::aqcc::QuantumModuleProxy &QMP =
      MAM.getResult<QuantumCompilerLinkageAnalysis>(M);
  replaceToffolis(*QMP.QM);

  return PreservedAnalyses::none();
}

#undef DEBUG_TYPE