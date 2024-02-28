#include <clang/Quantum/quintrinsics.h>
#include <clang/Quantum/qexpr.h>
#include <clang/Quantum/qlist.h>

using namespace qexpr;
using namespace qlist;

void local_listable() {
    
  qbit listable(reg, 4);
  qbit listable(q);

  eval_hold(_H(reg[2]));
  eval_hold(_H(q[0]));
  eval_hold(_H(*q));

}