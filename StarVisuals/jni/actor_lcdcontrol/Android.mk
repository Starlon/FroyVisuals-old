LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_C_INCLUDES := ./include
LOCAL_MODULE	:= LCDControl
LOCAL_SRC_FILES := $(addprefix /, $(notdir $(wildcard $(LOCAL_PATH)/*.c) $(wildcard $(LOCAL_PATH)/*.cpp)))
LOCAL_CFLAGS	:= -Wall -O0 -g $(LOCAL_PATH)/jsoncpp/libs/linux-gcc-4.6.1/libjson_linux-gcc-4.6.1_libmt.a
LOCAL_SHARED_LIBRARIES := libvisual visscript-lua
#include $(BUILD_SHARED_LIBRARY)

