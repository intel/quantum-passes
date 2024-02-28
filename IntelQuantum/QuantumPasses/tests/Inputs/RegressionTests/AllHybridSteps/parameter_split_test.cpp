#include <clang/Quantum/quintrinsics.h>

const int N =4;
qbit q[N];
cbit c;
double paras[2];

//Dummy function declarations
double fake_atan(double);

quantum_kernel void ConstTest(){
  RZ(q[0], 0.1);
  RZ(q[1], 0.2);
  MeasZ(q[0], c);
}

quantum_kernel void GArrayTest(){

  RZ(q[0], paras[0]);
  RZ(q[0], paras[1]/2);
  RZ(q[0], paras[0] + 0.1);
  RZ(q[0], paras[0] - paras[1]);

}

quantum_kernel void LocalTest(){

  double temp = fake_atan(0.1);

  RZ(q[0], temp);

  double temp2[2];
  temp2[0] = fake_atan(0.2);
  temp2[1] = fake_atan(0.3);

  RZ(q[0], temp2[0]);
  RZ(q[0], temp2[1]);
  
}

quantum_kernel void ArgTest(double a, double b[]){

  RZ(q[0], a);
  RZ(q[0], b[0]);
  RZ(q[0], a + b[1]);

}

quantum_kernel void FunctionTest(double a){

  RZ(q[0], fake_atan(a));

  double temp = fake_atan(a);

  RZ(q[0], temp/2);

}

quantum_kernel void BranchTest(bool br, unsigned pow){

  double temp;

  if(br) temp = paras[0];
  else temp = paras[1];

  RZ(q[0], temp);

  for(int i =0; i<pow; i++) temp *= temp;
  
   RZ(q[0], temp);

}

