LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_C_INCLUDES := ./include
LOCAL_MODULE	:= actor_lv_scope
LOCAL_SRC_FILES := actor_lv_scope.c
LOCAL_CFLAGS	:= -Wall -O0 -g -Wstrict-aliasing -Wcast-align -Wpointer-arith -Waddress
LOCAL_SHARED_LIBRARIES := libvisual
include $(BUILD_SHARED_LIBRARY)

