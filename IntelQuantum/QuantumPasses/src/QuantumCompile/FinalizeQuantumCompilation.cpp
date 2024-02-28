//===- FinalizeQuantumCompilation.cpp ------------------------------------====//
//
// Copyright (C) 2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//
//===----------------------------------------------------------------------===//
//
// This file implements a module pass which adds the initialize QRT call to
// pass:
//    -QSDA
//    -JSON file name (through command line)
//
//  as well as the dismantle call. These are added to the global Ctor and Dtor.
//
// This should be the last pass called before sending IR into the rest of Clang
//
//===----------------------------------------------------------------------===//

#include "IntelQuantumPasses/QuantumCompile/FinalizeQuantumCompilation.h"
#include "IntelQuantumPasses/InitializeQuantumPasses.h"
#include "IntelQuantumPasses/QuantumAnalysis/QuantumAnnotationGetters.h"
#include "IntelQuantumPasses/QuantumAnalysis/QuantumCompilerLinkage.h"
#include "IntelQuantumPasses/QuantumAnalysis/QuantumGateIdentifiers.h"
#include "IntelQuantumPasses/QuantumAnalysis/QuantumIterator.h"
#include "IntelQuantumPasses/QuantumAnalysis/QuantumModule.h"
#include "IntelQuantumPasses/QuantumAnalysis/QuantumRefs.h"
#include "IntelQuantumPasses/QuantumAnalysis/QuantumSpinNativeToJson.h"
#include "IntelQuantumPasses/QuantumUtils/QuantumGeneralUtils.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/GlobalVariable.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Intrinsics.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/Operator.h"
#include "llvm/Pass.h"
#include "llvm/Support/Casting.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Transforms/Utils/ModuleUtils.h"

using namespace llvm;
using namespace aqcc;

