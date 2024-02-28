set(LLVM_LINK_COMPONENTS Demangle)

# This builds both a shared library and a static library of the passes.
# right now it needs the code to be compiled so it is positionally indepedent
# but if this proves to be an issue, we can just compile a static library instead.
add_library(IntelQuantumIRPasses_obj OBJECT
  QuantumCompile/ConvertQbitToQID.cpp
  QuantumCompile/DecomposeToffoli.cpp
  QuantumCompile/FlattenQuantumKernel.cpp
  QuantumCompile/InsertQuantumAttributes.cpp
  QuantumCompile/InsertQuantumIntrinsics.cpp
  QuantumCompile/IntelQuantumPlugin.cpp
  QuantumCompile/FinalizeQuantumCompilation.cpp
  QuantumCompile/PrintCircuit.cpp
  QuantumCompile/PrintCircuitQBB.cpp
  QuantumCompile/PrinterASCII.cpp
  QuantumCompile/PrinterLaTex.cpp
  QuantumCompile/PrinterJson.cpp
  QuantumCompile/PrinterInterface.cpp
  QuantumCompile/ProofReader.cpp
  QuantumCompile/QuantumClassicalModuleSplit.cpp
  QuantumCompile/QuantumKernelStatsPrint.cpp
  QuantumCompile/QuantumLoopUnrolling.cpp
  QuantumCompile/Scheduler.cpp
  QuantumCompile/SeparateAndReplaceQBB.cpp
  QuantumCompile/SimpleQbitPlacement.cpp
  QuantumCompile/SpinNativeAnglesInRange.cpp
  QuantumCompile/SpinNativeConvertToIMM.cpp
  QuantumCompile/SpinNativeLowerFromCanonical.cpp
  QuantumCompile/ValidateAndConditionQBB.cpp

  QuantumAnalysis/GetAnnotations.cpp
  QuantumAnalysis/QuantumAnnotationGetters.cpp
  QuantumAnalysis/QuantumAnnotationsToJson.cpp
  QuantumAnalysis/QuantumCompilerLinkage.cpp
  QuantumAnalysis/QuantumFLEQGraph.cpp
  QuantumAnalysis/QuantumGateIdentifiers.cpp
  QuantumAnalysis/QuantumInitPlacement.cpp
  QuantumAnalysis/QuantumIterator.cpp
  QuantumAnalysis/QuantumModule.cpp
  QuantumAnalysis/QuantumRefs.cpp
  QuantumAnalysis/QuantumSpinNativeToJson.cpp

  QuantumUtils/QuantumFLEQUtils.cpp
  QuantumUtils/QuantumMapProgToPhysQubits.cpp
  QuantumUtils/QuantumGeneralUtils.cpp
  QuantumUtils/QuantumPlacementUtils.cpp
)

set_property(TARGET IntelQuantumIRPasses_obj PROPERTY POSITION_INDEPENDENT_CODE 1)
target_compile_definitions(IntelQuantumIRPasses_obj PRIVATE BUG_REPORT_URL=\"${BUG_REPORT_URL}\")

target_include_directories(IntelQuantumIRPasses_obj
    PRIVATE
        ${PROJECT_SOURCE_DIR}/IntelQuantum/QuantumPasses/include
        ${config-json_SOURCE_DIR}/include
        ${swapnet_SOURCE_DIR}/include
        ${config-json_SOURCE_DIR}/extern/nlohmann-json-lightweight/include/
)


add_library(IntelQuantumIRPasses_shared SHARED $<TARGET_OBJECTS:IntelQuantumIRPasses_obj>)
add_library(IntelQuantumIRPasses_static STATIC $<TARGET_OBJECTS:IntelQuantumIRPasses_obj>)

target_link_libraries(IntelQuantumIRPasses_shared PRIVATE
    LLVMDemangle
    platform-utils
    swapnet-utils
)

target_link_libraries(IntelQuantumIRPasses_static PRIVATE
    LLVMAnalysis
    LLVMCore
    LLVMDemangle
    LLVMTransformUtils
    platform-utils
    swapnet-utils
)

