#include <clang/Quantum/quintrinsics.h>
#include <clang/Quantum/qexpr.h>

using namespace qexpr;
using namespace qlist;

const int N = 10;
qbit listable(qs, N);
qbit q0;

//This was a bug where the user passed a qbit by value
//instead of by reference. FLEQ compilation was not 
//catching this and the qbit argument was being 
//copied to a local allocation and thus interpreted
//as a local qubit. After loop unrolling, the number
// of qubits required was beyond the 11 asked for in
// the source. The pass by value should now be caught.

QExpr num_qubits_qexpr(QList reg, qbit q) {
    return cIf(size(reg) > 0
        , _CNOT(q,reg[0]) + num_qubits_qexpr(reg+1,q)
        , identity()
    );
}

int main() {
    eval_hold(num_qubits_qexpr(qs, q0));

    return 0;
}