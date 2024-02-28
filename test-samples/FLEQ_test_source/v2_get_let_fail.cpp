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
    let(key, identity());
    let("IDENTITY",free_kernel_2());
    
    eval_hold(get("IDENTITY"));

}