LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_C_INCLUDES := ./include
LOCAL_MODULE	:= evaluator
LOCAL_SRC_FILES := evaluator.c
LOCAL_CFLAGS	:= -Wall -O0 -g
include $(BUILD_STATIC_LIBRARY)

