#include <clang/Quantum/quintrinsics.h>
#include <clang/Quantum/qexpr.h>
using namespace qexpr;

const int N = 4;
qbit q[N];
qbit ctl[2];

int main()
{
    cbit c;
    eval_hold(control(ctl[0], true, _MeasZ(q[0], c)));

    return 0;
}
