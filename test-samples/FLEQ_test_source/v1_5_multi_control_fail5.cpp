#include <clang/Quantum/quintrinsics.h>
#include <clang/Quantum/qexpr.h>

using namespace qexpr;

const int N = 4;
qbit q[N];
qbit ctl[10];

int main()
{

    eval_hold(control(ctl, 0, _RX(q[0], 0.1)));

    return 0;
}
