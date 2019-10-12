#!/bin/bash
rm -rf build_debug_coverage
mkdir build_debug_coverage
cd build_debug_coverage
cmake -DCOVERAGE=ON -DTRACE=ON -DCMAKE_BUILD_TYPE="Debug" ../.
make all
ctest -j8
lcov --directory . --capture --rc lcov_branch_coverage=1 --output-file jinjac_coverage.info
genhtml --output-directory coverage_results --demangle-cpp --num-spaces 4 --sort --title "jinjac test coverage" --function-coverage --branch-coverage --legend jinjac_coverage.info
cd ..
