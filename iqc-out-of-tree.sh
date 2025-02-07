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

TOOL_NAME="Intel(R) Quantum Compiler: 2024 (Developers' utility)"
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

KEEP_FILES=0

QUEUEING=""
PLACEMENT_PASS="simple-qbit-placement" ## equivalent to 'none', but w/o need of config file
PLACEMENT=()
SCHED_METHOD_FR="none"
SCHED_METHOD_BK="retrace"
SERIALIZE=""
USE_SYNTH=""
USE_PRINT=""
USE_VERBOSE=""
START_STAGE="begin"
END_STAGE="end"
DEBUG_PASSES=""
PRINT_FLAGS=()
PRINT_DIR_FLAGS=()
USER_GENERATED_PASSES_TO_RUN_PRECONDITION=""
USER_GENERATED_PASS_ARGS_PRECONDITION=()
USER_GENERATED_PASSES_TO_RUN_PRESYNTHESIS=""
USER_GENERATED_PASS_ARGS_PRESYNTHESIS=()
USER_GENERATED_PASSES_TO_RUN_PRESCHEDULE=""
USER_GENERATED_PASS_ARGS_PRESCHEDULE=()
USER_GENERATED_PASSES_TO_RUN_PRELOWER=""
USER_GENERATED_PASS_ARGS_PRELOWER=()
USER_GENERATED_PASSES_TO_RUN_PRESPLIT=""
USER_GENERATED_PASS_ARGS_PRESPLIT=()
GEN_OPT_ADDED_FLAGS=()

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
    echo "                  Must provide path to QRT files. Disabled by default."
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
    echo "    -g <FLAG> :   Pass additional flag to IQC; see README for a description of the options."
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
    echo "   -z <stage> :   Starting stage for compilation, valid names are: start, intrinsics, flatten, unroll,"
    echo "                  validate, synthesize, lower, schedule, separate, split, end"
    echo "   -Z <stage> :   Ending stage for compilation, valid names are: start, intrinsics, flatten, unroll,"
    echo "                  validate, synthesize, lower, schedule, separate, split, end"
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
    while getopts ":hb:t:Bo:I:L:R:d:T:M:l:vqsc:p:O:P:jJ:S:NK:mkf:g:F:a:A:e:E:F:X:x:D:z:Z:" opt; do
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
                USE_VERBOSE="--verbose"
                ;;
            J)
                if [[ "$OPTARG" == "basic" ]]; then
                    EO_DECOMP_FLAG="false"
                elif [[ "$OPTARG" == "library" ]]; then
                    EO_DECOMP_FLAG="true"
                fi
                ;;
            j)
                IMPLICIT_JZ_JN="true"
                ;;
            O)
                if [ "$OPTARG" -ge 2 ]; then
                    echo "No optimization option $OPTARG available..."
                    exit
                elif [ "$OPTARG" -ge 1 ]; then
                    USE_SYNTH="--use-synth"
                fi
                ;;
            P)
                prints+=("$OPTARG")
                USE_PRINT="--circuit-print"
                PRINTER_OUTPUT_DIR="$OUTPUT_DIR"/
                PRINT_DIR_FLAGS=("-odir" "$PRINTER_OUTPUT_DIR")
                PRINT_FLAGS+=("--print-format" "$OPTARG")
                ;;
            m)
                QUEUEING="-queue-measurements"
                ;;
            p)
                PLACEMENT=("--placement-method" "$OPTARG")
                ;;
            S)
                if [[ "$OPTARG" == "greedy" ]] || [[ "$OPTARG" == "none" ]] || [[ "$OPTARG" == "serial" ]] || [[ "$OPTARG" == "none,serial" ]] || [[ "$OPTARG" == "greedy,serial" ]]; then
                    SCHED_METHOD_FR="$OPTARG"
                    if [[ "$OPTARG" == *"serial"* ]]; then
                        SERIALIZE="--serialize-kernels"
                    fi
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
                USER_GENERATED_PASS_PATH="$OPTARG"
                ;;
            e)
                CURRENT_PASS_SECTION="$OPTARG"
                ;;
            a)
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
            z)
                START_STAGE=$OPTARG
                ;;
            Z)
                END_STAGE=$OPTARG
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
compile() {
    filename=$(basename -- "$INPUT_FILE")
    extension="${filename##*.}"
    filename="${filename%.*}"

    ## IR files
    compilation_file=""
    ir_file="$OUTPUT_DIR"/"$filename".ll
    ir_flattened="$OUTPUT_DIR"/"$filename"_flatten.ll
    ir_loop_unrolled="$OUTPUT_DIR"/"$filename"_unroll.ll
    ir_replaced="$OUTPUT_DIR"/"$filename"_intrinsics.ll
    ir_conditioned="$OUTPUT_DIR"/"$filename"_validate.ll
    ir_synthesized="$OUTPUT_DIR"/"$filename"_synthesize.ll
    ir_lowered="$OUTPUT_DIR"/"$filename"_lower.ll
    ir_scheduled="$OUTPUT_DIR"/"$filename"_scheduled.ll
    ir_separated="$OUTPUT_DIR"/"$filename"_separate.ll
    ir_quantum_final="$OUTPUT_DIR"/"$filename"_quantum.ll
    ir_classical_final="$OUTPUT_DIR"/"$filename"_classical_final.ll

    if [ "${END_STAGE}" == "intrinsics" ]; then
        compilation_file="$ir_replaced"
    elif [ "${END_STAGE}" == "flatten" ]; then
        compilation_file="$ir_flattened"
    elif [ "${END_STAGE}" == "unroll" ]; then
        compilation_file="$ir_loop_unrolled"
    elif [ "${END_STAGE}" == "validate" ]; then
        compilation_file="$ir_conditioned"
    elif [ "${END_STAGE}" == "synthesize" ]; then
        compilation_file="$ir_synthesized"
    elif [ "${END_STAGE}" == "lower" ]; then
        compilation_file="$ir_lowered"
    elif [ "${END_STAGE}" == "schedule" ]; then
        compilation_file="$ir_scheduled"
    elif [ "${END_STAGE}" == "separate" ]; then
        compilation_file="$ir_separated"
    else
        compilation_file="$ir_classical_final"
    fi

    ## Assembly file, Object file, and executable
    qs_64_file="$OUTPUT_DIR"/"$filename".qs
    qo_64_file="$OUTPUT_DIR"/"$filename".qo
    hybrid_exe_file="$OUTPUT_DIR"/"$filename"

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
        if [[ "$ir_file_stage" == 1 ]]; then
            START_STAGE="synthesize"
        elif [[ "$ir_file_stage" == 2 ]]; then
            START_STAGE="synthesize"
        elif [[ "$ir_file_stage" == 3 ]]; then
            START_STAGE="lower"
        elif [[ "$ir_file_stage" == 4 ]]; then
            START_STAGE="schedule"
        elif [[ "$ir_file_stage" == 5 ]]; then
            START_STAGE="schedule"
        elif [[ "$ir_file_stage" == 6 ]]; then
            START_STAGE="schedule"
        elif [[ "$ir_file_stage" == 7 ]]; then
            START_STAGE="schedule"
        elif [[ "$ir_file_stage" == 8 ]]; then
            START_STAGE="schedule"
        elif [[ "$ir_file_stage" == 9 ]]; then
            START_STAGE="separate"
        elif [[ "$ir_file_stage" == 10 ]]; then
            START_STAGE="split"
        elif [[ "$ir_file_stage" == 11 ]]; then
            echo "Error: Cannot accept a presplit module."
            exit 1
        fi
    fi

    echo "Intermediate representation (IR) output file: "
    realpath "$ir_file"
    result=0

    if [[ "$USER_GENERATED_PASS_PATH" != "" ]]; then
        "$(TOOLS_LOCATION)"/quantum-optimization-driver -load-pass-plugin "$USER_GENERATED_PASS_PATH" \
        -S "$ir_file" -o "$compilation_file" --start-stage "$START_STAGE"  --end-stage "$END_STAGE" \
        -q-config="$CONFIG_FILE" -q-sdk="$SDK_PATH"  ${SERIALIZE:+"$SERIALIZE"} "${PLACEMENT[@]}" ${QUEUEING:+"$QUEUEING"} \
        -forward-scheduling="$SCHED_METHOD_FR"  "${GEN_OPT_ADDED_FLAGS[@]}"\
        -backward-scheduling="$SCHED_METHOD_BK" ${USE_SYNTH:+"$USE_SYNTH"} ${USE_VERBOSE:+"$USE_VERBOSE"} ${USE_PRINT:+"$USE_PRINT"} "${PRINT_DIR_FLAGS[@]}" "${PRINT_FLAGS[@]}" \
        -custom-passes="precond:$USER_GENERATED_PASSES_TO_RUN_PRECONDITION" "${USER_GENERATED_PASS_ARGS_PRECONDITION[@]}" \
        -custom-passes="prelower:$USER_GENERATED_PASSES_TO_RUN_PRELOWER" "${USER_GENERATED_PASS_ARGS_PRELOWER[@]}" \
        -custom-passes="presynth:$USER_GENERATED_PASSES_TO_RUN_PRESYNTHESIS" "${USER_GENERATED_PASS_ARGS_PRESYNTHESIS[@]}" \
        -custom-passes="preschedule:$USER_GENERATED_PASSES_TO_RUN_PRESCHEDULE" "${USER_GENERATED_PASS_ARGS_PRESCHEDULE[@]}" \
        -custom-passes="presplit:$USER_GENERATED_PASSES_TO_RUN_PRESPLIT" "${USER_GENERATED_PASS_ARGS_PRESPLIT[@]}"
        result="$?"
    else
        "$(TOOLS_LOCATION)"/quantum-optimization-driver -S "$ir_file" -o "$compilation_file" \
        -q-config="$CONFIG_FILE" -q-sdk="$SDK_PATH" ${SERIALIZE:+"$SERIALIZE"} \
        ${QUEUEING:+"$QUEUEING"} --start-stage "$START_STAGE" --end-stage "$END_STAGE" "${GEN_OPT_ADDED_FLAGS[@]}" \
        ${USE_VERBOSE:+"$USE_VERBOSE"} ${USE_PRINT:+"$USE_PRINT"} "${PRINT_FLAGS[@]}" -forward-scheduling="$SCHED_METHOD_FR" \
        "${PRINT_DIR_FLAGS[@]}" "${PLACEMENT[@]}" ${USE_SYNTH:+"$USE_SYNTH"} -backward-scheduling="$SCHED_METHOD_BK"
        result="$?"
    fi

    if [ ! "$result" -eq 0 ]; then
        echo "Unexpected compilation error. Exiting..."
        exit 1
    fi

    ## Clean-up intermediate files
    if [ "$KEEP_FILES" -eq 0 ]; then
        rm "$ir_file"
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
