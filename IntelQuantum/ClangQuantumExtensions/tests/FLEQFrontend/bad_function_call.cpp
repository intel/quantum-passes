// RUN: %clang %include_dirs -O1 -S -emit-llvm -Xclang -verify %loadintelclangextensions -o - %s

#include <clang/Quantum/quintrinsics.h>
#include <clang/Quantum/qexpr.h>

int addition(int a, int b);

int c, d = 0;

QExpr bad_operator(qlist::QList qs) {  
    return qexpr::_X(qs[1]) / addition(c, d); // expected-error {{function must return a QExpr}}
} 
