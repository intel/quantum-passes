//===- QuantumAnnotationsToJson.h -----------------------------------------===//
//
// Copyright (C) 2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//
//===----------------------------------------------------------------------===//
/// \file "QuantumAnnotationsToJson.h"
///
/// This file provides the module pass which retrieves the quantum metadata
/// as a a map between the StringRef gate name and the JSON-parsed annotation
/// for that gate.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_QUANTUM_ANNOTATIONSTOJSON_H
#define LLVM_QUANTUM_ANNOTATIONSTOJSON_H

#include "IntelQuantumPasses/InitializeQuantumPasses.h"
#include "IntelQuantumPasses/QuantumAnalysis/QuantumCompilerLinkage.h"
#include "IntelQuantumPasses/QuantumAnalysis/QuantumModule.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/PassManager.h"
#include "llvm/IRReader/IRReader.h"
#include "llvm/InitializePasses.h"
#include "llvm/Pass.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/JSON.h" // JSON package
#include "llvm/Support/SourceMgr.h"
#include "llvm/Support/raw_ostream.h"

#include <map>

namespace llvm {

////////////////////////////////////////////////////////////////////////////////

// TODO: Parser of matrix from string to numerical matrix.
// TODO: How to handle a numerical matrix that is not real? json::Object has no
// complex type.
//       Currently we saparate the real and imaginary part.
//       Another is taking advantage of the advanced string parser above.
// TODO: For validation, what annotation fields are missing?
// TODO: For validation, how to handle parametric matrices?

class QuantumAnnotationsToJson {

public:
  static std::map<StringRef, json::Object> *qGateMetadata;
  static int num_custom_gates;
  static std::vector<std::string> PulseIntrinsicAnnotations;
  bool has_user_pulse_ = false;

  /// Clear, fill and validate qGateMetadata.
  bool doInitialization(Module &M, QuantumModule &QM);

  /// Find, parse, verify and print the quintrinsics annotations.
  ///
  /// * Look at the "llvm.global.annotations" inside the module.
  /// * Parse annotations using LLVM::json parser.
  /// * Validate annotations checking for consistency and filling missing
  /// fields.
  /// * Save validated annotations in qGateMetadata as a map<StringRef,
  /// json::Object>.
  /// * Print gate name and annotations using errs(), for debug only.
  void locateAndAddAnnotatedFunctions(Module *M);

  /// Given a CallInst.
  bool is_qgate(CallInst *CI);

  /// Get qGateMatadata
  std::map<StringRef, json::Object> *getQGateMetadata(Module *M) {
    return qGateMetadata;
  }

  /// Get the flag has_user_pulse
  bool hasUserPulse() { return has_user_pulse_; }

  /// Get the QGateMedata objecct.
  std::map<StringRef, json::Object> *getQGateMetadata() {
    return qGateMetadata;
  }

  /// Utility function to print the fields of the JSON object associated to the
  /// quantum annotations.
  void printFieldsOfAnnotationObject(json::Object *O);

private:
  /// Utility function to validate the consistency of fields.
  ///
  /// It also fills in missing fields using the numerical form of "Matrix".
  /// Returns 'false' if matrix is not convertible into numerical form or if
  /// inconsistences are detected.
  ///
  /// Expected behavior at validation:
  /// * matrix_real:   Check if it is a 1D array of doubles. If not, return
  /// false.
  ///                  TODO: decide what to do if matrix is not numeric.
  /// * matrix_imag:   Check if it is a 1D array of doubles. If not, return
  /// false.
  ///                  TODO: decide what to do if matrix is not numeric.
  /// * qubit_list:    Check that it is a 1D array of consecutive integers like
  /// [0,1,2].
  /// *                Use qubit_list to deduce the number of qubits and verify
  /// the matrix' size.
  /// * matrix_order:  Either 'rm' (row-major) or 'cm' (column-major).
  ///                  In the latter case transpose the matrix and set it to
  ///                  'rm'.
  /// * is_hermitian:  Check if matrix_real is symmetric and matrix_imag is
  /// antisymmetric.
  ///                  Comparison via equality, no numerical tolerance allowed.
  /// * is_unitary:    Check if U.U^/dag = id by comparing:
  ///                    Re[U].Im[U]^t - Im[U].Re[U]^t = 0
  ///                    Re[U].Re[U]^t + Im[U].Im[U]^t = identity
  ///                  Comparison allow for numerical precision: epsilon=1e-10
  /// * control_qubit_list:  For 1- and 2-qubit gates only. No 1-qubit gate is
  /// controlled.
  ///                        A 2-qubit gate can be controlled on both qubits
  ///                        (think of CZ). If present in the header, verify
  ///                        that control_qubit_list is accurate.
  /// * local_basis_list:    TODO: decide how to compute the local basis from
  /// the numeric matrix.
  ///                              For example:
  ///                              1. check if U is diagonal (Z basis)
  ///                              2. check if H.U.H is diagonal (X basis)
  ///                              3. check if Yb.U.Yb is diagonal (Y basis)
  ///                              3. check if U.H = H.U (H basis)
  ///                        TODO: for multi-qubit gates, is it helpful to have
  ///                        a small library
  ///                              to handle tensor-product of matrices and
  ///                              compute matrix multiplications?
  /// * name:           TODO: determine behavior
  bool validateFieldsOfAnnotationObject(json::Object *O);

