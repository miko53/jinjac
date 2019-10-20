#!/bin/bash
cppcheck --enable=all -I include -j8 --language=c --platform=unix64 --std=c99 libjinjac/src
