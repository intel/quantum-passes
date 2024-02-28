#include <clang/Quantum/quintrinsics.h>
#include <clang/Quantum/qexpr.h>
#include <clang/Quantum/qlist.h>

using namespace qexpr;
using namespace qlist;

const int N = 3;
qbit reg[N];
qbit reg_aux[N];
qbit tar;

template<typename... Args, typename... Arrays>
QExpr map_bwd(QExpr(*qk)(qbit&, Args...), QList qs, Arrays... arrays){
  return cIf(size(qs) > 0, 
           qk(qs[0], arrays[0]...) * map_bwd(qk, qs + 1, (arrays + 1)...),
           identity()
  );        
}

template<typename... Args, typename... Arrays>
QExpr map_fwd(QExpr(*qk)(qbit&, Args...), QList qs, Arrays... arrays){
  
  unsigned cur_idx =  size(qs) - 1;
  
  return cIf(size(qs) > 0, 
            qk(qs[cur_idx], arrays[cur_idx]...) * map_fwd(qk, qs << 1 , arrays...),
            identity()
  );        
} 

QExpr fold(QExpr(*k)(qbit&, QExpr), QList qs, QExpr base){
  return cIf(size(qs) > 0, 
           k(qs[0], fold(k, qs + 1, base)),
           base
  );
}

QExpr Xctl(qbit &ctl, QExpr to_ctl){
  return _H(ctl)
        *control(ctl, to_ctl)
        *_H(ctl);
}

int main(){

  double args1[N] = {0.0, 0.1, 0.2};
  double args2[N] = {0.0, 0.1, 0.2};

   eval_hold(map_bwd(_H, reg));
   eval_hold(map_bwd(_RZ, reg, args1));
   eval_hold(map_bwd(_RXY, reg, args1, args2));
   eval_hold(map_bwd(_CZ, reg, reg_aux));

  eval_hold(map_fwd(_H, reg));
  eval_hold(map_fwd(_RZ, reg, args1));
  eval_hold(map_fwd(_RXY, reg, args1, args2));
  eval_hold(map_fwd(_CZ, reg, reg_aux));

  eval_hold(fold(Xctl, reg, _X(tar)));

}