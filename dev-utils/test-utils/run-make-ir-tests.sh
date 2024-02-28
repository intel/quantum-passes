original_file=""
compilation_append=""
passes_list=""
output_file=""
extra_pass_run=""
extra_arguments_run=""
args_to_add=""
binary_folder=""
build_folder="./build"
initial_passes="q-annotations-to-json,spin-native-annotate"
library="./build/lib/libIntelQuantumIRPasses_shared.so"
runner_script="./iqc-out-of-tree.sh"
llvm_loc="../quantum-intrinsics/llvm"
while getopts "hf:s:p:o:a:e:E:b:B:T:L:i:R:l:" opt; do
    case "$opt" in
        f)
            original_file=$OPTARG
            ;;
        s)
            compilation_append=$OPTARG
            ;;
        p)
            passes_list=$OPTARG
            ;;
        o)
            output_file=$OPTARG
            ;;
        a)
            args_to_add+=" "$OPTARG
            ;;
        e)
            extra_arguments_run=$OPTARG
            ;;
        E)
            extra_pass_run=$OPTARG
            ;;
        B)
            build_folder=$OPTARG
            ;;
        L)
            library=$OPTARG
            ;;
        i)
            initial_passes=$OPTARG
            ;;
        R)
            runner_script=$OPTARG
            ;;
        l)
            llvm_loc=$OPTARG
            ;;
    esac
done

# Generate intermediate files as far as we can though the compilation process
${runner_script} -k -b ${binary_folder} $original_file
filename=$(basename -- "$original_file")
extension="${filename##*.}"
filename="${filename%.*}"
output_ll=${filename}$compilation_append
output_ll_2=${filename}${compilation_append}_temp

# If we need a custom passes flow, run that here
if [ "${extra_passes_run}" != "" ]; then
    ${binary_folder}/opt -S -load-pass-plugin ${library} -p="${extra_passes_run}" ${extra_arguments_run} -o $output_ll_2 $output_ll
else
    mv $output_ll $output_ll_2
fi

# Add check line and remove macros
echo "; RUN: opt -S %loadintelquantumpasses -p=\"${initial_passes},${passes_list}\" ${args_to_add} -o - %s | FileCheck %s --dump-input fail" | cat - $output_ll_2 > initial.ll
sed "s|%loadintelquantumpasses|-load-pass-plugin ${library}|g" initial.ll  > test.ll
run_command=$(cat test.ll | grep RUN)

# # Strip non-relevant metadata from the program
${binary_folder}/opt -strip-debug --strip-named-metadata test.ll -S > temp_strip.ll
echo "$run_command" | cat - temp_strip.ll > temp && mv temp temp_strip.ll
mv temp_strip.ll $output_ll
mv $output_ll $output_file

# Run the LLVM update script and readd the macros
${llvm_loc}/utils/update_test_checks.py --tool-binary ${binary_folder}/opt $output_file
sed -i "s|-load-pass-plugin ${library}|%loadintelquantumpasses|g" $output_file

# Replace PII
sed -i '/source_filename/d' $output_file
sed -E -i "s/@\.str\.([0-9]+).*quintrinsics\.h.*$/@.str.\1 = private unnamed_addr constant [37 x i8] c\"include\/clang\/Quantum\/quintrinsics.h\\\00\", section \"llvm.metadata\"/g" $output_file
sed -E -i "s/@\.str\.([0-9]+).*qexpr\.h.*$/@.str.\1 = private unnamed_addr constant [30 x i8] c\"include\/clang\/Quantum\/qexpr.h\\\00\", section \"llvm.metadata\"/g" $output_file
sed -E -i "s/@\.str\.([0-9]+).*qlist\.h.*$/@.str.\1 = private unnamed_addr constant [30 x i8] c\"include\/clang\/Quantum\/qlist.h\\\00\", section \"llvm.metadata\"/g" $output_file

# Remove empty check lines.
sed -i "/^; CHECK: *$/d" $output_file

# Cleanup
rm ${filename}*.ll ${filename}*.ll_temp ${filename}*.ll_replaced ${filename}*.qs ${filename}*.qo
parent_file_name=$(dirname $output_file)
rm $parent_file_name/*_quantum.ll