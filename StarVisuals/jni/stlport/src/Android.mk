LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_C_INCLUDES := ./include
LOCAL_MODULE	:= stlport
LOCAL_SRC_FILES := $(addprefix /, $(notdir $(wildcard $(LOCAL_PATH)/*.c) $(wildcard $(LOCAL_PATH)/*.cpp)))
LOCAL_CFLAGS	:= -Wall -O0 -g
include $(BUILD_SHARED_LIBRARY)

