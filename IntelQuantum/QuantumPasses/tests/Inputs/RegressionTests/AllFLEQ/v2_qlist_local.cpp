#include <clang/Quantum/quintrinsics.h>
#include <clang/Quantum/qexpr.h>
#include <clang/Quantum/qlist.h>

using namespace qexpr;
using namespace qlist;

quantum_kernel void local_listable() {
    
  qbit listable(reg, 4);
  qbit listable(q);

  H(reg[2]);
  H(q[0]);
  H(*q);

}