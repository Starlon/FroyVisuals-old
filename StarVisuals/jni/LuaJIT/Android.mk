LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_C_INCLUDES := ./include
LOCAL_MODULE	:= ae

LOCAL_CFLAGS += $(WARNING_FLAGS)
LOCAL_CFLAGS += $(DEBUG_FLAGS)
LOCAL_CFLAGS += $(OPTIM_FLAGS)

#LOCAL_LDLIBS    := -lm -ldl -llog 

LOCAL_LDLIBS += -L$(call host-path, $(LOCAL_PATH))/$(TARGET_ARCH_ABI) -llibluajit

LOCAL_SRC_FILES := ae/ae.c

include $(BUILD_STATIC_LIBRARY)
