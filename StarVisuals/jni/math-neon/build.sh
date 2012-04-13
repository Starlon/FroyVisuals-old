#!/bin/sh

export TOOL_PREFIX=~/CodeSourcery/Sourcery_CodeBench_Lite_for_ARM_EABI/bin/arm-none-linux-gnueabi
export CXX=$TOOL_PREFIX-g++
export AR=$TOOL_PREFIX-ar
export RANLIB=$TOOL_PREFIX-ranlib
export CC=$TOOL_PREFIX-gcc
export LD=$TOOL_PREFIX-ld

export CCFLAGS="-march=armv7-a -mcpu=cortex-a8 mfloat-abi=softfp -mfpu=neon -ftree-vectorize"
export ARM_TARGET_LIB=/CodeSourcery/Sourcery_CodeBench_Lite_for_ARM_EABI/arm-none-eabi/lib

scons wordsize=32 snapshot=off arch=arm sample=shell
