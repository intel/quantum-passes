#include <clang/Quantum/quintrinsics.h>
#include <clang/Quantum/qexpr.h>

using namespace qexpr;

qbit reg1[5];

QExpr free_kernel_2() 
{
   let("IDENTITY", identity());
   return get("IDENTITY");
}


int main(){
    const char key[] = "IDENTITY";
    let(key, identity());
    let("fk2",free_kernel_2());
    
    eval_hold(get("IDENTITY"));
    eval_hold(get(key));
    eval_hold(get("fk2"));
}