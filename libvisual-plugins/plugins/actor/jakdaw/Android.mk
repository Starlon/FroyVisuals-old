LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_C_INCLUDES := ./include
LOCAL_MODULE	:= actor_jakdaw
LOCAL_SRC_FILES := actor_jakdaw.c  feedback.c  plotter.c
LOCAL_CFLAGS    += $(ARCH_CFLAGS)
LOCAL_STATIC_LIBRARIES := libvisual
include $(BUILD_SHARED_LIBRARY)
