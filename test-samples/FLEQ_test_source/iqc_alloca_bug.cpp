#include <clang/Quantum/quintrinsics.h>
#include <clang/Quantum/qexpr.h>

QExpr op(qbit &q) {
  return qexpr::_X(q);
}
QExpr opOnN(datalist::DataList N) {
  qbit *qs_raw = datalist::IQC_alloca<qbit>("", N.to_int());
  qlist::QList qs (qs_raw);
  return op(qs[0]);
}

QExpr ctl_opOnN(datalist::DataList N) {
  qbit *qs_raw = datalist::IQC_alloca<qbit>("", N.to_int());
  qlist::QList qs (qs_raw);
  qbit tar;
  return qexpr::control(qs[1], op(tar));
}

QExpr mctl_opOnN(datalist::DataList N) {
  qbit *qs_raw = datalist::IQC_alloca<qbit>("", N.to_int());
  qlist::QList qs (qs_raw);
  qbit tar;
  return qexpr::control(qs, op(tar));
}

int main() {
  qexpr::eval_hold(qexpr::printQuantumLogic(opOnN("2")));
  qexpr::eval_hold(qexpr::printQuantumLogic(ctl_opOnN("2")));
  qexpr::eval_hold(qexpr::printQuantumLogic(mctl_opOnN("2")));
}
