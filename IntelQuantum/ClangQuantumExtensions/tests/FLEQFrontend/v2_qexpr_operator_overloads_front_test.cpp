// NOTE: Assertions have been autogenerated by utils/update_cc_test_checks.py UTC_ARGS: --version 3
// RUN: %clang %include_dirs -O1 -S -emit-llvm %loadintelclangextensions -o - %s | FileCheck %s --dump-input fail
#include <clang/Quantum/quintrinsics.h>
#include <clang/Quantum/qexpr.h>
#include <clang/Quantum/qlist.h>

qbit q1;
qbit listable(q2);
qbit listable(q3, 2);
qbit q4[2];

using namespace qexpr;
using namespace qlist;

// CHECK-LABEL: define dso_local noundef i32 @main(
// CHECK-SAME: ) local_unnamed_addr #[[ATTR4:[0-9]+]] {
// CHECK:    [[TMP1:%.*]] = tail call noundef nonnull dereferenceable(2) ptr @_ZN15quantum_builtin10__qlist_atEPtm(ptr noundef nonnull @q2_raw, i64 noundef 0) #[[ATTR5:[0-9]+]]
// CHECK-NEXT:    [[TMP2:%.*]] = tail call noundef i32 @_ZN15quantum_builtin9__qexpr_HEPt(ptr noundef nonnull [[TMP1]]) #[[ATTR5]]
// CHECK-NEXT:    [[TMP3:%.*]] = tail call noundef i32 @_ZN15quantum_builtin9__qexpr_HEPt(ptr noundef nonnull @q1) #[[ATTR5]]
// CHECK-NEXT:    [[TMP4:%.*]] = tail call noundef i32 @_ZN15quantum_builtin12__qexpr_joinEii(i32 noundef [[TMP2]], i32 noundef [[TMP3]]) #[[ATTR5]]
// CHECK-NEXT:    tail call void @_ZN15quantum_builtin17__qexpr_eval_holdEi(i32 noundef [[TMP4]]) #[[ATTR5]]
// CHECK-NEXT:    [[TMP5:%.*]] = tail call noundef i32 @_ZN15quantum_builtin9__qexpr_HEPt(ptr noundef nonnull @q1) #[[ATTR5]]
// CHECK-NEXT:    [[TMP6:%.*]] = tail call noundef nonnull dereferenceable(2) ptr @_ZN15quantum_builtin10__qlist_atEPtm(ptr noundef nonnull @q2_raw, i64 noundef 0) #[[ATTR5]]
// CHECK-NEXT:    [[TMP7:%.*]] = tail call noundef i32 @_ZN15quantum_builtin9__qexpr_HEPt(ptr noundef nonnull [[TMP6]]) #[[ATTR5]]
// CHECK-NEXT:    [[TMP8:%.*]] = tail call noundef i32 @_ZN15quantum_builtin12__qexpr_joinEii(i32 noundef [[TMP5]], i32 noundef [[TMP7]]) #[[ATTR5]]
// CHECK-NEXT:    tail call void @_ZN15quantum_builtin17__qexpr_eval_holdEi(i32 noundef [[TMP8]]) #[[ATTR5]]
// CHECK-NEXT:    [[TMP9:%.*]] = tail call noundef i32 @_ZN15quantum_builtin9__qexpr_HEPt(ptr noundef nonnull @q1) #[[ATTR5]]
// CHECK-NEXT:    [[TMP10:%.*]] = tail call noundef i32 @_ZN15quantum_builtin14__qexpr_invertEi(i32 noundef [[TMP9]]) #[[ATTR5]]
// CHECK-NEXT:    tail call void @_ZN15quantum_builtin17__qexpr_eval_holdEi(i32 noundef [[TMP10]]) #[[ATTR5]]
// CHECK-NEXT:    [[TMP11:%.*]] = tail call noundef i32 @_ZN15quantum_builtin9__qexpr_HEPt(ptr noundef nonnull @q1) #[[ATTR5]]
// CHECK-NEXT:    [[TMP12:%.*]] = tail call noundef i32 @_ZN15quantum_builtin14__qexpr_invertEi(i32 noundef [[TMP11]]) #[[ATTR5]]
// CHECK-NEXT:    tail call void @_ZN15quantum_builtin17__qexpr_eval_holdEi(i32 noundef [[TMP12]]) #[[ATTR5]]
// CHECK-NEXT:    [[TMP13:%.*]] = tail call noundef i32 @_ZN15quantum_builtin9__qexpr_HEPt(ptr noundef nonnull @q1) #[[ATTR5]]
// CHECK-NEXT:    [[TMP14:%.*]] = tail call noundef i32 @_ZN15quantum_builtin14__qexpr_invertEi(i32 noundef [[TMP13]]) #[[ATTR5]]
// CHECK-NEXT:    tail call void @_ZN15quantum_builtin17__qexpr_eval_holdEi(i32 noundef [[TMP14]]) #[[ATTR5]]
// CHECK-NEXT:    [[TMP15:%.*]] = tail call noundef i32 @_ZN15quantum_builtin9__qexpr_HEPt(ptr noundef nonnull @q1) #[[ATTR5]]
// CHECK-NEXT:    [[TMP16:%.*]] = tail call noundef i32 @_ZN15quantum_builtin13__qexpr_powerEji(i32 noundef 2, i32 noundef [[TMP15]]) #[[ATTR5]]
// CHECK-NEXT:    tail call void @_ZN15quantum_builtin17__qexpr_eval_holdEi(i32 noundef [[TMP16]]) #[[ATTR5]]
// CHECK-NEXT:    ret i32 0
//
int main() {

    eval_hold(_H(q1) * _H(q2[0]));
    eval_hold(_H(q1) + _H(q2[0]));

    eval_hold(!_H(q1)); //maybe but requires a cast
    eval_hold(~_H(q1)); // works as it translates to "power"
    eval_hold(-_H(q1));

    eval_hold(_H(q1)^2);

}
