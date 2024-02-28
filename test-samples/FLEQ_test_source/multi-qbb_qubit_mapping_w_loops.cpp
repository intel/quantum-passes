#include <clang/Quantum/quintrinsics.h>
#include <clang/Quantum/qexpr.h>
#include <clang/Quantum/qlist.h>

#include <iostream>

using namespace qexpr;
using namespace qlist;

qbit listable(reg1, 3);
qbit listable(reg2, 3);

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

void call_in_simple_loop(int N){
  for(int i = 0; i < N; i++)
    eval_hold(prep_all(reg1));
}

void call_before_simple_loop(int N){
  qbit listable(q_loc, 3);
  eval_hold(prep_all(q_loc));
  for(int i = 0; i < N; i++)
    eval_hold(prep_all(reg1));
}

void call_after_simple_loop(int N){
  for(int i = 0; i < N; i++)
    eval_hold(prep_all(reg1));
  qbit listable(q_loc, 3);
  eval_hold(prep_all(q_loc));
}

void two_calls_loop1(int N){
  qbit listable(q_loc, 3);
  for(int i = 0; i < N; i++){
    eval_hold(prep_all(reg1));
    eval_hold(_H(q_loc[0]));
  }
  eval_hold(prep_all(q_loc));
}

void two_calls_loop2(int N){
  qbit listable(q_loc, 3);
  cbit c[3];
  for(int i = 0; i < N; i++){
    eval_hold(prep_all(reg1));
    eval_release(meas_all(q_loc, c));
  }
  eval_hold(prep_all(q_loc));
}

void two_calls_loop3(int N){
  qbit listable(q_loc, 3);
  cbit c[3];
  for(int i = 0; i < N; i++){
    eval_release(meas_all(q_loc, c));
    eval_hold(prep_all(reg1));
  }
  eval_hold(prep_all(q_loc));
}

void branching_in_loop(int N, bool b1, bool b2, bool b3){
  qbit listable(q_loc, 3);
  cbit c[3];
  eval_hold(prep_all(q_loc));
  for(int i = 0; i < N; i++){
    eval_hold(
      cIf(b1, _H(reg1[0]),
      cIf(b2, _H(reg1[1]),
      cIf(b3, _H(reg1[2]), H_on_local())))
    ); 
    eval_hold(meas_all(reg1, c));
  }
  eval_release(meas_all(q_loc, c));
}

void branching_in_loop2(int N, bool b1, bool b2, bool b3){
  qbit listable(q_loc, 3);
  cbit c[3];
  eval_hold(prep_all(q_loc));
  for(int i = 0; i < N; i++){
    eval_hold(prep_all(reg1));
    for(int j = 0; j < N; j++){
      eval_hold(
        cIf(b1, _H(reg1[0]),
        cIf(b2, _H(reg1[1]),
        cIf(b3, _H(reg1[2]), H_on_local())))
      );
    }
    eval_hold(meas_all(reg1, c));
  }
  eval_release(meas_all(q_loc, c));
}

void branching_in_loop3(int N, bool b1, bool b2, bool b3){
  qbit listable(loc1);
  qbit listable(loc2);
  qbit listable(loc3);
  cbit c[3];
  for(int i = 0; i < N; i++){
    eval_hold(prep_all(reg1));
    for(int j = 0; j < N; j++){
      eval_hold(
        cIf(b1, _H(loc1[0]),
        cIf(b2, _H(loc2[0]),
        cIf(b3, _H(loc3[0]), H_on_local())))
      );
    }
    eval_hold(meas_all(reg1, c));
  }
  eval_release(meas_all(loc1 + loc2 + loc3, c));
}
