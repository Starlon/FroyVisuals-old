LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_STATIC_LIBRARIES := libvisual
LOCAL_C_INCLUDES := ./include
LOCAL_MODULE	:= input_opensl
LOCAL_SRC_FILES := input_opensl.c
LOCAL_CFLAGS    += $(ARCH_CFLAGS)
LOCAL_LDLIBS    += -landroid -lOpenSLES
include $(BUILD_SHARED_LIBRARY)

