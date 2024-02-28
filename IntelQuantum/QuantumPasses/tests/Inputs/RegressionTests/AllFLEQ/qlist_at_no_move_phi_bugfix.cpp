#include <quantum_full_state_simulator_backend.h>
#include <clang/Quantum/qlist.h>
#include <vector>


const int N = 3;
qbit listable(q, N);

//This tests a bug that was found where FLEQ compilation was 
//replacing Qlist.at calls with GEPs. Due to some internal
//functionality, the replacement was causing some Phi nodes
//to be moved in inappropriate places and causing the IR 
//verifier to fail. 

int main() {

    std::vector<std::reference_wrapper<qbit>> qbit_refs;
    for (int i=0; i<N; i++) {
        qbit_refs.push_back(std::ref(q[i]));
    }
    qbit_refs.push_back(q[0]);
    qbit_refs.push_back(q[1]);
    qbit_refs.push_back(q[2]);
    
    return 0;
}