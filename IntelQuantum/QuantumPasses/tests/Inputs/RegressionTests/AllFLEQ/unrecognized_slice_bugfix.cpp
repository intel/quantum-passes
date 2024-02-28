#include <clang/Quantum/quintrinsics.h>
#include <clang/Quantum/qexpr.h>

using namespace qexpr;
using namespace qlist;


const int N = 4;
qbit listable(qs, N);
qbit q;

// This is a test to verify a bugfix where the qubit argument for the
// single-qubit variant of qexpr::control was not being simplified the
// way it was for gate agruments. Whereas before, the first two eval
// calls would fail due to the unresolved qlist slicing, now all 
// four eval calls will appropriately compile. 


int main() {
  eval_hold(control((qs<<1)[0], _X(q))); // failed
  eval_hold(control(qs(2,3)[0], _X(q))); // failed
  eval_hold(_CNOT((qs<<1)[0], q)); // succeeded
  eval_hold(control(q, _X((qs<<1)[0]))); // succeeded
  return 0;
}