  bool minimumValidationForCanonicalGates(json::Object *O);
  bool completeValidationForCustomGates(json::Object *O);

  // This function is run after the QMetadata is populated
  // It verifies that each function listed in the metadata contains a quantum
  // intrinsic or else it is removed. It also adds and populates the
  // "is_user_pulse" flag. If a user pulse is found, the flag, has_user_pulse is
  // set to true.
  bool verifyAndCategorizeQuantumFunctions(Module &M);
};

class QuantumAnnotationsToJsonLegacyPass : public ModulePass {

public:
  static char ID;
  // TODO: implement this as an analysis pass so qGateMetadata
  // no longer has to be static.
  static std::map<StringRef, json::Object> *qGateMetadata;
  QuantumModule &QM = QuantumCompilerLinkageLegacyPass::QM;
  QuantumAnnotationsToJson QAJ;

  /// Minimal constructor.
  QuantumAnnotationsToJsonLegacyPass() : ModulePass(ID) {
    initializeQuantumAnnotationsToJsonLegacyPassPass(
        *PassRegistry::getPassRegistry());
  }
  /// Destructor.
  ~QuantumAnnotationsToJsonLegacyPass();

  void getAnalysisUsage(AnalysisUsage &AU) const override {

    AU.setPreservesAll();
    AU.addRequired<QuantumCompilerLinkageWrapperPass>();

  }

  /// Clear, fill and validate qGateMetadata.
  virtual bool doInitialization(Module &M) override;

  /// Get qGateMatadata
  std::map<StringRef, json::Object> *getQGateMetadata(Module *M) {
    return QAJ.getQGateMetadata(M);
  }

  /// Get the flag has_user_pulse
  bool hasUserPulse() { return QAJ.hasUserPulse(); }

  /// TODO:Why pass the Module?
  std::map<StringRef, json::Object> *getQGateMetadata() {
    return QAJ.getQGateMetadata();
  }

  // TODO: Add description.
  bool runOnModule(Module &M);
};

/// Return pointer to new QuantumAnnotationsToJsonPass.
ModulePass *createQuantumAnnotationsToJsonLegacyPass();

////////////////////////////////////////////////////////////////////////////////

class QuantumAnnotationsToJsonWrapperPass : public ModulePass {
  QuantumAnnotationsToJsonLegacyPass QAJ;

public:
  static char ID; // Pass identification, replacement for typeid

  QuantumAnnotationsToJsonWrapperPass() : ModulePass(ID) {
    initializeQuantumAnnotationsToJsonWrapperPassPass(
        *PassRegistry::getPassRegistry());
  }

  QuantumAnnotationsToJsonLegacyPass &quantumAnnotationsToJson() { return QAJ; }

  const QuantumAnnotationsToJsonLegacyPass &quantumAnnotationsToJson() const {
    return QAJ;
  }

  std::map<StringRef, json::Object> *getQGateMetadata(Module *M) {
    return QAJ.getQGateMetadata(M);
  }

  std::map<StringRef, json::Object> *getQGateMetadata() {
    return QAJ.getQGateMetadata();
  }

  bool hasUserPulse() { return QAJ.hasUserPulse(); }

  virtual bool doInitialization(Module &M) override {
    return QAJ.doInitialization(M);
  };

  /// Calculate the natural loop information for a given function.
  bool runOnModule(Module &M) override;
  void locateAndAddAnnotatedFunctions(Module *M);
};

ModulePass *createQuantumAnnotationsToJsonWrapperPass();

class QuantumAnnotationsToJsonPass
    : public PassInfoMixin<QuantumAnnotationsToJsonPass> {
public:
  QuantumAnnotationsToJson QAJ;
  PreservedAnalyses run(Module &M, ModuleAnalysisManager &MAM);
};

} // namespace llvm

#endif // for header guard LLVM_QUANTUM_ANNOTATIONSTOJSON_H
