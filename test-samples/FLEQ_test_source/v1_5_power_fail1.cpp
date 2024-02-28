
#include <clang/Quantum/quintrinsics.h>
#include <clang/Quantum/qexpr.h>

using namespace qexpr;

const int N = 4;
qbit q[N];

int main()
{

    eval_hold(power(-5, _PrepZ(q[0])));

    return 0;
}
