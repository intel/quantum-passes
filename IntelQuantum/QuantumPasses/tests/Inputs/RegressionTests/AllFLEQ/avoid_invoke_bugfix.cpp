#include <clang/Quantum/quintrinsics.h>
#include <clang/Quantum/qexpr.h>

#include <iostream>
#include <cassert>
#include <vector>

//This test is to verify a bug fix were the intermediate
//"quantum_builtin" FLEQ functions were being called in
//IR via an "invoke" instruction and thus being ignored
//by pass which replaced them with intrinsics.
//The fix is done in the header declaration of these
//functions by declaring them as "noexcept". 

const int N = 4;
qbit q[N];

int main() {

    std::string test = "this is a string.";

    qexpr::eval_hold(qexpr::_H(q[0]));

    return 0;
}