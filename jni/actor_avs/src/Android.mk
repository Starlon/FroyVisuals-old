LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE	:= actor_avs
LOCAL_SRC_FILES := actor_AVS.c
LOCAL_CFLAGS += $(WARNING_FLAGS)
LOCAL_CFLAGS += $(DEBUG_FLAGS)
LOCAL_CFLAGS += $(OPTIM_FLAGS)
LOCAL_LDFLAGS :=  
LOCAL_C_INCLUDES := \
	$(LOCAL_PATH)/../common/\
	$(LOCAL_PATH)/../include/\
	$(LOCAL_PATH)/../evaluator/\
	$(LOCAL_PATH)/../libxml2/
LOCAL_SHARED_LIBRARIES := libvisual
LOCAL_STATIC_LIBRARIES := libxml2 common
include $(BUILD_SHARED_LIBRARY)

