//===- IntializeQuantumPasses.h ---------------------------------*- C++ -*-===//
//
// Copyright (C) 2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//
//////////////////////////////////////////////////////////////////////

#ifndef LLVM_INITIALIZEQUANTUMPASSES_H
#define LLVM_INITIALIZEQUANTUMPASSES_H

namespace llvm {

class PassRegistry;

// TO BE REMOVED.
void initializeIntelQuantum(PassRegistry &);
void initializeGetAnnotationsPassPass(PassRegistry &);
void initializeFlattenQuantumKernelLegacyPassPass(PassRegistry &);
void initializeGetAnnotationsWrapperPassPass(PassRegistry &);
void initializeQuantumAnnotationsToJsonLegacyPassPass(PassRegistry &);
void initializeQuantumAnnotationsToJsonWrapperPassPass(PassRegistry &);
void initializeQuantumSpinNativeToJsonLegacyPassPass(PassRegistry &);
void initializeQuantumSpinNativeToJsonWrapperPassPass(PassRegistry &);
void initializeSchedulerLegacyPassPass(PassRegistry &);
void initializeQuantumSerializeKernelLegacyPassPass(PassRegistry &);
void initializeConvertQbitToQIDLegacyPassPass(PassRegistry &);
void initializeSpinNativeAnglesInRangeLegacyPassPass(PassRegistry &);
void initializeSeparateAndReplaceQBBLegacyPassPass(PassRegistry &);
void initializeQuantumClassicalModuleSplitLegacyPassPass(PassRegistry &);
void initializeFinalizeQuantumCompilationLegacyPassPass(PassRegistry &);
void initializeDecomposeToffoliLegacyPassPass(PassRegistry &);
void initializeQuantumKernelStatsPrintLegacyPassPass(PassRegistry &);
void initializePrintCircuitQBBLegacyPassPass(PassRegistry &);
void initializeSpinNativeConvertToIMMLegacyPassPass(PassRegistry &);
void initializeQuantumInitPlacementLegacyPassPass(PassRegistry &);
void initializeQuantumInitPlacementLegacyWrapperPassPass(PassRegistry &);
void initializeQuantumCompilerLinkageLegacyPassPass(PassRegistry &);
void initializeQuantumCompilerLinkageWrapperPassPass(PassRegistry &);
void initializeValidateAndConditionQBBLegacyPassPass(PassRegistry &);
void initializeInsertQuantumIntrinsicsLegacyPassPass(PassRegistry &);
void initializeInsertQuantumArgumentAttributesLegacyPassPass(PassRegistry &);
void initializeSimpleQbitPlacementLegacyPassPass(PassRegistry &);
void initializeQuantumLoopUnrollLegacyPassPass(PassRegistry &);
void initializeSpinNativeLowerFromCanonicalLegacyPassPass(PassRegistry &);

} // end namespace llvm

#endif // LLVM_INITIALIZEQUANTUMPASSES_H