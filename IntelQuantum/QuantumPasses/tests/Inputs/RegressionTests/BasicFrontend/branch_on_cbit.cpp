#include <clang/Quantum/quintrinsics.h>

const int N =1;
qbit q[N];

quantum_kernel void DoBody(cbit &c){

  MeasZ(q[0], c);

}

int main(){

  cbit c;

  do{
    DoBody(c);
   } while(c);

  return 0;
}
