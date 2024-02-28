original_file=""
compilation_append=""
passes_list=""
output_loc=""
extra_pass_run=""
extra_arguments_run=""
args_to_add=""
binary_folder=""
build_folder="./build"
runner_script="./iqc-out-of-tree.sh"
initial_passes="q-annotations-to-json,spin-native-annotate"
library="./build/lib/libIntelQuantumIRPasses_shared.so"
error_redirection="2>&1"
while getopts "hf:s:p:o:a:e:E:b:B:T:L:i:rR:" opt; do
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
            output_loc=$OPTARG
            ;;
        a)
            args_to_add=$OPTARG
            ;;
        e)
            extra_arguments_run=$OPTARG
            ;;
        E)
            extra_pass_run=$OPTARG
            ;;
        b)
            binary_folder=$OPTARG
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
        r)
            error_redirection=""
            ;;
        R)
            runner_script=$OPTARG
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
if [ "${extra_pass_run}" != "" ]; then
    ${binary_folder}/opt -S -load-pass-plugin ${library} -p="${extra_pass_run}" ${extra_arguments_run} -o $output_ll_2 $output_ll
else
    mv $output_ll $output_ll_2
fi

# Add the test line for lit
echo "; RUN: opt -S %loadintelquantumpasses -p=${initial_passes},${passes_list} ${args_to_add} -o /dev/null %s ${error_redirection} | FileCheck %s --dump-input fail" | cat - $output_ll_2 > test.ll
run_command=$(cat test.ll | grep RUN)

# Strip non-relevant metadata from the program
${binary_folder}/opt -strip-debug --strip-named-metadata test.ll -S > temp_strip.ll
parent_file_name=$(dirname $output_loc/$output_ll)
base_file_name=$(basename $output_loc/$output_ll)
echo "$run_command" | cat - temp_strip.ll > temp && mv temp temp_strip.ll
mv temp_strip.ll $output_ll
mv $output_ll $output_loc

# Replace macros in the test command
base_command=$(echo ${run_command} | cut -d "|" -f 1 | cut -d ":" -f 2 | sed "s|2>&1| |g" | sed "s|%loadintelquantumpasses|-load-pass-plugin ${library}|g" | sed "s|%S|${parent_file_name}|g" | sed "s|%s|${parent_file_name}/${base_file_name}|g" | sed "s|opt|${binary_folder}/opt|g")

# Run the command
if  [ "$error_redirection" == "" ]; then
    eval $(echo $base_command) > recovered_output.txt
else
    eval $(echo $base_command) 2> recovered_output.txt > /dev/null
fi
while IFS= read -r line

# Add the output lines to the test file
do
  echo "; CHECK: $line" >> $output_loc/$output_ll
done < recovered_output.txt

# Replace PII
sed -i '/source_filename/d' $output_loc/$output_ll
sed -E -i "s/@\.str\.([0-9]+).*quintrinsics\.h.*$/@.str.\1 = private unnamed_addr constant [37 x i8] c\"include\/clang\/Quantum\/quintrinsics.h\\\00\", section \"llvm.metadata\"/g" $output_loc/$output_ll
sed -E -i "s/@\.str\.([0-9]+).*expr\.h.*$/@.str.\1 = private unnamed_addr constant \[30 x i8\] c\"include\/clang\/Quantum\/qexpr.h\\\00\", section \"llvm.metadata\"/g" $output_loc/$output_ll
sed -E -i "s/@\.str\.([0-9]+).*qlist\.h.*$/@.str.\1 = private unnamed_addr constant \[30 x i8\] c\"include\/clang\/Quantum\/qlist.h\\\00\", section \"llvm.metadata\"/g" $output_loc/$output_ll

# Remove empty check lines.
sed -i "/^; CHECK: *$/d" $output_loc/$output_ll

# Cleanup
rm ${filename}*.ll ${filename}*.ll_temp ${filename}*.ll_replaced recovered_output.txt ${filename}*.qs ${filename}*.qo