#include <clang/Quantum/quintrinsics.h>
#include <quantum.hpp>
#include<vector>
#include<functional>
#include <iostream>
#include <cassert>
#include <algorithm>

// Allocate 2 global qubits.
const int N = 2;
qbit qumem[N];

// Type test 

// Works
bool int_contains(std::reference_wrapper<int> qbit_ref, std::vector<std::reference_wrapper<int>> container)
{
    return std::find(container.begin(), container.end(), qbit_ref) != container.end();
}

// Fails
namespace std {
bool operator==(const std::reference_wrapper<qbit> q_ref1, const std::reference_wrapper<qbit> q_ref2) {
    return &q_ref1 == &q_ref2;
} 
}

bool qbit_contains(std::reference_wrapper<qbit> qbit_ref, std::vector<std::reference_wrapper<qbit>> container)
{
    return std::find(container.begin(), container.end(), qbit_ref) != container.end();
}

bool qbit_contains(qbit* qbit_ref, std::vector<qbit*> container)
{
    return std::find(container.begin(), container.end(), qbit_ref) != container.end();
}

int main()
{
    return 0;
}