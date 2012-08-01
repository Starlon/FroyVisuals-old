#!/bin/bash
cc -o test test.c \
    -I../../../../LuaJIT/include \
    -L./x86 -lluajit -lm -ldl
