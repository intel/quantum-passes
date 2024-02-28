#include <clang/Quantum/quintrinsics.h>
#include <clang/Quantum/qexpr.h>

#include <math.h>

using namespace qexpr;
using namespace qlist;

const int N = 2;
qbit q[N];

QExpr rotation(qbit &ctrl, qbit &tgt, double m) {
    double angle = M_PI / std::pow(2.0, m-1);
    return _CPhase(ctrl, tgt, angle);
}


QExpr rotations(qbit &q, QList qs) {
    return cIf(size(qs)==0
                    , identity()
                    , rotations(q, qs+1) * rotation(q, qs[0], size(qs)));
}

QExpr qft_helper(QList qs) {
    return cIf( size(qs)==0
                     , identity()
                     , cIf( size(qs)==1
                                 , _H(qs[0])
                                 , _H(qs[0]) * rotations(qs[0], qs+1) * qft_helper(qs+1)
                                 )
                     );
}

QExpr swap_qubits(QList qs) {
    int n = size(qs);
    return cIf(
        n <= 1
      , identity()
      , swap_qubits(slice(qs, 1, n-1)) * _SWAP(qs[0], qs[n-1])
    );
}

QExpr qft(QList qs) {
    return swap_qubits(qs) * qft_helper(qs);
}


int main() {
    eval_hold(printQuantumLogic(qft(q)));
    return 0;
}