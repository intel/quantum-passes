//===- SpinNativeLowerFromCanonicalPass.cpp -------------------------------===//
//
// Copyright (C) 2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//
//===----------------------------------------------------------------------===//
//
// This file implements a function  pass which replaces canonical gates with
// there spin native equivalent (non-IMM). Note this is a one to one mapping. No
// optimizations are performed on the resulting circuit.
//
//===----------------------------------------------------------------------===//

#include "IntelQuantumPasses/QuantumCompile/SpinNativeLowerFromCanonical.h"
#include "IntelQuantumPasses/InitializeQuantumPasses.h"
#include "IntelQuantumPasses/QuantumAnalysis/QuantumAnnotationGetters.h"
#include "IntelQuantumPasses/QuantumAnalysis/QuantumCompilerLinkage.h"
#include "IntelQuantumPasses/QuantumAnalysis/QuantumGateIdentifiers.h"
#include "IntelQuantumPasses/QuantumAnalysis/QuantumIterator.h"
#include "IntelQuantumPasses/QuantumAnalysis/QuantumModule.h"
#include "IntelQuantumPasses/QuantumAnalysis/QuantumRefs.h"
#include "IntelQuantumPasses/QuantumAnalysis/QuantumSpinNativeToJson.h"
#include "IntelQuantumPasses/QuantumUtils/QuantumGeneralUtils.h"

#include "llvm/ADT/StringRef.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Intrinsics.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/Value.h"
#include "llvm/Pass.h"
#include "llvm/Support/Casting.h"
#include "llvm/Support/raw_ostream.h"
#include <string>

using namespace llvm;
using namespace aqcc;

#define DEBUG_TYPE "spin_lower_from_canonical"

void SpinNativeLowerFromCanonical::addDecomposedRz(
    double angle, std::vector<aqcc::ParaRef> &NParas, Type *TY, bool &Flag,
    aqcc::QBBIter &Qit, std::vector<aqcc::QbitRef> &Qubits,
    aqcc::ParaRef MidSlice) {
  NParas[2] = MidSlice;
  NParas[0].setValueResolved(FP_PI, TY);
  NParas[1].setValueResolved(angle * 0.5, TY);
  Flag = Flag && Qit.insertGate(kSpinRotXY, Qubits, NParas);
  NParas[0].setValueResolved(FP_PI, TY);
  NParas[1].setValueResolved(0.0, TY);
  Flag = Flag && Qit.insertGate(kSpinRotXY, Qubits, NParas);
}

