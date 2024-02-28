#!/bin/bash
###############################################################################
#
# Copyright (C) 2023 Intel Corporation
# SPDX-License-Identifier: Apache-2.0
#
###############################################################################
# This script serves as commandline user interface for the Quantum Compiler in
# the Intel(R) Quantum Software Development Kit (SDK).
#
# This workflow requires pre-built LLVM (with Intel Quantum extensions)
# binaries.
###############################################################################

TOOL_NAME="Intel(R) Quantum Compiler: 2023 (Developers' utility)"
ME="$(basename "$0")"
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"

DECORATOR="-------------------------------------------------------------------------------"

LLVM_BIN_DIR="$SCRIPT_DIR/../iqc_install/bin"
OOT_LIB_DIR="$SCRIPT_DIR/build/lib"
USER_GENERATED_PASS_PATH=""

## Defaults
INPUT_FILE=""
OUTPUT_DIR=$(pwd)
CONFIG_FILE=""
SDK_PATH=$SCRIPT_DIR

HYBRID_MODE="false"
SHARED_MODE="false"
BRIDGE_MODE="false"

OPTIMIZATION_OPT=0
VERBOSE_MODE=0
PRINT_CIRCUIT_QBB=0
USER_PLACED_PASS_MODE=0
KEEP_FILES=0

QUEUEING=""
PLACEMENT_PASS="simple-qbit-placement" ## equivalent to 'none', but w/o need of config file
PLACEMENT=""
SCHED_METHOD_FR="none"
SCHED_METHOD_BK="retrace"
USER_GENERATED_PASSES_TO_RUN_PRECONDITION=""
USER_GENERATED_PASS_ARGS_PRECONDITION=()
USER_GENERATED_PASSES_TO_RUN_PRESYNTHESIS=""
USER_GENERATED_PASS_ARGS_PRESYNTHESIS=()
USER_GENERATED_PASSES_TO_RUN_PRESCHEDULE=""
USER_GENERATED_PASS_ARGS_PRESCHDULE=()
USER_GENERATED_PASSES_TO_RUN_PRELOWER=""
USER_GENERATED_PASS_ARGS_PRELOWER=()
USER_GENERATED_PASSES_TO_RUN_PRESPLIT=""
USER_GENERATED_PASS_ARGS_PRESPLIT=()

QASMBRIDGE_RUNNER=""
declare -a OTHER_INCLUDE_DIRS=()
declare -a OTHER_LIB_DIRS=()
declare -a OTHER_RUNTIME_LIB_DIRS=()
declare -a LIBRARY_LIST=()
declare -a CLANG_FLAGS=()

## Include directory
INCLUDE_DIR() {
    realpath "$LLVM_BIN_DIR/../include"
}
## Compiler executables
CLANG() {
    echo "$(realpath "$LLVM_BIN_DIR")/clang++"
}
LLC() {
    realpath "$LLVM_BIN_DIR/llc"
}
LLD() {
    echo "$(realpath "$LLVM_BIN_DIR")/ld.lld"
}
OPT() {
    realpath "$LLVM_BIN_DIR/opt"
}
LIBRARY_LOCATION() {
    realpath "$OOT_LIB_DIR"
}
TOOLS_LOCATION() {
    realpath "$OOT_LIB_DIR/../bin"
}

GET_ALL_INCLUDE_DIRS() {
    ARG="--include-directory=$(INCLUDE_DIR)"
    for DIR in "${OTHER_INCLUDE_DIRS[@]}"; do
        ARG="${ARG} --include-directory=$DIR"
    done
    echo "$ARG"
}

GET_ALL_LIBRARY_DIRS() {
    ARG=""
    for DIR in "${OTHER_LIB_DIRS[@]}"; do
        ARG="${ARG} -L$DIR"
    done
    echo "$ARG"
}

GET_ALL_RUNTIME_LIBRARY_DIRS() {
    ARG=""
    for DIR in "${OTHER_RUNTIME_LIB_DIRS[@]}"; do
        ARG="${ARG} -rpath $DIR"
    done
    echo "$ARG"
}

## Following is invoked only when 'hybrid' is enabled, i.e. to get TPL / QRT
## dependencies
GET_DEPENDENCY_LIBRARIES() {
    ARG="-lpthread -ldl"
    for LIB in "${LIBRARY_LIST[@]}"; do
        ARG="${ARG} -l$LIB"
    done
    echo "$ARG"
}

GET_CLANG_FLAGS() {
    ARG=""
    for FLAG in "${CLANG_FLAGS[@]}"; do
        ARG="${ARG} $FLAG"
    done
    echo "$ARG"
}

###############################################################################
start_message() {
    echo "==============================================================================="
    echo "        $TOOL_NAME"
    echo "==============================================================================="
}

