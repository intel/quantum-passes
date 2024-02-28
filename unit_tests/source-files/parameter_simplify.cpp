#include "clang/Quantum/quintrinsics.h"
#include <cmath>
qbit q;

quantum_kernel void parameter_simplify(double a, double b){
  //Only intended to be used for extracting ParaRefs for a, b, and std::pow(a, 10)
  RZ(q, a);
  RZ(q, b);
  RZ(q, std::pow(a, 10));
}

