#include <clang/Quantum/quintrinsics.h>
#include <clang/Quantum/qexpr.h>
#include <clang/Quantum/qlist.h>

using namespace qexpr;
using namespace qlist;

qbit listable(q, 4);

void one_bind_test(){
  eval_hold(!(_RX(q[0], 1) << _RX(q[1], 1)));
}

void multi_bind_test(){
  eval_hold(!(_RX(q[0], 1) << _RX(q[1], 1)<< _RX(q[2], 1)));  
}

QExpr recursive_bind(QList qs){
  return cIf(qs.size() > 1, _RX(qs[0], 1) << recursive_bind(qs + 1), _RX(qs[0], 1));
}

void recursive_bind_test(){
  eval_hold(!recursive_bind(q));
}

PROTECT QExpr multi_inverts(){
  return _RX(q[0], 1) << _RX(q[1], 1) << !(_RX(q[2], 1) << _RX(q[3], 1));
}

void multi_inverts_test(){
  eval_hold(!multi_inverts());
}