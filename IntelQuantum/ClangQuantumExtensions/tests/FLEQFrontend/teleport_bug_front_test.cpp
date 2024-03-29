// NOTE: Assertions have been autogenerated by utils/update_cc_test_checks.py UTC_ARGS: --version 3
// RUN: %clang %include_dirs -O1 -S -emit-llvm %loadintelclangextensions -o - %s | FileCheck %s --dump-input fail
#include <clang/Quantum/quintrinsics.h>
#include <clang/Quantum/qexpr.h>

// This bug demonstrated two different problems:
// First, the reversing of direction for binds and joins meant that IR generation
// put instructions in an order such that LLVM optimizations did not recoginze that
// that the use of measurements by "bob" were dependent on the measurements being
// populated by "alice". Thus the initialization of the measurement bits was being
// propogated to the conditional, resulting in no branching being generated by FLEQ.
// Once the was resolved, it was also found that classical instructions attached to
// binds where not beinghandling properly. In this case, the instruction for loading
// the measurement results from "alice" and passing them to "bob" were being inserted
// at the begin of the eval_hold call instead of between the QBBs represnting "alice"
// and "bob".

// Prepare a Bell00 state |00> + |11>.
// CHECK-LABEL: define dso_local noundef i32 @_Z6bell00RtS_(
// CHECK-SAME: ptr noundef nonnull align 2 dereferenceable(2) [[TMP0:%.*]], ptr noundef nonnull align 2 dereferenceable(2) [[TMP1:%.*]]) local_unnamed_addr #[[ATTR4:[0-9]+]] {
// CHECK:    [[TMP3:%.*]] = tail call noundef i32 @_ZN15quantum_builtin13__qexpr_PrepZEPt(ptr noundef nonnull [[TMP0]]) #[[ATTR7:[0-9]+]]
// CHECK-NEXT:    [[TMP4:%.*]] = tail call noundef i32 @_ZN15quantum_builtin13__qexpr_PrepZEPt(ptr noundef nonnull [[TMP1]]) #[[ATTR7]]
// CHECK-NEXT:    [[TMP5:%.*]] = tail call noundef i32 @_ZN15quantum_builtin12__qexpr_joinEii(i32 noundef [[TMP3]], i32 noundef [[TMP4]]) #[[ATTR7]]
// CHECK-NEXT:    [[TMP6:%.*]] = tail call noundef i32 @_ZN15quantum_builtin9__qexpr_HEPt(ptr noundef nonnull [[TMP0]]) #[[ATTR7]]
// CHECK-NEXT:    [[TMP7:%.*]] = tail call noundef i32 @_ZN15quantum_builtin12__qexpr_joinEii(i32 noundef [[TMP5]], i32 noundef [[TMP6]]) #[[ATTR7]]
// CHECK-NEXT:    [[TMP8:%.*]] = tail call noundef i32 @_ZN15quantum_builtin12__qexpr_CNOTEPtS0_(ptr noundef nonnull [[TMP0]], ptr noundef nonnull [[TMP1]]) #[[ATTR7]]
// CHECK-NEXT:    [[TMP9:%.*]] = tail call noundef i32 @_ZN15quantum_builtin12__qexpr_joinEii(i32 noundef [[TMP7]], i32 noundef [[TMP8]]) #[[ATTR7]]
// CHECK-NEXT:    ret i32 [[TMP9]]
//
PROTECT QExpr bell00(qbit& a, qbit& b) {
    return qexpr::_PrepZ(a)
         + qexpr::_PrepZ(b)
         + qexpr::_H(a)
         + qexpr::_CNOT(a,b);
}


