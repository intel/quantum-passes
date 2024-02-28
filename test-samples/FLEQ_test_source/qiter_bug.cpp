#include <clang/Quantum/quintrinsics.h>
#include <clang/Quantum/qexpr.h>


const int N = 3;
qbit listable(q, N);

using namespace qexpr;
using namespace qlist;

//This was a bug where a user iterates past the end of the qbit array
//but compilation was not failing. Ultimately, the QIter insertion 
//function was avoiding inserting a gate out of bounds but spitting
//out a cryptic error. Now FLEQ compilation will catch this and 
//fail with a bit more info.

template<typename QExprFun, typename... Arrays>
QExpr map(QExprFun qk, QList qs, Arrays... arrays) noexcept {
  return cIf(size(qs) > 0, 
           qk(qs[0], arrays[0]...) + map(qk, qs + 1, (arrays + 1)...),
           identity()
  );
}

int main() {

    eval_hold(map(_CNOT,q,q>>1));

    return 0;
}