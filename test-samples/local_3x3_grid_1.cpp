/// NOTE: Pick an include path depending on whether running in development
/// enviromnment or production environment
//===----------------------------------------------------------------------===//
/// Production mode
#include <clang/Quantum/quintrinsics.h>

#include <cassert>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <quantum.hpp>
#include <quantum_service_apis.h>
#include <string.h>
const int N = 7;
cbit c[N];

/* Application graph: 

2 - 4 - 5
|   |   |
0 - 1 - 6
|   |   |
x - 3 - x

*/

quantum_kernel void test_kernel_1() {
  qbit q[N];
  CNOT(q[0], q[2]);
  CNOT(q[2], q[4]);
  CNOT(q[0], q[1]);
  CNOT(q[1], q[3]);
  CNOT(q[4], q[5]);
  CNOT(q[5], q[6]);
}

int main() {  
  return 0;
}
