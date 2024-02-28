#include <clang/Quantum/quintrinsics.h>
#include <clang/Quantum/qexpr.h>
#include <clang/Quantum/qlist.h>

using namespace qexpr;
using namespace qlist;

qbit reg1[4];

QExpr LocalBasisChange(qbit &q, unsigned op) {
  
  bool isX = (op == 1);
  bool isY = (op == 2);

  return cIf(isX, _H(q),
         cIf(isY, _H(q) * _Sdag(q),
         identity()));
}

QExpr MeasPauli(QList reg, cbit *outcomes, const unsigned *ops){
    return cIf(reg.size() > 0,
           MeasPauli(reg + 1, outcomes + 1, ops + 1) << cIf( ops[0] != 0, _MeasZ(reg[0], outcomes[0]), identity()),
           identity());

}

void getOps(unsigned pauli[]);

int main(){

  unsigned pauli[4];
  getOps(pauli);
  cbit out[4];

  eval_release(MeasPauli(reg1, out, pauli));

}