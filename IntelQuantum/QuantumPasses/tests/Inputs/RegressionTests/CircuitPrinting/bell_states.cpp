#include <clang/Quantum/quintrinsics.h>
#include <quantum_full_state_simulator_backend.h>

#include <iostream> 

/// Define our quantum resources
const int N = 4;                
qbit q[N];                      
cbit c[N];

quantum_kernel void BellPrep () {
  // Index to loop over later
  int Index = 0;
  
  // preparation of Bell pairs (T -> Infinity)
  for (Index = 0; Index < 2; Index++)
    RY(q[Index], 1.57079632679);
  for (Index = 0; Index < 2; Index++)
    CNOT(q[Index], q[Index + 2]);
}

// Quantum kernel for state preparation
quantum_kernel void state_preparation() {
  for (int i = 0; i < N; ++i)
      PrepZ(q[i]);
  
  BellPrep();
}

int main() {
    /// Setup quantum device
    iqsdk::IqsConfig iqs_config(/*num_qubits*/ N,
                                 /*simulation_type*/ "noiseless");
    iqsdk::FullStateSimulator iqs_device(iqs_config);

    if (iqsdk::QRT_ERROR_SUCCESS != iqs_device.ready())
      return 1;

    state_preparation();
}