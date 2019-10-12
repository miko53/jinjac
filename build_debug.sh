#!/bin/bash
rm -rf build
mkdir build
cd build
cmake -DCOVERAGE=OFF -DTRACE=ON -DCMAKE_BUILD_TYPE="Debug" ../.
cd ..

