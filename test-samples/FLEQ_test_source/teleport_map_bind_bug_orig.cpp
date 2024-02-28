#include <clang/Quantum/quintrinsics.h>
#include <quantum_full_state_simulator_backend.h>
#include <clang/Quantum/qexpr.h>

#include <iostream>
#include <cassert>
#include <vector>
#include <random>

template<typename QExprFun, typename... Args>
PROTECT QExpr map(QExprFun f, qlist::QList qs, Args... args) noexcept {
    return qexpr::cIfTrue(qs.size() > 0,
      f( qs[0], args[0]...)
      + map(f, qs + 1, (args + 1)...)
    );
}

///////////////////////
/// Building Blocks ///
///////////////////////

// Prepare a Bell00 state |00> + |11>.
PROTECT QExpr bell00(qbit& a, qbit& b) {
    return qexpr::_PrepZ(a)
         + qexpr::_PrepZ(b)
         + qexpr::_H(a)
         + qexpr::_CNOT(a,b);
}

// Entangle q and a and measure both, writing the results to x and y
// respectively.
PROTECT QExpr alice(qbit& q, qbit& a, bool& x, bool& y) {
    return qexpr::_CNOT(q, a)
         + qexpr::_H(q)
         + qexpr::_MeasZ(q, x)
         + qexpr::_MeasZ(a, y);
}

// Use x and y to apply corrections to the qubit b.
PROTECT QExpr bob(qbit& b, bool &x, bool &y) {
    return qexpr::cIf(y, qexpr::_X(b), qexpr::identity())
         + qexpr::cIf(x, qexpr::_Z(b), qexpr::identity());
}


// Prepare a GHZ state
PROTECT QExpr ghz(qlist::QList qs){
    int len = qs.size();
    return (map(qexpr::_PrepZ, qs)
          + qexpr::_H(qs[0])
          + map(qexpr::_CNOT, qs(0,len-1), qs(1,len)));
}

/////////////////
/// Main Body ///
/////////////////

int main() {
    iqsdk::IqsConfig iqs_config;
    iqsdk::FullStateSimulator iqs_device(iqs_config);
    iqsdk::QRT_ERROR_T status = iqs_device.ready();
    assert(status == iqsdk::QRT_ERROR_SUCCESS);


    const int N = 2;
    qbit listable(qs, N);
    qbit listable(as, N);
    qbit listable(bs, N);

    bool xs[N] = {false, false};
    bool ys[N] = {false, false};

    qexpr::eval_hold((ghz(qs) + map(bell00, as, bs))
                     << (map(alice, qs, as, xs, ys))
                     << map(bob, bs, xs, ys));

    // Separating these into separate evaluation calls produces the correct
    // answer.
    //qexpr::eval_hold(map(bell00, as, bs));
    //qexpr::eval_hold(ghz(qs) + map(alice, qs, as, xs, ys));
    //qexpr::eval_hold(map(bob, bs, xs, ys));


    // At the end, bs should be in the state |phi>=1/sqrt(2)(|0...0> + |1...1>)
    // (up to a global phase)
    auto outputRefs = to_ref_wrappers(bs);
    auto probsAfter = iqs_device.getProbabilities(outputRefs, {}, 0.01);

    std::cout << "Expecting GHZ state |0...0> + |1...1>\n";
    std::cout << "Qubits bs after teleportation:\n";
    iqsdk::FullStateSimulator::displayProbabilities(probsAfter);

    return 0;
}
