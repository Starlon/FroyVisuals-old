LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_STATIC_LIBRARIES := libvisual
LOCAL_C_INCLUDES := ./include
LOCAL_MODULE	:= morph_checkers
LOCAL_SRC_FILES := morph_checkers.c
LOCAL_CFLAGS    += $(ARCH_CFLAGS)
include $(BUILD_SHARED_LIBRARY)

