#include <clang/Quantum/quintrinsics.h>
#include <clang/Quantum/qexpr.h>

using namespace qexpr;

qbit reg1[5];

QExpr free_kernel_2() 
{ 
  return identity();
}


int main(){
    const char key[] = "IDENTITY";
    eval_hold(get(key));

}