LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_C_INCLUDES := ./include
LOCAL_MODULE	:= transform_avs_videodelay
LOCAL_SRC_FILES := transform_avs_videodelay.c
LOCAL_CFLAGS	:= -Wall -O0 -g
LOCAL_STATIC_LIBRARIES := libvisual
#include $(BUILD_SHARED_LIBRARY)

