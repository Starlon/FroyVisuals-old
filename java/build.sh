#!/bin/bash
export NDK_MODULE_PATH=`pwd`/../native
ndk-build $@
cp obj/local/armeabi/* libs/armeabi
ant debug install
