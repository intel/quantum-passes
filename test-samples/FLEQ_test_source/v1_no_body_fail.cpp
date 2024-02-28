#include <clang/Quantum/quintrinsics.h>
#include <clang/Quantum/qexpr.h>
#include <clang/Quantum/qlist.h>

using namespace qexpr;
using namespace qlist;

QExpr no_body();

int main() {

  eval_hold(no_body());

  return 0;

}