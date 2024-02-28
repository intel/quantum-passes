// RUN: %clang %include_dirs -O1 -S -emit-llvm -Xclang -verify %loadintelclangextensions -o - %s

#include <clang/Quantum/qexpr.h>
#include <clang/Quantum/qlist.h>

QExpr bad_returns(qlist::QList qs) {  // expected-error {{QExpr functions must only include one return statement.}}
    if (true)  
        return qexpr::_X(qs[1]);
    else
        return qexpr::_H(qs[1]);
} 
