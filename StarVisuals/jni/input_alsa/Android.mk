LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_STATIC_LIBRARIES := libvisual tinyalsa
LOCAL_C_INCLUDES := ./include
LOCAL_MODULE	:= input_alsa
LOCAL_SRC_FILES := input_alsa.c
LOCAL_CFLAGS    += $(ARCH_CFLAGS)
include $(BUILD_SHARED_LIBRARY)