bool SpinNativeLowerFromCanonical::lowerFromCanonical(BasicBlock &BB,
                                                      QuantumModule &QM) {

  Type *TY = Type::getDoubleTy(BB.getContext());
  bool Flag = true;
  std::string rz = "RZ";

  // In this first lowering, we put all gates as "middle" of the slice
  aqcc::ParaRef MidSlice(SLICE_MID, Type::getInt32Ty(BB.getContext()));

  // replace plan is to insert after the given gate, then go back to
  // said gate and delete. This should insure any attached instructions
  // remain in the proper order. It also means we are best off back-iterating
  aqcc::QBBIter Qit(BB);
  Qit.gotoEnd();

  // exit if BB is empty
  if (Qit.isBegin())
    return false;

  do {
    --Qit;
    int id = Qit.getIdentifier();
    std::vector<aqcc::QbitRef> Qubits = Qit.getQubitOperands();
    std::vector<aqcc::ParaRef> Paras = Qit.getParametricOperands();
    ++Qit;

    switch (id) {

    case kIDLE: {
      // TODO:Is there a QISA for Idle?
      break;
    }

    case kH: {
      std::vector<aqcc::ParaRef> NParas(3);
      NParas[2] = MidSlice;

      // X Pi pulse
      NParas[0].setValueResolved(FP_PI, TY);
      NParas[1].setValueResolved(0.0, TY);
      Flag = Flag && Qit.insertGate(kSpinRotXY, Qubits, NParas);

      // Y Pi/2 pulse
      NParas[0].setValueResolved(FP_PIby2, TY);
      NParas[1].setValueResolved(FP_PIby2, TY);
      Flag = Flag && Qit.insertGate(kSpinRotXY, Qubits, NParas);

      // Remove canonical H Gate
      --Qit;
      if (Qit.getIdentifier() == kH && Flag)
        Qit.removeGate();
      else {
        displayErrorAndExit("SpinNativeLowerFromCanonicalPass",
                            "lost the canonical H gate.");
      }

      break;
    }

    case kX: {
      std::vector<aqcc::ParaRef> NParas(3);
      NParas[2] = MidSlice;

      // X Pi pulse
      NParas[0].setValueResolved(FP_PI, TY);
      NParas[1].setValueResolved(0.0, TY);
      Flag = Flag && Qit.insertGate(kSpinRotXY, Qubits, NParas);

      // Remove canonical X Gate
      --Qit;
      if (Qit.getIdentifier() == kX && Flag)
        Qit.removeGate();
      else {
        displayErrorAndExit("SpinNativeLowerFromCanonicalPass",
                            "lost the canonical X gate.");
      }

      break;
    }

    case kY: {
      std::vector<aqcc::ParaRef> NParas(3);
      NParas[2] = MidSlice;

      // Y Pi pulse
      NParas[0].setValueResolved(FP_PI, TY);
      NParas[1].setValueResolved(FP_PIby2, TY);
      Flag = Flag && Qit.insertGate(kSpinRotXY, Qubits, NParas);

      // Remove canonical Y Gate
      --Qit;
      if (Qit.getIdentifier() == kY && Flag)
        Qit.removeGate();
      else {
        displayErrorAndExit("SpinNativeLowerFromCanonicalPass",
                            "lost the canonical Y gate.");
      }

      break;
    }

    case kZ: {
      if (QM.platform->platformConfigLoaded() &&
          !QM.isGateInPlatformConfig(rz)) {
        std::vector<aqcc::ParaRef> NParas(3);
        addDecomposedRz(FP_PI, NParas, TY, Flag, Qit, Qubits, MidSlice);
      } else {
        std::vector<aqcc::ParaRef> NParas(2);
        NParas[1] = MidSlice;

        // Z Pi pulse
        NParas[0].setValueResolved(FP_PI, TY);
        Flag = Flag && Qit.insertGate(kSpinRotZ, Qubits, NParas);
      }

      // Remove canonical Z Gate
      --Qit;
      if (Qit.getIdentifier() == kZ && Flag)
        Qit.removeGate();
      else {
        displayErrorAndExit("SpinNativeLowerFromCanonicalPass",
                            "lost the canonical Z gate.");
      }

      break;
    }

    case kS: {
      if (QM.platform->platformConfigLoaded() &&
          !QM.isGateInPlatformConfig(rz)) {
        std::vector<aqcc::ParaRef> NParas(3);
        addDecomposedRz(FP_PIby2, NParas, TY, Flag, Qit, Qubits, MidSlice);
      } else {
        std::vector<aqcc::ParaRef> NParas(2);
        NParas[1] = MidSlice;

        // Z Pi/2 pulse
        NParas[0].setValueResolved(FP_PIby2, TY);
        Flag = Flag && Qit.insertGate(kSpinRotZ, Qubits, NParas);
      }

      // Remove canonical S Gate
      --Qit;
      if (Qit.getIdentifier() == kS && Flag)
        Qit.removeGate();
      else {
        displayErrorAndExit("SpinNativeLowerFromCanonicalPass",
                            "lost the canonical S gate.");
      }

      break;
    }

    case kSDAG: {
      if (QM.platform->platformConfigLoaded() &&
          !QM.isGateInPlatformConfig(rz)) {
        std::vector<aqcc::ParaRef> NParas(3);
        addDecomposedRz(FP_3PIby2, NParas, TY, Flag, Qit, Qubits, MidSlice);
      } else {
        std::vector<aqcc::ParaRef> NParas(2);
        NParas[1] = MidSlice;

        // Z 3Pi/2 pulse
        NParas[0].setValueResolved(FP_3PIby2, TY);
        Flag = Flag && Qit.insertGate(kSpinRotZ, Qubits, NParas);
      }
      // Remove canonical SDAG Gate
      --Qit;
      if (Qit.getIdentifier() == kSDAG && Flag)
        Qit.removeGate();
      else {
        displayErrorAndExit("SpinNativeLowerFromCanonicalPass",
                            "lost the canonical SDAG gate.");
      }

      break;
    }

    case kT: {
      if (QM.platform->platformConfigLoaded() &&
          !QM.isGateInPlatformConfig(rz)) {
        std::vector<aqcc::ParaRef> NParas(3);
        addDecomposedRz(FP_PIby4, NParas, TY, Flag, Qit, Qubits, MidSlice);
      } else {
        std::vector<aqcc::ParaRef> NParas(2);
        NParas[1] = MidSlice;

        // Z Pi/4 pulse
        NParas[0].setValueResolved(FP_PIby4, TY);
        Flag = Flag && Qit.insertGate(kSpinRotZ, Qubits, NParas);
      }

      // Remove canonical T Gate
      --Qit;
      if (Qit.getIdentifier() == kT && Flag)
        Qit.removeGate();
      else {
        displayErrorAndExit("SpinNativeLowerFromCanonicalPass",
                            "lost the canonical T gate.");
      }

      break;
    }

    case kTDAG: {
      if (QM.platform->platformConfigLoaded() &&
          !QM.isGateInPlatformConfig(rz)) {
        std::vector<aqcc::ParaRef> NParas(3);
        addDecomposedRz(FP_7PIby4, NParas, TY, Flag, Qit, Qubits, MidSlice);
      } else {
        std::vector<aqcc::ParaRef> NParas(2);
        NParas[1] = MidSlice;

        // Z 7Pi/4 pulse
        NParas[0].setValueResolved(FP_7PIby4, TY);
        Flag = Flag && Qit.insertGate(kSpinRotZ, Qubits, NParas);
      }

      // Remove canonical T Gate
      --Qit;
      if (Qit.getIdentifier() == kTDAG && Flag)
        Qit.removeGate();
      else {
        displayErrorAndExit("SpinNativeLowerFromCanonicalPass",
                            "lost the canonical TDAG gate.");
      }

      break;
    }

    case kRX: {
      std::vector<aqcc::ParaRef> NParas(3);
      NParas[2] = MidSlice;

      // RotXY
      NParas[0] = Paras[0];
      NParas[1].setValueResolved(0.0, TY);
      Flag = Flag && Qit.insertGate(kSpinRotXY, Qubits, NParas);

      // Remove canonical RX Gate
      --Qit;
      if (Qit.getIdentifier() == kRX && Flag)
        Qit.removeGate();
      else {
        displayErrorAndExit("SpinNativeLowerFromCanonicalPass",
                            "lost the canonical RX gate.");
      }

      break;
    }

    case kRY: {
      std::vector<aqcc::ParaRef> NParas(3);
      NParas[2] = MidSlice;

      // RotXY
      NParas[0] = Paras[0];
      NParas[1].setValueResolved(FP_PIby2, TY);
      Flag = Flag && Qit.insertGate(kSpinRotXY, Qubits, NParas);

      // Remove canonical RY Gate
      --Qit;
      if (Qit.getIdentifier() == kRY && Flag)
        Qit.removeGate();
      else {
        displayErrorAndExit("SpinNativeLowerFromCanonicalPass",
                            "lost the canonical RY gate.");
      }

      break;
    }

    case kRZ: {
      // Check if config is provided and if Rz exists in platform config
      if (QM.platform->platformConfigLoaded() &&
          !QM.isGateInPlatformConfig(id)) {
        // Decompose into RXY if RZ doesn't exist in platform config
        std::vector<aqcc::ParaRef> NParas(3);

        NParas[2] = MidSlice;
        NParas[0].setValueResolved(FP_PI, TY);
        NParas[1] = Paras[0] * ParaRef(0.5);
        Flag = Flag && Qit.insertGate(kSpinRotXY, Qubits, NParas);
        NParas[0].setValueResolved(FP_PI, TY);
        NParas[1].setValueResolved(0.0, TY);
        Flag = Flag && Qit.insertGate(kSpinRotXY, Qubits, NParas);
      } else {
        // If no config is given or if Rz exists in the config, directly lower
        std::vector<aqcc::ParaRef> NParas(2);
        NParas[0] = Paras[0];
        NParas[1] = MidSlice;
        Flag = Flag && Qit.insertGate(kSpinRotZ, Qubits, NParas);
      }

      // Remove canonical RZ Gate
      --Qit;
      if (Qit.getIdentifier() == kRZ && Flag)
        Qit.removeGate();
      else {
        displayErrorAndExit("SpinNativeLowerFromCanonicalPass",
                            "lost the canonical RZ gate.");
      }

      break;
    }

    case kCX: {
      std::vector<aqcc::ParaRef> NParas1(2);
      NParas1[1] = MidSlice;
      std::vector<aqcc::ParaRef> NParas2(3);
      NParas2[2] = MidSlice;

      std::vector<aqcc::QbitRef> Target(1);
      Target[0] = Qubits[1];

      // Y Pi/2 pulse
      NParas2[0].setValueResolved(FP_PIby2, TY);
      NParas2[1] = NParas2[0];
      Flag = Flag && Qit.insertGate(kSpinRotXY, Target, NParas2);

      // CPhase Pi
      NParas1[0].setValueResolved(FP_PI, TY);
      Flag = Flag && Qit.insertGate(kSpinCPhase, Qubits, NParas1);

      // Y -Pi/2 pulse
      NParas2[1].setValueResolved(FP_3PIby2, TY);
      Flag = Flag && Qit.insertGate(kSpinRotXY, Target, NParas2);

      // Remove canonical CX Gate
      --Qit;
      if (Qit.getIdentifier() == kCX && Flag)
        Qit.removeGate();
      else {
        displayErrorAndExit("SpinNativeLowerFromCanonicalPass",
                            "lost the canonical CX gate.");
      }

      break;
    }

    case kCZ: {
      std::vector<aqcc::ParaRef> NParas(2);
      NParas[1] = MidSlice;

      // CPhase Pi
      NParas[0].setValueResolved(FP_PI, TY);
      Flag = Flag && Qit.insertGate(kSpinCPhase, Qubits, NParas);

      // Remove canonical CZ Gate
      --Qit;
      if (Qit.getIdentifier() == kCZ && Flag)
        Qit.removeGate();
      else {
        displayErrorAndExit("SpinNativeLowerFromCanonicalPass",
                            "lost the canonical CZ gate.");
      }

      break;
    }

    case kSWAP: {
      std::vector<aqcc::ParaRef> NParas(2);
      NParas[1] = MidSlice;

      // Swapalp Pi
      NParas[0].setValueResolved(FP_PI, TY);
      Flag = Flag && Qit.insertGate(kSpinSwapalp, Qubits, NParas);

      // Remove canonical SWAP Gate
      --Qit;
      if (Qit.getIdentifier() == kSWAP && Flag)
        Qit.removeGate();
      else {
        displayErrorAndExit("SpinNativeLowerFromCanonicalPass",
                            "lost the canonical SWAP gate.");
      }

      break;
    }

    case kTOFF: {
      std::vector<aqcc::ParaRef> NParas1(2);
      NParas1[1] = MidSlice;
      std::vector<aqcc::ParaRef> NParas2(3);
      NParas2[2] = MidSlice;
      std::vector<aqcc::QbitRef> NQubits1(1);
      std::vector<aqcc::QbitRef> NQubits2(2);

      // Using the following native Toffoli decomposition
      //
      // ctl0  --*--------------------------*----------------------------*---
      // --*--
      //         | pi/2                     |                            | |
      // ctl1  --*----------*---------------) pi --------*---------------) pi  =
      // --*--
      //                    | pi/2          |            | 3pi/2         | |
      // targ  --Ry(3pi/2)--*-----Ry(pi/2)--*--Ry(3pi/2)-*-----Ry(pi/2)--*---
      // --X--
      //
      //       *
      // where  |angle  =  cphase(angle)
      //       *

      // CPhase0targ Pi
      NParas1[0].setValueResolved(FP_PI, TY);
      NQubits2[0] = Qubits[0];
      NQubits2[1] = Qubits[2];
      Flag = Flag && Qit.insertGate(kSpinCPhase, NQubits2, NParas1);

      // Ytarg Pi/2 pulse
      NParas2[0].setValueResolved(FP_PIby2, TY);
      NParas2[1].setValueResolved(FP_PIby2, TY);
      NQubits1[0] = Qubits[2];
      Flag = Flag && Qit.insertGate(kSpinRotXY, NQubits1, NParas2);

      // CPhase1targ 3Pi/2
      NParas1[0].setValueResolved(FP_3PIby2, TY);
      NQubits2[0] = Qubits[1];
      NQubits2[1] = Qubits[2];
      Flag = Flag && Qit.insertGate(kSpinCPhase, NQubits2, NParas1);

      // Ytarg 3Pi/2 pulse
      NParas2[0].setValueResolved(FP_3PIby2, TY);
      NParas2[1].setValueResolved(FP_PIby2, TY);
      NQubits1[0] = Qubits[2];
      Flag = Flag && Qit.insertGate(kSpinRotXY, NQubits1, NParas2);

      // CPhase0targ Pi
      NParas1[0].setValueResolved(FP_PI, TY);
      NQubits2[0] = Qubits[0];
      NQubits2[1] = Qubits[2];
      Flag = Flag && Qit.insertGate(kSpinCPhase, NQubits2, NParas1);

      // Ytarg Pi/2 pulse
      NParas2[0].setValueResolved(FP_PIby2, TY);
      NParas2[1].setValueResolved(FP_PIby2, TY);
      NQubits1[0] = Qubits[2];
      Flag = Flag && Qit.insertGate(kSpinRotXY, NQubits1, NParas2);

      // CPhase1targ Pi/2
      NParas1[0].setValueResolved(FP_PIby2, TY);
      NQubits2[0] = Qubits[1];
      NQubits2[1] = Qubits[2];
      Flag = Flag && Qit.insertGate(kSpinCPhase, NQubits2, NParas1);

      // CPhase01 Pi/2
      NParas1[0].setValueResolved(FP_PIby2, TY);
      NQubits2[0] = Qubits[0];
      NQubits2[1] = Qubits[1];
      Flag = Flag && Qit.insertGate(kSpinCPhase, NQubits2, NParas1);

      // Ytarg 3Pi/2 pulse
      NParas2[0].setValueResolved(FP_3PIby2, TY);
      NParas2[1].setValueResolved(FP_PIby2, TY);
      NQubits1[0] = Qubits[2];
      Flag = Flag && Qit.insertGate(kSpinRotXY, NQubits1, NParas2);

      // Remove canonical Toffoli Gate
      --Qit;
      if (Qit.getIdentifier() == kTOFF && Flag)
        Qit.removeGate();
      else {
        displayErrorAndExit("SpinNativeLowerFromCanonicalPass",
                            "lost the canonical Toffoli gate.");
      }

      break;
    }

    // Preps and Meas
    case kPREPX: {
      std::vector<aqcc::ParaRef> NParas(3);
      NParas[2] = MidSlice;
      std::vector<aqcc::ParaRef> slice_only = {MidSlice};

      // Y Pi/2 pulse
      NParas[0].setValueResolved(FP_PIby2, TY);
      NParas[1].setValueResolved(FP_PIby2, TY);
      Flag = Flag && Qit.insertGate(kSpinRotXY, Qubits, NParas);

      // native prepz gate
      Flag = Flag && Qit.insertGate(kSpinPrepZ, Qubits, slice_only);

      // Y -Pi/2 pulse
      NParas[0].setValueResolved(FP_PIby2, TY);
      NParas[1].setValueResolved(FP_3PIby2, TY);
      Flag = Flag && Qit.insertGate(kSpinRotXY, Qubits, NParas);

      // Remove canonical PREPX Gate
      --Qit;
      if (Qit.getIdentifier() == kPREPX && Flag)
        Qit.removeGate();

      else {
        displayErrorAndExit("SpinNativeLowerFromCanonicalPass",
                            "lost the canonical PrepX gate.");
      }

      break;
    }

    case kPREPY: {
      std::vector<aqcc::ParaRef> NParas(3);
      NParas[2] = MidSlice;
      std::vector<aqcc::ParaRef> slice_only = {MidSlice};

      // X -Pi/2 pulse
      NParas[0].setValueResolved(FP_PIby2, TY);
      NParas[1].setValueResolved(FP_PI, TY);
      Flag = Flag && Qit.insertGate(kSpinRotXY, Qubits, NParas);

      // native prepz gate
      Flag = Flag && Qit.insertGate(kSpinPrepZ, Qubits, slice_only);

      // X Pi/2 pulse
      NParas[0].setValueResolved(FP_PIby2, TY);
      NParas[1].setValueResolved(0.0, TY);
      Flag = Flag && Qit.insertGate(kSpinRotXY, Qubits, NParas);

      // Remove canonical PREPY Gate
      --Qit;
      if (Qit.getIdentifier() == kPREPY && Flag)
        Qit.removeGate();

      else {
        displayErrorAndExit("SpinNativeLowerFromCanonicalPass",
                            "lost the canonical PrepY gate.");
      }

      break;
    }

    case kPREPZ: {
      std::vector<aqcc::ParaRef> slice_only = {MidSlice};

      // native prepz gate
      Flag = Flag && Qit.insertGate(kSpinPrepZ, Qubits, slice_only);

      // Remove canonical PREPZ Gate
      --Qit;
      if (Qit.getIdentifier() == kPREPZ && Flag)
        Qit.removeGate();

      else {
        displayErrorAndExit("SpinNativeLowerFromCanonicalPass",
                            "lost the canonical PrepZ gate.");
      }

      break;
    }

    case kMEASX: {
      std::vector<aqcc::ParaRef> NParas(3);
      NParas[2] = MidSlice;
      std::vector<aqcc::ParaRef> slice_only = {MidSlice};

      // Y Pi/2 pulse
      NParas[0].setValueResolved(FP_PIby2, TY);
      NParas[1].setValueResolved(FP_PIby2, TY);
      Flag = Flag && Qit.insertGate(kSpinRotXY, Qubits, NParas);

      // native measz gate
      Paras.push_back(MidSlice);
      Flag = Flag && Qit.insertGate(kSpinMeasZ, Qubits, Paras);

      // Y -Pi/2 pulse
      NParas[0].setValueResolved(FP_PIby2, TY);
      NParas[1].setValueResolved(FP_3PIby2, TY);
      Flag = Flag && Qit.insertGate(kSpinRotXY, Qubits, NParas);

      // Remove canonical MEASX Gate
      --Qit;
      if (Qit.getIdentifier() == kMEASX && Flag) {
        Qit.removeGate();
      } else {
        displayErrorAndExit("SpinNativeLowerFromCanonicalPass",
                            "lost the canonical MeasX gate.");
      }

      break;
    }

    case kMEASY: {
      std::vector<aqcc::ParaRef> NParas(3);
      NParas[2] = MidSlice;
      std::vector<aqcc::ParaRef> slice_only = {MidSlice};

      // X -Pi/2 pulse
      NParas[0].setValueResolved(FP_PIby2, TY);
      NParas[1].setValueResolved(FP_PI, TY);
      Flag = Flag && Qit.insertGate(kSpinRotXY, Qubits, NParas);

      // native measz gate
      Paras.push_back(MidSlice);
      Flag = Flag && Qit.insertGate(kSpinMeasZ, Qubits, Paras);

      // X Pi/2 pulse
      NParas[0].setValueResolved(FP_PIby2, TY);
      NParas[1].setValueResolved(0.0, TY);
      Flag = Flag && Qit.insertGate(kSpinRotXY, Qubits, NParas);

      // Remove canonical MEASY Gate
      --Qit;
      if (Qit.getIdentifier() == kMEASY && Flag) {
        Qit.removeGate();
      } else {
        displayErrorAndExit("SpinNativeLowerFromCanonicalPass",
                            "lost the canonical MeasY gate.");
      }

      break;
    }

    case kMEASZ: {
      std::vector<aqcc::ParaRef> slice_only = {MidSlice};

      // native measz gate
      Paras.push_back(MidSlice);
      Flag = Flag && Qit.insertGate(kSpinMeasZ, Qubits, Paras);

      // Remove canonical MEASZ Gate
      --Qit;
      if (Qit.getIdentifier() == kMEASZ && Flag) {
        Qit.removeGate();
      } else {
        displayErrorAndExit("SpinNativeLowerFromCanonicalPass",
                            "lost the canonical MeasZ gate.");
      }

      break;
    }

    // Canonical versions of spin native gates
    case kCPHASE: {
      std::vector<aqcc::ParaRef> NParas(2);
      NParas[1] = MidSlice;

      // CPhase Pi
      NParas[0] = Paras[0];
      Flag = Flag && Qit.insertGate(kSpinCPhase, Qubits, NParas);

      // Remove canonical CPhase Gate
      --Qit;
      if (Qit.getIdentifier() == kCPHASE && Flag)
        Qit.removeGate();
      else {
        displayErrorAndExit("SpinNativeLowerFromCanonicalPass",
                            "lost the canonical CPhase gate.");
      }

      break;
    }
    case kRXY: {
      std::vector<aqcc::ParaRef> NParas(3);
      NParas[2] = MidSlice;

      // RotXY
      NParas[0] = Paras[0];
      NParas[1] = Paras[1];
      Flag = Flag && Qit.insertGate(kSpinRotXY, Qubits, NParas);

      // Remove canonical RXY Gate
      --Qit;
      if (Qit.getIdentifier() == kRXY && Flag)
        Qit.removeGate();
      else {
        displayErrorAndExit("SpinNativeLowerFromCanonicalPass",
                            "lost the canonical RXY gate.");
      }

      break;
    }
    case kSWAPA: {
      std::vector<aqcc::ParaRef> NParas(2);
      NParas[1] = MidSlice;

      // Swapalp Pi
      NParas[0] = Paras[0];
      Flag = Flag && Qit.insertGate(kSpinSwapalp, Qubits, NParas);

      // Remove canonical SwapA Gate
      --Qit;
      if (Qit.getIdentifier() == kSWAPA && Flag)
        Qit.removeGate();
      else {
        displayErrorAndExit("SpinNativeLowerFromCanonicalPass",
                            "lost the canonical SwapA gate.");
      }

      break;
    }

    default: {
      --Qit;
      break;
    }
    }

  } while (!Qit.isBegin());
  return false;

} // lowerFromCanonical

