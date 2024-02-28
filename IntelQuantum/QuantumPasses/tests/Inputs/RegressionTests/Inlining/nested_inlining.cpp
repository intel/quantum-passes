#include <clang/Quantum/quintrinsics.h>

const int N =3;
qbit q[N];

bool min_classical(double &, double &);

quantum_kernel void RZZ(qbit &q1, qbit &q2, double a){

  CNOT(q1, q2);
  RZ(q2, a);
  CNOT(q1, q2);

}

quantum_kernel void Ising(qbit qs[N], double a){

  RZZ(qs[0], qs[1], a);
  RZZ(qs[1], qs[2], a);
  RZZ(qs[2], qs[0], a);

}

quantum_kernel void MeasureH(qbit qs[N], double &E){

  cbit c[N];

  MeasX(qs[0], c[0]);
  MeasX(qs[1], c[1]);
  MeasX(qs[2], c[2]);

  E = (double)c[0] + (double)c[1] +(double)c[2];

}

quantum_kernel void compute(double coef, double &E){
  PrepX(q[0]);
  PrepX(q[0]);
  PrepX(q[0]);

  Ising(q, coef);
  MeasureH(q, E);

}

double minimize(){
  
  double E = 0;
  double coef = 0.1;

  while(min_classical(coef, E)){
    compute(coef, E);
  }

  return E;

}

int main(){
  double min = minimize();
  return 0;
}



