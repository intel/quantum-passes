#include <clang/Quantum/quintrinsics.h>
#include <clang/Quantum/qexpr.h>
#include <clang/Quantum/qlist.h>

using namespace qexpr;
using namespace qlist;

qbit listable(reg1, 3);
qbit listable(reg2, 3);
qbit listable(meas);


QExpr prep_all(QList reg){
  return cIf(size(reg) > 0, 
           _PrepZ(reg[0]) * prep_all(reg + 1),
           identity());
}

int main(){

  auto all_qbits = reg1 + reg2 + meas;
  eval_hold(prep_all(all_qbits));

}