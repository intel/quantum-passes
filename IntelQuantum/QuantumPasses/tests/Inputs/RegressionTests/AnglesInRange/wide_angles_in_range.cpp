#include <clang/Quantum/quintrinsics.h>

qbit gq;

quantum_kernel void wide_angle(){

  RX(gq, -63.25);
  RY(gq, -46.4623);
  RZ(gq, -54.1343);
  RX(gq, -13.235);
  RY(gq, -12.41);
  RX(gq, -1.234);
  RY(gq, -1.3874);
  RZ(gq, -.9245);
  RX(gq, 1.534);
  RY(gq, .942);
  RZ(gq, 1.235);
  RX(gq, 15.23);
  RY(gq, 16.432);
  RZ(gq, 12.542);
  RX(gq, 64.322);
  RY(gq, 73.236);
  RZ(gq, 85.262);

}

int main(){
  wide_angle();
}

