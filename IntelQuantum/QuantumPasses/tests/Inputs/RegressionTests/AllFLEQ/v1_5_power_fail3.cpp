#include <clang/Quantum/quintrinsics.h>
#include <clang/Quantum/qexpr.h>

using namespace qexpr;

const int N = 4;
qbit q[N];

int getInt();

int main()
{

    eval_hold(power(getInt(), _H(q[0])));

    return 0;
}
