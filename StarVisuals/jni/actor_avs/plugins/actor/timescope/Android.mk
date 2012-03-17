LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_C_INCLUDES := \
    $(LOCAL_PATH)/../../../common \
    $(LOCAL_PATH)/../../../include \
    $(LOCAL_PATH)/../../../src\
    $(LOCAL_PATH)/../../../evaluator
LOCAL_MODULE	:= actor_avs_timescope
LOCAL_SRC_FILES := actor_avs_timescope.c
LOCAL_CFLAGS	:= -Wall -O0 -g
LOCAL_SHARED_LIBRARIES := libvisual common
LOCAL_STATIC_LIBRARIES := evaluator
#FIXME This isn't working! include $(BUILD_SHARED_LIBRARY)

