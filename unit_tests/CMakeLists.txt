# Copyright 2023 Intel Corporation.

# Build the suite of unit-tests for the project.
# Approach suggested in:
# https://github.com/google/googletest/blob/master/googletest/README.md
# The googletest project as being included in the main CMakeLists.txt

# Link against gtest or gtest_main as needed.
add_executable(utests
    suite_of_tests.cpp
    QuantumAnnotationsToJsonTest.cpp
    QuantumBBIteratorTest.cpp
    QuantumCbitRefTest.cpp
    QuantumCompilerLinkageFinalTest.cpp
    QuantumCompilerLinkageTest.cpp
    QuantumGetPlatformTest.cpp
    QuantumInitPlacementTest.cpp
    QuantumIteratorTest.cpp
    QuantumIteratorUnresolvedParameters.cpp
    QuantumLocalSearchTest.cpp
    QuantumParaRefSimplifyTest.cpp
    QuantumParaRefTest.cpp
    QuantumQbitRefTest.cpp
    QuantumSchedulerTest.cpp
    QuantumSpinNativeToJsonTest.cpp
)
target_include_directories(utests PRIVATE
    include
    ${PROJECT_SOURCE_DIR}/_deps/googletest-src/googletest/include
    ${PROJECT_SOURCE_DIR}/IntelQuantum/QuantumPasses/include
    ${config-json_SOURCE_DIR}/include
    ${config-json_SOURCE_DIR}/extern/nlohmann-json-lightweight/include
    ${swapnet_SOURCE_DIR}/include)

target_link_libraries(utests PRIVATE
    IntelQuantumIRPasses_static
    gtest
    gtest_main
    LLVMAnalysis
    LLVMCore
    LLVMIRPrinter
    LLVMIRReader
    LLVMSupport
    LLVMDemangle
    LLVMPasses)

################################################################################

#Add configuration file paths
target_compile_definitions(utests PRIVATE CONFIGURATION_DIR="${PROJECT_SOURCE_DIR}/unit_tests/configurations")
target_compile_definitions(utests PRIVATE
    EXAMPLE_OF_DEFINITION="${PROJECT_SOURCE_DIR}/unit_tests/extra_files/XXX.zzz")

################################################################################
