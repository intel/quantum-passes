#include <clang/Quantum/quintrinsics.h>
#include <clang/Quantum/qexpr.h>
#include <clang/Quantum/qlist.h>

using namespace qexpr;
using namespace qlist;

qbit listable(q, 6);


bool getBool();

unsigned getUInt();

void dummyCall(int);


QExpr branch_over_bind_w_join1(bool b){
  return cIf(b, _X(q[0]), _X(q[1]) << _X(q[2])) + _X(q[3]);
}

void test_w_join1(){
    eval_hold(branch_over_bind_w_join1(getBool()));
}

QExpr branch_over_bind_w_join2(bool b){
  return cIf(b, _X(q[0]), _X(q[1]) << _X(q[2])) * _X(q[3]);
}

void test_w_join2(){
    eval_hold(branch_over_bind_w_join2(getBool()));
}

template<unsigned i>
PROTECT QExpr inlined(){
  dummyCall(i);
  X(q[i-1]);
  dummyCall(-i);
  return this_as_expr;
}

PROTECT QExpr branch_over_bind_w_inline_w_join1(bool b){
  dummyCall(0);
  return cIf(b, inlined<1>(), inlined<2>() << inlined<3>()) + inlined<4>();
}

void inline_test1(){
  eval_hold(branch_over_bind_w_inline_w_join1(getBool()));
}

PROTECT QExpr branch_over_bind_w_inline_w_join2(bool b){
  dummyCall(0);
  return cIf(b, inlined<1>(), inlined<2>() << inlined<3>()) * inlined<4>();
}

void inline_test2(){
  eval_hold(branch_over_bind_w_inline_w_join2(getBool()));
}

template<unsigned i>
PROTECT QExpr inlined_w_loop(unsigned N){
  for(int j = 0; j < N; j++){
  dummyCall(i);
  }
  return _X(q[i-1]);
}

PROTECT QExpr branch_over_bind_w_loop_w_join1(bool b, unsigned N, unsigned M){
  for(int i = 0; i < N; i++){
    dummyCall(0);
  }
  return cIf(b, inlined_w_loop<1>(M), inlined_w_loop<2>(M) << inlined_w_loop<3>(M)) 
  + inlined_w_loop<4>(M);
}

void inline_w_loop_test1(){
  eval_hold(branch_over_bind_w_loop_w_join1(getBool(), getUInt(), getUInt()));
}

PROTECT QExpr branch_over_bind_w_loop_w_join2(bool b, unsigned N, unsigned M){
  for(int i = 0; i < N; i++){
    dummyCall(0);
  }
  return cIf(b, inlined_w_loop<1>(M), inlined_w_loop<2>(M) << inlined_w_loop<3>(M)) * inlined_w_loop<4>(M);
}

void inline_w_loop_test2(){
  eval_hold(branch_over_bind_w_loop_w_join2(getBool(), getUInt(), getUInt()));
}

PROTECT QExpr join_on_binds(bool b){
  return cIf(b, inlined<1>(), inlined<2>() << inlined<3>()) 
         + cIf(b, inlined<4>(), inlined<5>() << inlined<6>());
}

void join_on_binds_test(){
  eval_hold(join_on_binds(getBool()));
}
