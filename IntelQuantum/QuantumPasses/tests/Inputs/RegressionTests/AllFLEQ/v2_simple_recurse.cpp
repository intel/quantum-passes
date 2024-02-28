#include <clang/Quantum/quintrinsics.h>
#include <clang/Quantum/qexpr.h>
#include <clang/Quantum/qlist.h>

using namespace qexpr;
using namespace qlist;

qbit listable(reg1, 5);
qbit listable(reg2, 10);

QExpr HOnAll(QList reg){
  return cIf(size(reg) == 0, 
           identity(),
           _H(reg[0]) * HOnAll(reg + 1)
         );
}

int main(){
  eval_hold(HOnAll(reg1));
  eval_hold(HOnAll(reg2));
}