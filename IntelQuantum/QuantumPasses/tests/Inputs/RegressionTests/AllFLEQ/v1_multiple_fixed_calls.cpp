#include <clang/Quantum/quintrinsics.h>
#include <clang/Quantum/qexpr.h>

using namespace qexpr;

qbit q[3];
qbit check;

PROTECT QExpr PrepAll(){

  PrepZ(q[0]);
  PrepZ(q[1]);
  PrepZ(q[2]);

  PrepX(check);

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

PROTECT QExpr CheckBit(qbit &q, qbit &check_qbit, bool &bit){
  cbit temp;
  CNOT(check_qbit, q);
  MeasZ(q, temp);
  bit = temp;
  return this_as_expr;
}

PROTECT QExpr MeasReg(qbit qs[], cbit cs[]){
  MeasZ(qs[0], cs[0]);
  MeasZ(qs[1], cs[1]);
  MeasZ(qs[2], cs[2]);

  return this_as_expr;
}

PROTECT QExpr FixState(){
  RX(q[2], 1.1);
  CNOT(q[1], q[2]);
  CNOT(q[0], q[1]);

  return this_as_expr;
}

int main() {

  double thetas[] = {0.1, 0.2, 0.3};
  double phis[] = {0.4, 0.5, 0.6};
  double gammas[] = {0.7, 0.8, 0.9};
  bool cont = true;
  cbit outcome[3];

  do{

    eval_hold(
      CheckBit(q[2], check, cont)
      * U3(q[2], thetas[2], phis[2], gammas[2])
      * freeCNOT(q[1], q[2])
      * U3(q[1], thetas[1], phis[1], gammas[1])
      * freeCNOT(q[0], q[1])
      * U3(q[0], thetas[0], phis[0], gammas[0])
      * PrepAll() 
    );

    if(cont){
      eval_hold(FixState());
    }

  } while(cont);

  eval_release(
    MeasReg(q, outcome)
    * freeCNOT(q[2], check)
    );
   
  return 0;

}