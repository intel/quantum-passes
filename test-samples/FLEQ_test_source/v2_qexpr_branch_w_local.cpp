#include <clang/Quantum/quintrinsics.h>
#include <clang/Quantum/qexpr.h>
#include <clang/Quantum/qlist.h>

using namespace qexpr;
using namespace qlist;

bool getBool();

QExpr H_on_local(){
  qbit q;
  return _H(q);
}

QExpr conditional_qbit(bool add_q){
  return cIf(add_q,
             H_on_local(),
             identity()
            );
}


int main(){

  eval_hold(conditional_qbit(getBool()));
  eval_hold(conditional_qbit(getBool()));

}