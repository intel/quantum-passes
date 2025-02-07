# Writing a Pass for the Intel (R) Quantum Compiler

## Tips for Writing an LLVM-Based Intel Quantum Pass
Information concerning an LLVM Pass in general can be found in the LLVM Documentation.  Examples for writing and compiling an Intel Quantum pass can be found in the ExamplePasses directory.  There are some quantum specific notes detailed in this section.

### Identifying Quantum Kernels
Quantum Kernels are compiled in a special `.qbbs_text` section of the program. And are marked with the according attribute in the LLVM IR.  Additionally, after compilation adds intrinsics and attributes to the program, the function name will also include a `.stub` at the end.  You can check if a function is marked as a quantum kernel using the `isQKernel` function from `QuantumGeneralUtils.h`.

### Iterating Over Gates
At present, some of the LLVM specific details and implementation are hidden through the use of the `QIter` and `QBBIter` classes.  This allows for the iteration over only the quantum instructions in an entire function in the case of the `QIter`, and over a specific basic block in the case of `QBBIter`.

These functions include several helper functions as well.  These include attributes of the gate currently being iterated over, getting the operands of the gates, going to the next instance of a particular gate, or of a particular qubit in the iterator construct.

### Changing Quantum Programs
The `QIter` and `QBBIter` classes also include insertion, deletion and modification of gates.  Modification occurs in place. Deletion removes the instruction and move thes iterator to the following instruction. Insertion occurs after the current operation in iteration.

During insertion of quantum operations, instructions that handle dereferencing qubit and parameter operands may need to be moved. By default, this is done after each insertion is performed.  However, this can be deferred by calling `QIter.insertGate(gate_id, qubits, paras, false)`.  The false flag will denote that the parameters for this call may need to be placed earlier in the program.  After all operations using a specific `QIter` are complete, use the `updateGateDependencies()` method to perform a cleanup step. The results will be the same, but batching can improve performance, especially during many repeated calls to `insertGate` and `deleteGate`.

## Integrating User-Written Passes
Writing your own pass is no good on its own, it needs to be integrated with the rest of the compilation.  Both the compilation program in the Intel Quantum SDK and the included script in the repository offer several ways to do this.

### How Can I Run My Passes?
There are several ways to run a pass for the Intel Quantum Compiler, using the `opt` tool, using the provided interface from the driver script, or by using the `quantum-optimization-driver`.

#### Using opt
Passes can be run on their own using the `opt` tools included in the SDK or when built from source from the `quantum-intrinsics` directory.  This is relatively simple, using the command `opt -load-pass-plugin <intel_quantum_pass_library> -load-pass-plugin <library_loc> -p="<pass_name_1>,<pass_name_2>,...,<pass_name_n>" -o <output_file.ll> <input_file.ll>`.  However, you will need to start with one of the intermediate IR files in this case.  Intermediate files can be saved by using the `-k` command line argument or by ending compilation early, as described below.

#### Using the quantum-optimization-driver
Using the `quantum-optimization-driver` is relatively similar to using `opt`, but it has the ability to run the entirety of the quantum processing for the Intel Quantum Compiler without it being explicitly noted through the pass list. The external pass library can be loaded as with `opt` but the Intel Quantum passes library does not need to be loaded separately. To insert passes in between sections of compilation, use the `-custom-passes="<stage_name>:pass_1,pass_2...pass_n` flag. Valid stage names are `precond`, `prelower`, `presynth`, `preschedule` and `presplit`. More details can be found in the documentation for the [compilation flow](./intel-quantum-compilation-flow.md).

#### Using the Driver Script
They can be run in conjunction with the rest of the compilation pipeline through the use of the driver script included in the repository.  Each of the different stages of compilation have a different name: precond, prelower, presynth, presched, presplit.  Each of these sections is marked near the relevant developer note in this document.  In order to specify passes at a particular point, first identify the location of the library with `-E <library_loc>`, second specify the section with `-a <pass_1>[,<pass_2>,...,<pass_n>]`.  The argument in this pass is a comma separated list of pass names.  If any arguments need to be passed to this section each argument must be prepended with a `-A`. So, for example, to pass in `-arg_name <arg_val>`, pass `-A -arg_name -A <arg_val>` to the driver script.

### Where Can Passes Be Integrated?
See the **Developer Notes** in the compilation flow documentation. for instructions for where each optimization pass can be added.

### Examining Intermediate Files
It can be useful to examine the intermediate steps of compilation. By specifying the `-k` flag,
three `.ll` files will be generated. The first is always generated during compilation, the
`<file_name>.ll`. This is the initial IR generated by the frontend by the Intel Quantum Compiler,
it has had no quantum optimizations applied to it. The additional two files, `<file_name>_classical_final.ll`
and `<file_name>_quantum.ll`, are the final IR files with the quantum and classical components separated
after all quantum optimizations have been applied.

For more granularity, users can specify the `-Z <stage_name>` flag. `<stage_name>`
is the step of compilation they wish to examine. This will stop compilation after that stage is run.
Allowed options are: `flatten`, `unroll`, `validate`, `lower`, `synthesize`, `schedule`,
`separate`.  This will generated a file with the naming convention `<file_name>_<stage_name>.ll`.

It is also possible to start compilation from a specific stage. This can be done by specifying the
`-z <stage_name>` flag with any of the same stages listed above. If able, the compiler will
process the input IR file, and continue compilation starting with the specified stage.

### What If I Want to Run My Pass with Intel Optimizations or an Intel Quantum Simulator?
There are two ways to integrate passes with the rest of Intel optimizations or run the resulting programs on the Intel Quantum Simulator.

The first is to use the driver script included in the provided repository until it provides a IR file at the end of compilation.  This can be given to the Intel Quantum SDK compiler to be compiled into a final binary and the simulator programs from whichever point the compilation was paused previously.  This is a two step option and only provides the final step of compilation.  The rest must be achieved by using the repository.

The second, is to use the driver program provided in the Intel Quantum SDK.  User-written passes can be added as they are with the script in the passes repository.  However, when using the `-O1` specification, a different synthesis optimization will be run as well.  Additionally, it will compile through to a final program.