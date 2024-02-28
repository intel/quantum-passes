#include <clang/Quantum/quintrinsics.h>
#include <math.h>
#include <vector>

const int N = 4;
qbit QubitReg[N];
cbit c[N];
quantum_shared_double_array QuantumVariableParams[8];

quantum_kernel void simpleRangeTest() {
  for (int Index = 0; Index < N; Index++)
    PrepZ(QubitReg[Index]);

  RX(QubitReg[0], M_PI);
  RY(QubitReg[1], 2.5 * M_PI);
  RZ(QubitReg[2], 3 * M_PI);

  RX(QubitReg[0], QuantumVariableParams[0]);
  RY(QubitReg[1], QuantumVariableParams[1]);
  RZ(QubitReg[2], QuantumVariableParams[7]);

  for (int Index = 0; Index < N; Index++)
    MeasZ(QubitReg[Index], c[Index]);
}

int main() {
  QuantumVariableParams[0] = M_PI;
  QuantumVariableParams[1] = 2.5 * M_PI;
  QuantumVariableParams[7] = 3 * M_PI;

  simpleRangeTest();
  return 0;
}

/* Testing of the intrinsics is covered in other Clang tests */
/* NOTE: IEE754 representation of double values :
  M_PI        = 3.14159265358979323846 --> 0x400921FB54442D18
  2.5 * M_PI  = 7.85398163397448309615 --> 0x401F6A7A2955385E
  3 * M_PI    = 9.42477796076937971538 --> 0x4022D97C7F3321D2
*/
