#include "quintrinsics_QbitRef_test.h"

qbit q_glob1;
qbit q_glob2[2];

void quantum_kernel CtorSetterTest( qbit &q0, qbit q1[2], unsigned idx_in) {

  qbit q3;
  qbit q4[2];

}

void quantum_kernel ResolutionTest( qbit &q0, qbit q1[2], unsigned idx_in) {

  qbit q3;
  qbit q4[2];

  int idx = 0;

  //directly resolved
  HGate(q0);
  HGate(q1[0]);
  HGate(q1[1]);
  HGate(q3);
  HGate(q4[0]);
  HGate(q4[1]);

  //resolved through const 
  HGate(q1[idx]);
  HGate(q4[idx]);
  
  //resolved through foldable const
  idx = idx+1;
  HGate(q1[idx]);
  HGate(q4[idx]);

  //unresolvable through argument
  HGate(q1[idx_in]);
  HGate(q4[idx_in]);
  
}

void quantum_kernel EqualityTest( qbit &q0, qbit q1[2], unsigned idx_in, unsigned idx_in1[2] ){

  qbit q3;
  qbit q4[2];

  //direct equality and inequality
  HGate(q0);
  HGate(q0);
  HGate(q1[0]);
  HGate(q1[0]);
  HGate(q1[1]);
  HGate(q1[1]);
  HGate(q3);
  HGate(q3);
  HGate(q4[0]);
  HGate(q4[0]);
  HGate(q4[1]);
  HGate(q4[1]);

  //Equality/ through unresolved argument
  HGate(q1[idx_in]);
  HGate(q1[idx_in]);
  HGate(q4[idx_in]);
  HGate(q4[idx_in]);
  HGate(q1[idx_in1[0]]);
  HGate(q1[idx_in1[0]]);
  HGate(q4[idx_in1[1]]);
  HGate(q4[idx_in1[1]]);

  //Inequality through unresolved argument
  HGate(q1[idx_in]);
  idx_in++;
  HGate(q1[idx_in]);
  HGate(q4[idx_in]);
  idx_in++;
  HGate(q4[idx_in]);
  HGate(q1[idx_in1[0]]);
  idx_in1[0]++;
  HGate(q1[idx_in1[0]]);
  HGate(q4[idx_in1[1]]);
  idx_in1[1]++;
  HGate(q4[idx_in1[1]]);


  //Unresolvable equality through sum of arguments
  HGate(q1[idx_in + 1]);
  HGate(q1[idx_in + 1]);
  HGate(q4[idx_in + 1]);
  HGate(q4[idx_in + 1]);
  HGate(q1[idx_in1[0] + 1]);
  HGate(q1[idx_in1[0] + 1]);
  HGate(q4[idx_in1[1] + 1]);
  HGate(q4[idx_in1[1] + 1]);
  HGate(q1[idx_in + idx_in1[0]]);
  HGate(q1[idx_in + idx_in1[0]]);
  HGate(q4[idx_in + idx_in1[1]]);
  HGate(q4[idx_in + idx_in1[1]]);

}

void quantum_kernel GlobalTest(int idx1, int idx2) {

  HGate(q_glob1);
  HGate(q_glob1);
  HGate(q_glob2[0]);
  HGate(q_glob2[0]);
  HGate(q_glob2[1]);
  HGate(q_glob2[1]);
  HGate(q_glob2[idx1]);
  HGate(q_glob2[idx1]);
  HGate(q_glob2[idx1 +1]);
  HGate(q_glob2[idx1 +1]);
  HGate(q_glob2[idx1 +idx2]);
  HGate(q_glob2[idx1 +idx2]);

}
