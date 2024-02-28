// <--- Test front-end flags --->
// REQUIRES: quantum-registered-target
// RUN: %clang_cc1 -triple quantum32-unknown-unknown -fquantum-is-device -std=c++11 -S -emit-llvm %s -o - | FileCheck -check-prefix=DEVICE32 %s
// RUN: %clang_cc1 -triple quantum64-unknown-unknown -fquantum-is-device -std=c++11 -S -emit-llvm %s -o - | FileCheck -check-prefix=DEVICE64 %s
// RUN: %clang_cc1 -triple x86_64-unknown-linux-gnu -std=c++11 -S -emit-llvm %s -o - | FileCheck -check-prefix=HOST %s

// DEVICE32: target triple = "quantum32-unknown-unknown"
// DEVICE64: target triple = "quantum64-unknown-unknown"
// HOST: target triple = "x86_64-unknown-linux-gnu"

int main() {
  return 0;
}
