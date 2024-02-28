//===- InsertQuantumIntrinsics--.cpp -------------------------------------====//
//
// Copyright (C) 2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//
//===----------------------------------------------------------------------===//
//
// This file implements a module pass which inlines quantum kernel calls which
// are inside other quantum kernels.
//
//===----------------------------------------------------------------------===//

#include "IntelQuantumPasses/QuantumCompile/InsertQuantumIntrinsics.h"
#include "IntelQuantumPasses/InitializeQuantumPasses.h"
#include "IntelQuantumPasses/QuantumUtils/QuantumGeneralUtils.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/DenseSet.h"
#include "llvm/Analysis/CallGraph.h"
#include "llvm/Demangle/Demangle.h"
#include "llvm/IR/Attributes.h"
#include "llvm/IR/Intrinsics.h"
#include "llvm/IR/IntrinsicsFLEQ.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/Allocator.h"

#define DEBUG_TYPE "insert-quantum-intrinsics"

using namespace llvm;
using namespace aqcc;

static Function *getIntrinsicFunctionFromMangledName(Module &M, StringRef &SR) {
  Intrinsic::ID IntrID;
  std::string DemangledString = demangle(SR.str());
  if (DemangledString == "__quantum_qubit(unsigned short*)")
    IntrID = Function::lookupIntrinsicID("llvm.quantum.qubit");
  else if (DemangledString == "__quantum_cbit(bool*)")
    IntrID = Function::lookupIntrinsicID("llvm.quantum.cbit");
  else if (DemangledString == "__quantum_param(double)")
    IntrID = Function::lookupIntrinsicID("llvm.quantum.param");
  else if (DemangledString == "release_quantum_state()")
    IntrID = Function::lookupIntrinsicID("llvm.quantum.release.quantum.state");

  // FLEQ
  // basic builtins
  else if (DemangledString == "quantum_builtin::__qexpr_this_expression()")
    IntrID = Function::lookupIntrinsicID("llvm.fleq.qexpr.this.expression");
  else if (DemangledString == "quantum_builtin::__qexpr_identity(double)")
    IntrID = Function::lookupIntrinsicID("llvm.fleq.qexpr.identity");
  else if (DemangledString == "quantum_builtin::__qexpr_eval_hold(int)")
    IntrID = Function::lookupIntrinsicID("llvm.fleq.qexpr.eval.hold");
  else if (DemangledString == "quantum_builtin::__qexpr_eval_release(int)")
    IntrID = Function::lookupIntrinsicID("llvm.fleq.qexpr.eval.release");
  // gate builtins
  else if (DemangledString == "quantum_builtin::__qexpr_H(unsigned short*)")
    IntrID = Function::lookupIntrinsicID("llvm.fleq.gate.qexpr.h");
  else if (DemangledString == "quantum_builtin::__qexpr_X(unsigned short*)")
    IntrID = Function::lookupIntrinsicID("llvm.fleq.gate.qexpr.x");
  else if (DemangledString == "quantum_builtin::__qexpr_Y(unsigned short*)")
    IntrID = Function::lookupIntrinsicID("llvm.fleq.gate.qexpr.y");
  else if (DemangledString == "quantum_builtin::__qexpr_Z(unsigned short*)")
    IntrID = Function::lookupIntrinsicID("llvm.fleq.gate.qexpr.z");
  else if (DemangledString == "quantum_builtin::__qexpr_S(unsigned short*)")
    IntrID = Function::lookupIntrinsicID("llvm.fleq.gate.qexpr.s");
  else if (DemangledString == "quantum_builtin::__qexpr_Sdag(unsigned short*)")
    IntrID = Function::lookupIntrinsicID("llvm.fleq.gate.qexpr.s.dag");
  else if (DemangledString == "quantum_builtin::__qexpr_T(unsigned short*)")
    IntrID = Function::lookupIntrinsicID("llvm.fleq.gate.qexpr.t");
  else if (DemangledString == "quantum_builtin::__qexpr_Tdag(unsigned short*)")
    IntrID = Function::lookupIntrinsicID("llvm.fleq.gate.qexpr.t.dag");
  else if (DemangledString ==
           "quantum_builtin::__qexpr_RX(unsigned short*, double)")
    IntrID = Function::lookupIntrinsicID("llvm.fleq.gate.qexpr.rx");
  else if (DemangledString ==
           "quantum_builtin::__qexpr_RY(unsigned short*, double)")
    IntrID = Function::lookupIntrinsicID("llvm.fleq.gate.qexpr.ry");
  else if (DemangledString ==
           "quantum_builtin::__qexpr_RZ(unsigned short*, double)")
    IntrID = Function::lookupIntrinsicID("llvm.fleq.gate.qexpr.rz");
  else if (DemangledString ==
           "quantum_builtin::__qexpr_CZ(unsigned short*, unsigned short*)")
    IntrID = Function::lookupIntrinsicID("llvm.fleq.gate.qexpr.cz");
  else if (DemangledString ==
           "quantum_builtin::__qexpr_CNOT(unsigned short*, unsigned short*)")
    IntrID = Function::lookupIntrinsicID("llvm.fleq.gate.qexpr.cnot");
  else if (DemangledString ==
           "quantum_builtin::__qexpr_SWAP(unsigned short*, unsigned short*)")
    IntrID = Function::lookupIntrinsicID("llvm.fleq.gate.qexpr.swap");
  else if (DemangledString == "quantum_builtin::__qexpr_Toffoli(unsigned "
                              "short*, unsigned short*, unsigned short*)")
    IntrID = Function::lookupIntrinsicID("llvm.fleq.gate.qexpr.tof");
  else if (DemangledString == "quantum_builtin::__qexpr_PrepX(unsigned short*)")
    IntrID = Function::lookupIntrinsicID("llvm.fleq.gate.qexpr.prepx");
  else if (DemangledString == "quantum_builtin::__qexpr_PrepY(unsigned short*)")
    IntrID = Function::lookupIntrinsicID("llvm.fleq.gate.qexpr.prepy");
  else if (DemangledString == "quantum_builtin::__qexpr_PrepZ(unsigned short*)")
    IntrID = Function::lookupIntrinsicID("llvm.fleq.gate.qexpr.prepz");
  else if (DemangledString ==
           "quantum_builtin::__qexpr_MeasX(unsigned short*, bool*)")
    IntrID = Function::lookupIntrinsicID("llvm.fleq.gate.qexpr.measx");
  else if (DemangledString ==
           "quantum_builtin::__qexpr_MeasY(unsigned short*, bool*)")
    IntrID = Function::lookupIntrinsicID("llvm.fleq.gate.qexpr.measy");
  else if (DemangledString ==
           "quantum_builtin::__qexpr_MeasZ(unsigned short*, bool*)")
    IntrID = Function::lookupIntrinsicID("llvm.fleq.gate.qexpr.measz");
  else if (DemangledString == "quantum_builtin::__qexpr_CPhase(unsigned "
                              "short*, unsigned short*, double)")
    IntrID = Function::lookupIntrinsicID("llvm.fleq.gate.qexpr.cphase");
  else if (DemangledString ==
           "quantum_builtin::__qexpr_RXY(unsigned short*, double, double)")
    IntrID = Function::lookupIntrinsicID("llvm.fleq.gate.qexpr.rxy");
  else if (DemangledString == "quantum_builtin::__qexpr_SwapA(unsigned short*, "
                              "unsigned short*, double)")
    IntrID = Function::lookupIntrinsicID("llvm.fleq.gate.qexpr.swapa");
  // unitary transforms builtins
  else if (DemangledString == "quantum_builtin::__qexpr_join(int, int)")
    IntrID = Function::lookupIntrinsicID("llvm.fleq.qexpr.join");
  else if (DemangledString == "quantum_builtin::__qexpr_bind(int, int)")
    IntrID = Function::lookupIntrinsicID("llvm.fleq.qexpr.bind");
  else if (DemangledString == "quantum_builtin::__qexpr_invert(int)")
    IntrID = Function::lookupIntrinsicID("llvm.fleq.qexpr.invert");
  else if (DemangledString ==
           "quantum_builtin::__qexpr_power(unsigned int, int)")
    IntrID = Function::lookupIntrinsicID("llvm.fleq.qexpr.power");
  else if (DemangledString ==
           "quantum_builtin::__qexpr_control(unsigned short*, bool, int)")
    IntrID = Function::lookupIntrinsicID("llvm.fleq.qexpr.control");
  else if (DemangledString == "quantum_builtin::__qexpr_multi_control(unsigned "
                              "short*, unsigned int, int)")
    IntrID = Function::lookupIntrinsicID("llvm.fleq.qexpr.multi.control");
  // control flow builtins
  else if (DemangledString == "quantum_builtin::__qexpr_branch(bool, int, int)")
    IntrID = Function::lookupIntrinsicID("llvm.fleq.qexpr.branch");
  else if (DemangledString == "quantum_builtin::__qexpr_let(char const*, int)")
    IntrID = Function::lookupIntrinsicID("llvm.fleq.qexpr.let");
  else if (DemangledString == "quantum_builtin::__qexpr_get(char const*)")
    IntrID = Function::lookupIntrinsicID("llvm.fleq.qexpr.get");
  // qlist builtins
  else if (DemangledString == "quantum_builtin::__qlist_size(unsigned short*)")
    IntrID = Function::lookupIntrinsicID("llvm.fleq.qlist.size");
  else if (DemangledString == "quantum_builtin::__qlist_slice(unsigned short*, "
                              "unsigned long, unsigned long)")
    IntrID = Function::lookupIntrinsicID("llvm.fleq.qlist.slice");
  else if (DemangledString ==
           "quantum_builtin::__qlist_join(unsigned short*, unsigned short*)")
    IntrID = Function::lookupIntrinsicID("llvm.fleq.qlist.join");
  else if (DemangledString ==
           "quantum_builtin::__qlist_at(unsigned short*, unsigned long)")
    IntrID = Function::lookupIntrinsicID("llvm.fleq.qlist.at");
  else if (DemangledString ==
           "quantum_builtin::__qlist_intergerize(unsigned short*)")
    IntrID = Function::lookupIntrinsicID("llvm.fleq.qlist.integerize");
  else if (DemangledString ==
           "quantum_builtin::__qlist_booleanize(unsigned short*)")
    IntrID = Function::lookupIntrinsicID("llvm.fleq.qlist.booleanize");
  else if (DemangledString == "quantum_builtin::__qlist_empty()")
    IntrID = Function::lookupIntrinsicID("llvm.fleq.qlist.empty");
  else if (DemangledString ==
           "quantum_builtin::__qlist_equal(unsigned short*, unsigned short*)")
    IntrID = Function::lookupIntrinsicID("llvm.fleq.qlist.equal");
  else if (DemangledString == "quantum_builtin::__qlist_build(unsigned short*)")
    IntrID = Function::lookupIntrinsicID("llvm.fleq.qlist.build");
  // datalist builtins
  else if (DemangledString == "quantum_builtin::__datalist_build(char const*)")
    IntrID = Function::lookupIntrinsicID("llvm.fleq.datalist.build");
  else if (DemangledString == "quantum_builtin::__datalist_from(int)")
    IntrID = Intrinsic::fleq_datalist_from;
  else if (DemangledString == "quantum_builtin::__datalist_empty()")
    IntrID = Function::lookupIntrinsicID("llvm.fleq.datalist.empty");
  else if (DemangledString ==
           "quantum_builtin::__datalist_integerize(char const*)")
    IntrID = Function::lookupIntrinsicID("llvm.fleq.datalist.integerize");
  else if (DemangledString ==
           "quantum_builtin::__datalist_floatize(char const*)")
    IntrID = Function::lookupIntrinsicID("llvm.fleq.datalist.floatize");
  else if (DemangledString ==
           "quantum_builtin::__datalist_booleanize(char const*)")
    IntrID = Function::lookupIntrinsicID("llvm.fleq.datalist.booleanize");
  else if (DemangledString ==
           "quantum_builtin::__datalist_at(char const*, unsigned long)")
    IntrID = Function::lookupIntrinsicID("llvm.fleq.datalist.at");
  else if (DemangledString == "quantum_builtin::__datalist_size(char const*)")
    IntrID = Function::lookupIntrinsicID("llvm.fleq.datalist.size");
  else if (DemangledString == "quantum_builtin::__datalist_slice(char const*, "
                              "unsigned long, unsigned long)")
    IntrID = Function::lookupIntrinsicID("llvm.fleq.datalist.slice");
  else if (DemangledString ==
           "quantum_builtin::__datalist_join(char const*, char const*)")
    IntrID = Function::lookupIntrinsicID("llvm.fleq.datalist.join");
  else if (DemangledString == "quantum_builtin::__datalist_find(char const*, "
                              "char const*, unsigned int)")
    IntrID = Function::lookupIntrinsicID("llvm.fleq.datalist.find");
  else if (DemangledString ==
           "quantum_builtin::__datalist_longer(char const*, char const*)")
    IntrID = Function::lookupIntrinsicID("llvm.fleq.datalist.longer");
  else if (DemangledString ==
           "quantum_builtin::__datalist_shorter(char const*, char const*)")
    IntrID = Function::lookupIntrinsicID("llvm.fleq.datalist.shorter");
  else if (DemangledString ==
           "quantum_builtin::__datalist_contains(char const*, char const*)")
    IntrID = Function::lookupIntrinsicID("llvm.fleq.datalist.contains");
  else if (DemangledString ==
           "quantum_builtin::__datalist_count(char const*, char const*)")
    IntrID = Function::lookupIntrinsicID("llvm.fleq.datalist.count");
  // QExpr debug/build control
  else if (DemangledString == "quantum_builtin::__qexpr_print_passthru(int)")
    IntrID = Function::lookupIntrinsicID("llvm.fleq.qexpr.print.passthru");
  else if (DemangledString ==
           "quantum_builtin::__datalist_print_passthru(char const*, int)")
    IntrID = Function::lookupIntrinsicID("llvm.fleq.datalist.print.passthru");
  else if (DemangledString == "quantum_builtin::__qexpr_ct_exit(char const*)")
    IntrID = Function::lookupIntrinsicID("llvm.fleq.qexpr.ct.exit");
  else if (DemangledString == "quantum_builtin::__qexpr_rt_exit(char const*)")
    IntrID = Function::lookupIntrinsicID("llvm.fleq.qexpr.rt.exit");
  else
    return nullptr;

  Function *intr = Intrinsic::getDeclaration(&M, IntrID);
  return intr;
}

