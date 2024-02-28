#include <clang/Quantum/quintrinsics.h>
#include <clang/Quantum/qexpr.h>
#include <clang/Quantum/qlist.h>

using namespace qexpr;
using namespace qlist;

qbit listable(reg1, 5);

bool doWeBranch();

QExpr Sbranch(QList reg, bool br){
  return cIf(br, _S(reg[0]), _Sdag(reg[0]));
}

QExpr Clifbranch1(QList reg, bool br1, bool br2){
  return cIf(br1, _S(reg[0]), 
                         _Sdag(reg[0]) 
                         * cIf(br2, _H(reg[0]), identity()));
}

QExpr Clifbranch2(QList reg, bool br1, bool br2){
  return cIf(br1, _S(reg[0]),  
                         cIf(br2, _H(reg[0]), identity())
                         * _Sdag(reg[0]));
}

int main(){
  eval_hold(Sbranch(reg1, doWeBranch()));
  eval_hold(Clifbranch1(reg1, doWeBranch(), doWeBranch()));
  eval_hold(Clifbranch2(reg1, doWeBranch(), doWeBranch()));
  eval_hold(Clifbranch2(reg1, true, doWeBranch()));
}