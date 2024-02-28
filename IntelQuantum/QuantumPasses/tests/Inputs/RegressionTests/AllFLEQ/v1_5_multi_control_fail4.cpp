#include <clang/Quantum/quintrinsics.h>
#include <clang/Quantum/qexpr.h>

using namespace qexpr;

const int N = 4;
qbit q[N];
qbit ctl[2];

int main()
{

    eval_hold(control(q, 0, _RX(q[0], 0.1)));

    return 0;
}
