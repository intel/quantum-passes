#include <clang/Quantum/quintrinsics.h>
#include <clang/Quantum/qexpr.h>
#include <clang/Quantum/qlist.h>

using namespace qexpr;
using namespace qlist;

qbit reg1[3];
qbit reg2[7];

quantum_kernel void deref_slice(){

  auto join_sl = qlist::join(&reg1[2], &reg2[5]);

  H(join_sl[0]);
  H(join_sl[1]);
  H(join_sl[2]);
  
}

