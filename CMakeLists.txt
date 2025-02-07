# NOTE: Refer to https://llvm.org/docs/CMake.html,
# section 'Embedding LLVM in your project'

cmake_minimum_required(VERSION 3.13.4)
project(IntelQuantumDevTools)

# Options for the build.
option(IqdtUtests "Enable unit test for Intel Quantum Dev Tools?" ON)

# See if the location of the SDK has been defined, we'll find the location
# of the libraries and the intrinsics from here if it is.
if(DEFINED LT_INTEL_QUANTUM_SDK_LOC)
  set(SDK_DEFINED TRUE)
  set(QUANTUM_INTRINSICS "${LT_INTEL_QUANTUM_SDK_LOC}/iqc")
  set(EXTERNAL_LIBS "${LT_INTEL_QUANTUM_SDK_LOC}/iqc")
endif()

# See if a custom set of LLVM intrinsics are specified, we will use that instead
# if it is.
if(DEFINED LT_LLVM_INSTALL_DIR)
  set(QUANTUM_INTRINSICS ${LT_LLVM_INSTALL_DIR})
endif()

# We want to usually fetch external respositories from the SDK,
# for internal development, we can specify otherwise on the command line.
if(NOT DEFINED FETCH_EXTERNAL)
  set(FETCH_EXTERNAL FALSE)
endif()

# 1.1 Check the "include" directory
set(LT_LLVM_INCLUDE_DIR "${QUANTUM_INTRINSICS}/include/llvm")
if(NOT EXISTS "${LT_LLVM_INCLUDE_DIR}")
message(FATAL_ERROR
  " LT_LLVM_INSTALL_DIR ALL_DIR (${LT_LLVM_INCLUDE_DIR}) is invalid.")
endif()

# 1.2 Check that the LLVMConfig.cmake file exists (the location depends on the
# OS)
set(LT_VALID_INSTALLATION FALSE)

# Ubuntu + Darwin
if(EXISTS "${QUANTUM_INTRINSICS}/lib/cmake/llvm/LLVMConfig.cmake")
  set(LT_VALID_INSTALLATION TRUE)
endif()

# Fedora
if(EXISTS "${QUANTUM_INTRINSICS}/lib64/cmake/llvm/LLVMConfig.cmake")
  set(LT_VALID_INSTALLATION TRUE)
endif()

if(NOT ${LT_VALID_INSTALLATION})
  message(FATAL_ERROR
    "LLVM installation directory, (${QUANTUM_INTRINSICS}), is invalid. Couldn't
    find LLVMConfig.cmake.")
endif()

list(APPEND CMAKE_PREFIX_PATH "${QUANTUM_INTRINSICS}/lib/cmake/llvm/")
list(APPEND CMAKE_PREFIX_PATH "${QUANTUM_INTRINSICS}/lib/cmake/clang/")

set(BUG_REPORT_URL "https://github.com/intel/quantum-passes" CACHE STRING
  "Default URL where bug reports are to be submitted.")

find_package(LLVM 17 QUIET CONFIG)
find_package(LLVM 18 QUIET CONFIG)
find_package(Clang REQUIRED CONFIG)

# Another sanity check
if(NOT "17" VERSION_LESS_EQUAL "${LLVM_VERSION_MAJOR}" AND NOT "18" VERSION_LESS_EQUAL "${LLVM_VERSION_MAJOR}")
  message(FATAL_ERROR "Found LLVM ${LLVM_VERSION_MAJOR}, but need LLVM 17 or 18")
endif()

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LT_LLVM_INSTALL_DIR}")

message("LLVM STATUS:
  Definitions ${LLVM_DEFINITIONS}
  Includes    ${LLVM_INCLUDE_DIRS}
  Libraries   ${LLVM_LIBRARY_DIRS}
  Targets     ${LLVM_TARGETS_TO_BUILD}"
)

message("CLANG STATUS:
  Includes (clang)    ${CLANG_INCLUDE_DIRS}
  Includes (llvm)     ${LLVM_INCLUDE_DIRS}"
)


# Set the LLVM header and library paths
include_directories(SYSTEM "${LLVM_INCLUDE_DIRS};${CLANG_INCLUDE_DIRS}")
link_directories(${LLVM_LIBRARY_DIRS})
add_definitions(${LLVM_DEFINITIONS})

list(APPEND CMAKE_MODULE_PATH "${LLVM_CMAKE_DIR}")
include(AddLLVM)

separate_arguments(LLVM_DEFINITIONS_LIST NATIVE_COMMAND ${LLVM_DEFINITIONS})
add_definitions(${LLVM_DEFINITIONS_LIST})
include_directories(${LLVM_INCLUDE_DIRS})

# Find the libraries that correspond to the LLVM components
# that we wish to use
llvm_map_components_to_libnames(llvm_libs support core irreader demangle)

# # Link against LLVM libraries
# target_link_libraries(simple-tool ${llvm_libs})

set(CMAKE_CXX_STANDARD 20 CACHE STRING "C++ standard to conform to")
set(CMAKE_CXX_STANDARD_REQUIRED YES)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "No build type selected, default to Debug")
  set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Build type (default Debug)" FORCE)
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -fno-rtti")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/lib")

include(FetchContent)
if(SDK_DEFINED AND (NOT ${FETCH_EXTERNAL}))
  set(platform-utils_SOURCE_DIR "${LT_INTEL_QUANTUM_SDK_LOC}/iqc/include/platform-utils")
  set(config-json_SOURCE_DIR "${LT_INTEL_QUANTUM_SDK_LOC}/iqc/include/platform-utils")
  set(swapnet_SOURCE_DIR "${LT_INTEL_QUANTUM_SDK_LOC}/iqc/include/swapnet")
  link_directories("${EXTERNAL_LIBS}/passes_lib")
  include_directories("${LT_INTEL_QUANTUM_SDK_LOC}/iqc/include/platform-utils/include")
  include_directories("${LT_INTEL_QUANTUM_SDK_LOC}/iqc/include/platform-utils/include/extern/nlohmann-json-lightweight/include")  
endif()

add_subdirectory(IntelQuantum)
add_subdirectory(ExamplePassLibraries)

# Add the unit test contained in /unit_tests
# If required, googletest framework is downloaded and configured.
if (IqdtUtests)
    include("cmake/gtest.cmake")
    message(STATUS "Unit tests for Intel Quantum Dev Tools : enabled.")
    add_subdirectory(unit_tests)
else()
    message(STATUS "Unit tests for Intel Quantum Dev Tools : disabled.")
endif()

