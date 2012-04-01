LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_C_INCLUDES := ./include

LOCAL_CFLAGS += $(WARNING_FLAGS)

LOCAL_LDLIBS += -L$(call host-path, $(LOCAL_PATH))/$(TARGET_ARCH_ABI) -llibluajit

LOCAL_MODULE	:= lua
LOCAL_SRC_FILES := visscript-lua/visscript-lua.c

LOCAL_SHARED_LIBRARIES := visual

include $(BUILD_STATIC_LIBRARY)
