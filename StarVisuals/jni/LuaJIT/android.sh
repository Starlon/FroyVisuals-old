#!/bin/sh
NDK=/opt/android-ndk-r7b
NDKABI=8
NDKVER=$NDK/toolchains/arm-linux-androideabi-4.4.3
NDKP=$NDKVER/prebuilt/linux-x86/bin/arm-linux-androideabi-
NDKF="--sysroot $NDK/platforms/android-$NDKABI/arch-arm"
make HOST_CC="gcc -m32" CROSS=$NDKP TARGET_FLAGS="$NDKF" TARGET=arm