// Entangle q and a and measure both, writing the results to x and y
// respectively.
// CHECK-LABEL: define dso_local noundef i32 @_Z5aliceRtS_RbS0_(
// CHECK-SAME: ptr noundef nonnull align 2 dereferenceable(2) [[TMP0:%.*]], ptr noundef nonnull align 2 dereferenceable(2) [[TMP1:%.*]], ptr noundef nonnull align 1 dereferenceable(1) [[TMP2:%.*]], ptr noundef nonnull align 1 dereferenceable(1) [[TMP3:%.*]]) local_unnamed_addr #[[ATTR4]] {
// CHECK:    [[TMP5:%.*]] = tail call noundef i32 @_ZN15quantum_builtin12__qexpr_CNOTEPtS0_(ptr noundef nonnull [[TMP0]], ptr noundef nonnull [[TMP1]]) #[[ATTR7]]
// CHECK-NEXT:    [[TMP6:%.*]] = tail call noundef i32 @_ZN15quantum_builtin9__qexpr_HEPt(ptr noundef nonnull [[TMP0]]) #[[ATTR7]]
// CHECK-NEXT:    [[TMP7:%.*]] = tail call noundef i32 @_ZN15quantum_builtin12__qexpr_joinEii(i32 noundef [[TMP5]], i32 noundef [[TMP6]]) #[[ATTR7]]
// CHECK-NEXT:    [[TMP8:%.*]] = tail call noundef i32 @_ZN15quantum_builtin13__qexpr_MeasZEPtPb(ptr noundef nonnull [[TMP0]], ptr noundef nonnull [[TMP2]]) #[[ATTR7]]
// CHECK-NEXT:    [[TMP9:%.*]] = tail call noundef i32 @_ZN15quantum_builtin12__qexpr_joinEii(i32 noundef [[TMP7]], i32 noundef [[TMP8]]) #[[ATTR7]]
// CHECK-NEXT:    [[TMP10:%.*]] = tail call noundef i32 @_ZN15quantum_builtin13__qexpr_MeasZEPtPb(ptr noundef nonnull [[TMP1]], ptr noundef nonnull [[TMP3]]) #[[ATTR7]]
// CHECK-NEXT:    [[TMP11:%.*]] = tail call noundef i32 @_ZN15quantum_builtin12__qexpr_joinEii(i32 noundef [[TMP9]], i32 noundef [[TMP10]]) #[[ATTR7]]
// CHECK-NEXT:    ret i32 [[TMP11]]
//
PROTECT QExpr alice(qbit& q, qbit& a, bool& x, bool& y) {
    return qexpr::_CNOT(q, a)
         + qexpr::_H(q)
         + qexpr::_MeasZ(q, x)
         + qexpr::_MeasZ(a, y);
}

// Use x and y to apply corrections to the qubit b.
// CHECK-LABEL: define dso_local noundef i32 @_Z3bobRtRbS0_(
// CHECK-SAME: ptr noundef nonnull align 2 dereferenceable(2) [[TMP0:%.*]], ptr nocapture noundef nonnull readonly align 1 dereferenceable(1) [[TMP1:%.*]], ptr nocapture noundef nonnull readonly align 1 dereferenceable(1) [[TMP2:%.*]]) local_unnamed_addr #[[ATTR4]] {
// CHECK:    [[TMP4:%.*]] = load i8, ptr [[TMP2]], align 1, !tbaa [[TBAA10:![0-9]+]], !range [[RNG12:![0-9]+]], !noundef !13
// CHECK-NEXT:    [[TMP5:%.*]] = icmp ne i8 [[TMP4]], 0
// CHECK-NEXT:    [[TMP6:%.*]] = tail call noundef i32 @_ZN15quantum_builtin9__qexpr_XEPt(ptr noundef nonnull [[TMP0]]) #[[ATTR7]]
// CHECK-NEXT:    [[TMP7:%.*]] = tail call noundef i32 @_ZN15quantum_builtin16__qexpr_identityEd(double noundef 0.000000e+00) #[[ATTR7]]
// CHECK-NEXT:    [[TMP8:%.*]] = tail call noundef i32 @_ZN15quantum_builtin14__qexpr_branchEbii(i1 noundef zeroext [[TMP5]], i32 noundef [[TMP6]], i32 noundef [[TMP7]]) #[[ATTR7]]
// CHECK-NEXT:    [[TMP9:%.*]] = load i8, ptr [[TMP1]], align 1, !tbaa [[TBAA10]], !range [[RNG12]], !noundef !13
// CHECK-NEXT:    [[TMP10:%.*]] = icmp ne i8 [[TMP9]], 0
// CHECK-NEXT:    [[TMP11:%.*]] = tail call noundef i32 @_ZN15quantum_builtin9__qexpr_ZEPt(ptr noundef nonnull [[TMP0]]) #[[ATTR7]]
// CHECK-NEXT:    [[TMP12:%.*]] = tail call noundef i32 @_ZN15quantum_builtin16__qexpr_identityEd(double noundef 0.000000e+00) #[[ATTR7]]
// CHECK-NEXT:    [[TMP13:%.*]] = tail call noundef i32 @_ZN15quantum_builtin14__qexpr_branchEbii(i1 noundef zeroext [[TMP10]], i32 noundef [[TMP11]], i32 noundef [[TMP12]]) #[[ATTR7]]
// CHECK-NEXT:    [[TMP14:%.*]] = tail call noundef i32 @_ZN15quantum_builtin12__qexpr_joinEii(i32 noundef [[TMP8]], i32 noundef [[TMP13]]) #[[ATTR7]]
// CHECK-NEXT:    ret i32 [[TMP14]]
//
PROTECT QExpr bob(qbit& b, bool &x, bool &y) {
    return qexpr::cIf(y, qexpr::_X(b), qexpr::identity())
         + qexpr::cIf(x, qexpr::_Z(b), qexpr::identity());
}

