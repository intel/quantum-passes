#include <clang/Quantum/quintrinsics.h>
#include <clang/Quantum/qexpr.h>
#include <clang/Quantum/qlist.h>
#include <clang/Quantum/datalist.h>

using namespace qexpr;
using namespace qlist;
using namespace datalist;

qbit listable(q, 5);
qbit bad_use[2];

QExpr loop_to_exit(QList qs, unsigned idx, bool no_exit){
  auto to_print = DataList("I want this to print: ") + DataList(qs.size()) + DataList("\n");
  return cIf(qs.size() > 0, loop_to_exit(qs + 1, idx, no_exit) 
                            + printDataList(to_print, _X(q[0])),
                            cIf(no_exit, printDataList(to_print + "\n\n", identity()), exitAtCompile("print this at exit")));
}

unsigned getIdx();

int main() {
    eval_hold(printDataList("\n", loop_to_exit(q, getIdx(), true)));
    eval_hold(loop_to_exit(q, getIdx(), false));
}