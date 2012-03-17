# This is just a stub so other elements have a module to call. 
LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE	:= lua

LOCAL_LDLIBS += -L$(call host-path, $(LOCAL_PATH))/$(TARGET_ARCH_ABI) -lliblua

include $(BUILD_STATIC_LIBRARY)
