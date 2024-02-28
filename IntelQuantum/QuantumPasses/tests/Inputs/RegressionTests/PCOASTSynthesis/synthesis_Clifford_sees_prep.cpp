#include <clang/Quantum/quintrinsics.h>

#include <cmath>

qbit q;

const double PIby2 = 3.1415926535897932384626433832795/2;

quantum_kernel void prep_clif_test(){
  
  //MWE of a former bug.
  //when the two RZs get combined into a Clifford
  //a seg fault was happening when the PoPRCircuit class
  //considered joining it with the PrepZ. In that case
  //a comparison was happening between a const ParaRef
  //and a NULL ParaRef(default held by the Prep as a node),
  //which was the cause of the seg fault. 
  //a simple check for NULL ParaRefs in the 
  //affending function fixes the problem.
  PrepZ(q);
  RX(q, 0.1);
  RZ(q, PIby2/2);
  RZ(q, PIby2/2);
  RY(q, 0.2);
  RX(q, 0.1);

}
