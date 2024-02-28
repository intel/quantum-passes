#include <clang/Quantum/quintrinsics.h>
#include <quantum.hpp>
#include <clang/Quantum/qexpr.h>
#include <clang/Quantum/qlist.h>

using namespace qexpr;
using namespace qlist;

qbit listable(q ,3);

namespace qrt0 {
void exit(char *msg);
}; // namespace qrt0

QExpr CTFailWithBranch(bool b){
  return cIf(b, 
             _H(q[0]),
             _H(q[1]) * exitAtRuntime("This failed because the condition evaluated to 'false'.\n"));
}

int main(){

  iqsdk::IqsConfig iqs_config;
  iqsdk::FullStateSimulator iqs_device(iqs_config);
  iqsdk::QRT_ERROR_T status = iqs_device.ready();

  eval_hold(CTFailWithBranch(false));

}