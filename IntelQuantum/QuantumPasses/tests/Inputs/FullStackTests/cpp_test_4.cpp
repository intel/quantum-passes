#include <clang/Quantum/quintrinsics.h>
#include <quantum.hpp>
#include <iostream>
#include <cassert>
#include <algorithm>

// Allocate 2 global qubits.
const int N = 2;
qbit qumem[N];
cbit c[N];


quantum_kernel cbit Prep_and_Meas()
{
    PrepZ(qumem[1]);
    PrepZ(qumem[0]);
    X(qumem[1]);
    MeasZ(qumem[1], c[0]);
    MeasZ(qumem[0], c[1]);
    return c[0];
}

int main()
{
   iqsdk::Iqs_Config iqs_config;
   iqsdk::Full_State_Simulator iqs_device(iqs_config);
   iqsdk::QRT_ERROR_T status = iqs_device.ready();
   assert(status == iqsdk::QRT_ERROR_SUCCESS);
    cbit c = Prep_and_Meas();
    std::cout << "cbit is " << (bool) c << std::endl;

    return 0;
    
}
