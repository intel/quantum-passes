#include <clang/Quantum/quintrinsics.h>
#include <clang/Quantum/qexpr.h>
#include <clang/Quantum/qlist.h>

using namespace qexpr;
using namespace qlist;

qbit listable(q ,5);
qbit listable(anc);

void mixedBinds(){
  //mixed binds
  eval_hold(_H(q[0]) << _H(q[1]) >> _H(q[2]) );
  //interpreted as 
  //  ( H(0) << H(1) ) >> H(2)
  //   i.e. H(2) << H(0) << H(1) 
}

void mixedBindAndJoin(){
  //mixed bind and join
  eval_hold(_H(q[0]) << _H(q[1]) + _X(q[1]) << _H(q[3]) * _X(q[3]));
  //interpreted as
  //  H(0) << ( H(1) + X(1) ) << ( H(3) * X(3) )

}

void mixedBindAndJoinAlt(){
  //mixed bind and join alternate
  eval_hold(_H(q[0]) << _H(q[1]) + ( _H(q[2]) << _H(q[3]) ) * _H(q[4]));
  //interpreted as 
  //  H(0) << ( H(1) + H(2) * H(4) ) << H(3)
  // consistent with "distributes only to the first bind"

}

void mixedBindAndJoinAltRev(){
  //mixed bind and join alternate reverse
  eval_hold(_H(q[0]) >> _H(q[1]) + ( _H(q[2]) >> _H(q[3]) ) * _H(q[4]));
  //interpreted as 
  //  ( H(1) + H(3) * H(4) ) << H(2) << H(0)
  // consistent with "distributes only to the first bind"

}
