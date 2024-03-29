// NOTE: Assertions have been autogenerated by utils/update_cc_test_checks.py UTC_ARGS: --version 3
// RUN: %clang %include_dirs -O1 -S -emit-llvm %loadintelclangextensions -o - %s | FileCheck %s --dump-input fail
#include <clang/Quantum/quintrinsics.h>
#include <clang/Quantum/qexpr.h>
#include <clang/Quantum/qlist.h>

qbit listable(q1, 5);
qbit listable(q2, 3);
qbit listable(q3, 2);

using namespace qexpr;
using namespace qlist;

// CHECK-LABEL: define dso_local noundef i32 @_Z6HOnAllN5qlist5QListE(
// CHECK-SAME: ptr [[TMP0:%.*]]) local_unnamed_addr #[[ATTR4:[0-9]+]] {
// CHECK:    [[TMP2:%.*]] = tail call noundef i64 @_ZN15quantum_builtin12__qlist_sizeEPt(ptr noundef [[TMP0]]) #[[ATTR7:[0-9]+]]
// CHECK-NEXT:    [[TMP3:%.*]] = icmp ne i64 [[TMP2]], 0
// CHECK-NEXT:    [[TMP4:%.*]] = getelementptr inbounds i16, ptr [[TMP0]], i64 1
// CHECK-NEXT:    [[TMP5:%.*]] = tail call noundef i32 @_Z6HOnAllN5qlist5QListE(ptr nonnull [[TMP4]])
// CHECK-NEXT:    [[TMP6:%.*]] = tail call noundef nonnull dereferenceable(2) ptr @_ZN15quantum_builtin10__qlist_atEPtm(ptr noundef [[TMP0]], i64 noundef 0) #[[ATTR7]]
// CHECK-NEXT:    [[TMP7:%.*]] = tail call noundef i32 @_ZN15quantum_builtin9__qexpr_HEPt(ptr noundef nonnull [[TMP6]]) #[[ATTR7]]
// CHECK-NEXT:    [[TMP8:%.*]] = tail call noundef i32 @_ZN15quantum_builtin12__qexpr_joinEii(i32 noundef [[TMP5]], i32 noundef [[TMP7]]) #[[ATTR7]]
// CHECK-NEXT:    [[TMP9:%.*]] = tail call noundef i32 @_ZN15quantum_builtin16__qexpr_identityEd(double noundef 0.000000e+00) #[[ATTR7]]
// CHECK-NEXT:    [[TMP10:%.*]] = tail call noundef i32 @_ZN15quantum_builtin14__qexpr_branchEbii(i1 noundef zeroext [[TMP3]], i32 noundef [[TMP8]], i32 noundef [[TMP9]]) #[[ATTR7]]
// CHECK-NEXT:    ret i32 [[TMP10]]
//
QExpr HOnAll(QList reg){
  return cIf(reg.size() > 0,
             _H(reg[0]) * HOnAll(++reg),
             identity());
}

// CHECK-LABEL: define dso_local noundef i32 @main(
// CHECK-SAME: ) local_unnamed_addr #[[ATTR6:[0-9]+]] {
// CHECK:    [[TMP1:%.*]] = tail call noundef ptr @_ZN15quantum_builtin13__qlist_sliceEPtmm(ptr noundef nonnull @q1_raw, i64 noundef 1, i64 noundef 3) #[[ATTR7]]
// CHECK-NEXT:    [[TMP2:%.*]] = tail call noundef ptr @_ZN15quantum_builtin12__qlist_joinEPtS0_(ptr noundef nonnull @q1_raw, ptr noundef nonnull @q2_raw) #[[ATTR7]]
// CHECK-NEXT:    [[TMP3:%.*]] = tail call noundef ptr @_ZN15quantum_builtin12__qlist_joinEPtS0_(ptr noundef [[TMP2]], ptr noundef nonnull getelementptr inbounds ([2 x i16], ptr @q3_raw, i64 0, i64 1)) #[[ATTR7]]
// CHECK-NEXT:    [[TMP4:%.*]] = tail call noundef i64 @_ZN15quantum_builtin12__qlist_sizeEPt(ptr noundef nonnull @q3_raw) #[[ATTR7]]
// CHECK-NEXT:    [[TMP5:%.*]] = add i64 [[TMP4]], -2
// CHECK-NEXT:    [[TMP6:%.*]] = tail call noundef ptr @_ZN15quantum_builtin13__qlist_sliceEPtmm(ptr noundef nonnull @q3_raw, i64 noundef 0, i64 noundef [[TMP5]]) #[[ATTR7]]
// CHECK-NEXT:    [[TMP7:%.*]] = tail call noundef ptr @_ZN15quantum_builtin12__qlist_joinEPtS0_(ptr noundef [[TMP1]], ptr noundef [[TMP6]]) #[[ATTR7]]
// CHECK-NEXT:    [[TMP8:%.*]] = tail call noundef i64 @_ZN15quantum_builtin12__qlist_sizeEPt(ptr noundef [[TMP7]]) #[[ATTR7]]
// CHECK-NEXT:    [[TMP9:%.*]] = tail call noundef i64 @_ZN15quantum_builtin12__qlist_sizeEPt(ptr noundef [[TMP3]]) #[[ATTR7]]
// CHECK-NEXT:    [[TMP10:%.*]] = tail call noundef nonnull dereferenceable(2) ptr @_ZN15quantum_builtin10__qlist_atEPtm(ptr noundef nonnull @q1_raw, i64 noundef 2) #[[ATTR7]]
// CHECK-NEXT:    [[TMP11:%.*]] = tail call noundef i32 @_ZN15quantum_builtin9__qexpr_HEPt(ptr noundef nonnull [[TMP10]]) #[[ATTR7]]
// CHECK-NEXT:    tail call void @_ZN15quantum_builtin17__qexpr_eval_holdEi(i32 noundef [[TMP11]]) #[[ATTR7]]
// CHECK-NEXT:    ret i32 0
//
int main() {

  auto q1slice = q1(1, 3);
  auto q1join2 = q1 + q2;
  auto qsljoin1 = q1join2 + (q3 >> 1);
  auto qsljoin2 = q1slice + (q3 << 2);

  ++q3; //== q3 >> 1

  qsljoin2.size();
  size(qsljoin1);

  eval_hold(_H(q1[2]));

}
