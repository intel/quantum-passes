//===- QuantumAnnotationsToJson.cpp - Convert annotations to JSON ---------===//
//
// Copyright (C) 2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//
//===----------------------------------------------------------------------===//
//
// This file implements a module pass to collect global annotations from a
// module and returns a map of functions to annotations as JSON objects.
//
//===----------------------------------------------------------------------===//

#include "IntelQuantumPasses/QuantumAnalysis/QuantumAnnotationsToJson.h"
#include "IntelQuantumPasses/QuantumAnalysis/QuantumCompilerLinkage.h"
// Enumerate for gate identifiers
#include "IntelQuantumPasses/QuantumAnalysis/QuantumGateIdentifiers.h"
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
#include <cmath>   // std::sqrt, std::pow
#include <complex> // std::conj
#include <fstream>
#include <iostream>
#include <optional>
#include <set>
#include <sstream>
#include <string>

using namespace llvm;

#define DEBUG_TYPE "quantum-annotations-to-json"

namespace llvm {

char QuantumAnnotationsToJsonLegacyPass::ID = 0;

std::map<std::string, json::Object> *QuantumAnnotationsToJson::qGateMetadata =
    nullptr;
std::map<std::string, json::Object>
    *QuantumAnnotationsToJsonLegacyPass::qGateMetadata = nullptr;

int QuantumAnnotationsToJson::num_custom_gates = 0;

std::vector<std::string> QuantumAnnotationsToJson::PulseIntrinsicAnnotations = {
    "{ \
    		\"matrix_real\" : \"StartPulse\", \
   		  \"matrix_imag\" : \"StartPulse\", \
    		\"matrix_order\" : \"rm\", \
    		\"is_hermitian\" : false, \
    		\"is_unitary\" : false, \
      	\"is_user_pulse\" : false, \
   		  \"is_mutable\" : false, \
    		\"qubit_list\" : [1, 2, 3], \
    		\"parametric_list\" : [0], \
    		\"control_qubit_list\" : [], \
    		\"local_basis_list\" : [0, 0, 0], \
    		\"identifier\" : " +
        std::to_string(kStartPulse) + " \
  		}",
    "{ \
    		\"matrix_real\" : \"DirectPulse\", \
   		  \"matrix_imag\" : \"DirectPulse\", \
    		\"matrix_order\" : \"rm\", \
    		\"is_hermitian\" : false, \
    		\"is_unitary\" : false, \
      	\"is_user_pulse\" : false, \
   		  \"is_mutable\" : false, \
    		\"qubit_list\" : [], \
    		\"parametric_list\" : [0], \
    		\"control_qubit_list\" : [], \
    		\"local_basis_list\" : [], \
    		\"identifier\" : " +
        std::to_string(kDirectPulse) + " \
  		}",
    "{ \
    		\"matrix_real\" : \"BarrierPulse\", \
   		  \"matrix_imag\" : \"BarrierPulse\", \
    		\"matrix_order\" : \"rm\", \
    		\"is_hermitian\" : false, \
    		\"is_unitary\" : false, \
      	\"is_user_pulse\" : false, \
   		  \"is_mutable\" : false, \
    		\"qubit_list\" : [1, 2, 3], \
    		\"parametric_list\" : [0], \
    		\"control_qubit_list\" : [], \
    		\"local_basis_list\" : [0, 0, 0], \
    		\"identifier\" : " +
        std::to_string(kBarrierPulse) + " \
  		}"};

////////////////////////////////////////////////////////////////////////////////

QuantumAnnotationsToJsonLegacyPass::~QuantumAnnotationsToJsonLegacyPass() {}

/////////////////////////////////////////////////////////////////////////////

bool QuantumAnnotationsToJson::doInitialization(Module &M, QuantumModule &QM) {
  // get metadata map from QM
  qGateMetadata = &QM.q_gate_metadata;
  qGateMetadata->clear();

  // Fill qGateMetadata
  locateAndAddAnnotatedFunctions(&M);
  verifyAndCategorizeQuantumFunctions(M);

  return false;
}

bool QuantumAnnotationsToJsonLegacyPass::doInitialization(Module &M) {
  QAJ.doInitialization(M, QM);
  qGateMetadata = &QM.q_gate_metadata;

  return false;
}

/////////////////////////////////////////////////////////////////////////////

void QuantumAnnotationsToJson::locateAndAddAnnotatedFunctions(Module *M) {
  for (Module::global_iterator I = M->global_begin(), E = M->global_end();
       I != E; ++I) {
    if (I->getName() == "llvm.global.annotations") {
      ConstantArray *CA = dyn_cast<ConstantArray>(I->getOperand(0));
      for (auto OI = CA->op_begin(); OI != CA->op_end(); ++OI) {
        ConstantStruct *CS = dyn_cast<ConstantStruct>(OI->get());
        if (!CS)
          return;

        if (User *first_elem = dyn_cast<User>(CS->getOperand(0))) {
          // Check for undef and return
          if ((dyn_cast<UndefValue>(first_elem)))
            continue;
          // this first element should be a bitcast of a function pointer
          // get the function it points to
          Function *F = dyn_cast<Function>(first_elem);
          if (!F)
            return;
          GlobalVariable *AnnotationGL =
              dyn_cast<GlobalVariable>(CS->getOperand(1));
          if (!AnnotationGL)
            return;
          auto *CDA =
              dyn_cast<ConstantDataArray>(AnnotationGL->getInitializer());
          if (!CDA)
            return;
          StringRef Annotation = CDA->getAsCString();

          // Parse the actual annotations
          Expected<json::Value> E = json::parse(Annotation);
          if (auto err = E.takeError()) {
            logAllUnhandledErrors(std::move(err), llvm::errs(),
                                  "[Parse Error] ");
            errs() << "ERROR: during the parsing of quantum annotations.\n";
          }

          if (json::Object *O = E->getAsObject()) {
            if (std::optional<bool> wrapper_flag =
                    O->getBoolean("fleq_wrapper")) {
              if (wrapper_flag.value())
                continue;
            }

            LLVM_DEBUG(dbgs() << "------ gate: " << F->getName() << " ---\n");
            LLVM_DEBUG(dbgs() << "--- validate and fill-in fields\n");
            bool are_annotations_valid =
                this->validateFieldsOfAnnotationObject(O);
            // TODO: decide whether stop execution here or simply output an
            // error message.
            if (!are_annotations_valid)
              errs() << "ERROR: not all annotations of above gate have been "
                        "validated.\n";
            LLVM_DEBUG(dbgs() << "--- validated annotations\n");

            qGateMetadata->insert(std::make_pair(F->getName(), std::move(*O)));
          } else
            errs() << "ERROR: quantum annotations cannot be parsed to a JSON "
                      "object.\n";
        }
      }
    }
  }
}

////////////////////////////////////////////////////////////////////////////////

void QuantumAnnotationsToJson::printFieldsOfAnnotationObject(json::Object *O) {
  // TODO: if present, use method provided in json.h instead of coding a custom
  // one.

  if (json::Array *mat_real = O->getArray("matrix_real")) {
    errs() << "matrix_real as 1D array = [ ";
    for (auto iter = mat_real->begin(); iter != mat_real->end(); ++iter) {
      if (std::optional<double> elem = iter->getAsNumber())
        errs() << elem << " ";
      // The array element should be a number, otherwise print irrespective of
      // type.
      else
        errs() << *iter << " ";
    }
    errs() << "]\n";
  } else if (std::optional<StringRef> mat_real = O->getString("matrix_real"))
    errs() << "matrix_real as string = " << mat_real.value() << "\n";

  if (json::Array *mat_imag = O->getArray("matrix_imag")) {
    errs() << "matrix_imag as 1D array = [ ";
    for (auto iter = mat_imag->begin(); iter != mat_imag->end(); ++iter) {
      if (std::optional<double> elem = iter->getAsNumber())
        errs() << elem << " ";
      else
        errs() << *iter << " ";
    }
    errs() << "]\n";
  } else if (std::optional<StringRef> mat_imag = O->getString("matrix_imag"))
    errs() << "matrix_imag as string = " << mat_imag.value() << "\n";

  if (std::optional<StringRef> mat_order = O->getString("matrix_order"))
    errs() << "matrix_order = " << mat_order.value() << "\n";

  if (std::optional<bool> is_hermitian = O->getBoolean("is_hermitian"))
    errs() << "is_hermitian operator = "
           << (is_hermitian.value() ? "true" : "false") << "\n";

  if (std::optional<bool> is_unitary = O->getBoolean("is_unitary"))
    errs() << "is_unitary operator = "
           << (is_unitary.value() ? "true" : "false") << "\n";

  if (std::optional<bool> is_mutable = O->getBoolean("is_mutable"))
    errs() << "is_mutable operator = "
           << (is_mutable.value() ? "true" : "false") << "\n";

  if (json::Array *qubits = O->getArray("qubit_list")) {
    errs() << "qubit_list as 1D array = [ ";
    for (auto iter = qubits->begin(); iter != qubits->end(); ++iter) {
      if (std::optional<long int> elem = iter->getAsInteger())
        errs() << elem << " ";
    }
    errs() << "]\n";
  }

  if (json::Array *pars = O->getArray("parametric_list")) {
    errs() << "parametric_list as 1D array = [ ";
    for (auto iter = pars->begin(); iter != pars->end(); ++iter) {
      if (std::optional<double> elem = iter->getAsNumber())
        errs() << elem << "\t";
    }
    errs() << "]\n";
  }

  if (json::Array *ctrl_qubits = O->getArray("control_qubit_list")) {
    errs() << "control_qubit_list as 1D array = [ ";
    for (auto iter = ctrl_qubits->begin(); iter != ctrl_qubits->end(); ++iter) {
      if (std::optional<long int> elem = iter->getAsInteger())
        errs() << elem << " ";
    }
    errs() << "]\n";
  }

  if (json::Array *bases = O->getArray("local_basis_list")) {
    errs() << "local_basis_list as 1D array = [ ";
    for (auto iter = bases->begin(); iter != bases->end(); ++iter) {
      if (std::optional<long int> elem = iter->getAsInteger())
        errs() << elem << " ";
    }
    errs() << "]\n";
  }

  errs() << "\n";
  return;
}

////////////////////////////////////////////////////////////////////////////////

bool QuantumAnnotationsToJson::validateFieldsOfAnnotationObject(
    json::Object *O) {
  // ---- Identifier
  // It should be present only for canonical gates.
  if (std::optional<int64_t> identifier = O->getInteger("identifier")) {
    LLVM_DEBUG(dbgs() << "canonical gate with id= " << identifier.value()
                      << "\n");
    return minimumValidationForCanonicalGates(O);
  } else {
    QuantumAnnotationsToJson::num_custom_gates += 1;
    (*O)["identifier"] = -QuantumAnnotationsToJson::num_custom_gates;
    return completeValidationForCustomGates(O);
  }
}

////////////////////////////////////////////////////////////////////////////////

bool QuantumAnnotationsToJson::minimumValidationForCanonicalGates(
    json::Object *O) {
  bool is_matrix_numeric = true;
  bool are_all_annotations_valid(true);

  // ---- Real part of the matrix.
  // If matrix is in numerical form, store it as a vector.
  // otherwise confirm that matrix is a string.
  std::vector<double> matrix_real;
  if (json::Array *mat_real = O->getArray("matrix_real")) {
    for (auto iter = mat_real->begin(); iter != mat_real->end(); ++iter) {
      if (std::optional<double> elem = iter->getAsNumber())
        matrix_real.push_back(elem.value());
      // The array element should be a number, otherwise return false.
      else {
        errs() << "ERROR: Matrix_real is an array of non-numerical types.\n";
        are_all_annotations_valid = false;
      }
    }
  } else if (std::optional<StringRef> mat_real = O->getString("matrix_real")) {
    is_matrix_numeric = false;
  } else {
    errs() << "ERROR: Matrix_real is not a string nor in numerical form.\n";
    are_all_annotations_valid = false;
  }

  // ---- Imaginary part of the matrix.
  std::vector<double> matrix_imag;
  if (json::Array *mat_imag = O->getArray("matrix_imag")) {
    for (auto iter = mat_imag->begin(); iter != mat_imag->end(); ++iter) {
      if (std::optional<double> elem = iter->getAsNumber())
        matrix_imag.push_back(elem.value());
      else {
        errs() << "ERROR: Matrix_imag is an array of non-numerical types.\n";
        are_all_annotations_valid = false;
      }
    }
  } else if (std::optional<StringRef> mat_imag = O->getString("matrix_imag")) {
    is_matrix_numeric = false;
  } else {
    errs() << "ERROR: Matrix_imag is not a string nor in numerical form.\n";
    are_all_annotations_valid = false;
  }

  // ---- Look at qubit list to determine the number of qubits.
  int num_qubits = 0;
  if (json::Array *qubit_list = O->getArray("qubit_list")) {
    std::vector<long int> temp_list;
    for (auto iter = qubit_list->begin(); iter != qubit_list->end(); ++iter) {
      if (std::optional<long int> qubit = iter->getAsInteger()) {
        temp_list.push_back(qubit.value());
        num_qubits += 1;
      }
      // The array element should be a number, otherwise return false.
      else {
        errs() << "ERROR: qubit_list is an array of non-numerical types.\n";
        are_all_annotations_valid = false;
      }
    }
    // Verify that qubit indices are not repeated
    for (std::size_t i = 0; i < temp_list.size(); i++)
      for (std::size_t j = i + 1; j < temp_list.size(); j++)
        if (temp_list[i] == temp_list[j]) {
          errs() << "ERROR: qubit_list has repetitions.\n";
          are_all_annotations_valid = false;
          i = temp_list.size();
          break;
        }
  } else {
    errs() << "ERROR: qubit_list is not an array.\n";
    are_all_annotations_valid = false;
  }

  // ---- Verify the correct size of the matrix (both real and imaginary parts).
  if (is_matrix_numeric) {
    if (matrix_real.size() != matrix_imag.size()) {
      errs() << "ERROR: Real and imaginary parts of the matrix have different "
                "dimensions.\n";
      are_all_annotations_valid = false;
    }
    //
    if (matrix_real.size() == std::pow(4, num_qubits))
      LLVM_DEBUG(dbgs() << "This is a " << num_qubits << "-qubit gate.\n");
    else {
      errs() << "ERROR: Wrong size for a " << num_qubits << "-qubit matrix.\n";
      are_all_annotations_valid = false;
    }
  }

  if (!are_all_annotations_valid)
    return false;

  // ---- Update the flatten form of the matrix according to its order (row- or
  // column-major).
  if (std::optional<StringRef> mat_order = O->getString("matrix_order")) {
    StringRef order = mat_order.value();
    if (order != "rm") {
      errs() << "ERROR: Matrix order should be row-major ('rm').\n";
      are_all_annotations_valid = false;
    }
  }

  // ---- Skip evaluation whether the matrix is hearmitioan and/or unitary.
  // ---- Skip checks for the control_qubit_list.
  // ---- Skip checks for the local basis list.

  return are_all_annotations_valid;
}

////////////////////////////////////////////////////////////////////////////////

bool QuantumAnnotationsToJson::completeValidationForCustomGates(
    json::Object *O) {
  bool is_matrix_numeric = true;
  bool are_all_annotations_valid(true);

  // ---- Real part of the matrix.
  // If matrix is in numerical form, store it as a vector.
  // Otherwise, return error.
  std::vector<double> matrix_real;
  if (json::Array *mat_real = O->getArray("matrix_real")) {
    for (auto iter = mat_real->begin(); iter != mat_real->end(); ++iter) {
      if (std::optional<double> elem = iter->getAsNumber())
        matrix_real.push_back(elem.value());
      // The array element should be a number, otherwise return false.
      else {
        errs() << "ERROR: Matrix_real is an array of non-numerical types.\n";
        are_all_annotations_valid = false;
      }
    }
  } else if (std::optional<StringRef> mat_real = O->getString("matrix_real")) {
    is_matrix_numeric = false;
  } else {
    errs() << "ERROR: Matrix_real is not a string nor in numerical form.\n";
    are_all_annotations_valid = false;
  }

  // ---- Imaginary part of the matrix.
  std::vector<double> matrix_imag;
  if (json::Array *mat_imag = O->getArray("matrix_imag")) {
    for (auto iter = mat_imag->begin(); iter != mat_imag->end(); ++iter) {
      if (std::optional<double> elem = iter->getAsNumber())
        matrix_imag.push_back(elem.value());
      else {
        errs() << "ERROR: Matrix_imag is an array of non-numerical types.\n";
        are_all_annotations_valid = false;
      }
    }
  } else if (std::optional<StringRef> mat_imag = O->getString("matrix_imag")) {
    is_matrix_numeric = false;
  } else {
    errs() << "ERROR: Matrix_imag is not a string nor in numerical form.\n";
    are_all_annotations_valid = false;
  }

  // ---- Look at qubit list to determine the number of qubits.
  int num_qubits = 0;
  if (json::Array *qubit_list = O->getArray("qubit_list")) {
    std::vector<long int> temp_list;
    for (auto iter = qubit_list->begin(); iter != qubit_list->end(); ++iter) {
      if (std::optional<long int> qubit = iter->getAsInteger()) {
        temp_list.push_back(qubit.value());
        num_qubits += 1;
      }
      // The array element should be a number, otherwise return false.
      else {
        errs() << "ERROR: qubit_list is an array of non-numerical types.\n";
        are_all_annotations_valid = false;
      }
    }
    // Verify that qubit indices are not repeated
    for (std::size_t i = 0; i < temp_list.size(); i++)
      for (std::size_t j = i + 1; j < temp_list.size(); j++)
        if (temp_list[i] == temp_list[j]) {
          errs() << "ERROR: qubit_list has repetitions.\n";
          are_all_annotations_valid = false;
          i = temp_list.size();
          break;
        }
  } else {
    errs() << "ERROR: qubit_list is not an array.\n";
    are_all_annotations_valid = false;
  }

  // ---- Verify the correct size of the matrix (both real and imaginary parts).
  if (is_matrix_numeric) {
    if (matrix_real.size() != matrix_imag.size()) {
      errs() << "ERROR: Real and imaginary parts of the matrix have different "
                "dimensions.\n";
      are_all_annotations_valid = false;
    }
    //
    if (matrix_real.size() == std::pow(4, num_qubits))
      LLVM_DEBUG(dbgs() << "This is a " << num_qubits << "-qubit gate.\n");
    else {
      errs() << matrix_real.size() << "!= " << std::pow(4, num_qubits) << "\n";
      errs() << "ERROR: Wrong size for a " << num_qubits << "-qubit matrix.\n";
      are_all_annotations_valid = false;
    }
    int dim = std::sqrt(matrix_real.size());
    if (!are_all_annotations_valid)
      return false;

    // ---- Update the flatten form of the matrix according to its order (row-
    // or column-major).
    if (std::optional<StringRef> mat_order = O->getString("matrix_order")) {
      StringRef order = mat_order.value();
      if (order == "rm") {
      } else if (order == "cm") {
        std::vector<double> matrix_real_in_cm = matrix_real;
        std::vector<double> matrix_imag_in_cm = matrix_imag;
        for (int i = 0; i < dim; ++i)
          for (int j = 0; j < dim; ++j) {
            matrix_real[i * dim + j] = matrix_real_in_cm[j * dim + i];
            matrix_imag[i * dim + j] = matrix_imag_in_cm[j * dim + i];
          }
        (*O)["matrix_real"] = matrix_real;
        (*O)["matrix_imag"] = matrix_imag;
        // errs() << "TODO: update JSON object with reordered matrix
        // representation.\n";
        LLVM_DEBUG(
            dbgs()
            << "Matrix order transformed from column-major to row-major.\n");
        StringRef new_order = "rm";
        // json::ObjectKey K("name");
        (*O)["matrix_order"] = new_order;
      } else {
        errs()
            << "ERROR: Matrix order is neither row-major not column-major.\n";
        are_all_annotations_valid = false;
      }
    }

    // ---- Confirm whether the matrix is hermitian or not.
    bool is_matrix_hermitian(true);
    for (int i = 0; i < dim; ++i) {
      for (int j = 0; j < dim; ++j) {
        if (matrix_real[i * dim + j] != matrix_real[j * dim + i] ||
            matrix_imag[i * dim + j] != -matrix_imag[j * dim + i]) {
          is_matrix_hermitian = false;
          i = dim - 1;
          j = dim - 1;
        }
      }
    }
    if (std::optional<bool> is_her = O->getBoolean("is_hermitian")) {
      if (is_her.value() != is_matrix_hermitian) {
        LLVM_DEBUG(dbgs() << "Field is_hermitian was incorrectly filled.\n");
        (*O)["is_hermitian"] = is_matrix_hermitian;
      }
    } else
      (*O)["is_hermitian"] = is_matrix_hermitian;

    // ---- Confirm whether the matrix is unitary or not.
    bool is_matrix_unitary(true);
    double accepted_precision = 1e-10;
    std::vector<double> temp_matrix(dim * dim, 0);
    // Re[U].Im[U]^t - Im[U].Re[U]^t = 0
    for (int i = 0; i < dim; ++i) {
      for (int j = 0; j < dim; ++j) {
        for (int k = 0; k < dim; ++k) {
          temp_matrix[i * dim + j] +=
              matrix_real[i * dim + k] * matrix_imag[k * dim + j];
          temp_matrix[i * dim + j] -=
              matrix_imag[i * dim + k] * matrix_real[k * dim + j];
        }
        if (std::abs(temp_matrix[i * dim + j]) > accepted_precision) {
          is_matrix_unitary = false;
          i = dim - 1;
          j = dim - 1;
        }
      }
    }
    // Re[U].Re[U]^t + Im[U].Im[U]^t = identity
    temp_matrix.assign(dim * dim, 0);
    for (int i = 0; i < dim; ++i) {
      if (is_matrix_unitary == false)
        break;
      for (int j = 0; j < dim; ++j) {
        for (int k = 0; k < dim; ++k) {
          temp_matrix[i * dim + j] +=
              matrix_real[i * dim + k] * matrix_real[k * dim + j];
          temp_matrix[i * dim + j] +=
              matrix_imag[i * dim + k] * matrix_imag[k * dim + j];
        }
        if ((i == j &&
             std::abs(temp_matrix[i * dim + j] - 1) > accepted_precision) ||
            (i != j &&
             std::abs(temp_matrix[i * dim + j]) > accepted_precision)) {
          is_matrix_unitary = false;
          i = dim - 1;
          j = dim - 1;
        }
      }
    }
    if (std::optional<bool> is_her = O->getBoolean("is_unitary")) {
      if (is_her.value() != is_matrix_unitary) {
        LLVM_DEBUG(dbgs() << "Field is_unitary was incorrectly filled.\n");
        (*O)["is_unitary"] = is_matrix_unitary;
      }
    } else
      (*O)["is_unitary"] = is_matrix_unitary;

    // ---- Fill the list of control qubits.
    // For the moment, only for 2-qubit gates and conditional execution on state
    // |1>. it is assumed the state order: {|00>, |01>, |10>, |11>} , with |q0
    // q1>
    std::vector<int64_t> ctrl_qubits;
    if (dim == 4) {
      if (matrix_real[0] == 1 && matrix_real[1] == 0 && matrix_real[4] == 0 &&
          matrix_real[5] == 1 && matrix_real[2] == 0 && matrix_real[3] == 0 &&
          matrix_real[6] == 0 && matrix_real[7] == 0 && matrix_real[8] == 0 &&
          matrix_real[9] == 0 && matrix_real[12] == 0 && matrix_real[13] == 0 &&
          matrix_imag[0] == 0 && matrix_imag[1] == 0 && matrix_imag[4] == 0 &&
          matrix_imag[5] == 0 && matrix_imag[2] == 0 && matrix_imag[3] == 0 &&
          matrix_imag[6] == 0 && matrix_imag[7] == 0 && matrix_imag[8] == 0 &&
          matrix_imag[9] == 0 && matrix_imag[12] == 0 && matrix_imag[13] == 0) {
        ctrl_qubits.push_back(0);
      }
      //
      if (matrix_real[0] == matrix_real[10] &&
          matrix_real[1] == matrix_real[11] &&
          matrix_real[4] == matrix_real[14] &&
          matrix_real[5] == matrix_real[15] && matrix_real[2] == 0 &&
          matrix_real[3] == 0 && matrix_real[6] == 0 && matrix_real[7] == 0 &&
          matrix_real[8] == 0 && matrix_real[9] == 0 && matrix_real[12] == 0 &&
          matrix_real[13] == 0 && matrix_imag[0] == matrix_imag[10] &&
          matrix_imag[1] == matrix_imag[11] &&
          matrix_imag[4] == matrix_imag[14] &&
          matrix_imag[5] == matrix_imag[15] && matrix_imag[2] == 0 &&
          matrix_imag[3] == 0 && matrix_imag[6] == 0 && matrix_imag[7] == 0 &&
          matrix_imag[8] == 0 && matrix_imag[9] == 0 && matrix_imag[12] == 0 &&
          matrix_imag[13] == 0) {
        ctrl_qubits.push_back(1);
      }
    }
    // Compare with the list provided in the header.
    if (json::Array *control_qubit_list = O->getArray("control_qubit_list")) {
      for (auto iter = control_qubit_list->begin();
           iter != control_qubit_list->end(); ++iter) {
        long int elem = iter->getAsInteger().value();
        bool is_present(false);
        for (unsigned k = 0; k < ctrl_qubits.size(); ++k) {
          if (elem == ctrl_qubits[k]) {
            is_present = true;
            break;
          }
        }
        if (is_present == false) {
          errs() << "ERROR: Field control_qubit_list was incorrectly filled.\n";
          are_all_annotations_valid = false;
        }
      }
    }
    //
    (*O)["control_qubit_list"] = ctrl_qubits;

    // ---- Fill the local basis list.
    int num_bases = 0;
    if (json::Array *basis_list = O->getArray("local_basis_list")) {
      for (auto iter = basis_list->begin(); iter != basis_list->end(); ++iter) {
        if (std::optional<long int> basis = iter->getAsInteger()) {
          // Check that range is correct: {0,1,2,3,4}
          if (basis.value() < 0 && basis.value() > 5) {
            errs() << "ERROR: Field local_basis_list was incorrectly filled.\n";
            are_all_annotations_valid = false;
          }
          // Check that the matrix actually commute with
          // TODO FIXME: use function like CheckLocalBasis(matrix_real,
          // matrix_imag, num_qubits, basis.value(), qubit);
          num_bases += 1;
        }
        // The array element should be a number, otherwise return false.
        else {
          errs() << "ERROR: local_basis_list is an array of non-numerical "
                    "types.\n";
          are_all_annotations_valid = false;
        }
      }
      if (num_bases > num_qubits) {
        errs() << "ERROR: local_basis_list has the wrong size.\n";
        are_all_annotations_valid = false;
      }
    } else {
      // TODO FIXME: fill the field!
      num_bases = num_qubits;
      std::vector<int> bases(num_bases, 0);
      (*O)["local_basis_list"] = bases;
    }
  }
  // ---- Check if the user-defined gate corresponds to a canonical one.
  long int identifier =
      DoesMatrixCorrespondToCanonicalGate(matrix_real, matrix_imag);
  if (identifier >= 0) {
    (*O)["identifier"] = identifier;
    errs() << "Custom gate corresponds to canonical one, with identifier = "
           << identifier << "\n";
  }

  return are_all_annotations_valid;
}
////////////////////////////////////////////////////////////////////////////////

bool QuantumAnnotationsToJson::is_qgate(CallInst *CI) {
  auto *F = CI->getCalledFunction();
  if (F) {
    std::string Name = F->getName().str();
    return (Name.find("QubitGate") < Name.length());
  }
  return false;
}

////////////////////////////////////////////////////////////////////////////////

// FIXME: This seems a dummy method since all the job is done in
// doInitialization()
//        Is this method required at all?
bool QuantumAnnotationsToJsonLegacyPass::runOnModule(Module &M) {
  // Empty do all work being done in doInitialization().
  return false;
}

bool QuantumAnnotationsToJson::verifyAndCategorizeQuantumFunctions(Module &M) {

  for (Function &F : M) {

    // TODO: verify every argument is consumed by an intrinsic

    bool valid = false;
    bool q_intr = false;
    bool is_user_pulse = false;
    bool start_found = false, pulse_found = false, barrier_found = true;
    unsigned qbit_args = 0, para_args = 0;

    if (qGateMetadata->find(F.getName().str()) != qGateMetadata->end()) {

      for (auto iter = inst_begin(F); iter != inst_end(F); ++iter) {
        CallInst *CI = dyn_cast<CallInst>(&*iter);
        if (CI) {
          auto *CF = CI->getCalledFunction();
          if (CF) {
            StringRef Name = CF->getName();
            if (Name == "llvm.quantum.qubit") {
              qbit_args++;
              q_intr = true;
            } else if (Name == "llvm.quantum.param") {
              para_args++;
              q_intr = true;
            } else if (Name == "llvm.quantum.cbit") {
              para_args++;
              q_intr = true;
            } else if (Name == "_Z15__quantum_qubitPt") {
              qbit_args++;
              q_intr = true;
            } else if (Name == "_Z15__quantum_paramd") {
              para_args++;
              q_intr = true;
            } else if (Name == "_Z14__quantum_cbitPb") {
              para_args++;
              q_intr = true;
            } else if (Name == "llvm.quantum.pulse.sequence.start") {
              start_found = true;
              barrier_found = false;
              valid = false;
            } else if (Name == "llvm.quantum.direct.pulse") {
              pulse_found = true;
              is_user_pulse = true;
              if (!start_found || barrier_found)
                valid = false;
            } else if (Name == "llvm.quantum.pulse.sequence.barrier") {
              valid = pulse_found && start_found;
              barrier_found = true;
              start_found = false;
              pulse_found = false;
              has_user_pulse_ = true;
            }
          } // valid CF
        }   // valid CI
      }     // instr iterator

      // check the number of qubit and parameteric operands

      json::Object *O = &(qGateMetadata->at(F.getName().str()));

      if (json::Array *arr = O->getArray("qubit_list")) {
        valid = (arr->size() == qbit_args);
      }
      if (json::Array *arr = O->getArray("parametric_list")) {
        valid = (arr->size() == para_args);
      }

      if (!valid) {

        if (start_found) {
          std::string ErrMsg =
              "found annotated function " + F.getName().str() +
              " with a direct pulse start instruction without any " +
              "direct pulses or barrier.\n";
          displayErrorAndExit("AnnotationsToJson", ErrMsg);
        }
        if (!barrier_found) {
          std::string ErrMsg =
              "found annotated function " + F.getName().str() +
              " with a direct pulse start instruction without any " +
              "direct pulses or start.\n";
          displayErrorAndExit("AnnotationsToJson", ErrMsg);
        }

        if (q_intr) {
          std::string ErrMsg =
              "found annotated function " + F.getName().str() +
              " with insufficient number of quantum intrinsics function " +
              "calls: " + std::to_string(qbit_args) + ", " +
              std::to_string(para_args) + "\n";
          displayErrorAndExit("AnnotationsToJson", ErrMsg);
        }

        else {
          errs() << "AnnotationsToJson Warning: found annotated function "
                 << F.getName()
                 << "  with no quantum intrinsics calls: it has been removed "
                    "from list of available gates...\n";
          qGateMetadata->erase(F.getName().str());
        }

      } else {
        // json::Object *O = &(qGateMetadata->at(F.getName()));
        (*O)["is_user_pulse"] = is_user_pulse;
      }
    }
  };
  return false;
}

} // namespace llvm

