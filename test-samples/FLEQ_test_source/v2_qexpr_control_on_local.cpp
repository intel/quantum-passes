#include <clang/Quantum/quintrinsics.h>
#include <clang/Quantum/qexpr.h>
#include <clang/Quantum/qlist.h>

using namespace qexpr;
using namespace qlist;

qbit listable(glob, 10);

  template<unsigned long MAX = 3>
  QExpr control_w_anc(QList ctrls, QExpr e);

  template<unsigned long MAX = 3>
  QExpr control_w_anc_helper(QList ctrls, QExpr e){
    qbit listable(anc);
    return control_w_anc<MAX>(ctrls(0, MAX), _X(anc[0]))
           + control_w_anc<MAX>(anc + (ctrls >> MAX), e)
           + !control_w_anc<MAX>(ctrls(0, MAX), _X(anc[0]));
  }
  
  template<unsigned long MAX>
  QExpr control_w_anc(QList ctrls, QExpr e){
    return cIf(size(ctrls) > MAX, 
               control_w_anc_helper<MAX>(ctrls, e), 
               control(ctrls, e));
  } 


int main(){
  qbit tar;
  qbit listable(reg, 10);
  eval_hold(printQuantumLogic(control_w_anc(reg, _H(tar))));
  eval_hold(control_w_anc(glob, _H(tar)));
  eval_hold(control(reg[0], _H(tar)));
}