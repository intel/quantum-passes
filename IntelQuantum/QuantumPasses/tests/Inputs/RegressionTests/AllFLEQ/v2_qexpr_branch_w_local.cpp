#include <clang/Quantum/quintrinsics.h>
#include <clang/Quantum/qexpr.h>
#include <clang/Quantum/qlist.h>

using namespace qexpr;
using namespace qlist;

bool getBool();

qbit listable(q, 3);

PROTECT QExpr H_on_local(){
  qbit q;
  return _H(q);
}

QExpr conditional_qbit(bool add_q){
  return cIf(add_q,
             H_on_local(),
             identity()
            );
}

QExpr conditional_control_w_local(bool add_q){
  return cIf(add_q,
             control(q[0], H_on_local()),
             _H(q[0])
            );
}

QExpr conditional_multi_control_w_local(bool add_q){
  return cIf(add_q,
             control(q, H_on_local()),
             _H(q[0])
            );
}


int main(){

  eval_hold(conditional_qbit(getBool()));

  eval_hold(conditional_control_w_local(getBool()));

  eval_hold(conditional_multi_control_w_local(getBool())); 

}