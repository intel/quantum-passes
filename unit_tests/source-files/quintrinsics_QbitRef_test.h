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
  __attribute__((section(".qbbs_text"))) __attribute__((noinline))

using qbit = unsigned short int;

void __quantum_qubit(qbit *q);

///////////////////////////////////////////////////////////////////////////////
/// Generic gate definitions
///////////////////////////////////////////////////////////////////////////////

/*
 * Hadamard (H) gate
 */
// ASKME: Using the function 'sqrt()' requires to make this matrix a string, can
// we directly use the value '0.70710678'?
__attribute__((noinline)) void HGate(qbit &q) __attribute__((annotate("{ \
    \"matrix_real\" : \"1/sqrt(2)*[1, 1, 1, -1]\", \
    \"matrix_imag\" : [0, 0, 0, 0], \
    \"matrix_order\" : \"rm\", \
    \"is_hermitian\" : true, \
    \"is_unitary\" : true, \
    \"is_mutable\" : true, \
    \"qubit_list\" : [0], \
    \"parametric_list\" : [], \
    \"control_qubit_list\" : [], \
    \"local_basis_list\" : [4], \
    \"identifier\" : 1 \
  }"))) {
  __quantum_qubit(&q);
}

///////////////////////////////////////////////////////////////////////////////

#endif // LLVM_CLANG_QUANTUM_QUINTRINSICS_H
