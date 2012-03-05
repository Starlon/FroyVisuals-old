#!/bin/sh
cd lyrical/data/data/com.starlon.froyvisuals/lib/
rm ./*.so
rm gdbserver
adb pull /data/data/com.starlon.froyvisuals/lib/