###############################################################################
show_help() {
    echo
    echo "Usage: ./$ME -q -b <BIN_DIR> -c <CONFIG_JSON> -o <OUT_DIR> -I <DIR> -L <DIR> -R <DIR> -l <LIB_NAME> INPUT_FILENAME"
    echo
    echo "Required argument:"
    echo "  INPUT_FILENAME :  Input C++/Openqasm file to process."
    echo
    echo "Optional arguments"
    echo "           -q :   Enable hybrid (quantum + classical) compilation."
    echo "                  Must provided path to QRT files. Disabled by default."
    echo " -b <BIN_DIR> :   Path of the LLVM / Clang bin directory containing all the"
    echo "                  compiler binaries. Defaults to <project-root>/../iqc_install/bin."
    echo " -t <OOT_DIR> :   Path of the out of tree libraries for IntelQuantum. Defaults"
    echo "                  to <script directory>/build/lib."
    echo "           -B :   Translate the openqasm input file to C++ file."
    echo " -o <OUT_DIR> :   Path of the output directory to save resultant file(s)."
    echo "                  Defaults to the current working directory."
    echo "     -I <DIR> :   Add directory to include search path."
    echo "     -L <DIR> :   Add directory to library search path."
    echo "     -R <DIR> :   Add directory to runtime library search path."
    echo "-l <LIB_NAME> :   Root name of library to use."
    echo "    -c <FILE> :   Path of the configuration file."
    echo "                  Defaults to 'intel-quantum-sdk.json'"
    echo "    -f <FLAG> :   Pass flag to clang."
    echo "    -F <FLAG> :   Pass flag to FLEQ compilation."
    echo "                  Usage example: -F <opt1> -F <opt2> ..."
    echo "                  Options:"
    echo "                  recursion-limit-power=<INT> : sets the FLEQ recursion limit to scale as a"
    echo "                                                power <INT> of the number of global qubits;"
    echo "                                                default = 1."
    echo "                  recursion-limit=<INT> : set the FLEQ recursion limit to a fixed number <INT>;"
    echo "                                          default = max(1000, or <Value from power>)."
    echo "                  verbose-cleanup=<BOOL> : prints to screen warnings related to the clean-up of"
    echo "                                           QList and DataList function; default = false."
    echo "                  bb-cleanup=<BOOL> : FLEQ compilation cleans up trivial branching in the IR;"
    echo "                                      default = true."
    echo "                  print=<OPT> : select when to print to screen any messages coming from QExpr"
    echo "                                print functions; OPT can be 'never', 'fail', 'success', 'always';"
    echo "                                default = 'always'."
    echo "     -O <0-1> :   Quantum optimization options; see README for description."
    echo "                  Defaults to 0 or minimal optimization."
    echo "           -v :   Verbose mode; Print basic statistics for each quantum kernel"
    echo "  -P <format> :   Print the circuit diagram for each quantum basic block"
    echo "                  Usage example: -P tex -P console -P json"
    echo "                  Options:"
    echo "                      tex: Prints out the circuit latex files for each quantum basic block (QBB)."
    echo "                           See documentation for suggestions of how to render into a pdf."
    echo "                  console: Prints out the circuit in the console for each QBB."
    echo "                     json: Prints out the circuit JSON files for each QBB in <OUT_DIR>/Visualization/."
    echo "    -p <type> :   Uses the type of qubit initial placement requested, defaults to 'none'"
    echo "                  Must provide a configuration file to specify platform details (connectivity etc.)"
    echo "                  Usage example (choose only one): -p trivial -p dense -p custom"
    echo "  -S <method> :   Uses the type of forward scheduling method requested, defaults to 'none'"
    echo "                  Must provide a configuration file to specify platform details (connectivity etc.)"
    echo "                  Usage example (choose only one): -S none -S greedy"
    echo "  -K <method> :   Uses the type of backward scheduling method requested, defaults to 'retrace'"
    echo "                  Must provide a configuration file to specify platform details (connectivity etc.)"
    echo "                  Usage example (choose only one): -K retrace -K bubble-sort -K oddeven-sort -K grid"
    echo "                  Refer to documentation for details on each method."
    echo "           -k :   Keep intermediate compilation files, do not remove at the end of compilation."
    echo ""
    echo "User Generated Pass Options"
    echo " -E <OPT_LIB> :   Specification of where to find user-written library of passes."
    echo " -e <OPT_LOC> :   Specification of when to run user-generated passes."
    echo "                  Defaults to: prelower. Options are:"
    echo "                    precondition (Before the quantum modules have been validated)"
    echo "                    presynth (Before synthesis when available, only available in >0 -O settings)"
    echo "                    prelower (Before gates are lowered to native decompositions)"
    echo "                    presched (Before gates are scheduled)"
    echo "                    presplit (Before the quantum operations are separated from classical operations)"
    echo "  -a <PASSES> :   Specification of what passes to run based on the current OPT_LOC. Is a"
    echo "                  comma-delimited list of pass names."
    echo "-A <PASS_ARG> :   Specification of what arguments are to be given to the passes during the currently"
    echo "                  specified OPT_LOC. Every argument to be given to the passes must be prefixed by this"
    echo "                  command line argument."
    echo " -k           :   Keep intermediate files."
    echo "$DECORATOR"
    echo "Usage: ./$ME -h"
    echo
    echo "Display this help message."
    echo "$DECORATOR"
    exit
}

###############################################################################
check_input_file() {
    if [ ! -f "$INPUT_FILE" ]; then
        echo "Invalid input file. Exiting..."
        exit 1
    fi
    echo "Processing source file:"
    echo "$INPUT_FILE "
    echo "$DECORATOR"
}

###############################################################################
check_llvm_bin_dir() {
    # Check directory exists
    if [ ! -d "$LLVM_BIN_DIR" ]; then
        echo "Directory $LLVM_BIN_DIR does not exist. Exiting..."
        exit 1
    fi
    echo "$LLVM_BIN_DIR"
    # Check required binaries exist
    [[ ! -f "$(CLANG)" ]] && echo "clang executable not found. Exiting..." && exit 1
    [[ ! -f "$(LLC)" ]] && echo "llc executable not found. Exiting..." && exit 1
    [[ ! -f "$(LLD)" ]] && echo "lld executable not found. Exiting..." && exit 1
    [[ ! -f "$(OPT)" ]] && echo "opt executable not found. Exiting..." && exit 1
    echo ""
}

###############################################################################
check_oot_lib_dir() {
    # Check directory exists
    if [ ! -d "$OOT_LIB_DIR" ]; then
        echo "Directory $OOT_LIB_DIR does not exist. Exiting..."
        exit 1
    fi
    echo "$OOT_LIB_DIR"
    # Check required libraries exist
    [[ ! -f "$OOT_LIB_DIR/libClangQuantumExtensions.so" ]] && echo "Quantum extensions library not found. Exiting..." && exit 1
    [[ ! -f "$OOT_LIB_DIR/libIntelQuantumIRPasses_shared.so" ]] && echo "Quantum passes library not found. Exiting..." && exit 1
    echo ""
}

###############################################################################
check_output_dir() {
    # Check directory exists
    if [ ! -d "$OUTPUT_DIR" ]; then
        echo "Output directory $OUTPUT_DIR does not exist. Exiting..."
        exit 1
    else
    # Check directory is writable
        path_to_test_file="$OUTPUT_DIR/.temp_file"
        if touch "$path_to_test_file"; then
            rm -f "$path_to_test_file"
        else
            echo "Output directory $OUTPUT_DIR is not writable. Exiting..."
            exit 1
        fi
    fi
}

