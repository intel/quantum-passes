#include <clang/Quantum/quintrinsics.h>
#include <clang/Quantum/qexpr.h>
#include <clang/Quantum/qlist.h>

using namespace qexpr;
using namespace qlist;

qbit listable(reg1, 5);
qbit listable(reg2, 7);

PROTECT QExpr slice_join_qbit_resolution(){

  auto q_sl = qlist::slice(reg2, 1, 4);
                 //= {reg2[1], reg2[2], reg2[3]}
  auto q_join = join(reg1, reg2);
                   //= {reg1[0],..., reg1[4], reg2[0], ..., reg2[6]}
  auto q_sl_and_join = join(qlist::slice(reg2, 4, 7), join(reg1, q_sl));
                         // = {reg2[4], reg2[5], reg2[6], reg1[0], ..., reg1[4], reg2[1], reg2[2], reg2[3]}
  auto  q_sl_of_sl_join = qlist::slice(q_sl_and_join, 1, size(q_sl_and_join));
  auto  q_sl_of_sl_join2 = qlist::slice(q_sl_and_join, 1, size(q_sl_and_join) - 1);

  H(reg1[0]);
  H(reg2[0]);
  H(q_sl[0]);
  H(q_join[0]);
  H(q_sl_and_join[0]); 
  H(q_sl_of_sl_join[0]); 
  H(q_sl_of_sl_join2[0]); 

  return this_as_expr;

}

PROTECT QExpr slice_join_qbit_resolution_w_at(){

  auto q_sl = qlist::slice(reg2, 1, 4);
                 //= {reg2[1], reg2[2], reg2[3]}
  auto q_join = join(reg1, reg2);
                   //= {reg1[0],..., reg1[4], reg2[0], ..., reg2[6]}
  auto q_sl_and_join = join(qlist::slice(reg2, 4, 7), join(reg1, q_sl));
                         // = {reg2[4], reg2[5], reg2[6], reg1[0], ..., reg1[4], reg2[1], reg2[2], reg2[3]}
  auto  q_sl_of_sl_join = qlist::slice(q_sl_and_join, 1, size(q_sl_and_join));
  auto  q_sl_of_sl_join2 = qlist::slice(q_sl_and_join, 1, size(q_sl_and_join) - 1);

  H(reg1.at(0));
  H(reg2.at(0));
  H(q_sl.at(0));
  H(q_join.at(0));
  H(q_sl_and_join.at(0)); 
  H(q_sl_of_sl_join.at(0)); 
  H(q_sl_of_sl_join2.at(0)); 

  return this_as_expr ;

}

int main(){
  eval_hold(slice_join_qbit_resolution());
  eval_hold(slice_join_qbit_resolution_w_at());
}
