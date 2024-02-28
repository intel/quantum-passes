#include "clang/Quantum/quintrinsics.h"

qbit global_qreg[2];
cbit c_global[3];


// Pass a qubit array in by pointer 
void quantum_kernel pass_qarray_by_pointer(qbit *qubit_vec){
    // no way to get the size unfortunately 
    for (int i=0; i < 2; i++)
        H(qubit_vec[i]);
}

// Pass a single qubit in by reference
void quantum_kernel pass_qbit_by_reference(qbit &q){
    X(q);
}


void quantum_kernel kernel1(){
    qbit local_qreg[2];
    qbit local_qbit0;
    qbit local_qbit1;
    cbit local_c[4];

    PrepZ(local_qreg[0]);
    PrepZ(local_qreg[1]);
    PrepZ(local_qbit0);
    PrepZ(local_qbit1);
    PrepZ(global_qreg[0]);
    PrepZ(global_qreg[1]);

    X(local_qreg[0]);
    Z(local_qreg[1]);
    H(local_qbit0);
    Y(local_qbit1);

    RX(global_qreg[0], 0.5);
    RY(global_qreg[1], 0.5);

    pass_qbit_by_reference(local_qbit0);
    pass_qbit_by_reference(local_qbit1);
    pass_qbit_by_reference(local_qreg[0]);
    pass_qbit_by_reference(local_qreg[1]);
    pass_qbit_by_reference(global_qreg[0]);
    pass_qbit_by_reference(global_qreg[1]);

    pass_qarray_by_pointer(global_qreg);
    pass_qarray_by_pointer(local_qreg);

    MeasX(global_qreg[0], c_global[0]);
    MeasX(global_qreg[1], c_global[1]);
    MeasZ(local_qreg[0], local_c[0]);
    MeasZ(local_qreg[1], local_c[1]);
    MeasZ(local_qbit0, local_c[2]);
    MeasZ(local_qbit1, local_c[3]);

}


void quantum_kernel kernel2(){
    qbit local_qreg[2];
    qbit local_qbit0;
    qbit local_qbit1;
    cbit local_c[4];

    PrepZ(local_qreg[0]);
    PrepZ(local_qreg[1]);
    PrepZ(local_qbit0);
    PrepZ(local_qbit1);
    PrepZ(global_qreg[0]);
    PrepZ(global_qreg[1]);

    X(local_qreg[0]);
    Z(local_qreg[1]);
    H(local_qbit0);
    Y(local_qbit1);

    RX(global_qreg[0], 0.5);
    RY(global_qreg[1], 0.5);

    pass_qbit_by_reference(local_qbit0);
    pass_qbit_by_reference(local_qbit1);
    pass_qbit_by_reference(local_qreg[0]);
    pass_qbit_by_reference(local_qreg[1]);
    pass_qbit_by_reference(global_qreg[0]);
    pass_qbit_by_reference(global_qreg[1]);

    pass_qarray_by_pointer(global_qreg);
    pass_qarray_by_pointer(local_qreg);

    release_quantum_state();

}


int main(){
    kernel1();
    kernel2();

	return 0;
}
