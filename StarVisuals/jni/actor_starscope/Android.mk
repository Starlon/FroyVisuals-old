LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_C_INCLUDES := ./include
LOCAL_MODULE	:= actor_starscope
LOCAL_SRC_FILES := actor_starscope.c
LOCAL_CFLAGS += $(WARNING_FLAGS)
LOCAL_SHARED_LIBRARIES := libvisual
include $(BUILD_SHARED_LIBRARY)

