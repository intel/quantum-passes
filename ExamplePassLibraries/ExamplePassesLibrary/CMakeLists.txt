add_library(ExampleMultiPass SHARED
  src/GatePrinterPass.cpp
  src/XToHZHPass.cpp
  src/ToffoliWithCircuitPass.cpp
  src/PluginImplementation.cpp
)

target_include_directories(ExampleMultiPass PRIVATE
    ${PROJECT_SOURCE_DIR}/ExamplePassLibraries/ExamplePassesLibrary/include
    ${PROJECT_SOURCE_DIR}/IntelQuantum/QuantumPasses/include
    ${config-json_SOURCE_DIR}/include
    ${config-json_SOURCE_DIR}/extern/nlohmann-json-lightweight/include/
)

target_link_libraries(ExampleMultiPass PRIVATE
    platform-utils
)

