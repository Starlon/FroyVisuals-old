LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_C_INCLUDES := \
    $(LOCAL_PATH)/../../../common \
    $(LOCAL_PATH)/../../../include \
    $(LOCAL_PATH)/../../../src\
    $(LOCAL_PATH)/../../../evaluator
LOCAL_MODULE	:= transform_avs_onetone
LOCAL_SRC_FILES := transform_avs_onetone.c
LOCAL_CFLAGS    += $(ARCH_CFLAGS)
LOCAL_SHARED_LIBRARIES := libvisual common
LOCAL_STATIC_LIBRARIES := evaluator
#include $(BUILD_SHARED_LIBRARY)

