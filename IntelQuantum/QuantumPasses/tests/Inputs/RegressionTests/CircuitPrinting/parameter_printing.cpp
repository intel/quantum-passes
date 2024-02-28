#include <clang/Quantum/quintrinsics.h>
#include <quantum.hpp>

#include <iostream> 

/// Define our quantum resources
const int N = 9;                
qbit q[N];                      
cbit c[5];
cbit csig;

quantum_kernel void const_angles(){
  RZ(q[0], 1234567.89);
  RZ(q[1], 12345.6789);
  RZ(q[2], 123.456789);
  RZ(q[3], 1.23456789);
  RZ(q[4], 0.0123456789);
  RZ(q[5], 0.000123456789);
  RZ(q[6], 0.00000123456789);
  RZ(q[7], -1.23456789);
  RZ(q[8], 0.);
}

double getAngle();
double getAngle(double);
double getAngle(double, int);
int i;

class Foo {
  private:
  double a;
  public:
  Foo(double in) : a(in){}
  double get();
};

Foo A(0.1);

quantum_kernel void function_angles(){
  RZ(q[0], getAngle());
  RZ(q[1], getAngle(0.1));
  RZ(q[2], getAngle(0.1 + getAngle()));
  RZ(q[3], getAngle(0.1, 2));
  RZ(q[4], getAngle(getAngle(), 2 + i));
  RZ(q[5], A.get());
}

double angle;
double angles[4];

quantum_kernel void scalar_angles(double a, double b[]){
  RZ(q[0], angle);
  RZ(q[1], angles[2]);
  RZ(q[2], angles[1 + i]);
  RZ(q[3], a);
  RZ(q[4], b[2]);
  RZ(q[5], b[1 + i]);
}

quantum_kernel void binary_op_angles(double a, double b, double c){

  RZ(q[0], a * b);
  RZ(q[1], a/b);
  RZ(q[2], a + b);
  RZ(q[3], a - b);
  RZ(q[4], a*b + c);
  RZ(q[5], a*(b + c));
  RZ(q[6], (a + b + c)/2.5);
  RZ(q[7], -(a + b + c));

}

quantum_kernel void meas_cbits(cbit &arg){
  MeasZ(q[0], csig);
  MeasZ(q[1], c[1]);
  MeasZ(q[2], c[i +2]);
  MeasZ(q[3], arg);
}

quantum_kernel void comparisons(unsigned a, unsigned b){
  RZ(q[0], a == b);
  RZ(q[1], a != b);
  RZ(q[2], a < b);
  RZ(q[3], a <= b);
  RZ(q[4], a > b);
  RZ(q[5], a >= b);
}

quantum_kernel void odds_and_ends(unsigned a, unsigned b){
  RZ(q[0], a % b);
  RZ(q[1], a << b);
  RZ(q[2], a >> b);
  RZ(q[3], a && b);
  RZ(q[4], a || b);
  RZ(q[5], a ^ b);
  RZ(q[6], (a < b) || (a ^ b));
}

int main() {
  const_angles();
}