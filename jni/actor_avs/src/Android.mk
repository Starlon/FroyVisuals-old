LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE	:= actor_avs
LOCAL_SRC_FILES := $(addprefix /, $(notdir $(wildcard $(LOCAL_PATH)/*.c) $(wildcard $(LOCAL_PATH)/*.cpp)))
LOCAL_CFLAGS += $(WARNING_FLAGS)
LOCAL_CFLAGS += $(DEBUG_FLAGS)
LOCAL_CFLAGS += $(OPTIM_FLAGS)
LOCAL_LDFLAGS :=  
LOCAL_C_INCLUDES := \
	$(LOCAL_PATH)/../common/\
	$(LOCAL_PATH)/../include/
LOCAL_SHARED_LIBRARIES := libvisual common
include $(BUILD_SHARED_LIBRARY)

