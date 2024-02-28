#include <clang/Quantum/quintrinsics.h>
#include <clang/Quantum/qexpr.h>

using namespace qexpr;

qbit reg[5];

QExpr PrepAll(qbit q[]){

  PrepX(q[0]);
  PrepX(q[1]);
  PrepX(q[2]);
  PrepX(q[3]);
  PrepX(q[4]);

  return this_as_expr;

}

QExpr ZZRot(qbit &q1, qbit &q2, double ang){
  CNOT(q1, q2);
  RZ(q2, ang);
  CNOT(q1, q2);

  return this_as_expr;

}

QExpr measH(qbit q[], double coeff[], double& E){

  cbit c[5];

  MeasX(q[0], c[0]);
  MeasX(q[1], c[1]);
  MeasX(q[2], c[2]);
  MeasX(q[3], c[3]);
  MeasX(q[4], c[4]);

  E = (1-2*(double)c[0]) * coeff[0]
    + (1-2*(double)c[1]) * coeff[1]
    + (1-2*(double)c[2]) * coeff[2]
    + (1-2*(double)c[3]) * coeff[3]
    + (1-2*(double)c[4]) * coeff[4];

  return this_as_expr;

}

int main() {

  double params[] = {-0.1, -0.2, -0.3, -0.4, -0.5};
  double e_coef[] = {0.1, 0.2, 0.3, 0.4, 0.5};
  double energy = 0;

  eval_release(
    measH(reg, e_coef, energy)
    * ZZRot(reg[0], reg[1], params[0])
    * ZZRot(reg[1], reg[2], params[1])
    * ZZRot(reg[2], reg[3], params[2])
    * ZZRot(reg[3], reg[4], params[3])
    * ZZRot(reg[4], reg[0], params[4])
    * PrepAll(reg) 
  );

  return 0;

}