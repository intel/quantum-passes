//===- QuantumGateIdentifiers.h -------------------------------------------===//
//
// Copyright (C) 2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//
//===----------------------------------------------------------------------===//
/// \file "QuantumGateIdentifiers.h"
///
/// This file provides the enumerate structure that associate to each
/// canonical gate a non-negative integer as a quick way to identify the
/// gate type.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_QUANTUM_GATEIDENTIFIERS_H
#define LLVM_QUANTUM_GATEIDENTIFIERS_H

#include "llvm/Support/CommandLine.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/raw_ostream.h"

#include <assert.h>
#include <cmath>
#include <string>
#include <vector>

namespace llvm {

////////////////////////////////////////////////////////////////////////////////
// Floating point values for PI and its multiples

const double FP_PI = 3.14159265358979323846;
const double FP_2PI = 6.28318530717958647692;

const double FP_PIby2 = 1.57079632679489661923;
const double FP_PIby4 = 0.78539816339744830962;
const double FP_PIby8 = 0.39269908169872415481;
const double FP_PIby16 = 0.19634954084936207740;

const double FP_3PIby2 = 4.71238898038468985769;
const double FP_5PIby2 = 7.85398163397448309615;
const double FP_7PIby2 = 10.9955742875642763346;

const double FP_3PIby4 = 2.35619449019234492885;
const double FP_5PIby4 = 3.92699081698724154808;
const double FP_7PIby4 = 5.49778714378213816731;

////////////////////////////////////////////////////////////////////////////////
// Enumerate to express the gate identifier in terms of human-friendly names.

enum GateIdentifier {
  // All One-qubit Clifford gates
  // NOTE: the relative order of these gates is not arbitrary.
  // it represents an encoding of these gates based on their
  // conjgation action (fwd) on the local frame, via
  // (key -kSQBegin) = ops + 2*swap + 6*z + 12* x,
  // where:
  // 		ops = 0,1,2 -> (z, x), (x, y), (y,z)
  // 		swap = 0,1 -> 1 if the ops order is swapped
  // 		z = 0,1 -> 1 if the op has a Z at end
  // 		x = 0,1 -> 1 if the op has a X at end
  //
  kIDLE = 0, // identity gate
  kHSDAG = 1,
  kSH = 2,
  kH = 3,
  kKDAG = 4,
  kS = 5,
  kZ = 6,
  kZHSDAG = 7,
  kZSH = 8,
  kZH = 9,
  kZKDAG = 10,
  kZS = 11,
  kX = 12,
  kXHSDAG = 13,
  kXSH = 14,
  kXH = 15,
  kXKDAG = 16,
  kXS = 17,
  kY = 18,
  kYHSDAG = 19,
  kYSH = 20,
  kYH = 21,
  kYKDAG = 22,
  kYS = 23,
  kT = 24,    // T gate = sqrt(S), equivalent to a RZ( 45) rotation apart from
              // global phase
  kTDAG = 25, // T_DAGger gate   , equivalent to a RZ(-45) rotation apart from
              // global phase
  //
  kRX = 26, // X rotation with parametrized angle
  kRY = 27, // Y rotation with parametrized angle
  kRZ = 28, // Z rotation with parametrized angle
  // Two-qubit Clifford Gates
  // NOTE: The relative order of these gates is not arbitrary.
  // It encodes the basis( 0 = X, 1 = Y, 2 = Z of their
  // ctrl(first qubit) and target(second qubit) via
  //  (key- kTQEBegin) = target_basis + 3 * ctrl_basis
  kAX = 29,
  kAY = 30,
  kAZ = 31,
  kBX = 32,
  kBY = 33,
  kBZ = 34,
  kCX = 35,
  kCY = 36,
  kCZ = 37,
  kSWAP = 38, // SWAP gate, last two-qubit gate
  // Three-qubit physical gates
  kTOFF = 39,  // Toffoli gate, only three-qubit gate
  kPREPX = 40, // iniitialize in the state |+>
  kPREPY = 41,
  kPREPZ = 42, // initialization in state |0>
  kMEASX = 43,
  kMEASY = 44,
  kMEASZ = 45, // measurement in the Z basis
  // Additional Spin Native gates (canonical version)
  // NOTE: no naming conflict with this below, because
  // have indicated the instrinsics with the "Spin" tag
  kCPHASE = 46,
  kRXY = 47,
  kRXZ = 48,
  kRYZ = 49,
  kRYX = 50,
  kRZX = 51,
  kRZY = 52,
  kSWAPA = 53,
  kNumGates = 54, // Number of gates including the identity
  // direct pulse intrinsics
  kStartPulse = 55,
  kDirectPulse = 56,
  kReadPulse = 57,
  kBarrierPulse = 58,
  kSTILL,

