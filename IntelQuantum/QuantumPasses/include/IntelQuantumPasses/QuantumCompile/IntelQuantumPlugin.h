//===- IntelQuantumPlugin.h --------------------------------------------====//
//
// Copyright (C) 2024 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//
//===----------------------------------------------------------------------===//
//
// This file is the header file for the pass plugin and pass registration.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_INTEL_QUANTUM_PLUGIN_H
#define LLVM_INTEL_QUANTUM_PLUGIN_H

#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/PrettyStackTrace.h"
#include "llvm/Support/Signals.h"

void registerIntelQuantumPassesCallBacks(llvm::PassBuilder &PB);

#endif
