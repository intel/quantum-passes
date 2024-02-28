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

4 - 1 - 0
|   |   |
5 - 2 - 6
|   |   |
x - 3 - x

*/

quantum_kernel void test_kernel_2() {
  qbit q[N];
  CZ(q[0], q[1]);
  CZ(q[1], q[2]);
  CZ(q[2], q[6]);
  CZ(q[2], q[3]);
  CZ(q[4], q[1]);
  CZ(q[5], q[4]);
}

int main() {  
  return 0;
}