  // Aliases
  kSQBegin = kIDLE,
  kSQCliffordBegin = kIDLE,
  kSQCliffordEnd = kT,
  kSQEnd = kAX,
  kI = kIDLE,
  kId = kIDLE,
  kXHS = kHSDAG,
  kXK = kKDAG,
  kSQRTXDAG = kKDAG,
  kSQRTZ = kS,
  kYHS = kZHSDAG,
  kSQRTYDAG = kZH,
  kYK = kZKDAG,
  kSDAG = kZS,
  kSQRTZDAG = kZS,
  kHS = kXHSDAG,
  kSQRTY = kXH,
  kK = kXKDAG,
  kSQRTX = kXKDAG,
  kTQEBegin = kAX,
  kTQEEnd = kSWAP,
  kCNOT = kCX,
  kINIT = kPREPZ,
  kMEAS = kMEASZ,
  k2AxisBegin = kRXY,
  k2AxisEnd = kSWAPA,
  kMaxIdCanonical = kSTILL,
  kNumOneQubitGates = kSQEnd - kSQBegin,
  kNumOneQubitCliffordGates = kSQCliffordEnd - kSQCliffordBegin,
  kNumTwoQubitGates = kTQEEnd - kTQEBegin,

  /// Spin-Native gate set
  kSpinRotZ = kMaxIdCanonical + 1, // 60 as of Oct/12/2022
  kSpinRotXY,
  kSpinCPhase,
  kSpinSwapalp,
  kSpinMeasZ,
  kSpinPrepZ,
  kSpinRotZ_IMM,
  kSpinRotXY_IMM,
  kSpinCPhase_IMM,
  kSpinSwapalp_IMM,
  kSpinMeasZ_IMM,
  kSpinPrepZ_IMM,
  kSpinRotZ_IMM_IDX,
  kSpinRotXY_IMM_IDX,
  kSpinCPhase_IMM_IDX,
  kSpinSwapalp_IMM_IDX,
  kSpinMeasZ_IMM_IDX,
  kSpinBegin = kSpinRotZ,
  kSpinEnd = kSpinPrepZ + 1,
  kSpinIMMBegin = kSpinRotZ_IMM,
  kSpinIMMEnd = kSpinPrepZ_IMM + 1,
  kSpinIMM_IDXBegin = kSpinRotZ_IMM_IDX,
  kSpinIMM_IDXEnd = kSpinMeasZ_IMM_IDX + 1

};

int getInverseSQGateID(int id);

////////////////////////////////////////////////////////////////////////////////
// Function to return the name or intrinsic id associated to the gate identifier

std::string GetNameFromGateIdentifier(int id);
/// Utility function to convert gate identifier to operation name according to
/// config.json
///
/// The names are not the same as those used in here.
/// The operation names for config.json are defined in:
///    TPL::Utilities::populateOpcodesToName();  <--
///    extern/config-json/src/utilities.cpp
/// This path will probably be changed and must be kept up-to-date.
std::string getPlatformNameFromGateIdentifier(int id);

int getInstrinsicIDFromGateIdentifier(int id);

int getSpinNativeVersionForCanonical(int id);
int getCanonicalVersionForSpinNative(int id);
int getBaseVersionForSpinNative(int id);
int getIMMVersionForSpinNative(int id);
int getIMM_IDXVersionForSpinNative(int id);
int getNoIMMVersionForAll(int id);

////////////////////////////////////////////////////////////////////////////////
// Function to return the identifier (if any) corresponding to the matrix in
// argument.
// TODO: verification for RX, RY, RZ.

int DoesMatrixCorrespondToCanonicalGate(std::vector<double> matrix_real,
                                        std::vector<double> matrix_imag);

////////////////////////////////////////////////////////////////////////////////

} // namespace llvm
#endif // for header guard LLVM_QUANTUM_GATEIDENTIFIERS_H
