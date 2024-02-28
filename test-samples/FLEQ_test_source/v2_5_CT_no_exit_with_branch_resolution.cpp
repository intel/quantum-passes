#include <clang/Quantum/quintrinsics.h>
#include <clang/Quantum/qexpr.h>
#include <clang/Quantum/qlist.h>

using namespace qexpr;
using namespace qlist;

qbit listable(q ,3);


QExpr CTFailWithBranch(bool b){
  return cIf(b, 
             identity(),
             exitAtCompile("This failed because the condition was not resolved to 'true'."));
}

int main(){

  eval_hold(CTFailWithBranch(true));

}