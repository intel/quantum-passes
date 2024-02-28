#include <clang/Quantum/quintrinsics.h>

void CtorSetterTest( int para1, int para2[2], double para3, double para4[2]) {

 int para5=0;
 int para6[] ={0,1};
 double para7 =.01;
 double para8[] = {.01,.02}; 

 qbit q;

  RX(q, para1);
  RX(q, para2[0]);
  RX(q, para2[1]);
  RX(q, para3);
  RX(q, para4[0]);
  RX(q, para4[1]);
  RX(q, para5);
  RX(q, para6[0]);
  RX(q, para6[1]);
  RX(q, para7);
  RX(q, para8[0]);
  RX(q, para8[1]);
 
}

void ResolutionTest( int para1, int para2[2], double para3, double para4[2]) {

 int para5=0;
 int para6[] ={0,1};
 double para7 =.01;
 double para8[] = {.01,.02}; 

  qbit q;

  //directly resolved
  RX(q, 0);
  RX(q, .01);

  //resolved through const 
  RX(q, para5);
  RX(q, para6[0]);
  RX(q, para6[1]);
  RX(q, para7);
  RX(q, para8[0]);
  RX(q, para8[1]);
  
  //resolved through foldable const
  para5 += 1;
  para6[0] +=1;
  para6[1] +=1;
  para7 += 1;
  para8[0] += 1;
  para8[1] += 1;

  RX(q, para5);
  RX(q, para6[0]);
  RX(q, para6[1]);
  RX(q, para7);
  RX(q, para8[0]);
  RX(q, para8[1]);
  
  //unresolvable through argument
  RX(q, para1);
  RX(q, para2[0]);
  RX(q, para2[1]);
  RX(q, para3);
  RX(q, para4[0]);
  RX(q, para4[1]);
 
}

void EqualityTest( int para1, int para2[2], double para3, double para4[2]) {

 int para5=0;
 int para6[] ={0,1};
 double para7 =.01;
 double para8[] = {.01,.02}; 

  qbit q;

  //direct equality and inequality
  RX(q, 1);
  RX(q, 1);
  RX(q, 0.1);
  RX(q, 0.1);

  //Equality/ through unresolved argument
  RX(q, para1);
  RX(q, para1);
  RX(q, para2[0]);
  RX(q, para2[0]);
  RX(q, para2[1]);
  RX(q, para2[1]);
  RX(q, para3);
  RX(q, para3);
  RX(q, para4[0]);
  RX(q, para4[0]);
  RX(q, para4[1]);
  RX(q, para4[1]);

  //Inequality through unresolved argument
  RX(q, para1);
  para1 +=1;
  RX(q, para1);
  RX(q, para2[0]);
  para2[0] +=1;
  RX(q, para2[0]);
  RX(q, para2[1]);
  para2[1] +=1;
  RX(q, para2[1]);
  RX(q, para3);
  para3 +=1;
  RX(q, para3);
  RX(q, para4[0]);
  para4[0] +=1;
  RX(q, para4[0]);
  RX(q, para4[1]);
  para4[1] +=1;
  RX(q, para4[1]);


  //Unresolvable equality through sum of arguments
  RX(q, para1+1);
  RX(q, para1+1);
  RX(q, para2[0]+1);
  RX(q, para2[0]+1);
  RX(q, para2[1]+1);
  RX(q, para2[1]+1);
  RX(q, para3+1);
  RX(q, para3+1);
  RX(q, para4[0]+1);
  RX(q, para4[0]+1);
  RX(q, para4[1]+1);
  RX(q, para4[1]+1);
  RX(q, para1+para2[0]);
  RX(q, para1+para2[0]);
  RX(q, para3+para4[0]);
  RX(q, para3+para4[0]);

}

void ArithmeticTest(double para1, double para2, double cnst) {

 qbit q;

 RX(q, 1.2);
 RX(q, 1.8);
 RX(q, para1);
 RX(q, para2);

}

