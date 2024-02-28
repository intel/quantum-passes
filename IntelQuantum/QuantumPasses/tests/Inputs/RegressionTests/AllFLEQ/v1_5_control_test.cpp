#include <clang/Quantum/quintrinsics.h>
#include <clang/Quantum/qexpr.h>
#include <clang/Quantum/qlist.h>

using namespace qexpr;
using namespace qlist;

const int N = 4;
qbit q[N];
qbit listable(ctl, 3);


double getAngle();
bool getBool();
unsigned getInt();

int main()
{
    double ang = getAngle();
    bool b = getBool();
    unsigned ui = getInt();

    eval_hold(control(ctl[0], true, _RX(q[0], ang)));
    eval_hold(control(ctl[0], false, _RX(q[0], ang)));
    eval_hold(control(ctl[0], b, _RX(q[0], ang)));

    eval_hold(control(ctl, 5, _RX(q[0], ang)));
    eval_hold(control(ctl, -1, _RX(q[0], ang)));
    eval_hold(control(ctl, ui, _RX(q[0], ang)));

    return 0;
}
