#include <clang/Quantum/quintrinsics.h>
#include <clang/Quantum/qexpr.h>
#include <clang/Quantum/qlist.h>
#include <quantum_full_state_simulator_backend.h>
#include <quantum.hpp>
#include <iostream>
#include <assert.h>

using namespace qexpr;
using namespace qlist;

#define PI_ 3.1415926535897932384626433832795

QExpr readoutState(iqsdk::FullStateSimulator *IQS, QList prepped, unsigned idx){
  std::vector<std::reference_wrapper<qbit>> q_refs;
  unsigned sz = prepped.size();
  for(int i = 0; i < sz; i++){
    q_refs.push_back(prepped[i]);
  }
  auto probability = IQS->getProbability(q_refs, iqsdk::QssIndex(sz, idx));
  std::cout<<"Prepped Probability: " << probability<<"\n";
  return identity();
}

QExpr prepTo(QList qs, unsigned to){
  unsigned sz = qs.size();
  return cIf(sz > 0,
           prepTo(qs >> 1, to)
           * _RX(qs[0], PI_ * (to >> (sz - 1) & 1)) 
           * _PrepZ(qs[0]),
           identity());
}

QExpr prepThenPrint(iqsdk::FullStateSimulator *IQS, QList qs, unsigned to){
  return readoutState(IQS, qs, to) >> prepTo(qs, to);
}

template<unsigned N>
bool check_mctrl(iqsdk::FullStateSimulator *IQS, unsigned init_to, unsigned ctrl_on){
  qbit listable(scratch, N);
  qbit flip;
  cbit out;
  eval_hold(  _MeasZ(flip, out)
            * control(scratch, ctrl_on, _X(flip)) 
            >> prepThenPrint(IQS, scratch, init_to) * _PrepZ(flip));
  return out;
}

const unsigned N = 4;
const unsigned TwoToN = 16;

int main() {

  iqsdk::IqsConfig iqs_config;
  iqsdk::FullStateSimulator iqs_device(iqs_config);
  iqsdk::QRT_ERROR_T status = iqs_device.ready();
  assert(status == iqsdk::QRT_ERROR_SUCCESS);

  bool TT[TwoToN][TwoToN] ={false};

  for(unsigned i = 0; i < TwoToN; i++){
    for(unsigned j = 0 ; j < TwoToN; j++){
      std::cout<<"starting "<< i << " " << j <<"\n";
      TT[i][j] = check_mctrl<N>(&iqs_device, i, j);
    }
  }
/*
  //print Truth table to screen
  std::cout<<"This is the Truth Table:\n";

  for(unsigned i = 0; i < TwoToN; i++){
    std::cout<<TT[i][0];
    for(unsigned j = 1 ; j < TwoToN; j++ ){
      std::cout<<", "<<TT[i][j];
    }
    std::cout << "\n";
  }
*/
}