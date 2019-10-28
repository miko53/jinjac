#!/bin/bash
cppcheck --enable=all -I libjinjac/include -j8 --language=c --platform=unix64 --std=c99 libjinjac/src
