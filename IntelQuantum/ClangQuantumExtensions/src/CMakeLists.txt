add_library(ClangQuantumExtensions MODULE
  RewriteQExpr.cpp
)

target_compile_definitions(ClangQuantumExtensions PRIVATE BUG_REPORT_URL=\"${BUG_REPORT_URL}\")

target_link_libraries(ClangQuantumExtensions PRIVATE
  LLVMSupport
)

if(LLVM_ENABLE_PLUGINS AND (WIN32 OR CYGWIN))

  target_link_libraries(ClangQuantumExtensions PRIVATE
    clangAST
    clangBasic
    clangFrontend
    clangLex
    clangTooling
  )
endif()
