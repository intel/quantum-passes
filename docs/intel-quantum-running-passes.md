# Integrating User-Written Passes
Writing your own pass is no good on its own, it needs to be integrated with the rest of the compilation.  The compilation program in the Intel (R) Quantum SDK and the included script in the open-source repository offer several ways to do this.

## How Can I Run My Passes?
Passes can be run on their own using the `opt` tools included in the SDK or when built from source from the `quantum-intrinsics` directory.  This is relatively simple, using the command `opt -load-pass-plugin <library_loc> -p="<pass_name_1>,<pass_name_2>,...,<pass_name_n>" -o <output_file.ll> <input_file.ll>`.  However, you will need to start with one of the intermediate IR files in this case.  Intermediate files can be saved by using the `-k` command line argument.

They can be run in conjunction with the rest of the compilation pipeline through the use of the driver script included in the repository.  Each of the different stages of compilation have a difference name: precond, prelower, presynth, presched, presplit.  Each of these sections is marked near the relevant developer note in this document.  In order to specify passes at a particular point, first identify the location of the library with `-E <library_loc>`, second specify the section with `-a <pass_1>[,<pass_2>,...,<pass_n>]`.  The argument in this pass is a comma separated list of pass names.  If any arguments need to be passed to this section each argument must be prepended with a `-A`. So, for example, to pass in `-arg_name <arg_val>`, pass `-A -arg_name -A <arg_val>` to the driver script.

These stages are described in more depth in the _Intel Quantum Developer's Guide_.

## Where Can Passes Be Integrated?
See the **Developer Notes** in [Compilation Flow](./intel-quantum-compilation-flow.md) for instructions for where optimization passes can be added.

## What If I Want to Run My Pass with Intel Optimizations or an Intel Quantum Simulator?
There are two ways to integrate passes with the rest of Intel optimizations or run the resulting programs on the Intel Quantum Simulator.

The first is to use the driver script included in the provided repository until it provides a IR file at the end of compilation.  This can be given to the Intel Quantum SDK compiler to be compiled into a final binary and the simulator programs from whichever point the compilation was paused previously.  This is a two step option and only provides the final step of compilation.  The rest must be achieved by using the repository.

The second, is to use the driver program provided in the Intel Quantum SDK.  User-written passes can be added as they are with the included script in the passes repository.  However, when using the `-O1` specification, a different synthesis optimization will be run than in the open source repository.  Additionally, it will compile through to a final binary program.