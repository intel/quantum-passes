#include <clang/Quantum/quintrinsics.h>
#include <clang/Quantum/qexpr.h>
#include <clang/Quantum/qlist.h>
#include <clang/Quantum/datalist.h>

#include <iostream>

using namespace qexpr;
using namespace qlist;

void dummy(double[]);
void dummy(qbit[]);
void dummy(int[]);

int main() {
  
  double *darry = IQC_alloc<double>(DataList("darry"), 4);
  dummy(darry);
  qbit *qarry = IQC_alloc<qbit>("qarry", 10);
  dummy(qarry);
  int *iarry = IQC_alloc<int>("iarry", 100);
  dummy(iarry);


}