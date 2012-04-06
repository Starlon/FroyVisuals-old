LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_C_INCLUDES := ./include $(LOCAL_PATH)/slb/include
LOCAL_MODULE	:= lutok
LOCAL_SRC_FILES := $(addprefix /, $(notdir $(wildcard $(LOCAL_PATH)/*.c) $(wildcard $(LOCAL_PATH)/*.cpp)))
LOCAL_CFLAGS    += $(ARCH_CFLAGS) -fexceptions
LOCAL_SHARED_LIBRARIES := libvisual slb gnustl
include $(BUILD_SHARED_LIBRARY)

