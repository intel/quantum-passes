#include <clang/Quantum/quintrinsics.h>
#include <clang/Quantum/qexpr.h>
#include <clang/Quantum/qlist.h>

using namespace qexpr;
using namespace qlist;

qbit listable(q ,3);

PROTECT QExpr simpleBarrier(){
  cbit c1, c2;
  return _MeasZ(q[0], c1) << _MeasZ(q[1], c2);
}

PROTECT QExpr getContinue(cbit & c){
  return _CNOT(q[0], q[1])
        * _MeasZ(q[1], c)
        * _CNOT(q[0], q[1]);
}

PROTECT QExpr doWeContinue(cbit & c, QExpr cont){
  return  cIf(c, 
             identity(),
             cont);
}

int main(){

  eval_release(simpleBarrier());

  cbit c; 
  eval_hold(getContinue(c) << doWeContinue(c, _H(q[2])));

  eval_hold(_H(q[0]) << _H(q[1]) << _H(q[2]));

}
