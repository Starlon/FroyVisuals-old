LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := main

LOCAL_C_INCLUDES := ./include

LOCAL_CFLAGS := -DPLAY_MOD

LOCAL_CFLAGS += $(WARNING_FLAGS)
LOCAL_CFLAGS += $(DEBUG_FLAGS)
LOCAL_CFLAGS += $(OPTIM_FLAGS)

LOCAL_SRC_FILES := starvisuals.c

LOCAL_SHARED_LIBRARIES := libvisual
LOCAL_STATIC_LIBRARIES := tinyalsa

LOCAL_LDLIBS := -lGLESv1_CM -lm -llog -ljnigraphics

include $(BUILD_SHARED_LIBRARY)

