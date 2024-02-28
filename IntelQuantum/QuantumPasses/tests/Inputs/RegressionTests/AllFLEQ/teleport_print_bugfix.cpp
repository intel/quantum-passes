#include <clang/Quantum/quintrinsics.h>
#include <clang/Quantum/qexpr.h>

#include <iostream>
#include <cassert>
#include <vector>
#include <random>

//This bug was caused by the printing in 'bob' to cause 
//the 'teleport_bind' function to to be invoked instead
//of called in the main function. Because the 
//InsertQuantumAttributes was not handling InvokeInst
//The function attributes as a qexpr-returning function
//was not getting set and the call was treated as a bad node 

// Prepare a Bell00 state |00> + |11>.
QExpr bell00(qbit& a, qbit& b) {
    return qexpr::_PrepZ(a)
         + qexpr::_PrepZ(b)
         + qexpr::_H(a)
         + qexpr::_CNOT(a,b);
}

// Entangle q and a and measure both, writing the results to x and y
// respectively.
QExpr alice(qbit& q, qbit& a, bool& x, bool& y) {
    return qexpr::_CNOT(q, a)
         + qexpr::_H(q)
         + qexpr::_MeasZ(q, x)
         + qexpr::_MeasZ(a, y);
}

// Use x and y to apply corrections to the qubit b.
PROTECT QExpr bob(qbit& b, bool &x, bool &y) {
    std::cout << "Got bools " << x << " and " << y << "\n";
    return qexpr::cIf(y, qexpr::_X(b), qexpr::identity())
         + qexpr::cIf(x, qexpr::_Z(b), qexpr::identity());
}

PROTECT QExpr teleport1_bind(qbit& q, qbit& a, qbit& b) {
    bool x = false;
    bool y = false;
    return (bell00(a,b)
            + alice(q, a, x, y))
            << bob(b,x,y);
}



int main() {

    std::string dummy = "This is a string";

    qbit q;
    qbit a;
    qbit b;

    qexpr::eval_hold(qexpr::printQuantumLogic(qexpr::_PrepZ(q)
                    + teleport1_bind(q,a,b)));

    return 0;
}