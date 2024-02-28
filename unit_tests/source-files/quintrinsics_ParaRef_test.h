//===--- quintrinsics.h - Quantum extensions---------------------*- C++ -*-===//
//
//===----------------------------------------------------------------------===//
//
// Copyright (C) 2021 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Quantum-specific intrinsics and operations
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_CLANG_QUANTUM_QUINTRINSICS_H
#define LLVM_CLANG_QUANTUM_QUINTRINSICS_H

#define quantum_kernel                                                         \
  __attribute__((quantum_kernel)) __attribute__((section(".qbbs_text")))

#define quantum_shared_double_array double

///////////////////////////////////////////////////////////////////////////////
/// Generic gate definitions
///////////////////////////////////////////////////////////////////////////////

using qbit = unsigned short int;
using cbit = bool;

void __quantum_qubit(qbit *q);
void __quantum_param(double d);

/*
 * Arbitrary rotation around X-axis (RX Gate)
*/
// TODO: Ongoing discussion on representing parametric values, to be updated.
// Until then, treat matrix as string.
void RXGate(qbit &q, double angle) __attribute__((annotate("{ \
    \"matrix_real\" : \"[cos(angle/2), 0, 0, cos(angle/2)]\", \
    \"matrix_imag\" : \"[0, -sin(angle/2), sin(angle/2), 0]\", \
    \"matrix_order\" : \"rm\", \
    \"is_hermitian\" : false, \
    \"is_unitary\" : true, \
    \"is_mutable\" : true, \
    \"qubit_list\" : [0], \
    \"parametric_list\" : [1], \
    \"control_qubit_list\" : [], \
    \"local_basis_list\" : [1], \
    \"identifier\" : 9 \
  }"))) {
  __quantum_qubit(&q);
  __quantum_param(angle);
}

///////////////////////////////////////////////////////////////////////////////

#endif // LLVM_CLANG_QUANTUM_QUINTRINSICS_H