###############################################################################
check_config_file() {
    if [[ ! -f "$CONFIG_FILE" ]]; then
        echo "ERROR: Invalid configuration file. Exiting..."
        exit 1
    fi
}

###############################################################################
collect_mpi_options() {
    base="$1"
    # ASKME: Okay to require Intel MPI only?
    # TODO: Check directory structure for other distributions of MPI
    ## MPI include directory
    dir="$base"/"include"
    if [ -d "$dir" ]; then
        OTHER_INCLUDE_DIRS+=("$(realpath "$dir")")
    else
        echo "ERROR: Missing MPI include directory."
        exit 1
    fi
    ## MPI library directories
    dir="$base"/"lib"
    if [ -d "$dir" ]; then
        OTHER_LIB_DIRS+=("$(realpath "$dir")")
    else
        echo "ERROR: Missing MPI library directory."
        exit 1
    fi
    dir="$base"/"lib/release"
    if [ -d "$dir" ]; then
        OTHER_LIB_DIRS+=("$(realpath "$dir")")
    else
        echo "ERROR: Missing MPI library directory."
        exit 1
    fi
    LIBRARY_LIST+=("mpicxx" "mpifort" "mpi" "rt" "pthread" "gomp")
    echo "NOTE: Application enabled with MPI. Execute with 'mpirun'."
}

parse_options() {
    while getopts ":hb:t:Bo:I:L:R:l:vqsc:p:O:P:S:K:mkf:a:A:e:E:F:" opt; do
        case "$opt" in
            h)
                show_help
                ;;
            b)
                LLVM_BIN_DIR="$(realpath "$OPTARG")"
                check_llvm_bin_dir
                ;;
            t)
                OOT_LIB_DIR="$(realpath "$OPTARG")"
                check_oot_lib_dir
                ;;
            B)
                BRIDGE_MODE="true"
                ;;
            o)
                OUTPUT_DIR="$(realpath "$OPTARG")"
                check_output_dir
                ;;
            I)
                OTHER_INCLUDE_DIRS+=("$(realpath "$OPTARG")")
                ;;
            L)
                OTHER_LIB_DIRS+=("$(realpath "$OPTARG")")
                ;;
            R)
                OTHER_RUNTIME_LIB_DIRS+=("$(realpath "$OPTARG")")
                ;;
            l)
                LIBRARY_LIST+=("$OPTARG")
                ;;
            q)
                HYBRID_MODE="true"
                ;;
            s)
                SHARED_MODE="true"
                ;;
            c)
                CONFIG_FILE="$(realpath "$OPTARG")"
                check_config_file
                ;;
            f)
                CLANG_FLAGS+=("$OPTARG")
                ;;
            F)
                FLEQ_AND_SYNTHESIS_FLAGS+=("-fleq-$OPTARG")
                ;;
            k)
                KEEP_FILES=1
                ;;
            v)
                VERBOSE_MODE=1
                ;;
            O)
                if [ "$OPTARG" -ge 2 ]; then
                    echo "No optimization option $OPTARG available..."
                    exit
                fi
                OPTIMIZATION_OPT=$OPTARG
                ;;
            P)
                PRINT_CIRCUIT_QBB=1
                prints+=("$OPTARG")
                ;;
            m)
                QUEUEING="-queue-measurements"
                ;;
            p)
                PLACEMENT_PASS="q-init-placement"
                PLACEMENT=("--placement-method" "$OPTARG")
                ;;
            S)
                if [[ "$OPTARG" == "greedy" ]] || [[ "$OPTARG" == "none" ]] || [[ "$OPTARG" == "serial" ]] || [[ "$OPTARG" == "none,serial" ]] || [[ "$OPTARG" == "greedy,serial" ]]; then
                    SCHED_METHOD_FR="$OPTARG"
                else
                    echo "ERROR: Unsupported forward scheduling method."
                    exit 1
                fi
                ;;
            K)
                if [[ "$OPTARG" == "retrace" ]] || [[ "$OPTARG" == "bubble-sort" ]] || [[ "$OPTARG" == "oddeven-sort" ]] || [[ "$OPTARG" == "grid" ]]; then
                    SCHED_METHOD_BK="$OPTARG"
                else
                    echo "ERROR: Unsupported backward scheduling method."
                    exit 1
                fi
                ;;
            E)
                USER_PLACED_PASS_MODE=1
                USER_GENERATED_PASS_PATH="$OPTARG"
                ;;
            e)
                USER_PLACED_PASS_MODE=1
                CURRENT_PASS_SECTION="$OPTARG"
                ;;
            a)
                USER_PLACED_PASS_MODE=1
                if [ "$CURRENT_PASS_SECTION" == "precond" ]; then
                    USER_GENERATED_PASSES_TO_RUN_PRECONDITION="$OPTARG"
                elif [ "$CURRENT_PASS_SECTION" == "prelower" ]; then
                    USER_GENERATED_PASSES_TO_RUN_PRELOWER="$OPTARG"
                elif [ "$CURRENT_PASS_SECTION" == "presynth" ]; then
                    USER_GENERATED_PASSES_TO_RUN_PRESYNTHESIS="$OPTARG"
                elif [ "$CURRENT_PASS_SECTION" == "presched" ]; then
                    USER_GENERATED_PASSES_TO_RUN_PRESCHEDULE="$OPTARG"
                elif [ "$CURRENT_PASS_SECTION" == "presplit" ]; then
                    USER_GENERATED_PASSES_TO_RUN_PRESPLIT="$OPTARG"
                else
                    echo "Unknown optimization location $CURRENT_PASS_SECTION";
                    exit 1
                fi
                ;;
            A)
                if [ "$CURRENT_PASS_SECTION" == "precond" ]; then
                    USER_GENERATED_PASS_ARGS_PRECONDITION+=("$OPTARG")
                elif [ "$CURRENT_PASS_SECTION" == "prelower" ]; then
                    USER_GENERATED_PASS_ARGS_PRELOWER+=("$OPTARG")
                elif [ "$CURRENT_PASS_SECTION" == "presynth" ]; then
                    USER_GENERATED_PASS_ARGS_PRESYNTHESIS+=("$OPTARG")
                elif [ "$CURRENT_PASS_SECTION" == "presched" ]; then
                    USER_GENERATED_PASS_ARGS_PRESCHEDULE+=("$OPTARG")
                elif [ "$CURRENT_PASS_SECTION" == "presplit" ]; then
                    USER_GENERATED_PASS_ARGS_PRESPLIT+=("$OPTARG")
                else
                    echo "Unknown optimization location $CURRENT_PASS_SECTION";
                    exit 1
                fi
                ;;
            *)
                echo "ERROR: Invalid options provided. Please refer to the help message for supported options..."
                show_help
                ;;
        esac
    done
    INPUT_VAL="${*:$OPTIND:1}"
    if [ -n "${INPUT_VAL}" ]; then
        INPUT_FILE="$(realpath "${@:$OPTIND:1}")"
        check_input_file
    else
        echo "No input file provided. Exiting..."
        exit
    fi
}

