#include <clang/Quantum/quintrinsics.h>
#include <clang/Quantum/qexpr.h>
#include <quantum_full_state_simulator_backend.h>

#include <cassert>

using namespace qexpr;
using namespace qlist;


int main() {

    iqsdk::IqsConfig iqs_config;
    iqsdk::FullStateSimulator iqs_device(iqs_config);
    iqsdk::QRT_ERROR_T status = iqs_device.ready();
    assert(status == iqsdk::QRT_ERROR_SUCCESS);


    qbit a;
    qbit b;

    std::vector<std::reference_wrapper<qbit>> qs_refs;
    qs_refs.push_back(a);
    qs_refs.push_back(b);

    eval_hold(_PrepZ(a) + _PrepX(b));
    eval_hold(_X(a));
    eval_hold(_Y(b));

    auto prob_map = iqs_device.getProbabilities(qs_refs, {}, 0.01);
    iqsdk::FullStateSimulator::displayProbabilities(prob_map);

    return 0;
}