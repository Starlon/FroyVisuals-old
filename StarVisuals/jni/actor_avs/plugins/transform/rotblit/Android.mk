LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_C_INCLUDES := ./include
LOCAL_MODULE	:= transform_avs_rotblit
LOCAL_SRC_FILES := transform_avs_rotblit.c
LOCAL_CFLAGS    += $(ARCH_CFLAGS)
LOCAL_STATIC_LIBRARIES := libvisual common
#include $(BUILD_SHARED_LIBRARY)

