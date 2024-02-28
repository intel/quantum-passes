#include <clang/Quantum/quintrinsics.h>
#include <clang/Quantum/qexpr.h>
#include <clang/Quantum/qlist.h>

qbit q1;
qbit listable(q2);
qbit listable(q3, 2);
qbit q4[2];

using namespace qexpr;
using namespace qlist;

int main() {
    
    eval_hold(_H(q1) * _H(q2[0]));
    eval_hold(_H(q1) + _H(q2[0]));
  
    //eval_hold(!_H(q1)); //maybe but requires a cast 
    eval_hold(~_H(q1)); // works as it translates to "power"
    eval_hold(-_H(q1)); 
    
    eval_hold(_H(q1)^2);

}