#include <clang/Quantum/quintrinsics.h>
#include <clang/Quantum/qexpr.h>

using namespace qexpr;

const int N = 4;
qbit q[N];

double getAngle();

QExpr qexpr_conjugate(QExpr unitary, QExpr body){
    return unitary * body * ~unitary;
}

QExpr ZZgate(qbit &q1, qbit &q2, double ang){
  return qexpr_conjugate(_CNOT(q1, q2), _RZ(q2, ang));
}

PROTECT QExpr gate_qk(){
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

    eval_hold(~_RX(q[0], ang));

    eval_hold(~ZZgate(q[0], q[1], ang));
    
    eval_hold(~gate_qk());

    return 0;
}