void SpinNativeLowerFromCanonical::cleanUpGateDefs(QuantumModule &QM) {

  // Iterate over all the quantum gates, delete metadata and erase from parent
  // NOTE: QuantumGates includes All gates, including spin native

  std::vector<QuantumGates::iterator> gates_to_rm;
  QuantumGates AllGates = QM.gates();

  for (auto gate_iter = AllGates.begin(), end = AllGates.end();
       gate_iter != end; ++gate_iter) {
    if (gate_iter->isNullValue()) {
      displayErrorAndExit("SpinNativeLowerFromCanonicalPass",
                          "Found null value on iterator.");
    }

    if (getGateIdentifier(gate_iter) < kNumGates) {
      gate_iter->clearMetadata();
      gate_iter->dropAllReferences();

      // check that gate calls have all been removed
      for (auto user : gate_iter->users()) {
        if (CallInst *CI = dyn_cast<CallInst>(user)) {
          displayErrorAndExit(
              "SpinNativeLowerFromCanonicalPass",
              "Found a gate call outside of a quantum kernel in '" +
                  DemangleQuantumKernelName(
                      CI->getParent()->getParent()->getName()) +
                  "'.");
        }
      }
      // to remove references in global annotations
      gate_iter->replaceAllUsesWith(UndefValue::get(gate_iter->getType()));
      if (gate_iter->use_empty()) {
        gate_iter->eraseFromParent();
        gates_to_rm.push_back(gate_iter);
      } else
        displayErrorAndExit("SpinNativeLowerFromCanonicalPass",
                            "The gate " + (gate_iter->getName()).str() +
                                " has a leftover use.\n");
    }
  };

  // remove gates from AllGates
  for (int i = 0; i < gates_to_rm.size(); i++)
    QM.removeGate(gates_to_rm[i]);

  // Now remove left over intrinsics

  Intrinsic::ID IntrID;
  char Prefix[] = "quantum";
  IntrID = Function::lookupIntrinsicID("llvm.quantum.qubit");
  Function *qbit_intr = Intrinsic::getDeclaration(QM.getBaseModule(), IntrID);
  qbit_intr->dropAllReferences();
  qbit_intr->removeFromParent();

  IntrID = Function::lookupIntrinsicID("llvm.quantum.param");
  Function *param_intr = Intrinsic::getDeclaration(QM.getBaseModule(), IntrID);
  param_intr->dropAllReferences();
  param_intr->removeFromParent();

  IntrID = Function::lookupIntrinsicID("llvm.quantum.cbit");
  Function *cbit_intr = Intrinsic::getDeclaration(QM.getBaseModule(), IntrID);
  cbit_intr->dropAllReferences();
  cbit_intr->removeFromParent();

} // cleanUpGateDefs

