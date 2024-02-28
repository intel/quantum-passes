// RUN: %clang %include_dirs -O1 -S -emit-llvm -Xclang -verify %loadintelclangextensions -o - %s

#include <clang/Quantum/quintrinsics.h>
#include <clang/Quantum/qexpr.h>

QExpr bad_operator(qlist::QList qs) {  
    return qexpr::_X(qs[1]) / qexpr::_X(qs[1]); // expected-error {{invalid operator for QExpr operations}}
} 
