// NOTE: Assertions have been autogenerated by utils/update_cc_test_checks.py UTC_ARGS: --check-globals --include-generated-funcs --version 3
// RUN: %clang %include_dirs -O1 -S -emit-llvm %loadintelclangextensions -o - %s | FileCheck %s --dump-input fail
#include <clang/Quantum/datalist.h>

#include <iostream>

using namespace datalist;

const DataList src_str("This is from the source: 23");

import_with_name_begin(file_str)
#include "test.txt"
import_with_name_end(file_str);

void dDummy(double);
void iDummy(int);
void bDummy(bool);
void cDummy(char);
void cstarDummy(const char *);

int main() {

  auto src_slice = src_str(1,3);
  cstarDummy(to_char_array(src_slice));
  auto file_slice= file_str(1,3);
  cstarDummy(to_char_array(file_slice));

  iDummy(src_str.size());
  cstarDummy(to_char_array(src_str + file_str));
  cstarDummy(to_char_array(++src_str));
  cstarDummy(to_char_array(src_str>>1));
  cstarDummy(to_char_array(src_str<<1));

  cstarDummy(to_char_array(src_str(0, ":")));
  cstarDummy(to_char_array(src_str.after_next("This", "the", "from")));
  cstarDummy(to_char_array(src_str.after_next("the", "from")));
  cstarDummy(to_char_array(src_str.after_last("This", "the", "from")));
  double d_val = _d(file_str.after_next(": "));
  dDummy(d_val);
  unsigned i_val = _i(src_str.after_next(": "));
  iDummy(i_val);
  bool b_val = _b(src_str.after_next(": "));
  bDummy(b_val);

  cDummy(src_str[src_str.find("source")]);
  cDummy('s');
  cDummy(src_str[src_str.find_any("01234")]);
  cDummy('2');
  cDummy(src_str[src_str.find_not("This ")]);
  cDummy('f');
  cDummy(src_str[src_str.find_not_last("01234 ")]);
  cDummy(':');

  iDummy(src_str.count("i", "from"));
  bDummy(src_str.contains("This is from the source"));
  bDummy(src_str.contains("This is not from the source"));

}
//
// CHECK-LABEL: define dso_local noundef i32 @main(
// CHECK-SAME: ) local_unnamed_addr #[[ATTR2:[0-9]+]] {
// CHECK-NEXT:  entry:
// CHECK-NEXT:    [[CALL_I:%.*]] = tail call noundef ptr @_ZN15quantum_builtin16__datalist_sliceEPKcmm(ptr noundef nonnull @.str.25, i64 noundef 1, i64 noundef 3) #[[ATTR6:[0-9]+]]
// CHECK-NEXT:    [[CALL_I108:%.*]] = tail call noundef ptr @_ZN15quantum_builtin16__datalist_buildEPKc(ptr noundef [[CALL_I]]) #[[ATTR6]]
// CHECK-NEXT:    tail call void @_Z10cstarDummyPKc(ptr noundef [[CALL_I108]])
// CHECK-NEXT:    [[CALL_I109:%.*]] = tail call noundef ptr @_ZN15quantum_builtin16__datalist_sliceEPKcmm(ptr noundef nonnull @_ZL12file_str_raw, i64 noundef 1, i64 noundef 3) #[[ATTR6]]
// CHECK-NEXT:    [[CALL_I110:%.*]] = tail call noundef ptr @_ZN15quantum_builtin16__datalist_buildEPKc(ptr noundef [[CALL_I109]]) #[[ATTR6]]
// CHECK-NEXT:    tail call void @_Z10cstarDummyPKc(ptr noundef [[CALL_I110]])
// CHECK-NEXT:    [[CALL_I111:%.*]] = tail call noundef i64 @_ZN15quantum_builtin15__datalist_sizeEPKc(ptr noundef nonnull @.str.25) #[[ATTR6]]
// CHECK-NEXT:    [[CONV:%.*]] = trunc i64 [[CALL_I111]] to i32
// CHECK-NEXT:    tail call void @_Z6iDummyi(i32 noundef [[CONV]])
// CHECK-NEXT:    [[CALL_I112:%.*]] = tail call noundef ptr @_ZN15quantum_builtin15__datalist_joinEPKcS1_(ptr noundef nonnull @.str.25, ptr noundef nonnull @_ZL12file_str_raw) #[[ATTR6]]
// CHECK-NEXT:    [[CALL_I113:%.*]] = tail call noundef ptr @_ZN15quantum_builtin16__datalist_buildEPKc(ptr noundef [[CALL_I112]]) #[[ATTR6]]
// CHECK-NEXT:    tail call void @_Z10cstarDummyPKc(ptr noundef [[CALL_I113]])
// CHECK-NEXT:    [[CALL_I114:%.*]] = tail call noundef ptr @_ZN15quantum_builtin16__datalist_buildEPKc(ptr noundef nonnull getelementptr inbounds ([28 x i8], ptr @.str.25, i64 0, i64 1)) #[[ATTR6]]
// CHECK-NEXT:    tail call void @_Z10cstarDummyPKc(ptr noundef [[CALL_I114]])
// CHECK-NEXT:    [[CALL_I115:%.*]] = tail call noundef ptr @_ZN15quantum_builtin16__datalist_buildEPKc(ptr noundef nonnull getelementptr inbounds ([28 x i8], ptr @.str.25, i64 0, i64 1)) #[[ATTR6]]
// CHECK-NEXT:    tail call void @_Z10cstarDummyPKc(ptr noundef [[CALL_I115]])
// CHECK-NEXT:    [[CALL_I116:%.*]] = tail call noundef i64 @_ZN15quantum_builtin15__datalist_sizeEPKc(ptr noundef nonnull @.str.25) #[[ATTR6]]
// CHECK-NEXT:    [[SUB_I:%.*]] = add i64 [[CALL_I116]], -1
// CHECK-NEXT:    [[CALL3_I:%.*]] = tail call noundef ptr @_ZN15quantum_builtin16__datalist_sliceEPKcmm(ptr noundef nonnull @.str.25, i64 noundef 0, i64 noundef [[SUB_I]]) #[[ATTR6]]
// CHECK-NEXT:    [[CALL_I117:%.*]] = tail call noundef ptr @_ZN15quantum_builtin16__datalist_buildEPKc(ptr noundef [[CALL3_I]]) #[[ATTR6]]
// CHECK-NEXT:    tail call void @_Z10cstarDummyPKc(ptr noundef [[CALL_I117]])
// CHECK-NEXT:    [[CALL_I_I:%.*]] = tail call noundef i64 @_ZN15quantum_builtin15__datalist_findEPKcS1_j(ptr noundef nonnull @.str.25, ptr noundef nonnull @.str.27, i32 noundef 0) #[[ATTR6]]
// CHECK-NEXT:    [[CALL3_I118:%.*]] = tail call noundef ptr @_ZN15quantum_builtin16__datalist_sliceEPKcmm(ptr noundef nonnull @.str.25, i64 noundef 0, i64 noundef [[CALL_I_I]]) #[[ATTR6]]
// CHECK-NEXT:    [[CALL_I119:%.*]] = tail call noundef ptr @_ZN15quantum_builtin16__datalist_buildEPKc(ptr noundef [[CALL3_I118]]) #[[ATTR6]]
// CHECK-NEXT:    tail call void @_Z10cstarDummyPKc(ptr noundef [[CALL_I119]])
// CHECK-NEXT:    [[CALL_I_I_I:%.*]] = tail call noundef i64 @_ZN15quantum_builtin15__datalist_findEPKcS1_j(ptr noundef nonnull @.str.25, ptr noundef nonnull @.str.28, i32 noundef 0) #[[ATTR6]]
// CHECK-NEXT:    [[CALL_I7_I_I:%.*]] = tail call noundef i64 @_ZN15quantum_builtin15__datalist_sizeEPKc(ptr noundef nonnull @.str.28) #[[ATTR6]]
// CHECK-NEXT:    [[ADD_I_I:%.*]] = add i64 [[CALL_I7_I_I]], [[CALL_I_I_I]]
// CHECK-NEXT:    [[CALL_I8_I_I:%.*]] = tail call noundef i64 @_ZN15quantum_builtin15__datalist_sizeEPKc(ptr noundef nonnull @.str.25) #[[ATTR6]]
// CHECK-NEXT:    [[CALL5_I_I:%.*]] = tail call noundef ptr @_ZN15quantum_builtin16__datalist_sliceEPKcmm(ptr noundef nonnull @.str.25, i64 noundef [[ADD_I_I]], i64 noundef [[CALL_I8_I_I]]) #[[ATTR6]]
// CHECK-NEXT:    [[CALL_I_I_I_I:%.*]] = tail call noundef i64 @_ZN15quantum_builtin15__datalist_findEPKcS1_j(ptr noundef nonnull @.str.25, ptr noundef nonnull @.str.29, i32 noundef 0) #[[ATTR6]]
// CHECK-NEXT:    [[CALL_I7_I_I_I:%.*]] = tail call noundef i64 @_ZN15quantum_builtin15__datalist_sizeEPKc(ptr noundef nonnull @.str.29) #[[ATTR6]]
// CHECK-NEXT:    [[ADD_I_I_I:%.*]] = add i64 [[CALL_I7_I_I_I]], [[CALL_I_I_I_I]]
// CHECK-NEXT:    [[CALL_I8_I_I_I:%.*]] = tail call noundef i64 @_ZN15quantum_builtin15__datalist_sizeEPKc(ptr noundef nonnull @.str.25) #[[ATTR6]]
// CHECK-NEXT:    [[CALL5_I_I_I:%.*]] = tail call noundef ptr @_ZN15quantum_builtin16__datalist_sliceEPKcmm(ptr noundef nonnull @.str.25, i64 noundef [[ADD_I_I_I]], i64 noundef [[CALL_I8_I_I_I]]) #[[ATTR6]]
// CHECK-NEXT:    [[CALL_I_I13_I_I:%.*]] = tail call noundef i64 @_ZN15quantum_builtin15__datalist_findEPKcS1_j(ptr noundef nonnull @.str.25, ptr noundef nonnull @.str.30, i32 noundef 0) #[[ATTR6]]
// CHECK-NEXT:    [[CALL_I7_I14_I_I:%.*]] = tail call noundef i64 @_ZN15quantum_builtin15__datalist_sizeEPKc(ptr noundef nonnull @.str.30) #[[ATTR6]]
// CHECK-NEXT:    [[ADD_I15_I_I:%.*]] = add i64 [[CALL_I7_I14_I_I]], [[CALL_I_I13_I_I]]
// CHECK-NEXT:    [[CALL_I8_I16_I_I:%.*]] = tail call noundef i64 @_ZN15quantum_builtin15__datalist_sizeEPKc(ptr noundef nonnull @.str.25) #[[ATTR6]]
// CHECK-NEXT:    [[CALL5_I17_I_I:%.*]] = tail call noundef ptr @_ZN15quantum_builtin16__datalist_sliceEPKcmm(ptr noundef nonnull @.str.25, i64 noundef [[ADD_I15_I_I]], i64 noundef [[CALL_I8_I16_I_I]]) #[[ATTR6]]
// CHECK-NEXT:    [[CALL11_I_I:%.*]] = tail call noundef ptr @_ZN15quantum_builtin17__datalist_longerEPKcS1_(ptr noundef [[CALL5_I_I_I]], ptr noundef [[CALL5_I17_I_I]]) #[[ATTR6]]
// CHECK-NEXT:    [[CALL13_I:%.*]] = tail call noundef ptr @_ZN15quantum_builtin17__datalist_longerEPKcS1_(ptr noundef [[CALL5_I_I]], ptr noundef [[CALL11_I_I]]) #[[ATTR6]]
// CHECK-NEXT:    [[CALL_I120:%.*]] = tail call noundef ptr @_ZN15quantum_builtin16__datalist_buildEPKc(ptr noundef [[CALL13_I]]) #[[ATTR6]]
// CHECK-NEXT:    tail call void @_Z10cstarDummyPKc(ptr noundef [[CALL_I120]])
// CHECK-NEXT:    [[CALL_I_I_I121:%.*]] = tail call noundef i64 @_ZN15quantum_builtin15__datalist_findEPKcS1_j(ptr noundef nonnull @.str.25, ptr noundef nonnull @.str.29, i32 noundef 0) #[[ATTR6]]
// CHECK-NEXT:    [[CALL_I7_I_I122:%.*]] = tail call noundef i64 @_ZN15quantum_builtin15__datalist_sizeEPKc(ptr noundef nonnull @.str.29) #[[ATTR6]]
// CHECK-NEXT:    [[ADD_I_I123:%.*]] = add i64 [[CALL_I7_I_I122]], [[CALL_I_I_I121]]
// CHECK-NEXT:    [[CALL_I8_I_I124:%.*]] = tail call noundef i64 @_ZN15quantum_builtin15__datalist_sizeEPKc(ptr noundef nonnull @.str.25) #[[ATTR6]]
// CHECK-NEXT:    [[CALL5_I_I125:%.*]] = tail call noundef ptr @_ZN15quantum_builtin16__datalist_sliceEPKcmm(ptr noundef nonnull @.str.25, i64 noundef [[ADD_I_I123]], i64 noundef [[CALL_I8_I_I124]]) #[[ATTR6]]
// CHECK-NEXT:    [[CALL_I_I13_I:%.*]] = tail call noundef i64 @_ZN15quantum_builtin15__datalist_findEPKcS1_j(ptr noundef nonnull @.str.25, ptr noundef nonnull @.str.30, i32 noundef 0) #[[ATTR6]]
// CHECK-NEXT:    [[CALL_I7_I14_I:%.*]] = tail call noundef i64 @_ZN15quantum_builtin15__datalist_sizeEPKc(ptr noundef nonnull @.str.30) #[[ATTR6]]
// CHECK-NEXT:    [[ADD_I15_I:%.*]] = add i64 [[CALL_I7_I14_I]], [[CALL_I_I13_I]]
// CHECK-NEXT:    [[CALL_I8_I16_I:%.*]] = tail call noundef i64 @_ZN15quantum_builtin15__datalist_sizeEPKc(ptr noundef nonnull @.str.25) #[[ATTR6]]
// CHECK-NEXT:    [[CALL5_I17_I:%.*]] = tail call noundef ptr @_ZN15quantum_builtin16__datalist_sliceEPKcmm(ptr noundef nonnull @.str.25, i64 noundef [[ADD_I15_I]], i64 noundef [[CALL_I8_I16_I]]) #[[ATTR6]]
// CHECK-NEXT:    [[CALL11_I:%.*]] = tail call noundef ptr @_ZN15quantum_builtin17__datalist_longerEPKcS1_(ptr noundef [[CALL5_I_I125]], ptr noundef [[CALL5_I17_I]]) #[[ATTR6]]
// CHECK-NEXT:    [[CALL_I126:%.*]] = tail call noundef ptr @_ZN15quantum_builtin16__datalist_buildEPKc(ptr noundef [[CALL11_I]]) #[[ATTR6]]
// CHECK-NEXT:    tail call void @_Z10cstarDummyPKc(ptr noundef [[CALL_I126]])
// CHECK-NEXT:    [[CALL_I_I_I127:%.*]] = tail call noundef i64 @_ZN15quantum_builtin15__datalist_findEPKcS1_j(ptr noundef nonnull @.str.25, ptr noundef nonnull @.str.28, i32 noundef 1) #[[ATTR6]]
// CHECK-NEXT:    [[CALL_I7_I_I128:%.*]] = tail call noundef i64 @_ZN15quantum_builtin15__datalist_sizeEPKc(ptr noundef nonnull @.str.28) #[[ATTR6]]
// CHECK-NEXT:    [[ADD_I_I129:%.*]] = add i64 [[CALL_I7_I_I128]], [[CALL_I_I_I127]]
// CHECK-NEXT:    [[CALL_I8_I_I130:%.*]] = tail call noundef i64 @_ZN15quantum_builtin15__datalist_sizeEPKc(ptr noundef nonnull @.str.25) #[[ATTR6]]
// CHECK-NEXT:    [[CALL5_I_I131:%.*]] = tail call noundef ptr @_ZN15quantum_builtin16__datalist_sliceEPKcmm(ptr noundef nonnull @.str.25, i64 noundef [[ADD_I_I129]], i64 noundef [[CALL_I8_I_I130]]) #[[ATTR6]]
// CHECK-NEXT:    [[CALL_I_I_I_I132:%.*]] = tail call noundef i64 @_ZN15quantum_builtin15__datalist_findEPKcS1_j(ptr noundef nonnull @.str.25, ptr noundef nonnull @.str.29, i32 noundef 1) #[[ATTR6]]
// CHECK-NEXT:    [[CALL_I7_I_I_I133:%.*]] = tail call noundef i64 @_ZN15quantum_builtin15__datalist_sizeEPKc(ptr noundef nonnull @.str.29) #[[ATTR6]]
// CHECK-NEXT:    [[ADD_I_I_I134:%.*]] = add i64 [[CALL_I7_I_I_I133]], [[CALL_I_I_I_I132]]
// CHECK-NEXT:    [[CALL_I8_I_I_I135:%.*]] = tail call noundef i64 @_ZN15quantum_builtin15__datalist_sizeEPKc(ptr noundef nonnull @.str.25) #[[ATTR6]]
// CHECK-NEXT:    [[CALL5_I_I_I136:%.*]] = tail call noundef ptr @_ZN15quantum_builtin16__datalist_sliceEPKcmm(ptr noundef nonnull @.str.25, i64 noundef [[ADD_I_I_I134]], i64 noundef [[CALL_I8_I_I_I135]]) #[[ATTR6]]
// CHECK-NEXT:    [[CALL_I_I13_I_I137:%.*]] = tail call noundef i64 @_ZN15quantum_builtin15__datalist_findEPKcS1_j(ptr noundef nonnull @.str.25, ptr noundef nonnull @.str.30, i32 noundef 1) #[[ATTR6]]
// CHECK-NEXT:    [[CALL_I7_I14_I_I138:%.*]] = tail call noundef i64 @_ZN15quantum_builtin15__datalist_sizeEPKc(ptr noundef nonnull @.str.30) #[[ATTR6]]
// CHECK-NEXT:    [[ADD_I15_I_I139:%.*]] = add i64 [[CALL_I7_I14_I_I138]], [[CALL_I_I13_I_I137]]
// CHECK-NEXT:    [[CALL_I8_I16_I_I140:%.*]] = tail call noundef i64 @_ZN15quantum_builtin15__datalist_sizeEPKc(ptr noundef nonnull @.str.25) #[[ATTR6]]
// CHECK-NEXT:    [[CALL5_I17_I_I141:%.*]] = tail call noundef ptr @_ZN15quantum_builtin16__datalist_sliceEPKcmm(ptr noundef nonnull @.str.25, i64 noundef [[ADD_I15_I_I139]], i64 noundef [[CALL_I8_I16_I_I140]]) #[[ATTR6]]
// CHECK-NEXT:    [[CALL11_I_I142:%.*]] = tail call noundef ptr @_ZN15quantum_builtin18__datalist_shorterEPKcS1_(ptr noundef [[CALL5_I_I_I136]], ptr noundef [[CALL5_I17_I_I141]]) #[[ATTR6]]
// CHECK-NEXT:    [[CALL13_I143:%.*]] = tail call noundef ptr @_ZN15quantum_builtin18__datalist_shorterEPKcS1_(ptr noundef [[CALL5_I_I131]], ptr noundef [[CALL11_I_I142]]) #[[ATTR6]]
// CHECK-NEXT:    [[CALL_I144:%.*]] = tail call noundef ptr @_ZN15quantum_builtin16__datalist_buildEPKc(ptr noundef [[CALL13_I143]]) #[[ATTR6]]
// CHECK-NEXT:    tail call void @_Z10cstarDummyPKc(ptr noundef [[CALL_I144]])
// CHECK-NEXT:    [[CALL_I_I145:%.*]] = tail call noundef i64 @_ZN15quantum_builtin15__datalist_findEPKcS1_j(ptr noundef nonnull @_ZL12file_str_raw, ptr noundef nonnull @.str.31, i32 noundef 0) #[[ATTR6]]
// CHECK-NEXT:    [[CALL_I7_I:%.*]] = tail call noundef i64 @_ZN15quantum_builtin15__datalist_sizeEPKc(ptr noundef nonnull @.str.31) #[[ATTR6]]
// CHECK-NEXT:    [[ADD_I:%.*]] = add i64 [[CALL_I7_I]], [[CALL_I_I145]]
// CHECK-NEXT:    [[CALL_I8_I:%.*]] = tail call noundef i64 @_ZN15quantum_builtin15__datalist_sizeEPKc(ptr noundef nonnull @_ZL12file_str_raw) #[[ATTR6]]
// CHECK-NEXT:    [[CALL5_I:%.*]] = tail call noundef ptr @_ZN15quantum_builtin16__datalist_sliceEPKcmm(ptr noundef nonnull @_ZL12file_str_raw, i64 noundef [[ADD_I]], i64 noundef [[CALL_I8_I]]) #[[ATTR6]]
// CHECK-NEXT:    [[CALL_I146:%.*]] = tail call noundef double @_ZN15quantum_builtin19__datalist_floatizeEPKc(ptr noundef [[CALL5_I]]) #[[ATTR6]]
// CHECK-NEXT:    tail call void @_Z6dDummyd(double noundef [[CALL_I146]])
// CHECK-NEXT:    [[CALL_I_I147:%.*]] = tail call noundef i64 @_ZN15quantum_builtin15__datalist_findEPKcS1_j(ptr noundef nonnull @.str.25, ptr noundef nonnull @.str.31, i32 noundef 0) #[[ATTR6]]
// CHECK-NEXT:    [[CALL_I7_I148:%.*]] = tail call noundef i64 @_ZN15quantum_builtin15__datalist_sizeEPKc(ptr noundef nonnull @.str.31) #[[ATTR6]]
// CHECK-NEXT:    [[ADD_I149:%.*]] = add i64 [[CALL_I7_I148]], [[CALL_I_I147]]
// CHECK-NEXT:    [[CALL_I8_I150:%.*]] = tail call noundef i64 @_ZN15quantum_builtin15__datalist_sizeEPKc(ptr noundef nonnull @.str.25) #[[ATTR6]]
// CHECK-NEXT:    [[CALL5_I151:%.*]] = tail call noundef ptr @_ZN15quantum_builtin16__datalist_sliceEPKcmm(ptr noundef nonnull @.str.25, i64 noundef [[ADD_I149]], i64 noundef [[CALL_I8_I150]]) #[[ATTR6]]
// CHECK-NEXT:    [[CALL_I152:%.*]] = tail call noundef i32 @_ZN15quantum_builtin21__datalist_integerizeEPKc(ptr noundef [[CALL5_I151]]) #[[ATTR6]]
// CHECK-NEXT:    tail call void @_Z6iDummyi(i32 noundef [[CALL_I152]])
// CHECK-NEXT:    [[CALL_I_I153:%.*]] = tail call noundef i64 @_ZN15quantum_builtin15__datalist_findEPKcS1_j(ptr noundef nonnull @.str.25, ptr noundef nonnull @.str.31, i32 noundef 0) #[[ATTR6]]
// CHECK-NEXT:    [[CALL_I7_I154:%.*]] = tail call noundef i64 @_ZN15quantum_builtin15__datalist_sizeEPKc(ptr noundef nonnull @.str.31) #[[ATTR6]]
// CHECK-NEXT:    [[ADD_I155:%.*]] = add i64 [[CALL_I7_I154]], [[CALL_I_I153]]
// CHECK-NEXT:    [[CALL_I8_I156:%.*]] = tail call noundef i64 @_ZN15quantum_builtin15__datalist_sizeEPKc(ptr noundef nonnull @.str.25) #[[ATTR6]]
// CHECK-NEXT:    [[CALL5_I157:%.*]] = tail call noundef ptr @_ZN15quantum_builtin16__datalist_sliceEPKcmm(ptr noundef nonnull @.str.25, i64 noundef [[ADD_I155]], i64 noundef [[CALL_I8_I156]]) #[[ATTR6]]
// CHECK-NEXT:    [[CALL_I158:%.*]] = tail call noundef zeroext i1 @_ZN15quantum_builtin21__datalist_booleanizeEPKc(ptr noundef [[CALL5_I157]]) #[[ATTR6]]
// CHECK-NEXT:    tail call void @_Z6bDummyb(i1 noundef zeroext [[CALL_I158]])
// CHECK-NEXT:    [[CALL_I159:%.*]] = tail call noundef i64 @_ZN15quantum_builtin15__datalist_findEPKcS1_j(ptr noundef nonnull @.str.25, ptr noundef nonnull @.str.32, i32 noundef 0) #[[ATTR6]]
// CHECK-NEXT:    [[CALL_I160:%.*]] = tail call noundef signext i8 @_ZN15quantum_builtin13__datalist_atEPKcm(ptr noundef nonnull @.str.25, i64 noundef [[CALL_I159]]) #[[ATTR6]]
// CHECK-NEXT:    tail call void @_Z6cDummyc(i8 noundef signext [[CALL_I160]])
// CHECK-NEXT:    tail call void @_Z6cDummyc(i8 noundef signext 115)
// CHECK-NEXT:    [[CALL_I161:%.*]] = tail call noundef i64 @_ZN15quantum_builtin15__datalist_findEPKcS1_j(ptr noundef nonnull @.str.25, ptr noundef nonnull @.str.33, i32 noundef 2) #[[ATTR6]]
// CHECK-NEXT:    [[CALL_I162:%.*]] = tail call noundef signext i8 @_ZN15quantum_builtin13__datalist_atEPKcm(ptr noundef nonnull @.str.25, i64 noundef [[CALL_I161]]) #[[ATTR6]]
// CHECK-NEXT:    tail call void @_Z6cDummyc(i8 noundef signext [[CALL_I162]])
// CHECK-NEXT:    tail call void @_Z6cDummyc(i8 noundef signext 50)
// CHECK-NEXT:    [[CALL_I163:%.*]] = tail call noundef i64 @_ZN15quantum_builtin15__datalist_findEPKcS1_j(ptr noundef nonnull @.str.25, ptr noundef nonnull @.str.34, i32 noundef -1) #[[ATTR6]]
// CHECK-NEXT:    [[CALL_I164:%.*]] = tail call noundef signext i8 @_ZN15quantum_builtin13__datalist_atEPKcm(ptr noundef nonnull @.str.25, i64 noundef [[CALL_I163]]) #[[ATTR6]]
// CHECK-NEXT:    tail call void @_Z6cDummyc(i8 noundef signext [[CALL_I164]])
// CHECK-NEXT:    tail call void @_Z6cDummyc(i8 noundef signext 102)
// CHECK-NEXT:    [[CALL_I165:%.*]] = tail call noundef i64 @_ZN15quantum_builtin15__datalist_findEPKcS1_j(ptr noundef nonnull @.str.25, ptr noundef nonnull @.str.35, i32 noundef -2) #[[ATTR6]]
// CHECK-NEXT:    [[CALL_I166:%.*]] = tail call noundef signext i8 @_ZN15quantum_builtin13__datalist_atEPKcm(ptr noundef nonnull @.str.25, i64 noundef [[CALL_I165]]) #[[ATTR6]]
// CHECK-NEXT:    tail call void @_Z6cDummyc(i8 noundef signext [[CALL_I166]])
// CHECK-NEXT:    tail call void @_Z6cDummyc(i8 noundef signext 58)
// CHECK-NEXT:    [[CALL_I_I167:%.*]] = tail call noundef i64 @_ZN15quantum_builtin16__datalist_countEPKcS1_(ptr noundef nonnull @.str.25, ptr noundef nonnull @.str.36) #[[ATTR6]]
// CHECK-NEXT:    [[CALL_I6_I:%.*]] = tail call noundef i64 @_ZN15quantum_builtin16__datalist_countEPKcS1_(ptr noundef nonnull @.str.25, ptr noundef nonnull @.str.30) #[[ATTR6]]
// CHECK-NEXT:    [[ADD_I168:%.*]] = add i64 [[CALL_I6_I]], [[CALL_I_I167]]
// CHECK-NEXT:    [[CONV101:%.*]] = trunc i64 [[ADD_I168]] to i32
// CHECK-NEXT:    tail call void @_Z6iDummyi(i32 noundef [[CONV101]])
// CHECK-NEXT:    [[CALL_I169:%.*]] = tail call noundef zeroext i1 @_ZN15quantum_builtin19__datalist_containsEPKcS1_(ptr noundef nonnull @.str.25, ptr noundef nonnull @.str.37) #[[ATTR6]]
// CHECK-NEXT:    tail call void @_Z6bDummyb(i1 noundef zeroext [[CALL_I169]])
// CHECK-NEXT:    [[CALL_I170:%.*]] = tail call noundef zeroext i1 @_ZN15quantum_builtin19__datalist_containsEPKcS1_(ptr noundef nonnull @.str.25, ptr noundef nonnull @.str.38) #[[ATTR6]]
// CHECK-NEXT:    tail call void @_Z6bDummyb(i1 noundef zeroext [[CALL_I170]])
// CHECK-NEXT:    ret i32 0
