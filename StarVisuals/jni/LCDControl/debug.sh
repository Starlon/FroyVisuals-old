#!/bin/sh
#LD_LIBRARY_PATH=/opt/qtsdk-2010.05/qt/lib/ valgrind ./LCDControl
LD_LIBRARY_PATH=/usr/include/qt/lib/ gdb ./LCDControl
#LD_LIBRARY_PATH=/opt/qtsdk-2010.05/qt/lib/ memprof ./LCDControl
