add_executable(quantum-module-stage
  quantum-module-stage.cpp
)

target_include_directories(quantum-module-stage
    PRIVATE
        ${PROJECT_SOURCE_DIR}/IntelQuantum/QuantumPasses/include
        ${config-json_SOURCE_DIR}/include
        ${swapnet_SOURCE_DIR}/include
        ${config-json_SOURCE_DIR}/extern/nlohmann-json-lightweight/include/
)

target_link_libraries(quantum-module-stage PRIVATE
  LLVMAnalysis
  LLVMCore
  LLVMDemangle
  LLVMSupport
  LLVMIRReader
  platform-utils
  swapnet-utils
  IntelQuantumIRPasses_static
)