////////////////////////////////////////////////////////////////////////////////

ModulePass *llvm::createQuantumAnnotationsToJsonLegacyPass() {
  return new QuantumAnnotationsToJsonLegacyPass();
}

INITIALIZE_PASS_BEGIN(QuantumAnnotationsToJsonLegacyPass, "q-annotate-json",
                      "Get LLVM QIR Annotations in JSON format.", false, true)
INITIALIZE_PASS_END(QuantumAnnotationsToJsonLegacyPass, "q-annotate-json",
                    "Get LLVM QIR Annotations in JSON format", false, true)

////////////////////////////////////////////////////////////////////////////////

char QuantumAnnotationsToJsonWrapperPass::ID = 0;

ModulePass *llvm::createQuantumAnnotationsToJsonWrapperPass() {
  return new QuantumAnnotationsToJsonWrapperPass();
}

bool QuantumAnnotationsToJsonWrapperPass::runOnModule(Module &M) {
  QAJ.runOnModule(M);
  return false;
}

INITIALIZE_PASS_BEGIN(QuantumAnnotationsToJsonWrapperPass,
                      "q-annotate-json-wrap", "Annotations to JSON wrapper",
                      true, true)
INITIALIZE_PASS_END(QuantumAnnotationsToJsonWrapperPass, "q-annotate-json-wrap",
                    "Annotations to JSON wrapper", true, true)

PreservedAnalyses QuantumAnnotationsToJsonPass::run(Module &M,
                                                    ModuleAnalysisManager &AM) {
  QuantumModuleProxy &QMP = AM.getResult<QuantumCompilerLinkageAnalysis>(M);
  QAJ.doInitialization(M, *QMP.QM);
  updateCompilerLinkage(M, *QMP.QM);
  return PreservedAnalyses::all();
}

#undef DEBUG_TYPE
