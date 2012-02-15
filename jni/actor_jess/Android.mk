LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_C_INCLUDES := ./include
LOCAL_MODULE	:= actor_jess
LOCAL_SRC_FILES := analyser.c  distorsion.c  draw.c  draw_low_level.c  jess.c  pal.c  projection.c  renderer.c

LOCAL_CFLAGS	:= -Wall -O0 -g
LOCAL_STATIC_LIBRARIES := libvisual
#include $(BUILD_SHARED_LIBRARY)

