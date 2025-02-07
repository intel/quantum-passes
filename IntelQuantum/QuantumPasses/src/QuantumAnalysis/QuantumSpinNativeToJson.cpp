//===- QuantumSpinNativeToJson.cpp - Add Spin Native annotations to JSON --===//
//
// Copyright (C) 2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//
//===----------------------------------------------------------------------===//
//
// This file implements a module pass which adds a fixed set of Metadata to the
// JSON map corresponding to spin native gates.
//
//===----------------------------------------------------------------------===//

#include "IntelQuantumPasses/QuantumAnalysis/QuantumSpinNativeToJson.h"
#include "IntelQuantumPasses/QuantumAnalysis/QuantumGateIdentifiers.h" // Enumerate for gate identifiers
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

#include <cmath>   // std::sqrt, std::pow
#include <complex> // std::conj
#include <fstream>
#include <iostream>
#include <set>
#include <sstream>
#include <string>

using namespace llvm;

namespace llvm {

std::vector<std::string>
    QuantumSpinNativeToJson::SpinNativeGateAnnotations = {"{ \
    	\"matrix_real\" : \"NativeRZ\", \
   		\"matrix_imag\" : \"NativeRZ\", \
    	\"matrix_order\" : \"rm\", \
    	\"is_hermitian\" : false, \
    	\"is_unitary\" : true, \
    	\"is_mutable\" : true, \
    	\"is_user_pulse\" : false, \
   		\"qubit_list\" : [0], \
    	\"parametric_list\" : [1, 2], \
    	\"control_qubit_list\" : [], \
    	\"local_basis_list\" : [1], \
    	\"identifier\" : " + std::to_string(kSpinRotZ) + " \
  	}",
                                                          "{ \
    	\"matrix_real\" : \"NativeRXY\", \
   		\"matrix_imag\" : \"NativeRXY\", \
    	\"matrix_order\" : \"rm\", \
    	\"is_hermitian\" : false, \
    	\"is_unitary\" : true, \
    	\"is_mutable\" : true, \
    	\"is_user_pulse\" : false, \
   		\"qubit_list\" : [0], \
    	\"parametric_list\" : [1, 2, 3], \
    	\"control_qubit_list\" : [], \
    	\"local_basis_list\" : [0], \
    	\"identifier\" : " + std::to_string(kSpinRotXY) + " \
  	}",
                                                          "{ \
   		\"matrix_real\" : \"NativeCPhase\", \
   	    \"matrix_imag\" : \"NativeCPhase\", \
   		\"matrix_order\" : \"rm\", \
   		\"is_hermitian\" : false, \
   		\"is_unitary\" : true, \
   		\"is_mutable\" : true, \
    	\"is_user_pulse\" : false, \
  		\"qubit_list\" : [0, 1], \
    	\"parametric_list\" : [2, 3], \
    	\"control_qubit_list\" : [0], \
    	\"local_basis_list\" : [1, 1], \
    	\"identifier\" : " + std::to_string(kSpinCPhase) + " \
  	}",
                                                          "{ \
    	\"matrix_real\" : \"NativeSwapalp\", \
   		\"matrix_imag\" : \"NativeSwapalp\", \
    	\"matrix_order\" : \"rm\", \
    	\"is_hermitian\" : false, \
    	\"is_unitary\" : true, \
    	\"is_user_pulse\" : false, \
   		\"is_mutable\" : true, \
    	\"qubit_list\" : [0, 1], \
    	\"parametric_list\" : [2, 3], \
    	\"control_qubit_list\" : [], \
    	\"local_basis_list\" : [0, 0], \
    	\"identifier\" : " + std::to_string(kSpinSwapalp) + " \
  	}",
                                                          "{ \
    	\"matrix_real\" : \"NativeMeasZ\", \
   		\"matrix_imag\" : \"NativeMeasZ\", \
    	\"matrix_order\" : \"rm\", \
    	\"is_hermitian\" : true, \
    	\"is_unitary\" : false, \
    	\"is_mutable\" : true, \
    	\"is_user_pulse\" : false, \
  		\"qubit_list\" : [0], \
    	\"parametric_list\" : [1, 2 ,3], \
    	\"control_qubit_list\" : [], \
    	\"local_basis_list\" : [1], \
    	\"identifier\" : " + std::to_string(kSpinMeasZ) + " \
  	}",
                                                          "{ \
    	\"matrix_real\" : \"NativePrepZ\", \
   		\"matrix_imag\" : \"NativePrepZ\", \
    	\"matrix_order\" : \"rm\", \
    	\"is_hermitian\" : false, \
    	\"is_unitary\" : false, \
    	\"is_mutable\" : true, \
    	\"is_user_pulse\" : false, \
 		\"qubit_list\" : [0], \
    	\"parametric_list\" : [1], \
    	\"control_qubit_list\" : [], \
    	\"local_basis_list\" : [1], \
    	\"identifier\" : " + std::to_string(kSpinPrepZ) + " \
  	}",
                                                          "{ \
    	\"matrix_real\" : \"NativeRZ_IMM\", \
   		\"matrix_imag\" : \"NativeRZ_IMM\", \
    	\"matrix_order\" : \"rm\", \
    	\"is_hermitian\" : false, \
    	\"is_unitary\" : true, \
    	\"is_mutable\" : true, \
    	\"is_user_pulse\" : false, \
   		\"qubit_list\" : [0], \
    	\"parametric_list\" : [1, 2], \
    	\"control_qubit_list\" : [], \
    	\"local_basis_list\" : [1], \
    	\"identifier\" : " + std::to_string(kSpinRotZ_IMM) + " \
  	}",
                                                          "{ \
    	\"matrix_real\" : \"NativeRXY_IMM\", \
   		\"matrix_imag\" : \"NativeRXY_IMM\", \
    	\"matrix_order\" : \"rm\", \
    	\"is_hermitian\" : false, \
    	\"is_unitary\" : true, \
    	\"is_mutable\" : true, \
    	\"is_user_pulse\" : false, \
   		\"qubit_list\" : [0], \
    	\"parametric_list\" : [1, 2, 3], \
    	\"control_qubit_list\" : [], \
    	\"local_basis_list\" : [0], \
    	\"identifier\" : " + std::to_string(kSpinRotXY_IMM) + " \
  	}",
                                                          "{ \
   		\"matrix_real\" : \"NativeCPhase_IMM\", \
   	    \"matrix_imag\" : \"NativeCPhase_IMM\", \
   		\"matrix_order\" : \"rm\", \
   		\"is_hermitian\" : false, \
   		\"is_unitary\" : true, \
   		\"is_mutable\" : true, \
    	\"is_user_pulse\" : false, \
  		\"qubit_list\" : [0, 1], \
    	\"parametric_list\" : [2, 3], \
    	\"control_qubit_list\" : [0], \
    	\"local_basis_list\" : [1, 1], \
    	\"identifier\" : " + std::to_string(kSpinCPhase_IMM) + " \
  	}",
                                                          "{ \
    	\"matrix_real\" : \"NativeSwapalp_IMM\", \
   		\"matrix_imag\" : \"NativeSwapalp_IMM\", \
    	\"matrix_order\" : \"rm\", \
    	\"is_hermitian\" : false, \
    	\"is_unitary\" : true, \
    	\"is_user_pulse\" : false, \
   		\"is_mutable\" : true, \
    	\"qubit_list\" : [0, 1], \
    	\"parametric_list\" : [2, 3], \
    	\"control_qubit_list\" : [], \
    	\"local_basis_list\" : [0, 0], \
    	\"identifier\" : " + std::to_string(kSpinSwapalp_IMM) +
                                                              " \
  	}",
                                                          "{ \
    	\"matrix_real\" : \"NativeMeasZ_IMM\", \
   		\"matrix_imag\" : \"NativeMeasZ_IMM\", \
    	\"matrix_order\" : \"rm\", \
    	\"is_hermitian\" : true, \
    	\"is_unitary\" : false, \
    	\"is_mutable\" : true, \
    	\"is_user_pulse\" : false, \
  		\"qubit_list\" : [0], \
    	\"parametric_list\" : [1], \
    	\"control_qubit_list\" : [], \
    	\"local_basis_list\" : [1], \
    	\"identifier\" : " + std::to_string(kSpinMeasZ_IMM) + " \
  	}",
                                                          "{ \
    	\"matrix_real\" : \"NativePrepZ_IMM\", \
   		\"matrix_imag\" : \"NativePrepZ_IMM\", \
    	\"matrix_order\" : \"rm\", \
    	\"is_hermitian\" : false, \
    	\"is_unitary\" : false, \
    	\"is_mutable\" : true, \
    	\"is_user_pulse\" : false, \
 		\"qubit_list\" : [0], \
    	\"parametric_list\" : [1], \
    	\"control_qubit_list\" : [], \
    	\"local_basis_list\" : [1], \
    	\"identifier\" : " + std::to_string(kSpinPrepZ_IMM) + " \
  	}",
                                                          "{ \
    	\"matrix_real\" : \"NativeRZ_IMM_IDX\", \
   		\"matrix_imag\" : \"NativeRZ_IMM_IDX\", \
    	\"matrix_order\" : \"rm\", \
    	\"is_hermitian\" : false, \
    	\"is_unitary\" : true, \
    	\"is_mutable\" : true, \
    	\"is_user_pulse\" : false, \
   		\"qubit_list\" : [0], \
    	\"parametric_list\" : [1, 2], \
    	\"control_qubit_list\" : [], \
    	\"local_basis_list\" : [1], \
    	\"identifier\" : " + std::to_string(kSpinRotZ_IMM_IDX) +
                                                              " \
  	}",
                                                          "{ \
    	\"matrix_real\" : \"NativeRXY_IMM_IDX\", \
   		\"matrix_imag\" : \"NativeRXY_IMM_IDX\", \
    	\"matrix_order\" : \"rm\", \
    	\"is_hermitian\" : false, \
    	\"is_unitary\" : true, \
    	\"is_mutable\" : true, \
    	\"is_user_pulse\" : false, \
   		\"qubit_list\" : [0], \
    	\"parametric_list\" : [1, 2, 3], \
    	\"control_qubit_list\" : [], \
    	\"local_basis_list\" : [0], \
    	\"identifier\" : " + std::to_string(kSpinRotXY_IMM_IDX) +
                                                              " \
  	}",
                                                          "{ \
   		\"matrix_real\" : \"NativeCPhase_IMM_IDX\", \
   	    \"matrix_imag\" : \"NativeCPhase_IMM_IDX\", \
   		\"matrix_order\" : \"rm\", \
   		\"is_hermitian\" : false, \
   		\"is_unitary\" : true, \
   		\"is_mutable\" : true, \
    	\"is_user_pulse\" : false, \
  		\"qubit_list\" : [0, 1], \
    	\"parametric_list\" : [2, 3], \
    	\"control_qubit_list\" : [0], \
    	\"local_basis_list\" : [1, 1], \
    	\"identifier\" : " + std::to_string(kSpinCPhase_IMM_IDX) +
                                                              " \
  	}",
                                                          "{ \
    	\"matrix_real\" : \"NativeSwapalp_IMM_IDX\", \
   		\"matrix_imag\" : \"NativeSwapalp_IMM_IDX\", \
    	\"matrix_order\" : \"rm\", \
    	\"is_hermitian\" : false, \
    	\"is_unitary\" : true, \
    	\"is_user_pulse\" : false, \
   		\"is_mutable\" : true, \
    	\"qubit_list\" : [0, 1], \
    	\"parametric_list\" : [2, 3], \
    	\"control_qubit_list\" : [], \
    	\"local_basis_list\" : [0, 0], \
    	\"identifier\" : " + std::to_string(kSpinSwapalp_IMM_IDX) +
                                                              " \
  	}",
                                                          "{ \
    	\"matrix_real\" : \"NativeMeasZ_IMM_IDX\", \
   		\"matrix_imag\" : \"NativeMeasZ_IMM_IDX\", \
    	\"matrix_order\" : \"rm\", \
    	\"is_hermitian\" : true, \
    	\"is_unitary\" : false, \
    	\"is_mutable\" : true, \
    	\"is_user_pulse\" : false, \
  		\"qubit_list\" : [0], \
    	\"parametric_list\" : [1, 2], \
    	\"control_qubit_list\" : [], \
    	\"local_basis_list\" : [1], \
    	\"identifier\" : " + std::to_string(kSpinMeasZ_IMM_IDX) +
                                                              " \
  	}"};

const char Prefix[] = "quantum";

///////////////////////////////////////////////////////////////////////////////

bool QuantumSpinNativeToJson::addSpinNativeFunctions(Module &M,
                                                     QuantumModule &QM) {

  // Get GateMetadata map
  std::map<std::string, json::Object> *qGateMetadata = &QM.q_gate_metadata;

  for (int id = kSpinBegin; id != kSpinIMM_IDXEnd; id++) {

    // Add the Intrinsic declaration to IR

    int IntrID = getInstrinsicIDFromGateIdentifier(id);

    Function *native_gate = Intrinsic::getDeclaration(&M, IntrID);
    StringRef annotation(SpinNativeGateAnnotations[id - kSpinBegin]);

    Expected<json::Value> E = json::parse(annotation);
    if (auto err = E.takeError()) {
      logAllUnhandledErrors(std::move(err), llvm::errs(), "[Parse Error] ");
      errs() << "ERROR: during the parsing of spin native metadata: " << id
             << ".\n";
    }

    if (json::Object *O = E->getAsObject()) {
      qGateMetadata->insert(
          std::make_pair(native_gate->getName(), std::move(*O)));
    } else {
      errs() << "ERROR: spin native metadata cannot be parsed to a JSON "
                "object.\n";
    }
  }

  return true;
}

////////////////////////////////////////////////////////////////////////////////

} // namespace llvm

