#include <clang/Quantum/qexpr.h>
#include <clang/Quantum/qlist.h>
#include <clang/Quantum/datalist.h>

using namespace qexpr;
using namespace qlist;
using namespace datalist;

const bool datalistEq(const DataList &a, const DataList &b) {
    return a.contains(b) && b.contains(a);
    //return a.contains(b);
}

QExpr prep(const DataList &src, qbit& q) {
    // Expect src to have the form "x" where
    // state ::= 0 | 1 | + | -
    return
        cIf(datalistEq(src, DataList("0")), _PrepZ(q),
        cIf(datalistEq(src, DataList("1")), _PrepZ(q) + _X(q),
        cIf(datalistEq(src, DataList("+")), _PrepZ(q) + _H(q),
        cIf(datalistEq(src, DataList("-")), _PrepZ(q) + _X(q) + _H(q),
        exitAtCompile("Expected state of the form 0 | 1 | + | -")
        ))));
}

int main() {

    qbit q;
    eval_hold(prep(DataList("0"), q)); // Gives PHINode error

}