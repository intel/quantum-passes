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
library="./build/lib/libClangQuantumExtensions.so"
runner_script="./iqc-out-of-tree.sh"
llvm_loc="../quantum-intrinsics/llvm"
while getopts "hf:s:p:o:a:e:E:b:B:T:L:i:R:l:" opt; do
    case "$opt" in
        f)
            original_file=$OPTARG
            ;;
        a)
            args_to_add=$OPTARG
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
        l)
            llvm_loc=$OPTARG
            ;;
        o)
            output_file=$OPTARG
            ;;
    esac
done

# Generate intermediate files as far as we can though the compilation process
filename=$(basename -- "$original_file")
extension="${filename##*.}"
filename="${filename%.*}"
output_ll=${filename}.ll


# Add check line and remove macros
echo "// RUN: %clang -S -O1 -emit-llvm %include_dirs %loadintelclangextensions ${args_to_add} %s -o - | FileCheck %s --dump-input fail" | cat - $original_file > temp.cpp
sed "s|%loadintelclangextensions|-Xclang -load -Xclang ${library} -Xclang -add-plugin -Xclang rewrite-qexpr|g" temp.cpp  > test.cpp
sed -i "s|%include_dirs|--include-directory=\"${binary_folder}/../include\"|g" test.cpp
mv test.cpp $output_file

# Run the LLVM update script and readd the macros
${llvm_loc}/utils/update_cc_test_checks.py --include-generated-funcs  --check-globals --llvm-bin ${binary_folder} --clang ${binary_folder}/clang $output_file
sed -i "s|-Xclang -load -Xclang ${library} -Xclang -add-plugin -Xclang rewrite-qexpr|%loadintelclangextensions|g" $output_file
sed -i "s|--include-directory=\"${binary_folder}/../include\"|%include_dirs|g" $output_file

# Remove empty check lines.
sed -i "/^\/\/ CHECK: *$/d" $output_file