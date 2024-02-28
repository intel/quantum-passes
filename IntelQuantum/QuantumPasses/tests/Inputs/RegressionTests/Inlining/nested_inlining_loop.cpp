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

  for(int i = 0; i < N; i++){
    RZZ(qs[i], qs[(i+1)%N], a);
  }

}

quantum_kernel void MeasureH(qbit qs[N], double &E){

  cbit c[N];

  for(int i = 0; i < N; i++){
    MeasX(qs[i], c[i]);
  }

  E = (double)c[0] + (double)c[1] +(double)c[2];

}

quantum_kernel void compute(double coef, double &E){
  
  for(int i = 0; i < N; i++){
    PrepX(q[i]);
  }

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



