#include <clang/Quantum/quintrinsics.h>
#include <clang/Quantum/qexpr.h>
#include <clang/Quantum/qlist.h>

using namespace qexpr;
using namespace qlist;

qbit listable(q, 3);

//This was a bug where a join over a bind argument was failing, specifically
//if the earlier argument was the bind i.e. join_over_bind1 would fail
//while join_over_bind2 would not. This was because FLEQ compilation was 
//interpreting the two QBB generated by the bind as distinct unresolved
//branches and in the branch multiplication step would overwrite the 
//second bind QBB with a copy of the first, and ultimately cause a seg
//fault. The fix specifically looks for branch multiplication to only 
//happen between QBBs which have the same exit. 

QExpr join_over_bind1(){
  return (_H(q[0]) << _H(q[1])) + _H(q[2]);
}

QExpr join_over_bind2(){
  return _H(q[0]) + (_H(q[1]) << _H(q[2]));
}

int main(){
    eval_hold(join_over_bind1());
    eval_hold(join_over_bind2());
}