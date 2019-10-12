#!/bin/bash
rm -rf build_release
mkdir build_release
cd build_release
cmake -DCOVERAGE=OFF -DTRACE=OFF -DCMAKE_BUILD_TYPE="Release" ../.
cd ..