namespace {

const std::string qsda_final_name = "aqcc.qsda.final";

static Value *getGEP(Value *array) {
  if (!array)
    displayErrorAndExit("Finalize Quantum Compilation Pass",
                        "An array wasn't found.");
  LLVMContext &C = array->getContext();

  PointerType *PtrType = dyn_cast<PointerType>(array->getType());
  if (PtrType == nullptr) {
    displayErrorAndExit("Finalize Quantum Compilation Pass",
                        "Unable to retrieve array type.");
  }

  GlobalValue *GV = dyn_cast<GlobalValue>(array);
  if (!GV)
    displayErrorAndExit("Finalize Quantum Compilation Pass",
                        "Unable to retrieve array type.");
  Type *arryty = GV->getValueType();

  SmallVector<Constant *, 2> IdxList = {
      ConstantInt::get(IntegerType::get(C, 64), 0),
      ConstantInt::get(IntegerType::get(C, 64), 0)};
  auto *gep = ConstantExpr::getInBoundsGetElementPtr(
      arryty, dyn_cast<Constant>(array), IdxList);
  return dyn_cast<Value>(gep);
}

static void cleanUpUnusedQSDA(Module &M, QuantumModule &QM) {

  Value *qsda_final = createNewQSDArray(M, qsda_final_name, QM.max_QSDA_used);
  if (!qsda_final)
    displayErrorAndExit("Finalize Quantum Compilation Pass",
                        "Unable to create new quantum-shared 'double' array.");

  // get initializer with reserve values
  std::vector<double> init(QM.max_QSDA_used, 0.);
  // init[0] = 0.;
  init[1] = FP_PIby2;
  init[2] = FP_PI;
  init[3] = FP_3PIby2;
  Constant *LLVM_init = ConstantDataArray::get(M.getContext(), init);
  auto *QSDAFinal = dyn_cast<GlobalVariable>(qsda_final);
  if (!QSDAFinal)
    displayErrorAndExit("Finalize Quantum Compilation Pass",
                        "Unable to make quantum-shared array global.");
  QSDAFinal->setInitializer(LLVM_init);

  auto *QSDA_PtrType = dyn_cast<PointerType>(QM.QSDA->getType());
  GlobalValue *GV = dyn_cast<GlobalValue>(QM.QSDA);
  if (!QSDA_PtrType)
    displayErrorAndExit("Finalize Quantum Compilation Pass",
                        "QM.QSDA is not a PointerType.");
  if (!GV)
    displayErrorAndExit("Finalize Quantum Compilation Pass",
                        "QM.QSDA is not a GlobalValue.");
  // POINTER FIX
  Type *arryty_old = GV->getValueType();
  auto *QSDAFinal_PtrType = dyn_cast<PointerType>(qsda_final->getType());
  if (!QSDAFinal_PtrType)
    displayErrorAndExit("Finalize Quantum Compilation Pass",
                        "New and final QSDA is not a PointerType.");
  // POINTER FIX
  Type *arryty_new = QSDAFinal->getValueType();

  LLVMContext &C = M.getContext();

  for (int i = 0; i < QM.max_QSDA_used; i++) {

    // get gep for old and new qsda
    SmallVector<Constant *, 2> IdxList = {
        ConstantInt::get(IntegerType::get(C, 64), 0),
        ConstantInt::get(IntegerType::get(C, 64), i)};
    auto *gep_old = dyn_cast<Value>(ConstantExpr::getInBoundsGetElementPtr(
        arryty_old, dyn_cast<Constant>(QM.QSDA), IdxList));
    if (!gep_old)
      displayErrorAndExit("Finalize Quantum Compilation Pass",
                          "Value type expected.");
    auto *gep_new = dyn_cast<Value>(ConstantExpr::getInBoundsGetElementPtr(
        arryty_new, dyn_cast<Constant>(qsda_final), IdxList));
    if (!gep_new)
      displayErrorAndExit("Finalize Quantum Compilation Pass",
                          "Value type expected.");

    gep_old->replaceAllUsesWith(gep_new);
  };

  // remove old qsda
  auto qsda_gv = dyn_cast<GlobalVariable>(QM.QSDA);
  if (!qsda_gv)
    displayErrorAndExit(
        "Finalize Quantum Compilation Pass",
        "The quantum_shared 'double' array must be global variable.");
  qsda_gv->dropAllReferences();
  qsda_gv->removeFromParent();

  QM.QSDA = qsda_final;
}

static void addGlobalQRTInit(Module &M, QuantumModule &QM,
                             std::vector<Value *> args) {

  // add QRT initialization to global Ctor
  auto *FuncTy =
      FunctionType::get(Type::getVoidTy(M.getContext()), /*isVarArg*/ false);
  auto *Func = Function::Create(FuncTy, GlobalValue::InternalLinkage,
                                ".aqcc.qrt.initialize", &M);
  Func->setSection(".text.startup");

  FunctionCallee InitCallee =
      FunctionCallee(QM.QRT_initialize->getFunctionType(), QM.QRT_initialize);
  // Construct function body
  IRBuilder<> Builder(BasicBlock::Create(M.getContext(), "entry", Func));
  Builder.CreateCall(InitCallee, args, "QRTcall");
  Builder.CreateRetVoid();

  // Add this function to constructors.
  appendToGlobalCtors(M, Func, 0);
}

static void addGlobalQRTDismantle(Module &M, QuantumModule &QM) {

  // add QRT initialization to global Ctor
  auto *FuncTy =
      FunctionType::get(Type::getVoidTy(M.getContext()), /*isVarArg*/ false);
  auto *Func = Function::Create(FuncTy, GlobalValue::InternalLinkage,
                                ".aqcc.qrt.dismantle", &M);
  Func->setSection(".text.startup");

  FunctionCallee InitCallee =
      FunctionCallee(QM.QRT_dismantle->getFunctionType(), QM.QRT_dismantle);
  // Construct function body
  IRBuilder<> Builder(BasicBlock::Create(M.getContext(), "entry", Func));
  Builder.CreateCall(InitCallee, {}, "QRTcall");
  Builder.CreateRetVoid();

  // Add this function to constructors.
  appendToGlobalDtors(M, Func, 0);
}

static bool finalizeQuantumCompilation(Module &M, QuantumModule &QM) {

  if (!QM.QSDA)
    displayErrorAndExit("Finalize Quantum Compilation Pass",
                        "QSDA was not found.");

  // add config as a constant char array
  Constant *LLVM_array =
      ConstantDataArray::getString(M.getContext(), QM.sdk_json_filename, true);
  Type *ArTy = LLVM_array->getType();
  GlobalVariable *config_gv = new GlobalVariable(
      M, ArTy, true, GlobalValue::LinkageTypes::ExternalLinkage, LLVM_array,
      "config");
  if (!config_gv)
    displayErrorAndExit(
        "Finalize Quantum Compilation Pass",
        "Unable to create configuration-related global variable.");
  Value *config_val = dyn_cast<Value>(config_gv);
  if (!config_val)
    displayErrorAndExit("Finalize Quantum Compilation Pass",
                        "JSON config was not found.");

  // add path to sdk location is a constant char array
  Constant *SDK_array =
      ConstantDataArray::getString(M.getContext(), QM.sdk_path, true);
  Type *SDKArTy = SDK_array->getType();
  GlobalVariable *sdk_gv = new GlobalVariable(
      M, SDKArTy, true, GlobalValue::LinkageTypes::ExternalLinkage, SDK_array,
      "config");
  if (!sdk_gv)
    displayErrorAndExit(
        "Finalize Quantum Compilation Pass",
        "Unable to create configuration-related global variable.");
  Value *sdk_val = dyn_cast<Value>(sdk_gv);
  if (!sdk_val)
    displayErrorAndExit("Finalize Quantum Compilation Pass",
                        "JSON config was not found.");

  // remove unused QSDA entries
  cleanUpUnusedQSDA(M, QM);

  // add QRT initialization to global Ctor
  std::vector<Value *> args = {getGEP(QM.QSDA), getGEP(config_val),
                               getGEP(sdk_val)};
  addGlobalQRTInit(M, QM, args);

  // add QRT dismantle to global Dtor
  addGlobalQRTDismantle(M, QM);

  QM.setFinalized();

  return false;

} // runOnModule

struct FinalizeQuantumCompilationLegacyPass : public ModulePass {

public:
  static char ID; // Pass identification, replacement for typeid

