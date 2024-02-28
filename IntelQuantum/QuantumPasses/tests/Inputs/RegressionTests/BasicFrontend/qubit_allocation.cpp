#include <clang/Quantum/quintrinsics.h>

/* <--- Main function : spot check ---> */
int main() {
  // CHECK: main
  qbit q1[1];
  // CHECK: alloca [1 x i16]
  qbit q2[2];
  // CHECK: alloca [2 x i16]
  qbit q4[4];
  // CHECK: alloca [4 x i16]
  qbit q8[8];
  // CHECK: alloca [8 x i16]
  qbit q16[16];
  // CHECK: alloca [16 x i16]
  qbit q32[32];
  // CHECK: alloca [32 x i16]
  qbit q64[64];
  // CHECK: alloca [64 x i16]
  qbit q128[128];
  // CHECK: alloca [128 x i16]
  qbit q256[256];
  // CHECK: alloca [256 x i16]
  qbit q512[512];
  // CHECK: alloca [512 x i16]
  qbit q1024[1024];
  // CHECK: alloca [1024 x i16]
  return 0;
  // CHECK: ret i32 0
}
