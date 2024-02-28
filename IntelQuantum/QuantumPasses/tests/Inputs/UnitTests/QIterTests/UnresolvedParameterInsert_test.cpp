#include <clang/Quantum/quintrinsics.h>
#include <clang/Quantum/quantum.hpp>

double paras[5];
qbit q;

quantum_kernel void ParameterInsertTest(){

   RZ(q, paras[0]);
   RZ(q, paras[1]);
   RZ(q, paras[2]);
   RZ(q, paras[3]);
   RZ(q, paras[4]);

}