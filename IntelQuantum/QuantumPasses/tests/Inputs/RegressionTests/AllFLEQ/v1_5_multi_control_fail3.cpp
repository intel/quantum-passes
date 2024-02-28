#include <clang/Quantum/quintrinsics.h>
#include <clang/Quantum/qexpr.h>

using namespace qexpr;

const int N = 4;
qbit q[N];
qbit ctl[2];

int main()
{

    eval_hold(control(q, 0, _H(q[0])));

    return 0;
}
