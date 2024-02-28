#include <clang/Quantum/quintrinsics.h>
#include <clang/Quantum/qexpr.h>
#include <clang/Quantum/qlist.h>

#include <iostream>

using namespace qexpr;
using namespace qlist;

qbit listable(reg1, 3, 2, 5, 11);
qbit listable(reg2, 3, 3, 4, 6);

QExpr H_on_local(){
  qbit q;
  return _H(q);
}

QExpr prep_all(QList reg){
  return cIf(size(reg) > 0, 
           _PrepZ(reg[0]) * prep_all(reg + 1),
           identity());
}

QExpr meas_all(QList reg, cbit c[]){
  return cIf(size(reg) > 0, 
           _MeasZ(reg[0], c[0]) * meas_all(reg + 1, c + 1),
           identity());
}

void simple_two_holds(){
  qbit listable(q_loc, 3);
  eval_hold(prep_all(reg1));
  eval_hold(prep_all(q_loc));
}

void simple_hold_and_release(){
  qbit listable(q_loc, 3);
  cbit c[3];
  eval_hold(prep_all(reg1));
  eval_release(meas_all(reg1, c));
  eval_hold(prep_all(q_loc));
  eval_release(meas_all(q_loc, c));
}

void linear_binds(){
  qbit listable(q_loc, 3);
  cbit c[3];
  eval_release(prep_all(reg1) << meas_all(reg1, c));
  eval_release(prep_all(q_loc) << meas_all(q_loc, c));
}

void diamond_branch_wo_qexpr(bool b){
  qbit listable(q_loc, 3);
  cbit c[3];
  eval_hold(prep_all(reg1));
  if(b)
    eval_release(meas_all(q_loc, c));
  else
    eval_hold(_S(reg1[0]));
  eval_release(meas_all(reg1, c));
}

void diamond_branch_w_qexpr(bool b){
  qbit listable(q_loc, 3);
  cbit c[3];
  eval_release(prep_all(reg1)
    << cIf(b, meas_all(q_loc, c), _S(reg1[0]))
    << meas_all(reg1, c));
}

void multi_branch_single_call(bool b1, bool b2, bool b3){
  eval_hold(
    cIf(b1, _H(reg1[0]),
    cIf(b2, _H(reg1[1]),
    cIf(b3, _H(reg1[2]), H_on_local())))
  );
}

void multi_branch_release(bool b1, bool b2, bool b3){
  cbit c[3];
  eval_release(
    cIf(b1, _H(reg1[0]),
    cIf(b2, _H(reg1[1]),
    cIf(b3, _H(reg1[2]), H_on_local())))
     + meas_all(reg1, c)
  );
    qbit listable(q_loc, 3);
  eval_hold(prep_all(q_loc));
}
