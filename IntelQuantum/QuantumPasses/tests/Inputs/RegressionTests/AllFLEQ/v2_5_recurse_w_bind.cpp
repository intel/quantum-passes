#include <clang/Quantum/quintrinsics.h>
#include <clang/Quantum/qexpr.h>
#include <clang/Quantum/qlist.h>

using namespace qexpr;
using namespace qlist;

qbit listable(q1 ,3);
qbit listable(q2 ,5);

QExpr bindAllH(QList qs) {
  return cIf(qs.size() > 0,
               _H(qs[0]) << bindAllH(++qs),
               identity()
             );
}

int main(){

  eval_hold(bindAllH(q1));

  eval_hold(bindAllH(q2));

}
