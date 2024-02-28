//------------------------------------------------------------------------------
//
// Copyright (C) 2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//
//===----------------------------------------------------------------------===//

/******************************************************************************
# This is a (mock) linkable quantum runtime library that contains all of the functions
# necessary for a quantum application program to:
# - initialize a Tall Pine Lake architecture derived system stack,
# - schedule quantum kernels to be run on a quantum device by its 
# - index into the qbbs table, and
# - return any results of quantum measurements in a register or memory region
# - when requested.
*/

#ifndef _QUANTUM_RUNTIME_LIBRARY
#define _QUANTUM_RUNTIME_LIBRARY

//#include <string>
//#include <qrt0_errors.hpp>

#include <stdint.h>

enum QRT0_ERROR_T {\
    QRT0_ERROR_SUCCESS = 0,
    QRT0_ERROR_WARNING,
    QRT0_ERROR_FAIL
};

namespace _qrt0
{
    bool _is_initialized();
    void update_measurement_map(bool* c_bit_pointer, unsigned long classical_bit_register_id, bool flip);
    QRT0_ERROR_T _initialize_qrt_system64(double *quantum_params_addr, char *sdk_config);
    QRT0_ERROR_T schedule_kernel(void(*QBB)(), unsigned length);
    QRT0_ERROR_T _dismantle_qrt_system();

};

bool _aqcc_dummy_bit;
double _aqcc_dummy_para[1];
char _aqcc_dummy_config[] = "a";
void _aqcc_dummy_QBB() {return;}


void _aqcc_dummy_foo(){
  _qrt0::_initialize_qrt_system64(_aqcc_dummy_para, _aqcc_dummy_config);
  _qrt0::_is_initialized();
  _qrt0::update_measurement_map(&_aqcc_dummy_bit, 0, false);
  _qrt0::schedule_kernel(&_aqcc_dummy_QBB, 0);
  _qrt0::_dismantle_qrt_system();
}

#endif