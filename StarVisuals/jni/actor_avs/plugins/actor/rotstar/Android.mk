LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_C_INCLUDES := \
    $(LOCAL_PATH)/../../../common \
    $(LOCAL_PATH)/../../../include \
    $(LOCAL_PATH)/../../../src\
    $(LOCAL_PATH)/../../../evaluator
LOCAL_MODULE	:= actor_avs_rotstar
LOCAL_SRC_FILES := actor_avs_rotstar.c
LOCAL_CFLAGS    += $(ARCH_CFLAGS)
LOCAL_SHARED_LIBRARIES := libvisual common
LOCAL_STATIC_LIBRARIES := evaluator
#include $(BUILD_SHARED_LIBRARY)

