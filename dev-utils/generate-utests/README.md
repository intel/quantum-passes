# Utilities to write a unit test

- Snippets of LLVM IR can be generated by running the driver script using the `-k` option:
```bash
./iqc-out-of-tree.sh -o dev-utils/generate-utests/ -b iqc_install/bin \
   -c unit_tests/QuantumPlatformTestFiles/intel-quantum-sdk_12q_linear.json \
   -S greedy -K bubble-sort -P console -v -k \
   unit_tests/source-files/CNOTs_beyond_linear_connectivity.cpp 
```
- The configuration needs to be stored in: `<repo>/unit_tests/QuantumPlatformTestFiles`
- Temporarily, a few configurations are in folder: `<repo>/unit_tests/configurations`
- To run a single utest, let's say the test named `TEST(QuantumCompilerLinkageFinalTest, BasicTest)`,
go to `./build/bin` and run:  
```bash
./utests --gtest_filter=QuantumCompilerLinkageFinalTest.BasicTest
```
