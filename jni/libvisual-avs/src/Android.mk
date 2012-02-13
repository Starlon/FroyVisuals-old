LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE	:= actor_avs
LOCAL_SRC_FILES := actor_AVS.c avs_parse.c avs_serialize.c lvavs_pipeline.c lvavs_preset.c main.c
LOCAL_CFLAGS	:= -Wall -O0 -g
LOCAL_LDFLAGS := 
LOCAL_C_INCLUDES := \
	$(LOCAL_PATH)/../common/\
	$(LOCAL_PATH)/../include/\
	$(LOCAL_PATH)/../evaluator/
LOCAL_STATIC_LIBRARIES := libvisual common
include $(BUILD_SHARED_LIBRARY)

