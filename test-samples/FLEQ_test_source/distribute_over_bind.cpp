#include <clang/Quantum/quintrinsics.h>
#include <clang/Quantum/qexpr.h>
#include <clang/Quantum/qlist.h>

using namespace qexpr;
using namespace qlist;

qbit listable(q, 3);
qbit listable(tar, 2);
qbit listable(ctl, 1);

QExpr control_over_bind(){
  return control(ctl, _X(q[0]) << _X(q[1]));
}

QExpr m_control_over_bind(){
  return control(q, _X(tar[0]) << _X(tar[1]));
}

QExpr power_over_bind(){
  return power(5, _RX(q[0], 1) << _RX(q[1], 1));
}

int main(){
    eval_hold(printQuantumLogic(control_over_bind()));
    eval_hold(printQuantumLogic(m_control_over_bind()));
    eval_hold(printQuantumLogic(power_over_bind()));
}