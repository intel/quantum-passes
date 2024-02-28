#include <clang/Quantum/quintrinsics.h>
#include <clang/Quantum/qexpr.h>
#include <clang/Quantum/qlist.h>

#include <iostream>

using namespace qexpr;
using namespace qlist;

qbit listable(reg1, 3);

QExpr prep_all(QList reg){
  return cIf(size(reg) > 0, 
           _PrepZ(reg[0]) * prep_all(reg + 1),
           identity());
}

int main(){
  
  qbit q_loc[2];
  QList ql_loc(q_loc);
  eval_hold(prep_all(reg1));
  eval_hold(prep_all(ql_loc));

}