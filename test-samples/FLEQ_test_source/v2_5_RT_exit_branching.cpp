#include <clang/Quantum/quintrinsics.h>
#include <quantum.hpp>
#include <clang/Quantum/qexpr.h>
#include <clang/Quantum/qlist.h>

using namespace qexpr;
using namespace qlist;

qbit listable(q ,3);


QExpr CTFailWithBranch(bool b){
  return cIf(b, 
             _H(q[0]),
             _H(q[1]) * exitAtRuntime("This failed because the condition evaluated to 'false'.\n"));
}

PROTECT bool doWeBranch() { return false; };

int main(){
  
  iqsdk::IqsConfig iqs_config;
  iqsdk::FullStateSimulator iqs_device(iqs_config);
  iqsdk::QRT_ERROR_T status = iqs_device.ready();

  bool do_we_branch = false;
  std::cout<<"Which way do we branch?\n";
  std::cin >> do_we_branch;

  eval_hold(CTFailWithBranch(do_we_branch));

}