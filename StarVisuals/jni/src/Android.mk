LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := main

LOCAL_C_INCLUDES := ./include
LOCAL_CFLAGS    += $(ARCH_CFLAGS) $(USLEEP)
LOCAL_SRC_FILES := starvisuals.c

LOCAL_SHARED_LIBRARIES := visual
LOCAL_STATIC_LIBRARIES := tinyalsa

LOCAL_LDLIBS := -lm -llog -ljnigraphics

#include $(BUILD_SHARED_LIBRARY)

