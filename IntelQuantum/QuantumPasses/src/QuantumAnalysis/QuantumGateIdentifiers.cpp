//===- QuantumGateIdentifiers.cpp------------------------------------------===//
//
// Copyright (C) 2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//
//===----------------------------------------------------------------------===//
///
/// This file provides the enumerate structure that associate to each
/// canonical gate a non-negative integer as a quick way to identify the
/// gate type.
///
//===----------------------------------------------------------------------===//

#include "IntelQuantumPasses/QuantumAnalysis/QuantumGateIdentifiers.h"

#include "llvm/IR/Function.h"
#include "llvm/IR/Intrinsics.h"

namespace llvm {

int getInverseSQGateID(int id) {

  // switch to invert gate
  switch (id) {
  case kIDLE:
    return kIDLE;
  case kHSDAG:
    return kSH;
  case kSH:
    return kHSDAG;
  case kH:
    return kH;
  case kKDAG:
    return kXKDAG;
  case kS:
    return kZS;
  case kZ:
    return kZ;
  case kZHSDAG:
    return kYSH;
  case kZSH:
    return kXHSDAG;
  case kZH:
    return kXH;
  case kZKDAG:
    return kZKDAG;
  case kZS:
    return kS;
  case kX:
    return kX;
  case kXHSDAG:
    return kZSH;
  case kXSH:
    return kYHSDAG;
  case kXH:
    return kZH;
  case kXKDAG:
    return kKDAG;
  case kXS:
    return kXS;
  case kY:
    return kY;
  case kYHSDAG:
    return kXSH;
  case kYSH:
    return kZHSDAG;
  case kYH:
    return kYH;
  case kYKDAG:
    return kYKDAG;
  case kYS:
    return kYS;
  case kT:
    return kTDAG;
  case kTDAG:
    return kT;
  }
  return -1;
}

std::string GetNameFromGateIdentifier(int id) {
  if (id < 0) {
    std::string out = "custom_" + std::to_string(-1 * id);
    return out;
  }

  switch (id) {
  case kIDLE:
    return "identity";
  case kHSDAG:
    return "HSDag";
  case kSH:
    return "SH";
  case kH:
    return "H";
  case kKDAG:
    return "KDag";
  case kS:
    return "S";
  case kZ:
    return "Z";
  case kZHSDAG:
    return "ZHSDag";
  case kZSH:
    return "ZSH";
  case kZH:
    return "ZH";
  case kZKDAG:
    return "ZKDag";
  case kZS:
    return "Sdag";
  case kX:
    return "X";
  case kXHSDAG:
    return "XHSDag";
  case kXSH:
    return "XSH";
  case kXH:
    return "XH";
  case kXKDAG:
    return "XKDag";
  case kXS:
    return "XS";
  case kY:
    return "Y";
  case kYHSDAG:
    return "YHSDag";
  case kYSH:
    return "YSH";
  case kYH:
    return "YH";
  case kYKDAG:
    return "YKDag";
  case kYS:
    return "YS";
  case kT:
    return "T";
  case kTDAG:
    return "Tdag";
  case kRX:
    return "RX";
  case kRY:
    return "RY";
  case kRZ:
    return "RZ";
  case kAX:
    return "AX";
  case kAY:
    return "AY";
  case kAZ:
    return "AZ";
  case kBX:
    return "BX";
  case kBY:
    return "BY";
  case kBZ:
    return "BZ";
  case kCX:
    return "CNOT";
  case kCY:
    return "CY";
  case kCZ:
    return "CZ";
  case kSWAP:
    return "SWAP";
  case kTOFF:
    return "Toffoli";
  case kPREPX:
    return "PrepX";
  case kPREPY:
    return "PrepY";
  case kPREPZ:
    return "PrepZ";
  case kMEASX:
    return "MeasX";
  case kMEASY:
    return "MeasY";
  case kMEASZ:
    return "MeasZ";
  case kCPHASE:
    return "CPhase";
  case kRXY:
    return "RXY";
  case kSWAPA:
    return "SwapA";
  case kNumGates:
    return "NumGates";
  case kStartPulse:
    return "StartPulse";
  case kDirectPulse:
    return "DirectPulse";
  case kReadPulse:
    return "ReadPulse";
  case kBarrierPulse:
    return "BarrierPulse";
  case kSpinRotZ:
    return "SpinRotZ";
  case kSpinRotXY:
    return "SpinRotXY";
  case kSpinCPhase:
    return "SpinCPhase";
  case kSpinSwapalp:
    return "SpinSwapalp";
  case kSpinMeasZ:
    return "SpinMeasZ";
  case kSpinPrepZ:
    return "SpinPrepZ";
  case kSpinRotZ_IMM:
    return "SpinRotZ_IMM";
  case kSpinRotZ_IMM_IDX:
    return "SpinRotZ_IMM_IDX";
  case kSpinRotXY_IMM:
    return "SpinRotXY_IMM";
  case kSpinRotXY_IMM_IDX:
    return "SpinRotXY_IMM_IDX";
  case kSpinCPhase_IMM:
    return "SpinCPhase_IMM";
  case kSpinCPhase_IMM_IDX:
    return "SpinCPhase_IMM_IDX";
  case kSpinSwapalp_IMM:
    return "SpinSwapalp_IMM";
  case kSpinSwapalp_IMM_IDX:
    return "SpinSwapalp_IMM_IDX";
  case kSpinMeasZ_IMM:
    return "SpinMeasZ_IMM";
  case kSpinMeasZ_IMM_IDX:
    return "SpinMeasZ_IMM_IDX";
  case kSpinPrepZ_IMM:
    return "SpinPrepZ_IMM";
  case kSpinIMM_IDXEnd:
    return "SpinEnd";
  default:
    llvm::errs() << "WARNING: unknown gate identifier encountered: " << id
                 << "\n";
    std::string out = "unknown_id_" + std::to_string(id);
    return out;
  }
}

int getInstrinsicIDFromGateIdentifier(int id) {

  const char Prefix[] = "llvm";

  switch (id) {

  case kSpinRotZ:
    return Function::lookupIntrinsicID(std::string(Prefix) +
                                       std::string(".quantum.rotz"));

  case kSpinRotXY:
    return Function::lookupIntrinsicID(std::string(Prefix) +
                                       std::string(".quantum.rotxy"));

  case kSpinCPhase:
    return Function::lookupIntrinsicID(std::string(Prefix) +
                                       std::string(".quantum.cphase"));

  case kSpinSwapalp:
    return Function::lookupIntrinsicID(std::string(Prefix) +
                                       std::string(".quantum.swapalp"));

  case kSpinMeasZ:
    return Function::lookupIntrinsicID(std::string(Prefix) +
                                       std::string(".quantum.measz"));

  case kSpinPrepZ:
    return Function::lookupIntrinsicID(std::string(Prefix) +
                                       std::string(".quantum.prepz"));

  case kSpinRotZ_IMM:
    return Function::lookupIntrinsicID(std::string(Prefix) +
                                       std::string(".quantum.rotz.imm"));

  case kSpinRotZ_IMM_IDX:
    return Function::lookupIntrinsicID(std::string(Prefix) +
                                       std::string(".quantum.rotz.imm.index"));

  case kSpinRotXY_IMM:
    return Function::lookupIntrinsicID(std::string(Prefix) +
                                       std::string(".quantum.rotxy.imm"));

  case kSpinRotXY_IMM_IDX:
    return Function::lookupIntrinsicID(std::string(Prefix) +
                                       std::string(".quantum.rotxy.imm.index"));

  case kSpinCPhase_IMM:
    return Function::lookupIntrinsicID(std::string(Prefix) +
                                       std::string(".quantum.cphase.imm"));

  case kSpinCPhase_IMM_IDX:
    return Function::lookupIntrinsicID(
        std::string(Prefix) + std::string(".quantum.cphase.imm.index"));

  case kSpinSwapalp_IMM:
    return Function::lookupIntrinsicID(std::string(Prefix) +
                                       std::string(".quantum.swapalp.imm"));

  case kSpinSwapalp_IMM_IDX:
    return Function::lookupIntrinsicID(
        std::string(Prefix) + std::string(".quantum.swapalp.imm.index"));

  case kSpinMeasZ_IMM:
    return Function::lookupIntrinsicID(std::string(Prefix) +
                                       std::string(".quantum.measz.imm"));

  case kSpinMeasZ_IMM_IDX:
    return Function::lookupIntrinsicID(std::string(Prefix) +
                                       std::string(".quantum.measz.imm.index"));

  case kSpinPrepZ_IMM:
    return Function::lookupIntrinsicID(std::string(Prefix) +
                                       std::string(".quantum.prepz.imm"));
  }

  return -1;
}

std::string getPlatformNameFromGateIdentifier(int id) {
  // The only "Platform Names" (used in config.json) that do not corresponds to
  // the "Gate Names" (used in IQC) are those related to Spin Native
  // architecture.

  int id_native = getBaseVersionForSpinNative(id);

  switch (id_native) {
  case GateIdentifier::kSpinRotZ:
    return "kRotz";
  case GateIdentifier::kSpinRotXY:
    return "kRotxy";
  case GateIdentifier::kSpinCPhase:
    return "kCz";
  case GateIdentifier::kSpinSwapalp:
    return "kSwapalp";
  case GateIdentifier::kSpinMeasZ:
    return "kMeasz";
  case GateIdentifier::kSpinPrepZ:
    return "kPrep";
  default:
    std::string temp = "unknown_id_" + std::to_string(id);
    return temp;
  }
}

int getSpinNativeVersionForCanonical(int id) {

  switch (id) {

  case kCPHASE:
    return kSpinCPhase;
  case kRXY:
    return kSpinRotXY;
  case kRZ:
    return kSpinRotZ;
  case kSWAPA:
    return kSpinSwapalp;
  case kPREPZ:
    return kSpinMeasZ;
  case kMEASZ:
    return kSpinPrepZ;
  }

  return id;
}

int getCanonicalVersionForSpinNative(int id) {
  switch (id) {
  case kSpinRotZ:
  case kSpinRotZ_IMM:
  case kSpinRotZ_IMM_IDX:
    return kRZ;
  case kSpinRotXY:
  case kSpinRotXY_IMM:
  case kSpinRotXY_IMM_IDX:
    return kRXY;
  case kSpinCPhase:
  case kSpinCPhase_IMM:
  case kSpinCPhase_IMM_IDX:
    return kCPHASE;
  case kSpinSwapalp:
  case kSpinSwapalp_IMM:
  case kSpinSwapalp_IMM_IDX:
    return kSWAPA;
  case kSpinMeasZ:
  case kSpinMeasZ_IMM:
  case kSpinMeasZ_IMM_IDX:
    return kMEASZ;
  case kSpinPrepZ:
  case kSpinPrepZ_IMM:
    return kPREPZ;
  }

  return id;
}

int getBaseVersionForSpinNative(int id) {
  if (id < kNumGates)
    return getSpinNativeVersionForCanonical(id);
  else if (id >= kSpinBegin && id < kSpinEnd)
    return id;
  else if (id >= kSpinIMMBegin && id < kSpinIMMEnd)
    return id + kSpinBegin - kSpinEnd;
  else if (id >= kSpinIMM_IDXBegin && id < kSpinIMM_IDXEnd)
    return id + kSpinBegin - kSpinIMMEnd;
  return id;
}

int getIMMVersionForSpinNative(int id) {
  if (id < kNumGates)
    return getIMMVersionForSpinNative(getSpinNativeVersionForCanonical(id));
  else if (id >= kSpinBegin && id < kSpinEnd)
    return id + kSpinEnd - kSpinBegin;
  else if (id >= kSpinIMMBegin && id < kSpinIMMEnd)
    return id;
  else if (id >= kSpinIMM_IDXBegin && id < kSpinIMM_IDXEnd)
    return id + kSpinIMMBegin - kSpinIMMEnd;
  return id;
}

int getIMM_IDXVersionForSpinNative(int id) {
  if (id < kNumGates)
    return getIMM_IDXVersionForSpinNative(getSpinNativeVersionForCanonical(id));
  else if (id >= kSpinBegin && id < kSpinEnd)
    return id + kSpinIMMEnd - kSpinBegin;
  else if (id >= kSpinIMMBegin && id < kSpinIMMEnd)
    return id + kSpinIMMEnd - kSpinIMMBegin;
  else if (id >= kSpinIMM_IDXBegin && id < kSpinIMM_IDXEnd)
    return id;
  return id;
}

int getNoIMMVersionForAll(int id) {
  if (id < kNumGates)
    return id;
  return getBaseVersionForSpinNative(id);
}

////////////////////////////////////////////////////////////////////////////////

/// TODO: Needs to be updated once the canonical gates are settled

int DoesMatrixCorrespondToCanonicalGate(std::vector<double> matrix_real,
                                        std::vector<double> matrix_imag) {
  assert(matrix_real.size() == matrix_imag.size() &&
         "ERR: expected same dimension of real and imag matrix.");
  double precision = 1e-10;
  double sqrt_onehalf = 0.70710678118;
  double cos_pifourth = sqrt_onehalf;
  double sin_pifourth = sqrt_onehalf;
  // One-qubit gates
  if (matrix_real.size() == 2 * 2) {
    if (std::abs(matrix_real[0] - sqrt_onehalf) < precision &&
        std::abs(matrix_real[1] - sqrt_onehalf) < precision &&
        std::abs(matrix_real[2] - sqrt_onehalf) < precision &&
        std::abs(matrix_real[3] + sqrt_onehalf) < precision &&
        matrix_imag[0] == 0 && matrix_imag[1] == 0 && matrix_imag[2] == 0 &&
        matrix_imag[3] == 0) {
      return GateIdentifier::kH;
    } else if (matrix_real[0] == 0 && matrix_real[1] == 1 &&
               matrix_real[2] == 1 && matrix_real[3] == 0 &&
               matrix_imag[0] == 0 && matrix_imag[1] == 0 &&
               matrix_imag[2] == 0 && matrix_imag[3] == 0) {
      return GateIdentifier::kX;
    } else if (matrix_real[0] == 0 && matrix_real[1] == 0 &&
               matrix_real[2] == 0 && matrix_real[3] == 0 &&
               matrix_imag[0] == 0 && matrix_imag[1] == -1 &&
               matrix_imag[2] == 1 && matrix_imag[3] == 0) {
      return GateIdentifier::kY;
    } else if (matrix_real[0] == 1 && matrix_real[1] == 0 &&
               matrix_real[2] == 0 && matrix_real[3] == -1 &&
               matrix_imag[0] == 0 && matrix_imag[1] == 0 &&
               matrix_imag[2] == 0 && matrix_imag[3] == 0) {
      return GateIdentifier::kZ;
    } else if (matrix_real[0] == 1 && matrix_real[1] == 0 &&
               matrix_real[2] == 0 && matrix_real[3] == 0 &&
               matrix_imag[0] == 0 && matrix_imag[1] == 0 &&
               matrix_imag[2] == 0 && matrix_imag[3] == 1) {
      return GateIdentifier::kS;
    } else if (matrix_real[0] == 1 && matrix_real[1] == 0 &&
               matrix_real[2] == 0 && matrix_real[3] == 0 &&
               matrix_imag[0] == 0 && matrix_imag[1] == 0 &&
               matrix_imag[2] == 0 && matrix_imag[3] == -1) {
      return GateIdentifier::kSDAG;
    } else if (matrix_real[0] == 1 && matrix_real[1] == 0 &&
               matrix_real[2] == 0 &&
               std::abs(matrix_real[3] - cos_pifourth) < precision &&
               matrix_imag[0] == 0 && matrix_imag[1] == 0 &&
               matrix_imag[2] == 0 &&
               std::abs(matrix_imag[3] - sin_pifourth) < precision) {
      return GateIdentifier::kT;
    } else if (matrix_real[0] == 1 && matrix_real[1] == 0 &&
               matrix_real[2] == 0 &&
               std::abs(matrix_real[3] - cos_pifourth) < precision &&
               matrix_imag[0] == 0 && matrix_imag[1] == 0 &&
               matrix_imag[2] == 0 &&
               std::abs(matrix_imag[3] + sin_pifourth) < precision) {
      return GateIdentifier::kTDAG;
    }
    /// TODO: verification for RX, RY, RZ
  }
  // Two-qubit gates
  else if (matrix_real.size() == 4 * 4) {
    if (matrix_real[0] == 1 && matrix_real[1] == 0 && matrix_real[2] == 0 &&
        matrix_real[3] == 0 && matrix_real[4] == 0 && matrix_real[5] == 1 &&
        matrix_real[6] == 0 && matrix_real[7] == 0 && matrix_real[8] == 0 &&
        matrix_real[9] == 0 && matrix_real[10] == 0 && matrix_real[11] == 1 &&
        matrix_real[12] == 0 && matrix_real[13] == 0 && matrix_real[14] == 1 &&
        matrix_real[15] == 0 && matrix_imag[0] == 0 && matrix_imag[1] == 0 &&
        matrix_imag[2] == 0 && matrix_imag[3] == 0 && matrix_imag[4] == 0 &&
        matrix_imag[5] == 0 && matrix_imag[6] == 0 && matrix_imag[7] == 0 &&
        matrix_imag[8] == 0 && matrix_imag[9] == 0 && matrix_imag[10] == 0 &&
        matrix_imag[11] == 0 && matrix_imag[12] == 0 && matrix_imag[13] == 0 &&
        matrix_imag[14] == 0 && matrix_imag[15] == 0) {
      return GateIdentifier::kCX;
    } else if (matrix_real[0] == 1 && matrix_real[1] == 0 &&
               matrix_real[2] == 0 && matrix_real[3] == 0 &&
               matrix_real[4] == 0 && matrix_real[5] == 1 &&
               matrix_real[6] == 0 && matrix_real[7] == 0 &&
               matrix_real[8] == 0 && matrix_real[9] == 0 &&
               matrix_real[10] == 1 && matrix_real[11] == 0 &&
               matrix_real[12] == 0 && matrix_real[13] == 0 &&
               matrix_real[14] == 0 && matrix_real[15] == -1 &&
               matrix_imag[0] == 0 && matrix_imag[1] == 0 &&
               matrix_imag[2] == 0 && matrix_imag[3] == 0 &&
               matrix_imag[4] == 0 && matrix_imag[5] == 0 &&
               matrix_imag[6] == 0 && matrix_imag[7] == 0 &&
               matrix_imag[8] == 0 && matrix_imag[9] == 0 &&
               matrix_imag[10] == 0 && matrix_imag[11] == 0 &&
               matrix_imag[12] == 0 && matrix_imag[13] == 0 &&
               matrix_imag[14] == 0 && matrix_imag[15] == 0) {
      return GateIdentifier::kCZ;
    } else if (matrix_real[0] == 1 && matrix_real[1] == 0 &&
               matrix_real[2] == 0 && matrix_real[3] == 0 &&
               matrix_real[4] == 0 && matrix_real[5] == 0 &&
               matrix_real[6] == 1 && matrix_real[7] == 0 &&
               matrix_real[8] == 0 && matrix_real[9] == 1 &&
               matrix_real[10] == 0 && matrix_real[11] == 0 &&
               matrix_real[12] == 0 && matrix_real[13] == 0 &&
               matrix_real[14] == 0 && matrix_real[15] == 1 &&
               matrix_imag[0] == 0 && matrix_imag[1] == 0 &&
               matrix_imag[2] == 0 && matrix_imag[3] == 0 &&
               matrix_imag[4] == 0 && matrix_imag[5] == 0 &&
               matrix_imag[6] == 0 && matrix_imag[7] == 0 &&
               matrix_imag[8] == 0 && matrix_imag[9] == 0 &&
               matrix_imag[10] == 0 && matrix_imag[11] == 0 &&
               matrix_imag[12] == 0 && matrix_imag[13] == 0 &&
               matrix_imag[14] == 0 && matrix_imag[15] == 0) {
      return GateIdentifier::kSWAP;
    }
  }
  // Three-qubit gates
  else if (matrix_real.size() == 8 * 8) {
    bool is_toffoli = true;
    for (int i = 0; i < 8 * 8; ++i) {
      if (matrix_imag[i] != 0)
        is_toffoli = false;
      if (i == 0 || i == 9 || i == 18 || i == 27 || i == 36 || i == 45 ||
          i == 54 + 1 || i == 63 - 1) {
        if (matrix_real[i] != 1)
          is_toffoli = false;
      } else if (matrix_real[i] != 0)
        is_toffoli = false;
    }
    if (is_toffoli)
      return GateIdentifier::kTOFF;
  }
  return -1;
}

////////////////////////////////////////////////////////////////////////////////

} // namespace llvm