////////////////////////////////////////////////////////////////////////////////

char QuantumSpinNativeToJsonLegacyPass::ID = 0;

ModulePass *llvm::createQuantumSpinNativeToJsonLegacyPass() {
  return new QuantumSpinNativeToJsonLegacyPass();
}

INITIALIZE_PASS_BEGIN(QuantumSpinNativeToJsonLegacyPass, "spin-native-annotate",
                      "Add Spin Native Annotations", false, true)
INITIALIZE_PASS_END(QuantumSpinNativeToJsonLegacyPass, "spin-native-annotate",
                    "Add Spin Native Annotations", false, true)

////////////////////////////////////////////////////////////////////////////////

char QuantumSpinNativeToJsonWrapperPass::ID = 0;

ModulePass *llvm::createQuantumSpinNativeToJsonWrapperPass() {
  return new QuantumSpinNativeToJsonWrapperPass();
}

INITIALIZE_PASS_BEGIN(QuantumSpinNativeToJsonWrapperPass,
                      "spin-native-annotate-wrap",
                      "Spin Native Annotations wrapper", true, true)
INITIALIZE_PASS_END(QuantumSpinNativeToJsonWrapperPass,
                    "spin-native-annotate-wrap",
                    "Spin Native Annotations wrapper", true, true)

////////////////////////////////////////////////////////////////////////////////

PreservedAnalyses QuantumSpinNativeToJsonPass::run(Module &M,
                                                   ModuleAnalysisManager &MAM) {
  QuantumModuleProxy &QMP = MAM.getResult<QuantumCompilerLinkageAnalysis>(M);
  QSNJ.addSpinNativeFunctions(M, *QMP.QM);
  updateCompilerLinkage(M, *QMP.QM);
  return PreservedAnalyses::all();
}

////////////////////////////////////////////////////////////////////////////////
