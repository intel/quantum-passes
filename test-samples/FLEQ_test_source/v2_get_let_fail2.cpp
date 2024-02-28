#include <clang/Quantum/quintrinsics.h>
#include <clang/Quantum/qexpr.h>

using namespace qexpr;

qbit reg1[5];

int getSize();

QExpr free_kernel_2() 
{ 
  return identity();
}


int main(){
    const char key[] = "IDENTITY";
    //let(key, identity());

    for(int i = 0; i < 3; i++){
      eval_hold(get(key));
      let(key, identity());
    }

}