LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_C_INCLUDES := ./include
LOCAL_MODULE	:= morph_alphablend
LOCAL_SRC_FILES := morph_alphablend.c
LOCAL_CFLAGS += $(WARNING_FLAGS)
LOCAL_STATIC_LIBRARIES := libvisual
include $(BUILD_SHARED_LIBRARY)

