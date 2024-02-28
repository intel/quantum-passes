#include <clang/Quantum/quintrinsics.h>
#include <clang/Quantum/qexpr.h>
#include <clang/Quantum/qlist.h>

using namespace qexpr;
using namespace qlist;

qbit listable(q ,3);


QExpr RTFailWithoutBranch(){
  return exitAtRuntime("This will always fail.");
}

bool doWeBranch();

int main(){

  eval_hold(RTFailWithoutBranch());

}