namespace {
struct SpinNativeLowerFromCanonicalLegacyPass : public ModulePass {

  static char ID; // Pass identification, replacement for typeid

  QuantumModule &QM = QuantumCompilerLinkageLegacyPass::QM;

  SpinNativeLowerFromCanonical SN;

  SpinNativeLowerFromCanonicalLegacyPass() : ModulePass(ID) {
    initializeSpinNativeLowerFromCanonicalLegacyPassPass(
        *PassRegistry::getPassRegistry());
  }

  void getAnalysisUsage(AnalysisUsage &AU) const override {

    AU.setPreservesAll();
    AU.addRequired<QuantumSpinNativeToJsonWrapperPass>();
    // AU.addRequired<QuantumCompilerLinkageLegacyPass>();
  }

  bool runOnModule(Module &M) override {

    for (auto &QK : QM) {
      for (auto &QBB : QK) {
        SN.lowerFromCanonical(QBB, QM);
        setSingleSliceBeginAndEnd(QBB);
      }
    }

    // Clean-up
    SN.cleanUpGateDefs(QM);

    QM.setNative();

    return false;
  }
}; // end of struct SpinNativeLowerFromCanonicalLegacyPass

} // end of anonymous namespace

char SpinNativeLowerFromCanonicalLegacyPass::ID = 0;

INITIALIZE_PASS_BEGIN(SpinNativeLowerFromCanonicalLegacyPass,
                      "spin-lower-from-canonical",
                      "Spin native lower from canonical legacy pass", false,
                      false)
INITIALIZE_PASS_END(SpinNativeLowerFromCanonicalLegacyPass,
                    "spin-lower-from-canonical",
                    "Spin native lower from canonical legacy pass", false,
                    false)

PreservedAnalyses
SpinNativeLowerFromCanonicalPass::run(Module &M, ModuleAnalysisManager &MAM) {
  QuantumModuleProxy QMP = MAM.getResult<QuantumCompilerLinkageAnalysis>(M);
  QuantumModule *QM = QMP.QM;
  for (auto &QK : *QM) {
    for (auto &QBB : QK) {
      SN.lowerFromCanonical(QBB, *QM);
      setSingleSliceBeginAndEnd(QBB);
    }
  }

  // Clean-up
  SN.cleanUpGateDefs(*QM);

  (*QM).setNative();

  return PreservedAnalyses::all();
}

#undef DEBUG_TYPE
