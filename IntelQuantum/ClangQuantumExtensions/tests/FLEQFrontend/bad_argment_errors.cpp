// RUN: %clang %include_dirs -O1 -S -emit-llvm -Xclang -verify %loadintelclangextensions -o - %s

#include <clang/Quantum/quintrinsics.h>
#include <clang/Quantum/qexpr.h>

QExpr controlledNot(qlist::QList qs) {  
    return qexpr::control(0, 120); // expected-error {{value is not a QExpr expression}}
}

QExpr wrappedInvert(qlist::QList qs) {  
    return qexpr::invert(123); // expected-error {{value is not a QExpr expression}}
}

QExpr powered1(qlist::QList qs) {  
    return qexpr::power(qexpr::_X(qs[1]), 2); // expected-error {{value is not a QExpr expression}}
} 

QExpr powered2(qlist::QList qs) {  
    return qexpr::power(qexpr::_X(qs[1]), qexpr::_X(qs[1]));
} 

QExpr powered3(qlist::QList qs) {  
    return qexpr::power(5, 2); // expected-error {{value is not a QExpr expression}}
} 