#include <clang/Quantum/quintrinsics.h>
#include <clang/Quantum/qexpr.h>

//This bug was a logical failure for the outcome. Part of the quantum logic 
//was not being built because the recycling of memory for a llvm::Instruction
//for an InvokeInst caused a new created Instrucition (a call to 'alice') in 
//the same memory location to be labelled as "dead". The solution was to not 
//delete any Instruction that has a node in the current FLEQGraph.


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

    const int N = 2;
    qbit listable(qs, N);
    qbit listable(as, N);
    qbit listable(bs, N);

    bool xs[N] = {false, false};
    bool ys[N] = {false, false};

    qexpr::eval_hold((ghz(qs) + map(bell00, as, bs))
                     << qexpr::printQuantumLogic(map(alice, qs, as, xs, ys))
                     << map(bob, bs, xs, ys));

    return 0;
}