static bool InsertQuantumIntrinsics(Module &M) {
  std::vector<CallInst *> BranchToReplace;

  for (Function &F : M) {
    if (F.getSection() == QBBS_TEXT_SECTION &&
        F.getName().find(".stub") == StringRef::npos) {
      std::string newName = F.getName().str() + ".stub";
      F.setName(newName);
    }

    for (BasicBlock &BB : F)
      for (Instruction &I : BB) {
        CallInst *CI = dyn_cast<CallInst>(&I);
        if (!CI)
          continue;

        if (!CI->isIndirectCall()) {
          Function *CF = CI->getCalledFunction();
          StringRef CalledName = CF->getName();
          Function *IF = getIntrinsicFunctionFromMangledName(M, CalledName);
          if (IF) {
            CI->setCalledFunction(IF);
            CI->setTailCall(false);
          }
        }
      }
  }

  return false;
}

struct InsertQuantumIntrinsicsLegacyPass : public ModulePass {

  static char ID; // Pass identification
  InsertQuantumIntrinsicsLegacyPass() : ModulePass(ID) {
    initializeInsertQuantumIntrinsicsLegacyPassPass(
        *PassRegistry::getPassRegistry());
  }

  bool runOnModule(Module &M) override;

  bool run(Module &M, CallGraph &CG);

  virtual void getAnalysisUsage(AnalysisUsage &AU) const {}

}; // End of struct InsertQuantumIntrinsicsPass

char InsertQuantumIntrinsicsLegacyPass::ID = 0;
static RegisterPass<InsertQuantumIntrinsicsLegacyPass>
    X("insert-q-intrinsics", "InsertQuantumIntrinsicsLegacyPass", false, false);

bool InsertQuantumIntrinsicsLegacyPass::runOnModule(Module &M) {
  // We will use the call graph scc ordering to order the inlining
  return InsertQuantumIntrinsics(M);
}

INITIALIZE_PASS_BEGIN(InsertQuantumIntrinsicsLegacyPass, "insert-q-intrinsics",
                      "InsertQuantumIntrinsicsLegacyPass", false, false)
INITIALIZE_PASS_END(InsertQuantumIntrinsicsLegacyPass, "insert-q-intrinsics",
                    "InsertQuantumIntrinsicsLegacyPass", false, false)

PreservedAnalyses InsertQuantumIntrinsicsPass::run(Module &M,
                                                   ModuleAnalysisManager &AM) {
  InsertQuantumIntrinsics(M);

  return PreservedAnalyses::none();
}

#undef DEBUG_TYPE
