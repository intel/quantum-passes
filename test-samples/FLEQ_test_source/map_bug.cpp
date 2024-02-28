#include <clang/Quantum/quintrinsics.h>
#include <clang/Quantum/qexpr.h>

#include <iostream>
#include <cassert>
#include <quantum_full_state_simulator_backend.h>
#include <vector>


const int N = 4;
qbit listable(q, 4);

using namespace qexpr;
using namespace qlist;


QExpr prep_all(QList reg){
  return cIf(size(reg) > 0, 
           _PrepZ(reg[0]) * prep_all(reg + 1),
           identity());
}


template<typename QExprFun>
QExpr map (QExprFun f, QList qs){
    return cIf(size(qs) > 0,
                f(qs[0]) * map(f,qs+1),
                identity()
    );
}

template<typename... Args, typename... Arrays>
QExpr map_gen(QExpr(*qk)(qbit&, Args...), QList qs, Arrays... arrays) noexcept {
  return cIf(size(qs) > 0, 
           qk(qs[0], arrays[0]...) * map_gen(qk, qs + 1, (arrays + 1)...),
           identity()
  );        
}

//

int main() {

    //std::string dummy = "This should introduce invokes.";

    iqsdk::IqsConfig iqs_config;
    iqsdk::FullStateSimulator iqs_device(iqs_config);
    iqsdk::QRT_ERROR_T status = iqs_device.ready();
    assert(status == iqsdk::QRT_ERROR_SUCCESS);

    eval_hold(prep_all(q));
    eval_hold(map(_PrepZ, q));
    eval_hold(map_gen(_PrepZ, q));

    return 0;
}