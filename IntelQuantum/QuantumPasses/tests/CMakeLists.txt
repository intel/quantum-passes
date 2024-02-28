set(TEST_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}")

configure_lit_site_cfg(
  ${TEST_SRC_DIR}/lit.site.cfg.py.in
  ${TEST_SRC_DIR}/lit.site.cfg.py
  MAIN_CONFIG
  ${TEST_SRC_DIR}/lit.cfg.py
  "CMAKE_LIBRARY_OUTPUT_DIRECTORY"
  "${CMAKE_SOURCE_DIR}/IntelQuantum/QuantumPasses/tests/InputFiles"
  )
