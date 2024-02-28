#include <clang/Quantum/quintrinsics.h>
#include <clang/Quantum/qexpr.h>

using namespace qexpr;

qbit reg1[5];

bool doWeBranch();

QExpr Sbranch(qbit *reg, bool br){
  return br ? _S(reg[0]) : _Sdag(reg[0]);
}

QExpr Clifbranch1(qbit *reg, bool br1, bool br2){
  return br1 ? _S(reg[0]) : 
                         _Sdag(reg[0]) 
                         * (br2 ? _H(reg[0]) : identity());
}

QExpr Clifbranch2(qbit *reg, bool br1, bool br2){
  return br1 ? _S(reg[0]) :  
                         (br2 ? _H(reg[0]) : identity())
                         * _Sdag(reg[0]);
}

int main(){
  eval_hold(Sbranch(reg1, doWeBranch()));
  eval_hold(Clifbranch1(reg1, doWeBranch(), doWeBranch()));
  eval_hold(Clifbranch2(reg1, doWeBranch(), doWeBranch()));
  eval_hold(Clifbranch2(reg1, true, doWeBranch()));
}