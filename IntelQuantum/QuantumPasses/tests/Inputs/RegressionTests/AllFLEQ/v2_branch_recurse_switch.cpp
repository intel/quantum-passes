#include <clang/Quantum/quintrinsics.h>
#include <clang/Quantum/qexpr.h>
#include <clang/Quantum/qlist.h>

using namespace qexpr;
using namespace qlist;

qbit reg1[2];

PROTECT QExpr LocalBasisChange(qbit &q, unsigned op) {
  //This example technically works because
  //LLVM removes temp and replaces it with
  //SSA variables and a PHI node.
  //Not sure we "advertise" this, but it is here
  QExpr temp;

  switch(op){
    case 1:
      temp = _H(q);
      break;
    case 2:
      temp = _H(q) * _Sdag(q);
      break;
    default:
      temp = identity();

  };

  return temp * identity();    
} 

PROTECT QExpr MeasPauli(QList reg, cbit *outcomes, const unsigned *ops){
    return size(reg) == 0 ? identity() :
           ( ops[0] != 0 ? _MeasZ(reg[0], outcomes[0]) * LocalBasisChange(reg[0], ops[0]) : identity())
           * MeasPauli(reg + 1, outcomes + 1, ops + 1);
}

void getOps(unsigned pauli[]);

int main(){

  unsigned pauli[4];
  getOps(pauli);
  cbit out[4];

  eval_release(MeasPauli(reg1, out, pauli));

}