LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_C_INCLUDES += \
    $(LOCAL_PATH)/../../../common \
    $(LOCAL_PATH)/../../../include \
    $(LOCAL_PATH)/../../../src
LOCAL_MODULE	:= actor_avs_superscope
LOCAL_SRC_FILES := actor_avs_superscope.c

LOCAL_CFLAGS += $(WARNING_FLAGS)
LOCAL_CFLAGS += $(DEBUG_FLAGS)
LOCAL_CFLAGS += $(OPTIM_FLAGS)

LOCAL_LDLIBS += -L$(call host-path, $(LOCAL_PATH))/$(TARGET_ARCH_ABI) -lluajit -ldl
LOCAL_SHARED_LIBRARIES := libvisual common
LOCAL_STATIC_LIBRARIES := visscript-lua
include $(BUILD_SHARED_LIBRARY)

