#include <clang/Quantum/quintrinsics.h>
#include <clang/Quantum/qexpr.h>

using namespace qexpr;

qbit reg1[5];

void getKey(char*, unsigned);

QExpr free_kernel_2() 
{ 
  return identity();
}

int main(){
  
    char key[9];
    getKey(key, 9);
    let(key, identity());
    eval_hold(get(key));

}