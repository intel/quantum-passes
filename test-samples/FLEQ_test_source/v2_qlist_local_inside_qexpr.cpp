#include <clang/Quantum/quintrinsics.h>
#include <clang/Quantum/qexpr.h>
#include <clang/Quantum/qlist.h>

using namespace qexpr;
using namespace qlist;

QExpr inner_passed(qbit &arg){
  qbit listable(q);
  return _H(*q) * _H(arg);
}

QExpr local_listable() {
    
  qbit listable(reg, 4);
  qbit listable(q);

return _H(*q) * inner_passed(reg[2]);

}

int main(){
  eval_hold(local_listable());
}