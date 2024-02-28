#include <clang/Quantum/quintrinsics.h>
#include <clang/Quantum/qexpr.h>

using namespace qexpr;

const int N=3;
qbit qs[N];

void power_rot_pos(){
    eval_hold(power(3, _RX(qs[0],1)));
}

void power_rot_neg(){
    eval_hold(power(-2, _RY(qs[0],1)));
}

void power_rot_1(){
    eval_hold(power(1, _RZ(qs[0],1)));
}

void power_rot_0(){
    eval_hold(power(0, _RZ(qs[0],1)));
}

void power_join(){
    eval_hold(power(3, _RX(qs[0],1)) * power(-2, _RX(qs[0],1)));
}

void join_power(){
    eval_hold(power(2, _RX(qs[0],1) * _PrepZ(qs[0])));
}