#include "clang/Quantum/quintrinsics.h"

qbit q[5];
cbit c[3];

void quantum_kernel CbitEqualTest(){

       MeasZ(q[0], c[0]);
       MeasZ(q[1], c[0]);

       int i = 1;

       MeasZ(q[2], c[i]);
       MeasZ(q[3], c[i]);
       MeasZ(q[4], c[i+1]);
       MeasZ(q[0], c[i+1]);

     }

void quantum_kernel ParameterAccessTest(){
    RX(q[0], 0.1);
    MeasX(q[1], c[0]);
    MeasY(q[2], c[1]);
    MeasZ(q[3], c[2]);
    }


void quantum_kernel SwitchGateTest(){

    int i =1;
    MeasX(q[0], c[0]);
    MeasX(q[1], c[i]);

    }

void quantum_kernel InsertMeasTest(){

    H(q[0]);

    }

int main() {
    CbitEqualTest();
    ParameterAccessTest();
    SwitchGateTest();
    InsertMeasTest();
    return 0;
}