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

QExpr gate_qk(){
  H(q[0]);
  CZ(q[0], q[1]);
  RY(q[1], 0.1);
  CNOT(q[0], q[2]);
  Toffoli(q[0], q[1], q[2]);
  Z(q[2]);
  CNOT(q[1], q[2]);
  H(q[1]);

  return this_as_expr;

}

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
