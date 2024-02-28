#include <clang/Quantum/quintrinsics.h>
#include <clang/Quantum/qexpr.h>
#include <clang/Quantum/qlist.h>
#include <clang/Quantum/datalist.h>

using namespace qexpr;
using namespace qlist;
using namespace datalist;

qbit listable(q, 5);
qbit bad_use[2];

QExpr loop_to_bad_qbit(QList qs, unsigned idx){
  auto to_print = DataList("I want this to print: ") + DataList(qs.size()) + DataList("\n");
  return cIf(qs.size() > 0, printDataList(to_print, _X(q[0]))
                            + loop_to_bad_qbit(qs + 1, idx),
                            _X(bad_use[idx]));
}

unsigned getIdx();

int main() {
    eval_hold(loop_to_bad_qbit(q, getIdx()));
}