  QuantumModule &QM = QuantumCompilerLinkageLegacyPass::QM;

  FinalizeQuantumCompilationLegacyPass() : ModulePass(ID) {
    initializeFinalizeQuantumCompilationLegacyPassPass(
        *PassRegistry::getPassRegistry());
  }

  void getAnalysisUsage(AnalysisUsage &AU) const override {

    AU.setPreservesAll();
    // AU.addRequired<QuantumCompilerLinkageWrapperPass>();
  }

  bool runOnModule(Module &M) override {
    finalizeQuantumCompilation(M, QM);

    return false;
  }

}; // end of FinalizeQuantumCompilationLegacyPass

} // end of anonymous namespace

char FinalizeQuantumCompilationLegacyPass::ID = 0;

INITIALIZE_PASS_BEGIN(FinalizeQuantumCompilationLegacyPass,
                      "finalize-quantum-compilation",
                      "Finalize quantum compilation legacy pass", false, false)
INITIALIZE_PASS_END(FinalizeQuantumCompilationLegacyPass,
                    "finalize-quantum-compilation",
                    "Finalize quantum compilation legacy pass", false, false)

PreservedAnalyses
FinalizeQuantumCompilationPass::run(Module &M, ModuleAnalysisManager &MAM) {
  QuantumModuleProxy QMP = MAM.getResult<QuantumCompilerLinkageAnalysis>(M);
  finalizeQuantumCompilation(M, *QMP.QM);

  return PreservedAnalyses::all();
}
