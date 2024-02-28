#include <clang/Quantum/quintrinsics.h>
#include <clang/Quantum/qexpr.h>
#include <clang/Quantum/qlist.h>

qbit listable(q1, 5);
qbit listable(q2, 3);
qbit listable(q3, 2);

using namespace qexpr;
using namespace qlist;

QExpr HOnAll(QList reg){
  return cIf(reg.size() > 0, 
             _H(reg[0]) * HOnAll(++reg),
             identity());
}

int main() {

  auto q1slice = q1(1, 3);
  auto q1join2 = q1 + q2;
  auto qsljoin1 = q1join2 + (q3 >> 1); 
  auto qsljoin2 = q1slice + (q3 << 2);

  ++q3; //== q3 >> 1

  qsljoin2.size();
  size(qsljoin1);

  eval_hold(_H(q1[2]));

}