// CHECK-LABEL: define dso_local noundef i32 @_Z14teleport1_bindRtS_S_(
// CHECK-SAME: ptr noundef nonnull align 2 dereferenceable(2) [[TMP0:%.*]], ptr noundef nonnull align 2 dereferenceable(2) [[TMP1:%.*]], ptr noundef nonnull align 2 dereferenceable(2) [[TMP2:%.*]]) local_unnamed_addr #[[ATTR4]] {
// CHECK:    [[TMP4:%.*]] = alloca i8, align 1
// CHECK-NEXT:    [[TMP5:%.*]] = alloca i8, align 1
// CHECK-NEXT:    call void @llvm.lifetime.start.p0(i64 1, ptr nonnull [[TMP4]]) #[[ATTR7]]
// CHECK-NEXT:    store i8 0, ptr [[TMP4]], align 1, !tbaa [[TBAA10]]
// CHECK-NEXT:    call void @llvm.lifetime.start.p0(i64 1, ptr nonnull [[TMP5]]) #[[ATTR7]]
// CHECK-NEXT:    store i8 0, ptr [[TMP5]], align 1, !tbaa [[TBAA10]]
// CHECK-NEXT:    [[TMP6:%.*]] = tail call noundef i32 @_Z6bell00RtS_(ptr noundef nonnull align 2 dereferenceable(2) [[TMP1]], ptr noundef nonnull align 2 dereferenceable(2) [[TMP2]])
// CHECK-NEXT:    [[TMP7:%.*]] = call noundef i32 @_Z5aliceRtS_RbS0_(ptr noundef nonnull align 2 dereferenceable(2) [[TMP0]], ptr noundef nonnull align 2 dereferenceable(2) [[TMP1]], ptr noundef nonnull align 1 dereferenceable(1) [[TMP4]], ptr noundef nonnull align 1 dereferenceable(1) [[TMP5]])
// CHECK-NEXT:    [[TMP8:%.*]] = call noundef i32 @_ZN15quantum_builtin12__qexpr_joinEii(i32 noundef [[TMP6]], i32 noundef [[TMP7]]) #[[ATTR7]]
// CHECK-NEXT:    [[TMP9:%.*]] = call noundef i32 @_Z3bobRtRbS0_(ptr noundef nonnull align 2 dereferenceable(2) [[TMP2]], ptr noundef nonnull align 1 dereferenceable(1) [[TMP4]], ptr noundef nonnull align 1 dereferenceable(1) [[TMP5]])
// CHECK-NEXT:    [[TMP10:%.*]] = call noundef i32 @_ZN15quantum_builtin12__qexpr_bindEii(i32 noundef [[TMP8]], i32 noundef [[TMP9]]) #[[ATTR7]]
// CHECK-NEXT:    call void @llvm.lifetime.end.p0(i64 1, ptr nonnull [[TMP5]]) #[[ATTR7]]
// CHECK-NEXT:    call void @llvm.lifetime.end.p0(i64 1, ptr nonnull [[TMP4]]) #[[ATTR7]]
// CHECK-NEXT:    ret i32 [[TMP10]]
//
PROTECT QExpr teleport1_bind(qbit& q, qbit& a, qbit& b) {
    bool x = false;
    bool y = false;
    return (bell00(a,b) + alice(q, a, x, y))
            << bob(b,x,y);
}

