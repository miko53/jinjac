#!/bin/bash
# launch memcheck with valgrind on vm execution
cd build
ctest -T memcheck -R _vm

