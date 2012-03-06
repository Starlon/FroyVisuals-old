LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_C_INCLUDES := ./include
LOCAL_MODULE	:= transform_avs_fastbright
LOCAL_SRC_FILES := transform_avs_fastbright.c
LOCAL_CFLAGS	:= -Wall -O0 -g
LOCAL_STATIC_LIBRARIES := libvisual common
#include $(BUILD_SHARED_LIBRARY)

