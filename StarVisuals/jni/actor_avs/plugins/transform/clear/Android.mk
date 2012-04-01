LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE	:= transform_avs_clear
LOCAL_SRC_FILES := transform_avs_clear.c
LOCAL_C_INCLUDES := \
    $(LOCAL_PATH)/../../../common \
    $(LOCAL_PATH)/../../../include \
    $(LOCAL_PATH)/../../../src\
    $(LOCAL_PATH)/../../../evaluator
LOCAL_CFLAGS    += $(ARCH_CFLAGS)
LOCAL_STATIC_LIBRARIES := evaluator
LOCAL_SHARED_LIBRARIES := libvisual common 
include $(BUILD_SHARED_LIBRARY)

