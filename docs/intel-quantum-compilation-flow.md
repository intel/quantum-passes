# Description of Intel (R) Quantum Compiler Compilation Flow

The Intel Quantum Compiler runs many sets of compiler passes to take a program written against the SDK, to LLVM with embedded quantum instructions, to a executable linked against the Intel Quantum Runtime.  This document describes this process, and where user-written passes can be integrated into the compilation flow.  These are higher level overviews of what each pass in the pipeline does.  For deeper information, please see the comments in the passes themselves.

## Pass Pipeline
The following describes, in order, the different compilation passes and flow of a quantum program to compiled binary. Compilation start with a `.cpp` file that includes any quantum header files and simulator header files, which is passed through through the clang compiler, stopping once the process generates LLVM IR.

### Adding to the Frontend
Additions to the C++ frontend, such as the DSL constructs for the _Functional Extension for Quantum_ (FLEQ), are implemented as clang plugins.  You can find information about implement clang plugins [here](https://clang.llvm.org/docs/ClangPlugins.html).  These plugins walk the clang Abstract Syntax Tree (AST) and rewrites sections once certain patterns are found.  For example, in the case of FLEQ, operators are replaced with function calls to function declarations from `qexpr.h`.  However, for changes to the AST to propagate to the rest of compilation, the `AddBeforeMainAction` must be used instead.  If the default `AddAfterMainAction` is used, the LLVM IR Code generation will be run prior to the AST analysis and changes to the AST will not be propagated.

### Replace Quantum Intrinsics and Add Attributes
The clang compiler used contains no builtin quantum calls.  The header files contains function declarations for each gate, and for other quantum SDK functions, but are not immediately runnable on a machine.  This pass accepts the generated LLVM IR from the clang compiler, and checks every call instruction.  The function name check against a list of functions, and replace it with a corresponding intrinsic function.

Following this, the pass iterates over the call graph, and determine where each argument is used and what value is used in each argument.  By using the "leaves", of the call tree, certain arguments are marked as qubits, and certain arguments as classical bits.  There are similar strategies to identify what functions accept QExpr values, and which return QExpr values.  This allows us to differentiate between integers used as integers and integers used as the typedefs for quantum values.

### Flatten Quantum Kernels
At present, the Quantum Runtime is unable to run control flow mixed with quantum instructions within a specific quantum kernel.  All control flow within a specific quantum kernel must be able to be determined at compile time.  This pass recursively inlines all quantum kernels until all quantum instructions are in a single top-level function that is marked as a quantum kernel.

### Unroll Loops
This pass is directly related to the goals of flattening quantum kernels in the previous pass.  After flattening the quantum kernels, branching structures like conditionals and loops need to be reduced.  This is actually a set of passes that iteratively unrolls loops and uses constant folding in quantum kernels to remove conditional statements from the program until there loop unrolling no longer has an effect on the program.

**Pass Developer Note**
This is the first place that passes can be inserted into the pass pipeline. Specified as the `precond` section. At this point, you can expect there are no native quantum gates, and there should be no control flow constructs in the quantum kernels.  If there are special control flow constructs that need to be handled that have been introduced by additions to the frontend, this will likely be where to handle them.  This is also a good place to replace your own custom functions with an intrinsic if needed. 

### Validate and Condition Quantum Kernels
The validate and condition sequence of passes is designed to ensure that the compilation can move on to bigger operations on the quantum programs and resulting circuits.  This checks that constant folding was successful such that each qubit has been fully resolved to a specific allocation of variables.  If this is not the case, the circuit cannot be built.  Similarly, quantum kernels must be entirely flattened to contain no control flow constructs and that any other control flow constructs outside of quantum kernels do not include references to qubit objects.  In either case, it means the references cannot be resolved at compile time, and are unable to build a circuit that can be optimized.

**Pass Developer Note**
There is an extra compilation step only run when `-O1` is specified is the next stage where developer passes can be added.  Specified as the `presynth` section. It has the same requirements as the following `prelower` section. At this point, you can expect that the remaining control flow structures will contain no quantum instructions.

### Synthesis
During this section, the circuit will be analyzed and reduced to larger unitary operations before being resynthesized into a potentially different, more efficient set of gates.

**Pass Developer Note**
This is the next stage where developer passes can be added.  Specified as the `prelower` section. If there are optimizations that act on the canonical gate set provided by the frontend, they should be performed here.  At this point, you can expect that the remaining control flow structures will contain no quantum instructions.

### Placement on Given Architecture
This pass goes through and assigns a physical to each program qubit in the program. By default, this is a trivial placement.  Placement starts with the qubits that are allocated at a global scope, and assigns physical qubit to each in of allocation.  The same is done with each set of local qubits within each quantum kernel.  These qubits can be reused as they are only instantiated within a specific kernel, and do not have to persist between each section.

### Lower to Native Gates from Canonical Gates
As mentioned, the initial lowering to LLVM IR does not include intrinsics calls to quantum functions, only the header files that were included in the intrinsics header.  Here, the library based quantum operation are replaced with custom LLVM intrinsics, breaking them down into multiple gates if necessary, specifically if there is not a one-to-one analog.  This includes breaking down CNOT gate into Rotation gates along the X and Y axes, and a CPHASE gate.  This conversion will likely be dependent on the backend that is being used.

**Pass Developer Note**
At this point, there should be no canonical gates left, they should be replaced with native gate decompositions.  It is specified as the specified as the `presched` section. If a gate was added that is not caught by the decomposition passes, this is the point to replace it with a native gate.  Additionally, this is the stage before routing, and scheduling.

### Scheduling on Given Architecture
Placing the qubits on a given architecture would be enough if every architecture had all-to-all connectivity, but this is not the case.  In some cases, qubits may need to interact with one another, but have no physical connection on device. In this case, we can add SWAP gates to move the data of a qubit from one physical qubit to another.  The initial placment uses the placement defined via a previous pass.  The default strategy uses a greedy strategy to move qubits within range of one another, perform the operation, and move the qubits back along a similar path to maintain some degree of locality.  However, there are different backwards routing strategies.

### Convert from Qubit to QID
After scheduling, many operations will have been remapped to physical qubits, but it is possible that not all qubits have been rewritten by this point.  This pass replaces all references to program qubits with references to physical qubits instead via a qid.  In addition, if there are allocations that do not have a physical mapping, they are replaced with an arbitrary mapping as well.

### Spin Angles to be In-Range
The final step in this stage is to correct the angle parameters given in separate gates are reduced to be between 0 and 2*Pi, as the parameters are periodic, this should not change the effect of the gates themselves. In this case, the pass simply iterates over the parameterized gates with immediate values, and correct them as necessary.  This does not occur if the arguments are dynamic.

**Pass Developer Note**
This is the last opportunity for third-party passes to be run.  At this point, the program acts on physical qubits and spin-native gates rather than the canonical gates.  It is specified as the `presplit` stage. Any changes made at this point must honor the connectivity of a device.  Passes that care about the physical qubits that the circuit is being run on should be made here.  Or, if you only want to optimize what will be run, and need guarantees about which qubits you are acting on, or the kind of gate that is being used, this is the place to do it.

After this stage of passes, user generated passes cannot be run.  If in the following passes you identify an element that is not handled, they should be handled at this stage.

### Lower to Immediate Version of Gates
This pass ensures that references to classical bits, or parameters referenced from data arrays are resolved.  That is, the instructions only use immediate values, the instructions contain a value rather than a reference to a global value or a register.  It is either a physical device index, or a floating point value.

At this stage, a quantum shared double array in created.  The angle arguments are converted into indices to be used to find values in this array before the quantum program is run.

### Separate the Quantum Basic Blocks
Here, the quantum program is prepared for hybrid execution.  The Intel Quantum SDK needs to separate the quantum code, and generate assembly language specifically for the Intel Quantum Backends, such as the [Intel Quantum Simulator](https://intel-qs.readthedocs.io/en/docs/index.html).  To do this, for each quantum kernel, the quantum basic block is isolated from the rest of the quantum kernel and put inside its own function.  Then, the content of the quantum basic block is replaced with a Quantum Runtime call that accepts a pointer to the newly made function.

### Split Classical and Quantum Modules
This pass simply takes the quantum functions made in the separation path and splits them into a separate file.  This allows them to be lowered separately into the distinct quantum backend developed for Intel systems.

### Finalize Compilation
This final step before code generation ensures that all of the needed pieces are included for code generation and linking.  This entails adding the needed Quantum Runtime calls, and ensuring that the references to global shared arrays are resolved for the Quantum Runtime when running in the quantum and classical environments.
