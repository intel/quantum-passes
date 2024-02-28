#include <clang/Quantum/quintrinsics.h>
#include <clang/Quantum/qexpr.h>

using namespace qexpr;


void calling_evals(){
  eval_hold(identity());
  eval_release(identity());
}

QExpr calling_join(qbit &q0, qbit &q1){
  return _H(q0) * _X(q1);
}

PROTECT QExpr calling_terminal_nodes(qbit &q0, qbit &q1){
   return _H(q0) * identity() * this_as_expr;
}

QExpr calling_single_arg(qbit &q, qbit reg[]){
    return !(control(q, _H(reg[0])) * control(reg, _H(q)));
}

QExpr calling_get_let(qbit &q){
  let("Key", _H(q));
  return get("Key");
}

QExpr calling_branch(bool b, QExpr A, QExpr B){
  return cIf(b, A, B);
}

QExpr custom(QExpr U){ return U; }

QExpr calling_custom(qbit &q){
  return custom(_H(q));
}