#!/bin/sh

adb pull /sdcard/gmon.out
arm-linux-gnueabi-gprof obj/local/armeabi-v7a/libvisual.so \
    -PprofCount -QprofCount -P__gnu_mcount_nc -Q__gnu_mcount_nc
