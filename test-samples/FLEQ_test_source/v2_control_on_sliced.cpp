#include <clang/Quantum/quintrinsics.h>
#include <clang/Quantum/qexpr.h>
#include <clang/Quantum/qlist.h>

using namespace qexpr;
using namespace qlist;

qbit listable(reg1, 3);
qbit listable(reg2, 7);
qbit tar;

QExpr multi_control_x(QList ctl, qbit &tar){
  return control(ctl, -1, _X(tar));
}

int main(){
  
  eval_hold(multi_control_x(reg1, tar));

  auto sl_ctl = reg2(2, 5);
  eval_hold(multi_control_x(sl_ctl, tar));

  auto join_ctl = (reg1 >> 2) + (reg2 >> 5);
  eval_hold(multi_control_x(join_ctl, tar));
  
}

