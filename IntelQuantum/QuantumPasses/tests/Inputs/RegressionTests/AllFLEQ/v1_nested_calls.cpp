#include <clang/Quantum/quintrinsics.h>
#include <clang/Quantum/qexpr.h>

using namespace qexpr;

qbit q[5];

PROTECT QExpr PrepOn5(){

  PrepZ(q[0]);
  PrepZ(q[1]);
  PrepZ(q[2]);
  PrepZ(q[3]);
  PrepZ(q[4]);

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

PROTECT QExpr U3On5(double theta, double phi, double gamma){

  return 
        U3(q[4], theta, phi, gamma)
        * U3(q[3], theta, phi, gamma)
        * U3(q[2], theta, phi, gamma)
        * U3(q[1], theta, phi, gamma)
        * U3(q[0], theta, phi, gamma);
}


PROTECT QExpr measHOn5(double coeff[], double& E, QExpr prep ){

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

  return join(prep, this_as_expr);

}

QExpr computeH(double theta, double phi, double gamma, double coeff[], double& E){

  return measHOn5(coeff, E, U3On5(theta, phi, gamma) * PrepOn5() );
    
}

int main() {

  double theta = 0.1;
  double phi = 0.2;
  double gamma = 0.3;
  double coeff[] = {0.4, 0.5, 0.6, 0.7, 0.8};
  double E;

  eval_release(computeH(theta, phi, gamma, coeff, E));

  return 0;

}