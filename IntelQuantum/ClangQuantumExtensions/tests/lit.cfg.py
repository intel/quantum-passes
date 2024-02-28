# -*- Python -*-

# Configuration file for the 'lit' test runner.

## Modified from  of 'applications.quantum.compiler-llvm10/llvm/test/lit.cfg.py'

import os

import lit.util
import lit.formats
from lit.llvm import llvm_config

# name: The name of this test suite.
config.name = 'IntelQuantumClangExtensions'

# testFormat: The test format to use to interpret tests.
## Ref: https://stackoverflow.com/questions/51008411/hello-world-llvm-lit-with-cfg-file
config.test_format = lit.formats.ShTest("0") ## lit.formats.ShTest(not llvm_config.use_lit_shell)

# suffixes: A list of file extensions to treat as test files. This is overridden
# by individual lit.local.cfg files in the test subdirectories.
## Dealing with only IR files in this test suite
config.suffixes = ['.cpp']

# excludes: A list of directories to exclude from the testsuite. The 'Inputs'
# subdirectories contain auxiliary inputs for various tests in their parent
# directories.
config.excludes = ['Inputs', 'CMakeLists.txt', 'README.txt', 'LICENSE.txt']

# test_source_root: The root path where tests are located.
config.test_source_root = os.path.dirname(__file__)

# test_exec_root: The root path where tests should be run.
config.test_exec_root = config.test_source_root

# Tweak the PATH to include the tools dir.
llvm_config.with_environment('PATH', config.llvm_tools_dir, append_path=True)

# Propagate some variables from the host environment.
llvm_config.with_system_environment(
    ['HOME', 'INCLUDE', 'LIB', 'TMP', 'TEMP'])

def calculate_arch_features(arch_string):
    features = []
    for arch in arch_string.split():
        features.append(arch.lower() + '-registered-target')
    return features

llvm_config.feature_config(
    [('--targets-built', calculate_arch_features)])

llvm_config.use_default_substitutions()

# Add site-specific substitutions.
config.substitutions.append(('%shlibdir', config.llvm_shlib_dir))
config.substitutions.append(('%shlibext', config.llvm_shlib_ext))
# config.substitutions.append(('%exeext', config.llvm_exe_ext))

tools = ['opt', 'clang', 'FileCheck', 'not']

config.substitutions.append(
    (
        "%clang",
        "{}/clang".format(
            config.llvm_tools_dir
        ),
    )
)

llvm_config.add_tool_substitutions(tools, config.llvm_tools_dir)

config.substitutions.append(
    (
        "%loadintelquantumpasses",
        "-load-pass-plugin={}/libIntelQuantumIRPasses_shared.so".format(
            config.intel_quantum_libraries
        ),
    )
)

config.substitutions.append(
    (
        "%loadintelclangextensions",
        "-Xclang -load -Xclang {}/libClangQuantumExtensions.so -Xclang -add-plugin -Xclang rewrite-qexpr".format(
            config.intel_quantum_libraries
        ),
    )
)

config.substitutions.append(
    (
        "%library_location",
        config.intel_quantum_libraries,
    )
)

config.substitutions.append(
    (
        "%include_dirs", 
        "--include-directory=\"{}/../include\"".format(config.llvm_tools_dir),
    )
)
