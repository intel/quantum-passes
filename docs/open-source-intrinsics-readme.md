# Intel (R) Quantum Intrinsics

This repository builds C++ based frontend for the Intel Quantum Compiler.  It is used in conjunction with the [Intel Quantum Passes](https://github.com/intel/quantum-passes).  It adapts the clang and LLVM compiler to use Quantum Intrinsics, and includes several header files with specific quantum libraries and functions.

## Building the Tools
Building the compiler is very similar to standard LLVM, and requires the same software, found [here](https://llvm.org/docs/GettingStarted.html#software).

After ensuring the system requirements are met, use the following commands:
```
mkdir build
cd build
cmake -G Ninja -S ../llvm -B . -DLLVM_INSTALL_UTILS=ON -DCMAKE_INSTALL_PREFIX=<iqc_install_dir> -DLLVM_ENABLE_PROJECTS="clang;lld" -DCMAKE_BUILD_TYPE=Release
ninja -C . install
```

`<iqc_install_dir>` is the location where you want IQC to be installed.  If it is not specified, it will attempt to be installed to `/usr/bin`.  If you do not want to install the compiler anywhere, simply use the command `ninja -C .` instead.

## Information for Developers
Much of the information that's helpful to use and develop with LLVM is helpful for our compiler as well, but this is some frequently useful advice.

### Debugging
In order to to get full debug information and assertions, pass the `-DCMAKE_BUILD_TYPE=Debug` rather than `Release`.

In order to get "print debugging" for LLVM classes through the `.dump()` method, include `-DLLVM_ENABLE_DUMP=On` to the cmake invocation.

### Intrinsics
If you find you need to generate your own intrinsics, the TableGen files for these can be found in `llvm/include/llvm/IR/IntrinsicsQuantum.td`.

### Header File and Quantum Library
If you find you need to add to the library of quantum functions, they can be found in `clang/include/Quantum`.  The same conventions can be used, mainly the use of the `noinline` and the `.qbbs_text` attributes.  These attributes ensure that the functions are not improperly inlined, which can cause problems later on in the compilation process. 