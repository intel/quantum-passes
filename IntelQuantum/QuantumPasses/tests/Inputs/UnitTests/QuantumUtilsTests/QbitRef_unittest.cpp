#include <clang/Quantum/quintrinsics.h>

qbit q_glob1;
qbit q_glob2[2];

void CtorSetterTest( qbit q0, qbit q1[2], unsigned idx_in) {

  qbit q3;
  qbit q4[2];

}

void ResolutionTest( qbit q0, qbit q1[2], unsigned idx_in) {

  qbit q3;
  qbit q4[2];

  int idx = 0;

  //directly resolved
  H(q0);
  H(q1[0]);
  H(q1[1]);
  H(q3);
  H(q4[0]);
  H(q4[1]);

  //resolved through const 
  H(q1[idx]);
  H(q4[idx]);
  
  //resolved through foldable const
  idx = idx+1;
  H(q1[idx]);
  H(q4[idx]);

  //unresolvable through argument
  H(q1[idx_in]);
  H(q4[idx_in]);
  
}

void EqualityTest( qbit q0, qbit q1[2], unsigned idx_in, unsigned idx_in1[2] ){

  qbit q3;
  qbit q4[2];

  //direct equality and inequality
  H(q0);
  H(q0);
  H(q1[0]);
  H(q1[0]);
  H(q1[1]);
  H(q1[1]);
  H(q3);
  H(q3);
  H(q4[0]);
  H(q4[0]);
  H(q4[1]);
  H(q4[1]);

  //Equality/ through unresolved argument
  H(q1[idx_in]);
  H(q1[idx_in]);
  H(q4[idx_in]);
  H(q4[idx_in]);
  H(q1[idx_in1[0]]);
  H(q1[idx_in1[0]]);
  H(q4[idx_in1[1]]);
  H(q4[idx_in1[1]]);

  //Inequality through unresolved argument
  H(q1[idx_in]);
  idx_in++;
  H(q1[idx_in]);
  H(q4[idx_in]);
  idx_in++;
  H(q4[idx_in]);
  H(q1[idx_in1[0]]);
  idx_in1[0]++;
  H(q1[idx_in1[0]]);
  H(q4[idx_in1[1]]);
  idx_in1[1]++;
  H(q4[idx_in1[1]]);


  //Unresolvable equality through sum of arguments
  H(q1[idx_in + 1]);
  H(q1[idx_in + 1]);
  H(q4[idx_in + 1]);
  H(q4[idx_in + 1]);
  H(q1[idx_in1[0] + 1]);
  H(q1[idx_in1[0] + 1]);
  H(q4[idx_in1[1] + 1]);
  H(q4[idx_in1[1] + 1]);
  H(q1[idx_in + idx_in1[0]]);
  H(q1[idx_in + idx_in1[0]]);
  H(q4[idx_in + idx_in1[1]]);
  H(q4[idx_in + idx_in1[1]]);

}

void GlobalTest(int idx1, int idx2) {

  H(q_glob1);
  H(q_glob1);
  H(q_glob2[0]);
  H(q_glob2[0]);
  H(q_glob2[1]);
  H(q_glob2[1]);
  H(q_glob2[idx1]);
  H(q_glob2[idx1]);
  H(q_glob2[idx1 +1]);
  H(q_glob2[idx1 +1]);
  H(q_glob2[idx1 +idx2]);
  H(q_glob2[idx1 +idx2]);

}

