LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_C_INCLUDES := $(LOCAL_PATH)/../projectM-android/source/
LOCAL_MODULE	:= actor_projectM
LOCAL_SRC_FILES := $(addprefix /, $(notdir $(wildcard $(LOCAL_PATH)/*.c) $(wildcard $(LOCAL_PATH)/*.cpp)))
LOCAL_CFLAGS    += $(ARCH_CFLAGS) -DPROJECTM_PREFIX=\"sdcard\"
LOCAL_SHARED_LIBRARIES := libvisual projectM
include $(BUILD_SHARED_LIBRARY)

