#include <clang/Quantum/quintrinsics.h>
#include <clang/Quantum/qexpr.h>

using namespace qexpr;

const int N = 4;
qbit q[N];

void freeH_test(){
  eval_hold(_H(q[0]));
}

void freeX_test(){
  eval_hold(_X(q[0]));
}

void freeY_test(){
  eval_hold(_Y(q[0]));
}

void freeZ_test(){
  eval_hold(_Z(q[0]));
}

void freeS_test(){
  eval_hold(_S(q[0]));
}

void freeSdag_test(){
  eval_hold(_Sdag(q[0]));
}

void freeT_test(){
  eval_hold(_T(q[0]));
}

void freeTdag_test(){
  eval_hold(_Tdag(q[0]));
}

void freeRX_test(double ang){
  eval_hold(_RX(q[0], ang));
}

void freeRY_test(double ang){
  eval_hold(_RY(q[0], ang));
}  

void freeRZ_test(double ang){
  eval_hold(_RZ(q[0], ang));
}  

void freeCZ_test(){
  eval_hold(_CZ(q[0], q[1]));
}

void freeCNOT_test(){
  eval_hold(_CNOT(q[0], q[1]));
}

void freeSWAP_test(){
  eval_hold(_SWAP(q[0], q[1]));
}

void freeToffoli_test(){
  eval_hold(_Toffoli(q[0], q[1], q[2]));
}

void freePrepX_test(){
  eval_hold(_PrepX(q[0]));
}  

void freePrepY_test(){
  eval_hold(_PrepY(q[0]));
} 

void freePrepZ_test(){
  eval_hold(_PrepZ(q[0]));
}

void freeMeasX_test(cbit &c){
  eval_hold(_MeasX(q[0], c));
} 

void freeMeasY_test(cbit &c){
  eval_hold(_MeasY(q[0], c));
} 

void freeMeasZ_test(cbit &c){
  eval_hold(_MeasZ(q[0], c));
}

void freeCPhase_test(double a){
  eval_hold(_CPhase(q[0], q[1], a));
} 

//void freeSWAPA_test(double a){
//  eval_hold(_SwapA(q[0], q[1], a));
//}

void freeRXY_test(double a, double b){
  eval_hold(_RXY(q[0], a, b));
}