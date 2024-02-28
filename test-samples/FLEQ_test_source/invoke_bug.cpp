#include <clang/Quantum/quintrinsics.h>
#include <clang/Quantum/qexpr.h>

#include <quantum_full_state_simulator_backend.h>
#include <iostream>
#include <cassert>
#include <vector>




const int N = 4;
qbit q[N];


int main() {

     iqsdk::IqsConfig iqs_config;
     iqsdk::FullStateSimulator iqs_device(iqs_config);
     iqsdk::QRT_ERROR_T status = iqs_device.ready();
     assert(status == iqsdk::QRT_ERROR_SUCCESS);

    qexpr::eval_hold(qexpr::_H(q[0]));

    return 0;
}