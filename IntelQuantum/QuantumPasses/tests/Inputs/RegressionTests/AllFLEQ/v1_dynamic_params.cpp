#include <clang/Quantum/quintrinsics.h>
#include <clang/Quantum/qexpr.h>

using namespace qexpr;

qbit q[5];

PROTECT QExpr PrepAll(){

  PrepX(q[0]);
  PrepX(q[1]);
  PrepX(q[2]);
  PrepX(q[3]);
  PrepX(q[4]);

  return this_as_expr;

}

void getU3Angles(double&, double&, double&);

PROTECT QExpr U3(qbit &q, double theta, double phi, double gamma){

  double tt = theta, p1 = phi, p2 = gamma;
  getU3Angles(tt, p1, p2);

  RXY(q, tt, p1);
  RXY(q, tt, p2);

  return this_as_expr;

}

PROTECT QExpr freeCNOT(qbit &q1, qbit &q2){
  CNOT(q1, q2);
  return this_as_expr;
}


int main() {

double thetas[] = {0.1, 0.2, 0.3};
double phis[] = {0.4, 0.5, 0.6};
double gammas[] = {0.7, 0.8, 0.9};

  eval_hold(
    U3(q[2], thetas[2], phis[2], gammas[2])
    * freeCNOT(q[1], q[2])
    * U3(q[1], thetas[1], phis[1], gammas[1])
    * freeCNOT(q[0], q[1])
    * U3(q[0], thetas[0], phis[0], gammas[0])
    * PrepAll() 
  );

  return 0;

}