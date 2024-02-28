#include <clang/Quantum/quintrinsics.h>
#include <clang/Quantum/qexpr.h>

using namespace qexpr;

const int N = 4;
qbit q[N];

int main()
{
    cbit c;
    eval_hold(-_MeasZ(q[0], c));

    return 0;
}
