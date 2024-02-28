add_library(ExampleSinglePass SHARED
  ExampleSinglePass.cpp
)

target_include_directories(ExampleSinglePass PRIVATE
    ${PROJECT_SOURCE_DIR}/IntelQuantum/QuantumPasses/include
    ${config-json_SOURCE_DIR}/include
    ${config-json_SOURCE_DIR}/extern/nlohmann-json-lightweight/include/
)

target_link_libraries(ExampleSinglePass PRIVATE
    platform-utils
)

