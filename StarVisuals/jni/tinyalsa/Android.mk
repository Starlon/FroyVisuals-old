LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_C_INCLUDES := ./include
LOCAL_MODULE	:= tinyalsa
LOCAL_SRC_FILES := pcm.c mixer.c
LOCAL_CFLAGS    += $(ARCH_CFLAGS)
include $(BUILD_STATIC_LIBRARY)

