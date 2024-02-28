# Writing an LLVM-Based Intel (R) Quantum Pass
Information concerning an LLVM Pass in general can be found in the [LLVM Documentation](https://llvm.org/docs/).  Examples for writing and compiling an Intel (R) Quantum pass can be found in the ExamplePasses directory.  There are some quantum specific notes detailed in this section.

## Quantum Data Types and References
The compiler mainly adds a single quantum data type, the :code:`qbit`, which is represented in the code as :code:`i16`. Any new qubit allocations should be this data type.  If a pointer to a qubit value is used in a function, it must be indicated through the use of a :code:`qubit_ref` function argument attribute.  This allows other optimizations to be aware of qubit versus classical arguments and identify how certain values should be treated.  Another way to do this is to rerun the `insert-q-attrs` pass, which will reidentify which arguments should be marked with this attribute based on the call graph of the program and the base level intrinsic functions.

## Quantum Compiler Linkage and Quantum Module

There are several analysis passes that are required to gather all the information to transform a quantum program. The main three analysis passes needed are described in the following sections.

The `QuantumCompilerLinkage` pass is the utility that iterates over the IR file, and collects information about the `QuantumModule`.  The `QuantumModule` includes the gates and what state of compilation the quantum module is in.  This is implemented as an Analysis pass, similar to the `CallGraph` Pass or `TargetTransformInfo` based passes in the main LLVM repository.  To use the `QuantumModule` in a pass, use the following block in the `run` method of a pass:

```
QuantumModuleProxy QMP = MAM.getResult<QuantumCompilerLinkageAnalysis>(M);
runPass(M, *QMP.QM);
```


There is a pointer to the `QuantumModule` in the proxy output from the `QuantumCompilerLinkageAnalysis`. It will give your pass access to the `QuantumModule` and will automatically run the QuantumCompilerLinkage pass as well, without needing to denote it on the command line.

The `QuantumModule` is a class that contains information about the quantum aspects of the program and the current stage of compilation.  The list of quantum kernels in the program can be accessed through the typical `begin()` and `end()` methods, similar to an LLVM Module.  The `QuantumModule` also contains information about the current platform that the program is being run on, such as the connectivity and the number of qubits.

The `QuantumKernel` class exists to contain information about each defined function that contains quantum gates and provides a mechanism to access the original functions.  These functions contain Quantum Basic Blocks or QBBs.  A quantum basic block is an LLVM basic block that only contains quantum instructions.  It does not include any of the processing of the output of measurement or the initial classical preprocessing.  It is a straight line application of quantum gates.  There can be more than one QBB per quantum kernel through the use of conditional instructions.

While this allows a way to iterate over the function, it should not be used directly. Instead, use the `QBBIter` and `QIter` classes, detailed below.

## Quantum Annotations from JSON

Included in the `quintrinsics.h` header file are the statistics, identification, and information describing the canonical gates used in the initial compilation.  This enables the `QIter` class to recognize and use these gates.  In order to have tracking information and recognize certain operations as quantum operations in these structure, this pass must be run to put the information in quantum module so it can be referenced later on.

## Spin Native Annotations from JSON

This pass does the same as the Quantum Annotations from JSON, but for the spin native set of gates.  The information for this is included in the pass and is structured slightly differently.

## Identifying Quantum Kernels

Quantum Kernels are compiled in a special `.qbbs_text` section of the program. They are marked with the according attribute in the LLVM IR.  Additionally, after compilation adds intrinsics and attributes to the program, the function name will also include a `.stub` at the end.  You can check if a function is marked as a quantum kernel using the `isQKernel` function from `QuantumGeneralUtils.h`.

## Iterating Over Gates

At present, some of the LLVM specific details and implementation are hidden through the use of the `QIter` and `QBBIter` classes.  This allows for the iteration over only the quantum instructions in an entire function in the case of the `QIter`, and over a specific basic block in the case of `QBBIter`.  To instantiate an instance of a `QIter` class, simply use `QIter(FunctionHere)`. The resulting object acts like a normal iterator.  Similarly for a `QBBIter` use `QBBIter(BasicBlockHere)` to pass in a single Basic Block and iterate over the quantum instructions.

These functions include several helper functions as well.  These include attributes of the gate currently being iterated over, getting the operands of the gates, going to the next instance of a particular gate, or of a particular qubit in the iterator construct.

## Changing Quantum Programs

The `QIter` and `QBBIter` classes also including insertion, deletion and modification of gates.  Modification occurs in place. Deletion removes the instruction and moves the iterator to the following instruction. Insertion occurs after the current operation in iteration.