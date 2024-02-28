#include <clang/Quantum/quintrinsics.h>
#include <clang/Quantum/qexpr.h>
#include <clang/Quantum/qlist.h>
#include <clang/Quantum/datalist.h>

#include <iostream>

using namespace qexpr;
using namespace qlist;

define_quantum_data(src_str, This is from the source: 23);

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