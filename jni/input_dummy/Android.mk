LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_STATIC_LIBRARIES := libvisual
LOCAL_C_INCLUDES := ./include
LOCAL_MODULE	:= input_dummy
LOCAL_SRC_FILES := input_dummy.c
LOCAL_CFLAGS	:= -Wall -O0 -g
include $(BUILD_SHARED_LIBRARY)
