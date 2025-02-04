set(LLVM_LINK_COMPONENTS
  AllTargetsAsmParsers
  AllTargetsCodeGens
  AllTargetsDescs
  AllTargetsInfos
  AggressiveInstCombine
  Analysis
  AsmParser
  BitWriter
  CFGuard
  CodeGen
  Core
  Coroutines
  Extensions
  IPO
  IRReader
  IRPrinter
  InstCombine
  Instrumentation
  MC
  ObjCARCOpts
  Remarks
  ScalarOpts
  Support
  Target
  TargetParser
  TransformUtils
  Vectorize
  Passes
  )

add_llvm_executable(quantum-optimization-driver
  quantum-optimization-driver.cpp

  DEPENDS
  SUPPORT_PLUGINS
)


target_include_directories(quantum-optimization-driver
    PRIVATE
        ${PROJECT_SOURCE_DIR}/IntelQuantum/QuantumPasses/include
        ${config-json_SOURCE_DIR}/include
        ${swapnet_SOURCE_DIR}/include
        ${pcoast_SOURCE_DIR}/include
        ${config-json_SOURCE_DIR}/extern/nlohmann-json-lightweight/include/
)

target_link_libraries(quantum-optimization-driver PRIVATE
  platform-utils
  swapnet-utils
  IntelQuantumIRPasses_static
)

export_executable_symbols_for_plugins(quantum-optimization-driver)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fexceptions")
set(CMAKE_LD_FLAGS "${CMAKE_CXX_FLAGS} -fexceptions")
target_compile_options(quantum-optimization-driver PUBLIC -fexceptions)
