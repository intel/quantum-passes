
/// Production mode
#include <clang/Quantum/quintrinsics.h>
#include <quantum.hpp>
#include <iostream>
#include <cassert>
#include <algorithm>

// Allocate 2 global qubits.
const int N = 2;
qbit qumem[N];

// Type test 2 


int main()
{
    try
    {
        qbit& q = qumem[0];
        throw(q);
    }
    catch(...)
    {
        std::cout << "Caught exception!" << std::endl;
    }

    return 0;
    
}