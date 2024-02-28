#include <clang/Quantum/quintrinsics.h>
#include <clang/Quantum/qexpr.h>
#include <clang/Quantum/qlist.h>

using namespace qexpr;
using namespace qlist;

qbit listable(reg, 10);

template<unsigned long gap>
QExpr every_other_qbit_helper(QExpr(*f)(QList), const QList arry_in, const QList arry_out){
  return cIf(size(arry_in) != 0,  
              cIf(size(arry_in) > gap,
                  every_other_qbit_helper<gap>(f, arry_in >> gap, arry_out + arry_in(0,1)),
                  f(arry_out + arry_in(0,1))
                ),
              f(arry_out)
            );
}

template<unsigned long gap>
QExpr map_every_other(QExpr(*f)(const QList), const QList ql){
  return cIf(size(ql) > gap, 
           every_other_qbit_helper<gap>(f, ql, QList::empty_list()),
           identity()
         );
}

QExpr XOnAll(const QList ql){
  return cIf(size(ql) > 0,
      _X(ql[0]) + XOnAll(++ql),
      identity()
  );
}

int main() {
  eval_hold(map_every_other<2>(XOnAll, reg));
  eval_hold(map_every_other<5>(XOnAll, reg));
  eval_hold(map_every_other<11>(XOnAll, reg));
  return 0;
}