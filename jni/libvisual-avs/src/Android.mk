LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE	:= actor_avs
LOCAL_SRC_FILES := actor_AVS.c avs_parse.c avs_serialize.c lvavs_pipeline.c lvavs_preset.c main.c
LOCAL_CFLAGS += $(WARNING_FLAGS)
LOCAL_CFLAGS += $(DEBUG_FLAGS)
LOCAL_CFLAGS += $(OPTIM_FLAGS)
LOCAL_LDFLAGS :=  
LOCAL_C_INCLUDES := \
	$(LOCAL_PATH)/../common/\
	$(LOCAL_PATH)/../include/\
	$(LOCAL_PATH)/../evaluator/\
	$(LOCAL_PATH)/../../libxml2/
LOCAL_SHARED_LIBRARIES := libvisual common
LOCAL_STATIC_LIBRARIES := xmms2
include $(BUILD_SHARED_LIBRARY)

