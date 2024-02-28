#include <clang/Quantum/quintrinsics.h>
#include <clang/Quantum/qexpr.h>
#include <clang/Quantum/qlist.h>

using namespace qexpr;
using namespace qlist;

qbit listable(q, 4);

// This bug is for the pattern of "branch_over_bind" where one branch
// contains a bind where the other one does not. It was known that
// many operations do not distribute over bind, but cIf represented
// particularly egregious case of this. Fixing the bug also meant
// that inlining had to be handled differently, so the test also 
// verifies that cases with non-trivial classical code is properly 
// handled. 

bool getBool();

unsigned getUInt();

void dummyCall(int);

QExpr branch_over_bind(bool b){
  return cIf(b, _X(q[0]), 
                _X(q[1]) << _X(q[2]));  
}


void simple_test(){
    eval_hold(branch_over_bind(getBool()));
}

template<unsigned i>
PROTECT QExpr inlined(){
  dummyCall(i);
  X(q[i-1]);
  dummyCall(-i);
  return this_as_expr;
}


PROTECT QExpr branch_over_bind_w_inline(bool b){
  dummyCall(0);
  return cIf(b, inlined<1>(), inlined<2>() << inlined<3>());
}

void inline_test(){
  eval_hold(branch_over_bind_w_inline(getBool()));
}


template<unsigned i>
PROTECT QExpr inlined_w_loop(unsigned N){
  for(int j = 0; j < N; j++){
  dummyCall(i);
  }
  return _X(q[i-1]);
}

PROTECT QExpr branch_over_bind_w_loop(bool b, unsigned N, unsigned M){
  for(int i = 0; i < N; i++){
    dummyCall(0);
  }
  return cIf(b, inlined_w_loop<1>(M), inlined_w_loop<2>(M) << inlined_w_loop<3>(M));
}

void inline_w_loop_test(){
  eval_hold(branch_over_bind_w_loop(getBool(), getUInt(), getUInt()));
}
