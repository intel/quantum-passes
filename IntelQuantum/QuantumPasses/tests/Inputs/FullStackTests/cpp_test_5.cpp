#include <clang/Quantum/quintrinsics.h>
#include <quantum.hpp>
#include <iostream>
#include <cassert>
#include <algorithm>

// Allocate 2 global qubits.
const int N = 2;
qbit qumem[N];


quantum_kernel void Prep_and_Meas(qbit q[], cbit c[])
{
    PrepZ(q[1]);
    PrepZ(q[0]);
    X(q[1]);
    MeasZ(q[1], c[0]);
    MeasZ(q[0], c[1]);
}

quantum_kernel void run_kernel(cbit c[])
{
    Prep_and_Meas(qumem, c);
}

int main()
{
    cbit cmem[N];
    iqsdk::Iqs_Config iqs_config;
    iqsdk::Full_State_Simulator iqs_device(iqs_config);
    iqsdk::QRT_ERROR_T status = iqs_device.ready();
    assert(status == iqsdk::QRT_ERROR_SUCCESS);
    run_kernel(cmem);
    for (size_t i = 0; i < N; ++i)
    {
        switch(cmem[i])
        {
	    case true:
	          std::cout << "cbit " << i << " is true" << std::endl;
	          break;
	    case false: 
	          std::cout << "cbit " << i << " is false" << std::endl;
	          break;
	    default:
	          std::cout << "cbit " << i << " is neither true or false" << std::endl;
	          break;
        }
    }

    return 0;
    
}
