#include <clang/Quantum/quintrinsics.h>
#include <clang/Quantum/qexpr.h>
#include <clang/Quantum/qlist.h>

using namespace qexpr;
using namespace qlist;

void test_empty(double& ang){
  ang = 0.1;
}

int main() {

  double val;

  eval_hold(convert<test_empty>(val));

  return 0;

}