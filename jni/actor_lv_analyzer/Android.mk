LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_C_INCLUDES := ./include
LOCAL_MODULE	:= actor_lv_analyzer
LOCAL_SRC_FILES := actor_lv_analyzer.c
LOCAL_CFLAGS += $(WARNING_FLAGS)
LOCAL_CFLAGS += $(DEBUG_FLAGS)
LOCAL_CFLAGS += $(OPTIM_FLAGS)
LOCAL_SHARED_LIBRARIES := libvisual
#include $(BUILD_SHARED_LIBRARY)

