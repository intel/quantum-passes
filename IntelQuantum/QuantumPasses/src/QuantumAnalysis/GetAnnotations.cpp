//===- GetAnnotations.cpp - Gathers global annotations from module --------===//
//
// Copyright (C) 2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//
//===----------------------------------------------------------------------===//
//
// This file implements a module pass to collect global annotations from a
// module and returns a map of functions to annotations.
//
//===----------------------------------------------------------------------===//

#include "IntelQuantumPasses/QuantumAnalysis/GetAnnotations.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/InstIterator.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/PassManager.h"
#include "llvm/IRReader/IRReader.h"
#include "llvm/Pass.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/SourceMgr.h"
#include "llvm/Support/raw_ostream.h"
#include <fstream>
#include <ios>
#include <iostream>
#include <map>
#include <optional>
#include <set>
#include <sstream>
#include <string>

using namespace llvm;

namespace llvm {

char GetAnnotationsPass::ID = 0;

// std::map<StringRef, StringRef> *GetAnnotationsPass::qGateMetadata =
//     new std::map<StringRef, StringRef>();
std::map<StringRef, StringRef> &
GetAnnotationsPass::getInstance_qGateMetadata() {
  static std::map<StringRef, StringRef> qGateMetadata =
      std::map<StringRef, StringRef>();
  return qGateMetadata;
}

void GetAnnotationsPass::locateAnnotatedFunctions(Module *M) {
  for (Module::global_iterator I = M->global_begin(), E = M->global_end();
       I != E; ++I) {

    if (I->getName() == "llvm.global.annotations") {
      ConstantArray *CA = dyn_cast<ConstantArray>(I->getOperand(0));
      for (auto OI = CA->op_begin(); OI != CA->op_end(); ++OI) {
        ConstantStruct *CS = dyn_cast<ConstantStruct>(OI->get());
        if (!CS)
          return;
        Function *F = dyn_cast<Function>(CS->getOperand(0)->getOperand(0));
        if (!F)
          return;
        GlobalVariable *AnnotationGL =
            dyn_cast<GlobalVariable>(CS->getOperand(1)->getOperand(0));
        if (!AnnotationGL)
          return;
        auto *CDA = dyn_cast<ConstantDataArray>(AnnotationGL->getInitializer());
        if (!CDA)
          return;
        StringRef Annotation = CDA->getAsCString();
        errs() << "Found annotation: " << Annotation << "\n";
        errs() << "Found annotated function " << F->getName() << "\n";
        getInstance_qGateMetadata().insert(
            std::make_pair(F->getName(), Annotation));
      }
    }
  }
}

std::map<StringRef, StringRef> *
GetAnnotationsPass::getQGateMetadata(Module *M) {
  // // std::map<StringRef, StringRef> *qGateMetadata =
  // //     new std::map<StringRef, StringRef>();

  for (Module::global_iterator I = M->global_begin(), E = M->global_end();
       I != E; ++I) {

    if (I->getName() == "llvm.global.annotations") {
      ConstantArray *CA = dyn_cast<ConstantArray>(I->getOperand(0));
      for (auto *OI = CA->op_begin(); OI != CA->op_end(); ++OI) {
        ConstantStruct *CS = dyn_cast<ConstantStruct>(OI->get());
        if (CS) {
          Function *F = dyn_cast<Function>(CS->getOperand(0)->getOperand(0));
          if (F) {
            GlobalVariable *AnnotationGL =
                dyn_cast<GlobalVariable>(CS->getOperand(1)->getOperand(0));
            if (AnnotationGL) {
              auto *CDA =
                  dyn_cast<ConstantDataArray>(AnnotationGL->getInitializer());
              if (CDA) {
                StringRef Annotation = CDA->getAsCString();
                getInstance_qGateMetadata().insert(
                    std::make_pair(F->getName(), Annotation));
              } // valid CDA
            }   // valid AnnotationGL
          }     // valid F
        }       // valid CS
      }         // operator iterator
    }           // llvm.global.annotations
  }             // global iterator
  return &getInstance_qGateMetadata();
}

bool GetAnnotationsPass::is_qgate(CallInst *CI) {
  auto *F = CI->getCalledFunction();
  if (F) {
    std::string Name = F->getName().str();
    return (Name.find("QubitGate") < Name.length());
  }
  return false;
}

bool GetAnnotationsPass::runOnModule(Module &M) { return false; }

} // namespace llvm

INITIALIZE_PASS_BEGIN(GetAnnotationsPass, "annotations",
                      "Get LLVM QIR Annotations", false, true)
INITIALIZE_PASS_END(GetAnnotationsPass, "annotations",
                    "Get LLVM QIR Annotations", false, true)

ModulePass *llvm::createGetAnnotationsPass() {
  return new GetAnnotationsPass();
}
ModulePass *llvm::createGetAnnotationsWrapperPass() {
  return new GetAnnotationsWrapperPass();
}

char GetAnnotationsWrapperPass::ID = 0;
INITIALIZE_PASS_BEGIN(GetAnnotationsWrapperPass, "annotations-wrap",
                      "Annotations wrapper", true, true)
INITIALIZE_PASS_END(GetAnnotationsWrapperPass, "annotations-wrap",
                    "Annotations wrapper", true, true)

bool GetAnnotationsWrapperPass::runOnModule(Module &M) {
  GA.runOnModule(M);
  return false;
}
