//===- QuantumPassPipeline.h ------------------------------------*- C++ -*-===//
//
//===----------------------------------------------------------------------===//
//
// Copyright (C) 2024 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//
//===----------------------------------------------------------------------===//
/// \file "QuantumPassPipeline.h"
///
/// Header file to define functions to help build quantum compilation pipelines.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_QUANTUM_PASS_PIPELINE_H
#define LLVM_QUANTUM_PASS_PIPELINE_H

#include "llvm/IR/Module.h"
#include "llvm/IR/PassManager.h"
#include "llvm/Passes/PassBuilder.h"

namespace llvm {
namespace aqcc {

/// Add the necessary module passes to run before running the unrolling passes.
/// These impact the entirety of the module and cannot be split between
/// different
// functions.
void addPreQuantumLoopUnrollingModulePasses(PassBuilder &PB,
                                            ModulePassManager &MPM);

/// Add the necessary module passes to run after running the unrolling
/// processes. These impact the entirety of the module and cannot be split
/// between different
// functions.
void addPostQuantumLoopUnrollingModulePasses(PassBuilder &PB,
                                             ModulePassManager &MPM);

/// Add the necessary function passes to run to effectively simplify the quantum
/// basic blocks of a kernel into straight line functions.
void addQuantumLoopUnrollingFunctionPasses(PassBuilder &PB,
                                           FunctionPassManager &FPM,
                                           PipelineTuningOptions &PTO,
                                           std::optional<PGOOptions> &PGOOpt);

/// Take the set of passes to run before loop unrolling, the set of passes to
/// run after and the loop unrolling passes themselves. The PreMPM is run first,
/// then the FPM is run on each quantum kernel if it is not valid. Then the
/// PostMPM is run again. This process is continued as long as there is an
/// invalid quantum kernel.
void runFunctionPassesQuantumKernels(ModulePassManager &PreMPM,
                                     ModulePassManager &PostMPM,
                                     FunctionPassManager &FPM, Module &M,
                                     ModuleAnalysisManager &MAM,
                                     FunctionAnalysisManager &FAM,
                                     unsigned AttemptLimit = 10);

/// Add the passes required to set up the quantum module for use to the pass
/// manager, and later compilation.
void addQuantumModulePasses(ModulePassManager &MPM);

/// Add the passes required to print the module to the console or file.
void addPrinterPass(ModulePassManager &MPM, std::string Stage = "",
                    bool PrintCircuit = false);

/// Add the passes required to print the statistics of the quantum circuit.
void addVerbosePasses(ModulePassManager &MPM, bool PrintVerbose = false,
                      bool AddPlacement = true);

/// Add the passes required to translate the declarations to intrinsics and add
/// function attributes to inform qubit argument information.
void addQuantumAttributePasses(ModulePassManager &MPM);

/// Add the passes to inline quantum kernels into the highest level kernel.
void addFlattenQuantumKernelPasses(ModulePassManager &MPM,
                                   bool PrintVerbose = false,
                                   bool PrintCircuit = false);

/// Add the passes to validate and normalize the quantum kernels.
void addValidatePasses(ModulePassManager &MPM, bool PrintVerbose = false,
                       bool PrintCircuit = false);

/// Add the passes to combine and resynthesize the circuits, and lower to
/// native operations.
void addSynthesisPasses(ModulePassManager &MPM, bool PrintVerbose = false,
                        bool PrintCircuit = false);

/// Add the passes to lower the canonical gates to native gates.
void addLoweringPasses(ModulePassManager &MPM, std::string DotType,
                       bool CanonicalScheduling = false, bool SkipFLEQ = false,
                       bool PrintVerbose = false, bool PrintCircuit = false);

/// Add the passes required to map and schedule the gates to a give architecture
/// configuration.
void addSchedulingPasses(ModulePassManager &MPM, std::string DotType,
                         bool UsePulse = true, bool Serialize = false,
                         bool PrintVerbose = false, bool PrintCircuit = false);

/// Add the passes to extract the quantum basic blocks from their classical
/// counterparts.
void addSeparationPasses(ModulePassManager &MPM, bool PrintVerbose = false,
                         bool PrintCircuit = false);

/// Add the passes to move the quantum basic block functions to their own
/// module.
void addSplitPasses(ModulePassManager &MPM);

/// Add the passes to tidy up the adjusted code.
void addCleanupPasses(ModulePassManager &MPM);

} // end namespace aqcc
} // end namespace llvm

#endif
