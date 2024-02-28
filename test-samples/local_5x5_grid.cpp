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
const int N = 25;
qbit q[N];
cbit c[N];

/* Application graph: (may be flipped in local pass)

0  -  1  - 16 - 17 - 18
|     |    |    |    |
2  - 19 -  15 - 20 - 21
|    |     |    |     | 
12 - 13 -  14 - 22 -  6
|    |     |    |     |
7  - 24 -  8 -  23 -  5
|    |     |    |     |
11 - 10 -  9 -  4  -  3

*/

quantum_kernel void test_kernel() {
  CZ(q[0], q[1]);
  CZ(q[0], q[2]);
  CZ(q[1], q[16]);
  CZ(q[1], q[19]);
  CZ(q[16], q[17]);
  CZ(q[16], q[15]);
  CZ(q[17], q[18]);
  CZ(q[17], q[20]);
  CZ(q[18], q[21]);
  CZ(q[2], q[19]);
  CZ(q[2], q[12]);
  CZ(q[19], q[15]);
  CZ(q[19], q[13]);
  CZ(q[15], q[20]);
  CZ(q[15], q[14]);
  CZ(q[20], q[21]);
  CZ(q[20], q[22]);
  CZ(q[21], q[6]);
  CZ(q[12], q[13]);
  CZ(q[12], q[7]);
  CZ(q[13], q[14]);
  CZ(q[13], q[24]);
  CZ(q[14], q[22]);
  CZ(q[14], q[8]);
  CZ(q[22], q[6]);
  CZ(q[22], q[23]);
  CZ(q[6], q[5]);
  CZ(q[7], q[24]);
  CZ(q[7], q[11]);
  CZ(q[24], q[8]);
  CZ(q[24], q[10]);
  CZ(q[8], q[23]);
  CZ(q[8], q[9]);
  CZ(q[23], q[5]);
  CZ(q[23], q[4]);
  CZ(q[5], q[3]);
  CZ(q[11], q[10]);
  CZ(q[10], q[9]);
  CZ(q[9], q[4]);
  CZ(q[4], q[3]);

}

int main() {  
  return 0;
}
