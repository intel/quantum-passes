#include <clang/Quantum/quintrinsics.h>
#include <clang/Quantum/qexpr.h>
#include <math.h>

using namespace qexpr;

const int N = 3;
qbit q[N];


double getAngle();

PROTECT QExpr prep0(){
    PrepZ(q[0]);
    return this_as_expr;
}

PROTECT QExpr invertibleQExpr1(){
    RX(q[0],M_PI/8);
    RY(q[0],M_PI/4);

    return this_as_expr;
}

PROTECT QExpr invertibleQExpr2(){
    T(q[1]);
    Y(q[1]);
    CNOT(q[0],q[1]);
    return this_as_expr;
}

PROTECT QExpr invertibleQExpr_dyn(double theta){
    RX(q[0], theta);
    RZ(q[0], 2*theta);
    return this_as_expr;
}

void invert1(){
  eval_hold(
    invert(invertibleQExpr1()));
}


void invert_join(){
    eval_hold(
        -(invertibleQExpr2() * invertibleQExpr1())
    );
}

void invert_join_invert(){
    eval_hold(
        -invertibleQExpr2() * -invertibleQExpr1()
    );
}

void invert_dynamic(){
    double theta = getAngle();
    eval_hold(
        invert(invertibleQExpr_dyn(theta))
    );
}

void invert_invert(){
    eval_hold(-(-invertibleQExpr1()));
}


QExpr conjugate(QExpr unitary, QExpr body){
    return unitary * body * -unitary;
}

void invert_surround(){
    eval_hold(conjugate(invertibleQExpr1(), prep0()));
}