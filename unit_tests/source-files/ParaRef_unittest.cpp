#include "quintrinsics_ParaRef_test.h"

void CtorSetterTest( int para1, int para2[2], double para3, double para4[2]) {

 int para5=0;
 int para6[] ={0,1};
 double para7 =.01;
 double para8[] = {.01,.02}; 

 qbit q;

  RXGate(q, para1);
  RXGate(q, para2[0]);
  RXGate(q, para2[1]);
  RXGate(q, para3);
  RXGate(q, para4[0]);
  RXGate(q, para4[1]);
  RXGate(q, para5);
  RXGate(q, para6[0]);
  RXGate(q, para6[1]);
  RXGate(q, para7);
  RXGate(q, para8[0]);
  RXGate(q, para8[1]);
 
}

void ResolutionTest( int para1, int para2[2], double para3, double para4[2]) {

 int para5=0;
 int para6[] ={0,1};
 double para7 =.01;
 double para8[] = {.01,.02}; 

  qbit q;

  //directly resolved
  RXGate(q, 0);
  RXGate(q, .01);

  //resolved through const 
  RXGate(q, para5);
  RXGate(q, para6[0]);
  RXGate(q, para6[1]);
  RXGate(q, para7);
  RXGate(q, para8[0]);
  RXGate(q, para8[1]);
  
  //resolved through foldable const
  para5 += 1;
  para6[0] +=1;
  para6[1] +=1;
  para7 += 1;
  para8[0] += 1;
  para8[1] += 1;

  RXGate(q, para5);
  RXGate(q, para6[0]);
  RXGate(q, para6[1]);
  RXGate(q, para7);
  RXGate(q, para8[0]);
  RXGate(q, para8[1]);
  
  //unresolvable through argument
  RXGate(q, para1);
  RXGate(q, para2[0]);
  RXGate(q, para2[1]);
  RXGate(q, para3);
  RXGate(q, para4[0]);
  RXGate(q, para4[1]);
 
}

void EqualityTest( int para1, int para2[2], double para3, double para4[2]) {

 int para5=0;
 int para6[] ={0,1};
 double para7 =.01;
 double para8[] = {.01,.02}; 

  qbit q;

  //direct equality and inequality
  RXGate(q, 1);
  RXGate(q, 1);
  RXGate(q, 0.1);
  RXGate(q, 0.1);

  //Equality/ through unresolved argument
  RXGate(q, para1);
  RXGate(q, para1);
  RXGate(q, para2[0]);
  RXGate(q, para2[0]);
  RXGate(q, para2[1]);
  RXGate(q, para2[1]);
  RXGate(q, para3);
  RXGate(q, para3);
  RXGate(q, para4[0]);
  RXGate(q, para4[0]);
  RXGate(q, para4[1]);
  RXGate(q, para4[1]);

  //Inequality through unresolved argument
  RXGate(q, para1);
  para1 +=1;
  RXGate(q, para1);
  RXGate(q, para2[0]);
  para2[0] +=1;
  RXGate(q, para2[0]);
  RXGate(q, para2[1]);
  para2[1] +=1;
  RXGate(q, para2[1]);
  RXGate(q, para3);
  para3 +=1;
  RXGate(q, para3);
  RXGate(q, para4[0]);
  para4[0] +=1;
  RXGate(q, para4[0]);
  RXGate(q, para4[1]);
  para4[1] +=1;
  RXGate(q, para4[1]);


  //Unresolvable equality through sum of arguments
  RXGate(q, para1+1);
  RXGate(q, para1+1);
  RXGate(q, para2[0]+1);
  RXGate(q, para2[0]+1);
  RXGate(q, para2[1]+1);
  RXGate(q, para2[1]+1);
  RXGate(q, para3+1);
  RXGate(q, para3+1);
  RXGate(q, para4[0]+1);
  RXGate(q, para4[0]+1);
  RXGate(q, para4[1]+1);
  RXGate(q, para4[1]+1);
  RXGate(q, para1+para2[0]);
  RXGate(q, para1+para2[0]);
  RXGate(q, para3+para4[0]);
  RXGate(q, para3+para4[0]);

}

void ArithmeticTest(double para1, double para2, double cnst) {

 qbit q;

 RXGate(q, 1.2);
 RXGate(q, 1.8);
 RXGate(q, para1);
 RXGate(q, para2);

}

