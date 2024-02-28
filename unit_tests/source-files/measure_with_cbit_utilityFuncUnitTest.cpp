#include "clang/Quantum/quintrinsics.h"

qbit q[5];
cbit c[3];
cbit test;

void quantum_kernel ParameterAccessTest(){
    RX(q[0], 0.1);
    MeasX(q[1], c[0]);
    MeasY(q[2], c[1]);
    MeasZ(q[3], c[2]);
    }


int main() {   
    ParameterAccessTest();
    return 0;
}