#include <clang/Quantum/quintrinsics.h>
#include <clang/Quantum/qlist.h>
#include <quantum_full_state_simulator_backend.h>
#include <quantum.hpp>

#include <assert.h>
#include <iostream>

using namespace qlist;

qbit listable(qs1, 4, (2, 5, 3, 7));
qbit listable(qs2, 4, (6, 1, 8, 4));
qbit listable(qs3, 2, (0, 9));


void testRefWrapper(const std::vector<std::reference_wrapper<qbit>> & wraps, unsigned test[]){
  for(int i = 0; i < wraps.size(); i++)
    assert(wraps[i] == test[i]);
}

void test_simple_array(){
  auto wraps = to_ref_wrappers(qs1); 
  unsigned compare[] = {2, 5, 3, 7};
  testRefWrapper(wraps, compare);
}

void test_join(){
  auto wraps = to_ref_wrappers(qs1 + qs2 + qs3); 
  unsigned compare[] = {2, 5, 3, 7, 6, 1, 8, 4, 0, 9};
  testRefWrapper(wraps, compare);
}

void test_slice(){
  auto wraps = to_ref_wrappers(qs1(0, 2) + qs2(1, 3)); 
  unsigned compare[] = {2, 5, 1, 8};
  testRefWrapper(wraps, compare);
}

void test_in_order(){
  auto wraps = to_ref_wrappers(  qs3(0, 1) // 0
                                 + qs2(1, 2) // 1
                                 + qs1(0, 1) // 2
                                 + qs1(2, 3) // 3
                                 + qs2(3, 4) // 4
                                 + qs1(1, 2) // 5
                                 + qs2(0, 1) // 6
                                 + qs1(3, 4) // 7
                                 + qs2(2, 3) // 8
                                 + qs3(1, 2) // 9
  );
  unsigned compare[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  testRefWrapper(wraps, compare);
}

int main() {

  iqsdk::IqsConfig iqs_config;
  iqsdk::FullStateSimulator iqs_device(iqs_config);
  iqsdk::QRT_ERROR_T status = iqs_device.ready();
  assert(status == iqsdk::QRT_ERROR_SUCCESS);

    test_simple_array();
    test_join();
    test_slice();
    test_in_order();
    std::cout<<"Success!\n";
    return 0;
}