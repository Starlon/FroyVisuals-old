LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_C_INCLUDES := ./include
LOCAL_MODULE	:= actor_avs_avi
LOCAL_SRC_FILES := actor_avs_avi.c
LOCAL_CFLAGS	+= $(ARCH_CFLAGS)
LOCAL_STATIC_LIBRARIES := libvisual common
#include $(BUILD_SHARED_LIBRARY)

