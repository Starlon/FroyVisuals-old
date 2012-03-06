LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_C_INCLUDES := ./include
LOCAL_MODULE	:= misc_bpm_scope
LOCAL_SRC_FILES := misc_bpm_scope.c
LOCAL_CFLAGS	:= -Wall -O0 -g
LOCAL_STATIC_LIBRARIES := libvisual
#include $(BUILD_SHARED_LIBRARY)