###############################################################################
qasmbridge() {
    if [ "$BRIDGE_MODE" == "true" ]; then
        filename=$(basename -- "$INPUT_FILE")
        extension="${filename##*.}"
        filename="${filename%.*}"
        output_filename="$OUTPUT_DIR/$filename.cpp"

        echo "Translating openqasm file..."
        python "$QASMBRIDGE_RUNNER" "$INPUT_FILE" "$OUTPUT_DIR" "$filename"

        if [ -f "$output_filename" ]; then
            echo "Final translated file: "
            realpath "$output_filename"
            echo "$DECORATOR"
        else
            echo "Unexpected translation error. Exiting..."
            exit 1
        fi
        exit 0
    fi
}

###############################################################################

# Function to run printer pass
# Takes 3 arguments, first is the IR file
# Second argument is the name of the compilation step we have reached
# Third argument is whether IR has been lowered to native gates yet 

printerPass() {
    LOWERED=""
    if [ "$3" == true ]; then
        LOWERED=",spin-native-annotate"
    fi

    for print_method in "${prints[@]}"; do
        PRINTER_OUTPUT_DIR="$OUTPUT_DIR"/
        echo "Printing circuit diagrams to $print_method..."
        if [ "$print_method" == "json" ]; then
            echo "Printing json circuit diagrams to directory 'Visualization'..."
        fi

        "$(OPT)" -S -load-pass-plugin "$(LIBRARY_LOCATION)"/libIntelQuantumIRPasses_shared.so ${QUEUEING:+"$QUEUEING"} -p="q-annotations-to-json$LOWERED,print-circuit-qbb" --print-format "$print_method" -comp-position="$2" -odir "$PRINTER_OUTPUT_DIR" "$1" -o "$1"
    done
}

