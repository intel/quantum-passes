#include <clang/Quantum/quintrinsics.h>
#include <clang/Quantum/qexpr.h>
#include <clang/Quantum/qlist.h>

using namespace qexpr;
using namespace qlist;

qbit q[3];

quantum_kernel void PrepAll(){

  PrepZ(q[0]);
  PrepZ(q[1]);
  PrepZ(q[2]);

}

void getU3Angles(double&, double&, double&);

quantum_kernel void U3(qbit &q, double theta, double phi, double gamma){

  double tt = theta, p1 = phi, p2 = gamma;
  getU3Angles(tt, p1, p2);

  RXY(q, tt, p1);
  RXY(q, tt, p2);

}

quantum_kernel void EntangleAll(){
  CNOT(q[0], q[1]);
  CNOT(q[1], q[2]);
}

quantum_kernel void UnEntangleAll(){
  CNOT(q[1], q[2]);
  CNOT(q[0], q[1]);
}

int main() {

double thetas[] = {0.1, 0.2, 0.3};
double phis[] = {0.4, 0.5, 0.6};
double gammas[] = {0.7, 0.8, 0.9};

  eval_hold(
    convert<PrepAll>()
    + convert<EntangleAll>()
    + convert<U3>(q[0], thetas[0], phis[0], gammas[0])
    + convert<U3>(q[1], thetas[1], phis[1], gammas[1])
    + convert<U3>(q[2], thetas[2], phis[2], gammas[2])
    + convert<UnEntangleAll>()         
    );

  return 0;

}