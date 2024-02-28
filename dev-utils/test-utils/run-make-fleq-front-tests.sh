original_file=""
compilation_append=""
passes_list=""
output_loc=""
extra_pass_run=""
extra_arguments_run=""
args_to_add=""
binary_folder=""
build_folder="./build"
initial_passes="q-annotations-to-json,spin-native-annotate"
library="./build/lib/libClangQuantumExtensions.so"
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
        R)
            runner_script=$OPTARG
            ;;
        l)
            llvm_loc=$OPTARG
            ;;
    esac
done

# Generate intermediate files as far as we can though the compilation process
filename=$(basename -- "$original_file")
extension="${filename##*.}"
filename="${filename%.*}"
output_cpp=${filename}_front_test.cpp
#output_ll_2=${filename}${compilation_append}_temp

# Add check line and remove macros
echo "// RUN: %clang %include_dirs -O1 -S -emit-llvm %loadintelclangextensions -o - %s | FileCheck %s --dump-input fail" | cat - $original_file > initial.cpp
sed "s|%loadintelclangextensions|-Xclang -load -Xclang ${library} -Xclang -add-plugin -Xclang rewrite-qexpr|g" initial.cpp  > test.cpp
sed -i "s|%include_dirs|--include-directory=\"${binary_folder}/../include\"|g" test.cpp
run_command=$(cat test.cpp | grep RUN)

# # Strip non-relevant metadata from the program
#${binary_folder}/opt -strip-debug --strip-named-metadata test.ll -S > temp_strip.ll
#echo "$run_command" | cat - temp_strip.ll > temp && mv temp temp_strip.ll
cp test.cpp $output_loc/$output_cpp

# Run the LLVM update script and readd the macros
${llvm_loc}/utils/update_cc_test_checks.py --llvm-bin=${binary_folder} $output_loc/$output_cpp
sed -i "s|-Xclang -load -Xclang ${library} -Xclang -add-plugin -Xclang rewrite-qexpr|%loadintelclangextensions|g" $output_loc/$output_cpp
sed -i "s|--include-directory=\"${binary_folder}/../include\"|%include_dirs|g" $output_loc/$output_cpp

# Replace PII
#sed -i '/source_filename/d' $output_loc/$output_ll
#sed -i "s/@\.str\.1.*quintrinsics\.h.*$/@.str.1 = private unnamed_addr constant [37 x i8] c\"include\/clang\/Quantum\/quintrinsics.h\\\00\", section \"llvm.metadata\"/g" $output_loc/$output_ll

# Remove empty check lines.
sed -i "/^\/\/ CHECK: *$/d" $output_loc/$output_cpp

# Cleanup
rm initial.cpp