double getAngle();

// CHECK-LABEL: define dso_local void @_Z14teleport1_bindv(
// CHECK-SAME: ) local_unnamed_addr #[[ATTR6:[0-9]+]] {
// CHECK:    [[TMP1:%.*]] = alloca i16, align 2
// CHECK-NEXT:    [[TMP2:%.*]] = alloca i16, align 2
// CHECK-NEXT:    [[TMP3:%.*]] = alloca i16, align 2
// CHECK-NEXT:    call void @llvm.lifetime.start.p0(i64 2, ptr nonnull [[TMP1]]) #[[ATTR7]]
// CHECK-NEXT:    call void @llvm.lifetime.start.p0(i64 2, ptr nonnull [[TMP2]]) #[[ATTR7]]
// CHECK-NEXT:    call void @llvm.lifetime.start.p0(i64 2, ptr nonnull [[TMP3]]) #[[ATTR7]]
// CHECK-NEXT:    [[TMP4:%.*]] = call noundef i32 @_ZN15quantum_builtin13__qexpr_PrepZEPt(ptr noundef nonnull [[TMP1]]) #[[ATTR7]]
// CHECK-NEXT:    [[TMP5:%.*]] = call noundef double @_Z8getAnglev()
// CHECK-NEXT:    [[TMP6:%.*]] = call noundef i32 @_ZN15quantum_builtin10__qexpr_RXEPtd(ptr noundef nonnull [[TMP1]], double noundef [[TMP5]]) #[[ATTR7]]
// CHECK-NEXT:    [[TMP7:%.*]] = call noundef i32 @_ZN15quantum_builtin12__qexpr_joinEii(i32 noundef [[TMP4]], i32 noundef [[TMP6]]) #[[ATTR7]]
// CHECK-NEXT:    [[TMP8:%.*]] = call noundef i32 @_Z14teleport1_bindRtS_S_(ptr noundef nonnull align 2 dereferenceable(2) [[TMP1]], ptr noundef nonnull align 2 dereferenceable(2) [[TMP2]], ptr noundef nonnull align 2 dereferenceable(2) [[TMP3]])
// CHECK-NEXT:    [[TMP9:%.*]] = call noundef i32 @_ZN15quantum_builtin12__qexpr_bindEii(i32 noundef [[TMP7]], i32 noundef [[TMP8]]) #[[ATTR7]]
// CHECK-NEXT:    call void @_ZN15quantum_builtin17__qexpr_eval_holdEi(i32 noundef [[TMP9]]) #[[ATTR7]]
// CHECK-NEXT:    call void @llvm.lifetime.end.p0(i64 2, ptr nonnull [[TMP3]]) #[[ATTR7]]
// CHECK-NEXT:    call void @llvm.lifetime.end.p0(i64 2, ptr nonnull [[TMP2]]) #[[ATTR7]]
// CHECK-NEXT:    call void @llvm.lifetime.end.p0(i64 2, ptr nonnull [[TMP1]]) #[[ATTR7]]
// CHECK-NEXT:    ret void
//
void teleport1_bind() {

    qbit q;
    qbit a;
    qbit b;

    qexpr::eval_hold((qexpr::_PrepZ(q) + qexpr::_RX(q, getAngle())) // prepare |phi>
                    << teleport1_bind(q,a,b));

}
