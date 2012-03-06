LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_C_INCLUDES := ./include
LOCAL_MODULE	:= actor_rotstar
LOCAL_SRC_FILES := actor_rotstar.c
LOCAL_CFLAGS	:= -Wall -O0 -g
LOCAL_STATIC_LIBRARIES := libvisual common
#include $(BUILD_SHARED_LIBRARY)

