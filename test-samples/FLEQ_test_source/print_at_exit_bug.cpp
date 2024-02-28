#include <clang/Quantum/quintrinsics.h>
#include <clang/Quantum/qexpr.h>


void q_before_exit(){
   qexpr::eval_hold(qexpr::printQuantumLogic(qexpr::exitAtCompile("Exit message\n")));
}

void d_before_exit(){
   qexpr::eval_hold(qexpr::printDataList("Print message\n", qexpr::exitAtCompile("Exit message\n")));
}

void print_branch_before_exit(bool b){
   qbit q; 
   qexpr::eval_hold(qexpr::printQuantumLogic(qexpr::cIf(b, qexpr::_H(q), qexpr::exitAtCompile("Exit message"))));
}