LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_C_INCLUDES := ./include
LOCAL_MODULE	:= actor_infinite
LOCAL_SRC_FILES := compute.c  display.c  file.c  lv_infinite.c  renderer.c
LOCAL_CFLAGS	:= -Wall -O0 -g -Wstrict-aliasing -Wcast-align -Wpointer-arith -Waddress
LOCAL_STATIC_LIBRARIES := libvisual
include $(BUILD_SHARED_LIBRARY)

