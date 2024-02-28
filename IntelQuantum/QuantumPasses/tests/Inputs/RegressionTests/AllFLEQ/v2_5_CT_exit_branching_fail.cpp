#include <clang/Quantum/quintrinsics.h>
#include <clang/Quantum/qexpr.h>
#include <clang/Quantum/qlist.h>

using namespace qexpr;
using namespace qlist;

qbit listable(q ,3);


QExpr CTFailWithBranch(bool b){
  return cIf(b, 
             identity(),
             exitAtCompile("This will fail if the condition is not resolved to 'true'."));
}

bool doWeBranch();

int main(){

  eval_hold(CTFailWithBranch(doWeBranch()));

}