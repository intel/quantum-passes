#include <clang/Quantum/quintrinsics.h>
#include <clang/Quantum/qexpr.h>
#include <clang/Quantum/qlist.h>

using namespace qexpr;
using namespace qlist;

qbit listable(q, 3);

QExpr bad_exit(QList reg, bool exit){
  return cIf(exit, 
           _PrepZ(reg[0]) * bad_exit(reg + 1, exit),
           identity());
}

bool getExit();

int main(){

  eval_hold(bad_exit(q, getExit()));

}