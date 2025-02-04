add_executable(quantum-circuit-object
  quantum-circuit-object.cpp
)

target_include_directories(quantum-circuit-object
    PRIVATE
        ${PROJECT_SOURCE_DIR}/IntelQuantum/QuantumPasses/include
        ${config-json_SOURCE_DIR}/include
        ${swapnet_SOURCE_DIR}/include
        ${pcoast_SOURCE_DIR}/include
        ${config-json_SOURCE_DIR}/extern/nlohmann-json-lightweight/include/
)

target_link_libraries(quantum-circuit-object PRIVATE
  LLVMAnalysis
  LLVMCore
  LLVMDemangle
  LLVMSupport
  LLVMIRReader
  platform-utils
  swapnet-utils
  IntelQuantumIRPasses_static
)