###############################################################################
compile() {
    filename=$(basename -- "$INPUT_FILE")
    extension="${filename##*.}"
    filename="${filename%.*}"

    ## IR files
    ir_file="$OUTPUT_DIR"/"$filename".ll
    ir_flattened="$OUTPUT_DIR"/"$filename"_flattened.ll
    ir_loop_unrolled="$OUTPUT_DIR"/"$filename"_unrolled.ll
    ir_loop_unrolled_user="$OUTPUT_DIR"/"$filename"_unrolled_user.ll
    ir_replaced="$OUTPUT_DIR"/"$filename"_replaced.ll
    ir_conditioned="$OUTPUT_DIR"/"$filename"_conditioned.ll
    ir_conditioned_user="$OUTPUT_DIR"/"$filename"_conditioned_user.ll
    ir_synthesized="$OUTPUT_DIR"/"$filename"_synthesized.ll
    ir_synthesized_user="$OUTPUT_DIR"/"$filename"_synthesized_user.ll
    ir_lowered="$OUTPUT_DIR"/"$filename"_lowered.ll
    ir_lowered_user="$OUTPUT_DIR"/"$filename"_lowered_user.ll
    ir_scheduled="$OUTPUT_DIR"/"$filename"_scheduled.ll
    ir_scheduled_user="$OUTPUT_DIR"/"$filename"_scheduled_user.ll
    ir_separated="$OUTPUT_DIR"/"$filename"_separated.ll
    ir_quantum_final="$OUTPUT_DIR"/"$filename"_separated_quantum.ll
    ir_classical_final="$OUTPUT_DIR"/"$filename"_classical_final.ll

    ## Assembly file, Object file, and executable
    qs_64_file="$OUTPUT_DIR"/"$filename".qs
    qo_64_file="$OUTPUT_DIR"/"$filename".qo
    hybrid_exe_file="$OUTPUT_DIR"/"$filename"

    ## Quantum Loop Unrolling Arguments
    ## The following list of arguments is based of the expanded form of -O1 and -O3, with the base
    ## -loop-unroll pass replaced with the quantum specific -q-loop-unroll pass. The original list
    ## of arguments is included for reference purposes.
    ##
    ## -O1 -loops -loop-simplify -loop-rotate -lcssa -loop-idiom -loop-deletion -loop-unroll -unroll-threshold=100000000 -sccp -instcombine -early-cse -mem2reg -simplifycfg
    quantum_loop_unroll_O1="-passes=no-op-module,function(instcombine)"
    quantum_loop_unroll_O1+=",rpo-function-attrs,globalopt,globaldce,float2int"
    quantum_loop_unroll_O1+=",lower-constant-intrinsics,loop-simplify,lcssa"
    quantum_loop_unroll_O1+=",loop-rotate"
    quantum_loop_unroll_O1+=",loop-distribute"
    quantum_loop_unroll_O1+=",loop-vectorize,loop-simplify"
    quantum_loop_unroll_O1+=",loop-load-elim"
    quantum_loop_unroll_O1+=",instcombine,simplifycfg"
    quantum_loop_unroll_O1+=",instcombine,loop-simplify,lcssa"
    quantum_loop_unroll_O1+=",cgscc(function(loop(q-loop-unroll))),instcombine,loop-simplify"
    quantum_loop_unroll_O1+=",lcssa,function(loop-mssa(licm))"
    quantum_loop_unroll_O1+=",transform-warning,alignment-from-assumptions,strip-dead-prototypes"
    quantum_loop_unroll_O1+=",loop-simplify,lcssa,loop-sink"
    quantum_loop_unroll_O1+=",instsimplify,div-rem-pairs,simplifycfg"
    quantum_loop_unroll_O1+=",loop-simplify,lcssa,loop-rotate,lcssa"
    quantum_loop_unroll_O1+=",loop-idiom,loop-deletion,no-op-loop,cgscc(function(loop(q-loop-unroll))),sccp"
    quantum_loop_unroll_O1+=",instcombine,early-cse,mem2reg,simplifycfg,verify"
    ## -O3 -loops -loop-simplify -loop-rotate -lcssa -loop-idiom -loop-deletion -loop-unroll -unroll-threshold=100000000 -sccp -instcombine -early-cse -mem2reg -simplifycfg
    quantum_loop_unroll_O3="-passes=forceattrs,inferattrs,callsite-splitting,ipsccp,called-value-propagation,attributor,globalopt"
    quantum_loop_unroll_O3+=",deadargelim"
    quantum_loop_unroll_O3+=",instcombine,simplifycfg,inline,function-attrs"
    quantum_loop_unroll_O3+=",argpromotion,sroa,speculative-execution"
    quantum_loop_unroll_O3+=",jump-threading,correlated-propagation,simplifycfg,aggressive-instcombine"
    quantum_loop_unroll_O3+=",instcombine,libcalls-shrinkwrap"
    quantum_loop_unroll_O3+=",pgo-memop-opt"
    quantum_loop_unroll_O3+=",tailcallelim,simplifycfg,reassociate"
    quantum_loop_unroll_O3+=",loop-simplify,lcssa,loop-rotate"
    quantum_loop_unroll_O3+=",function(loop-mssa(licm)),simple-loop-unswitch,simplifycfg"
    quantum_loop_unroll_O3+=",instcombine,loop-simplify,lcssa,indvars"
    quantum_loop_unroll_O3+=",loop-idiom,loop-deletion,function(loop(q-loop-unroll)),mldst-motion"
    quantum_loop_unroll_O3+=",gvn,memcpyopt,sccp"
    quantum_loop_unroll_O3+=",bdce,instcombine"
    quantum_loop_unroll_O3+=",jump-threading,correlated-propagation,dse"
    quantum_loop_unroll_O3+=",loop-simplify,lcssa,function(loop-mssa(licm)),adce,simplifycfg"
    quantum_loop_unroll_O3+=",instcombine"
    quantum_loop_unroll_O3+=",elim-avail-extern,rpo-function-attrs,globalopt,globaldce,float2int"
    quantum_loop_unroll_O3+=",lower-constant-intrinsics,lcssa"
    quantum_loop_unroll_O3+=",loop-rotate"
    quantum_loop_unroll_O3+=",loop-distribute"
    quantum_loop_unroll_O3+=",loop-vectorize,loop-simplify"
    quantum_loop_unroll_O3+=",loop-load-elim"
    quantum_loop_unroll_O3+=",instcombine,simplifycfg"
    quantum_loop_unroll_O3+=",slp-vectorizer"
    quantum_loop_unroll_O3+=",instcombine,loop-simplify,lcssa,function(loop(q-loop-unroll))"
    quantum_loop_unroll_O3+=",instcombine,loop-simplify,lcssa"
    quantum_loop_unroll_O3+=",function(loop-mssa(licm)),transform-warning"
    quantum_loop_unroll_O3+=",alignment-from-assumptions,strip-dead-prototypes,globaldce,constmerge"
    quantum_loop_unroll_O3+=",lcssa,loop-sink"
    quantum_loop_unroll_O3+=",instsimplify,div-rem-pairs,simplifycfg"
    quantum_loop_unroll_O3+=",loop-simplify,lcssa,loop-rotate,lcssa"
    quantum_loop_unroll_O3+=",loop-idiom,loop-deletion,function(loop(q-loop-unroll)),sccp"
    quantum_loop_unroll_O3+=",instcombine,early-cse,mem2reg,simplifycfg,verify"
    if [[ $extension != "cpp" && $extension != "ll" ]]; then
        echo "Invalid input file, expected C++ source file or LLVM source file, Exiting..."
        exit 1
    fi

    if [ "$SHARED_MODE" == "true" ] && [ "$HYBRID_MODE" == "true" ]; then
        echo "Cannot use both shared and hybrid mode. Exiting..."
        exit 1
    fi

    if [ "$extension" == "cpp" ]; then
        if [ "$SHARED_MODE" == "true" ]; then
            read -ra ALL_INCLUDE_DIRS < <(GET_ALL_INCLUDE_DIRS)
            read -ra ALL_CLANG_FLAGS < <(GET_CLANG_FLAGS)
            "$(CLANG)" "${ALL_INCLUDE_DIRS[@]}" "${ALL_CLANG_FLAGS[@]}" -fPIC -O1 -S -emit-llvm -Xclang -load -Xclang "$(LIBRARY_LOCATION)"/libClangQuantumExtensions.so -Xclang -add-plugin -Xclang rewrite-qexpr "$INPUT_FILE" -o "$ir_file"
        else
            read -ra ALL_INCLUDE_DIRS < <(GET_ALL_INCLUDE_DIRS)
            read -ra ALL_CLANG_FLAGS < <(GET_CLANG_FLAGS)
            "$(CLANG)" "${ALL_INCLUDE_DIRS[@]}" "${ALL_CLANG_FLAGS[@]}" -O1 -S -emit-llvm -Xclang -load -Xclang "$(LIBRARY_LOCATION)"/libClangQuantumExtensions.so -Xclang -add-plugin -Xclang rewrite-qexpr "$INPUT_FILE" -o "$ir_file"
        fi

        result="$?"
        if [ ! "$result" -eq 0 ]; then
            echo "Unexpected error. Exiting..."
            exit 1
        fi

        ir_file_stage=0
    else
        ir_file="$INPUT_FILE"
        tools_loc="$(TOOLS_LOCATION)"
        ir_file_stage=$("$tools_loc"/quantum-module-stage "$ir_file")
    fi

    echo "Intermediate representation (IR) output file: "
    realpath "$ir_file"

    echo "$DECORATOR"
    if [ "$ir_file_stage" -eq 0 ]; then
        echo "Transforming IR..."

        "$(OPT)" -S -load-pass-plugin "$(LIBRARY_LOCATION)"/libIntelQuantumIRPasses_shared.so -p="insert-q-intrinsics,insert-q-attrs" "$ir_file" -o "$ir_replaced"
        result="$?"
        if [ ! "$result" -eq 0 ]; then
            echo "Unexpected error replacing quantum functions. Exiting..."
            exit 1
        fi

        "$(OPT)" -S -load-pass-plugin "$(LIBRARY_LOCATION)"/libIntelQuantumIRPasses_shared.so -p="flatten-qk,function(dce)" "$ir_replaced" -o "$ir_flattened"
        result="$?"
        if [ ! "$result" -eq 0 ]; then
            echo "Unexpected error inlining functions. Exiting..."
            exit 1
        fi

        ### NOTE: Run unrolling step multiple times in a for loop until no change, or 51 times (whichever is earlier)
        "$(OPT)" -S -load-pass-plugin "$(LIBRARY_LOCATION)"/libIntelQuantumIRPasses_shared.so "$quantum_loop_unroll_O1" "$ir_flattened" -o "${ir_loop_unrolled}_0"
        for i in {1..50}
        do
            "$(OPT)" -S -load-pass-plugin "$(LIBRARY_LOCATION)"/libIntelQuantumIRPasses_shared.so "$quantum_loop_unroll_O3" "${ir_loop_unrolled}_$((i-1))" -o "${ir_loop_unrolled}_$i"
            ## compare the number of lines
            if [ "$(wc -l < "${ir_loop_unrolled}_$((i-1))")" -eq "$(wc -l < "${ir_loop_unrolled}_$i")" ]; then
                ## stop unrolling
                cp "${ir_loop_unrolled}_$i" "$ir_loop_unrolled"
                rm "${ir_loop_unrolled}_$((i-1))"
                rm "${ir_loop_unrolled}_$i"
                break
            else
                rm "${ir_loop_unrolled}_$((i-1))"
            fi
        done
        if [ "$i" -eq 50 ]; then
            cp "${ir_loop_unrolled}_$i" "$ir_loop_unrolled"
            rm "${ir_loop_unrolled}_$((i-1))"
            rm "${ir_loop_unrolled}_$i"
        fi
        rm -f "${ir_loop_unrolled}_0"
    else
        cp "$ir_file" "$ir_replaced"
        cp "$ir_replaced" "$ir_flattened"
        cp "$ir_flattened" "$ir_loop_unrolled"
    fi

    if [ "$ir_file_stage" -lt 2 ]; then
        if [[ "$USER_PLACED_PASS_MODE" -eq 1 && "$USER_GENERATED_PASSES_TO_RUN_PRECONDITION" != "" ]]; then 
            echo "Running User-Specified "
            "$(OPT)" -S -load-pass-plugin "$(LIBRARY_LOCATION)"/libIntelQuantumIRPasses_shared.so -load-pass-plugin "$USER_GENERATED_PASS_PATH" -p="q-annotations-to-json,spin-native-annotate,$USER_GENERATED_PASSES_TO_RUN_PRECONDITION" "${USER_GENERATED_PASS_ARGS_PRECONDITION[@]}" "$ir_loop_unrolled" -o "$ir_loop_unrolled_user"
            result="$?"
            if [ ! "$result" -eq 0 ]; then
                echo "Unexpected user pass error. Exiting..."
                exit 1
            fi
        else
            cp "$ir_loop_unrolled" "$ir_loop_unrolled_user"
        fi

        echo "Validating and processing quantum kernels..."
        "$(OPT)" -S -load-pass-plugin "$(LIBRARY_LOCATION)"/libIntelQuantumIRPasses_shared.so -p="q-annotations-to-json,validate-and-condition-qbb" "$ir_loop_unrolled_user" -o "$ir_conditioned"

        result="$?"
        if [ ! "$result" -eq 0 ]; then
            echo "Unexpected conditioning error. Exiting..."
            exit 1
        fi
        
        if [ "$PRINT_CIRCUIT_QBB" -eq 1 ]; then
            echo "Printing quantum circuit diagrams before decomposition to native gates..."
            printerPass "$ir_conditioned" "canonical" false
        fi

        if [ "$VERBOSE_MODE" -eq 1 ]; then
            echo "Printing quantum circuit stats before decomposition to native gates..."
            "$(OPT)" -S -load-pass-plugin "$(LIBRARY_LOCATION)"/libIntelQuantumIRPasses_shared.so ${QUEUEING:+"$QUEUEING"} -p="q-annotations-to-json,simple-qbit-placement,q-stats-print" -q-config="$CONFIG_FILE" -q-sdk="$SDK_PATH" "$ir_conditioned" -o "$ir_conditioned_user"
            echo "$DECORATOR"
        fi
    else
        cp "$ir_loop_unrolled" "$ir_loop_unrolled_user"
        cp "$ir_loop_unrolled_user" "$ir_conditioned"
    fi

    if [ "$ir_file_stage" -lt 3 ]; then
        echo "Optimizing using option $OPTIMIZATION_OPT..."
        if [ "$OPTIMIZATION_OPT" -eq 0 ]; then
            if [[ "$USER_PLACED_PASS_MODE" -eq 1 && "$USER_GENERATED_PASSES_TO_RUN_PRELOWER" != "" ]]; then 
                echo "Running User-Specified "
                USER_PASS_ARG=()
                if [ "$USER_GENERATED_PASS_PATH" != "" ]; then
                    USER_PASS_ARG=("-load-pass-plugin" "$USER_GENERATED_PASS_PATH")
                fi
                "$(OPT)" -S -load-pass-plugin "${IQC_PASSES_LIB_DIR}"/libIntelQuantumIRPasses_shared.so "${USER_PASS_ARG[@]}" -p="q-annotations-to-json,spin-native-annotate,$USER_GENERATED_PASSES_TO_RUN_PRELOWER" "${USER_GENERATED_PASS_ARGS_PRELOWER[@]}" "$ir_conditioned" -o "$ir_conditioned_user"
                result="$?"
                if [ ! "$result" -eq 0 ]; then
                    echo "Unexpected user pass error. Exiting..."
                    exit 1
                fi
            else
                cp "$ir_conditioned" "$ir_conditioned_user"
            fi

            "$(OPT)" -S -load-pass-plugin "$(LIBRARY_LOCATION)"/libIntelQuantumIRPasses_shared.so ${QUEUEING:+"$QUEUEING"} -p="q-annotations-to-json,$PLACEMENT_PASS,spin-native-annotate,spin-lower-from-canonical,dce" "${PLACEMENT[@]}" -q-config="$CONFIG_FILE" -q-sdk="$SDK_PATH" "$ir_conditioned_user" -o "$ir_lowered"
            result="$?"
            if [ ! "$result" -eq 0 ]; then
                echo "Unexpected lowering error. Exiting..."
                exit 1
            fi
        else
            if [[ "$USER_PLACED_PASS_MODE" -eq 1 && "$USER_GENERATED_PASSES_TO_RUN_PRESYNTHESIS" != "" ]]; then 
                echo "Running User-Specified "
                USER_PASS_ARG=()
                if [ "$USER_GENERATED_PASS_PATH" != "" ]; then
                    USER_PASS_ARG=("-load-pass-plugin" "$USER_GENERATED_PASS_PATH")
                fi
                "$(OPT)" -S -load-pass-plugin "${IQC_PASSES_LIB_DIR}"/libIntelQuantumIRPasses_shared.so "${USER_PASS_ARG[@]}" -p="q-annotations-to-json,spin-native-annotate,$USER_GENERATED_PASSES_TO_RUN_PRESYNTHESIS" "${USER_GENERATED_PASS_ARGS_PRESYNTHESIS[@]}" "$ir_conditioned" -o "$ir_conditioned_user"
                result="$?"
                if [ ! "$result" -eq 0 ]; then
                    echo "Unexpected user pass error. Exiting..."
                    exit 1
                fi
            else
                cp "$ir_conditioned" "$ir_conditioned_user"
            fi

            "$(OPT)" -S -load-pass-plugin "$(LIBRARY_LOCATION)"/libIntelQuantumIRPasses_shared.so ${QUEUEING:+"$QUEUEING"} -p="q-annotations-to-json,$PLACEMENT_PASS,dce" -warnings-as-errors=true -q-config="$CONFIG_FILE" -q-sdk="$SDK_PATH" "$ir_conditioned_user" -o "$ir_synthesized"
            result="$?"
            if [ ! "$result" -eq 0 ]; then
                echo "Unexpected synthesis error. Exiting..."
                exit 1
            fi

            if [ "$PRINT_CIRCUIT_QBB" -eq 1 ]; then
                echo "Printing quantum circuit diagrams after synthesis..."
                printerPass "$ir_synthesized" "synthesized" false
            fi

            if [ "$VERBOSE_MODE" -eq 1 ]; then
                echo "Printing quantum circuit stats after synthesis..."
                "$(OPT)" -S -load-pass-plugin "$(LIBRARY_LOCATION)"/libIntelQuantumIRPasses_shared.so ${QUEUEING:+"$QUEUEING"} -p="q-annotations-to-json,spin-native-annotate,simple-qbit-placement,q-stats-print" -q-config="$CONFIG_FILE" -q-sdk="$SDK_PATH" "$ir_synthesized" -o "$ir_synthesized"
                echo "$DECORATOR"
            fi

            if [[ "$USER_PLACED_PASS_MODE" -eq 1 && "$USER_GENERATED_PASSES_TO_RUN_PRELOWER" != "" ]]; then 
                echo "Running User-Specified "
                USER_PASS_ARG=()
                if [ "$USER_GENERATED_PASS_PATH" != "" ]; then
                    USER_PASS_ARG=("-load-pass-plugin" "$USER_GENERATED_PASS_PATH")
                fi
                "$(OPT)" -S -load-pass-plugin "${IQC_PASSES_LIB_DIR}"/libIntelQuantumIRPasses_shared.so "${USER_PASS_ARG[@]}" -p="q-annotations-to-json,spin-native-annotate,$USER_GENERATED_PASSES_TO_RUN_PRELOWER" "${USER_GENERATED_PASS_ARGS_PRELOWER[@]}" "$ir_synthesized" -o "$ir_synthesized_user"
                result="$?"
                if [ ! "$result" -eq 0 ]; then
                    echo "Unexpected user pass error. Exiting..."
                    exit 1
                fi
            else
                cp "$ir_synthesized" "$ir_synthesized_user"
            fi

            "$(OPT)" -S -load-pass-plugin "$(LIBRARY_LOCATION)"/libIntelQuantumIRPasses_shared.so ${QUEUEING:+"$QUEUEING"} "${PLACEMENT[@]}" -q-config="$CONFIG_FILE" -q-sdk="$SDK_PATH" -p="q-annotations-to-json,spin-native-annotate,spin-lower-from-canonical,dce" "$ir_synthesized_user" -o "$ir_lowered"
            result="$?"
            if [ ! "$result" -eq 0 ]; then
                echo "Unexpected optimization and lowering error. Exiting..."
                exit 1
            fi

            if [ "$KEEP_FILES" -ne 0 ]; then
                rm "$ir_synthesized"
                rm "$ir_synthesized_user"
            fi
        fi

        if [ "$PRINT_CIRCUIT_QBB" -eq 1 ]; then
            echo "Printing quantum circuit diagrams after decomposition to native gates..."
            printerPass "$ir_lowered" "lowered" true
        fi

        if [ "$VERBOSE_MODE" -eq 1 ]; then
            echo "Printing quantum circuit stats after decomposition to native gates..."
            "$(OPT)" -S -load-pass-plugin "$(LIBRARY_LOCATION)"/libIntelQuantumIRPasses_shared.so ${QUEUEING:+"$QUEUEING"} -p="q-annotations-to-json,spin-native-annotate,simple-qbit-placement,q-stats-print" -q-config="$CONFIG_FILE" -q-sdk="$SDK_PATH" "$ir_lowered" -o "$ir_lowered"
            echo "$DECORATOR"
        fi
    else
        cp "$ir_conditioned" "$ir_conditioned_user"
        cp "$ir_conditioned_user" "$ir_lowered"
    fi

    if [ "$ir_file_stage" -lt 5 ]; then
        if [[ "$USER_PLACED_PASS_MODE" -eq 1 && "$USER_GENERATED_PASSES_TO_RUN_PRESCHEDULE" != "" ]]; then 
            echo "Running User-Specified "
            USER_PASS_ARG=()
            if [ "$USER_GENERATED_PASS_PATH" != "" ]; then
                USER_PASS_ARG=("-load-pass-plugin" "$USER_GENERATED_PASS_PATH")
            fi
            "$(OPT)" -S -load-pass-plugin "${IQC_PASSES_LIB_DIR}"/libIntelQuantumIRPasses_shared.so "${USER_PASS_ARG[@]}" -p="q-annotations-to-json,spin-native-annotate,$USER_GENERATED_PASSES_TO_RUN_PRESCHEDULE" "${USER_GENERATED_PASS_ARGS_PRESCHDULE[@]}" "$ir_lowered" -o "$ir_lowered_user"
            result="$?"
            if [ ! "$result" -eq 0 ]; then
                echo "Unexpected user pass error. Exiting..."
                exit 1
            fi
        else
            cp "$ir_lowered" "$ir_lowered_user"
        fi

        echo "Scheduling quantum gates to qubit connectivity" 
        "$(OPT)" -S -load-pass-plugin "$(LIBRARY_LOCATION)"/libIntelQuantumIRPasses_shared.so ${QUEUEING:+"$QUEUEING"} -p="q-annotations-to-json,spin-native-annotate,$PLACEMENT_PASS,q-scheduler,convert-qbit-to-qid,dce,spin-angles-in-range" -q-config="$CONFIG_FILE" -q-sdk="$SDK_PATH" "${PLACEMENT[@]}" -forward-scheduling="$SCHED_METHOD_FR" -backward-scheduling="$SCHED_METHOD_BK" "$ir_lowered_user" -o "$ir_scheduled"

        if [ "$PRINT_CIRCUIT_QBB" -eq 1 ]; then
            echo "Printing quantum circuit diagrams after scheduling..."
            printerPass "$ir_scheduled" "scheduled" true
        fi

        if [ "$VERBOSE_MODE" -eq 1 ]; then
            echo "Printing quantum circuit stats after scheduling..."
            "$(OPT)" -S -load-pass-plugin "$(LIBRARY_LOCATION)"/libIntelQuantumIRPasses_shared.so ${QUEUEING:+"$QUEUEING"} -p="q-annotations-to-json,spin-native-annotate,simple-qbit-placement,q-stats-print" -q-config="$CONFIG_FILE" -q-sdk="$SDK_PATH" "$ir_scheduled" -o "$ir_scheduled"
            echo "$DECORATOR"
        fi
    else
        cp "$ir_lowered" "$ir_lowered_user"
        cp "$ir_lowered_user" "$ir_scheduled"
    fi

    if [ "$ir_file_stage" -lt 9 ]; then
        if [[ "$USER_PLACED_PASS_MODE" -eq 1 && "$USER_GENERATED_PASSES_TO_RUN_PRESPLIT" != "" ]]; then 
            echo "Running User-Specified "
            USER_PASS_ARG=()
            if [ "$USER_GENERATED_PASS_PATH" != "" ]; then
                USER_PASS_ARG=("-load-pass-plugin" "$USER_GENERATED_PASS_PATH")
            fi
            "$(OPT)" -S -load-pass-plugin "${IQC_PASSES_LIB_DIR}"/libIntelQuantumIRPasses_shared.so "${USER_PASS_ARG[@]}" -p="q-annotations-to-json,spin-native-annotate,$USER_GENERATED_PASSES_TO_RUN_PRESPLIT" "${USER_GENERATED_PASS_ARGS_PRESPLIT[@]}" "$ir_scheduled" -o "$ir_scheduled_user"
            result="$?"
            if [ ! "$result" -eq 0 ]; then
                echo "Unexpected user pass error. Exiting..."
                exit 1
            fi
        else
            cp "$ir_scheduled" "$ir_scheduled_user"
        fi

        echo "Separating quantum IR..."
        "$(OPT)" -S -load-pass-plugin "$(LIBRARY_LOCATION)"/libIntelQuantumIRPasses_shared.so ${QUEUEING:+"$QUEUEING"} -p="q-annotations-to-json,spin-native-annotate,spin-convert-to-imm,separate-and-replace-qbb" -q-config="$CONFIG_FILE" -q-sdk="$SDK_PATH" -forward-scheduling="$SCHED_METHOD_FR" -backward-scheduling="$SCHED_METHOD_BK" "$ir_scheduled_user" -o "$ir_separated"

        result="$?"
        if [ ! "$result" -eq 0 ]; then
            echo "Unexpected error during QBB separation. Exiting..."
            exit 1
        fi

        if [ "$PRINT_CIRCUIT_QBB" -eq 1 ]; then
            echo "Printing quantum circuit diagrams after QBB separation..."
            printerPass "$ir_separated" "separated" true
        fi

        if [ "$VERBOSE_MODE" -eq 1 ]; then
            echo "Printing quantum circuit stats after QBB separation..."
            "$(OPT)" -S -load-pass-plugin "$(LIBRARY_LOCATION)"/libIntelQuantumIRPasses_shared.so ${QUEUEING:+"$QUEUEING"} -p="q-annotations-to-json,spin-native-annotate,simple-qbit-placement,q-stats-print" -q-config="$CONFIG_FILE" -q-sdk="$SDK_PATH" "$ir_separated" -o "$ir_separated"
            echo "$DECORATOR"
        fi
    else
        cp "$ir_scheduled" "$ir_scheduled_user"
        cp "$ir_scheduled_user" "$ir_separated"
    fi

    ## Clean-up intermediate files
    if [ "$KEEP_FILES" -eq 0 ]; then
        rm "$ir_file"
        rm "$ir_flattened"
        rm "$ir_replaced"
        rm "$ir_loop_unrolled"
        rm "$ir_loop_unrolled_user"
        rm "$ir_conditioned"
        rm "$ir_conditioned_user"
        rm "$ir_lowered"
        rm "$ir_lowered_user"
        rm "$ir_scheduled"
        rm "$ir_scheduled_user"
        rm "$ir_separated"
    fi
}

###############################################################################
# set -x
# set -e

start_message
parse_options "$@"
qasmbridge
compile

# set +e
# set +x
###############################################################################
