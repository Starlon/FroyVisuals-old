LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_C_INCLUDES := ./include
LOCAL_MODULE	:= actor_avs
LOCAL_SRC_FILES := $(addprefix /, $(notdir $(wildcard $(LOCAL_PATH)/*.c) $(wildcard $(LOCAL_PATH)/*.cpp)))
LOCAL_CFLAGS	:= -Wall -O0 -g
LOCAL_LDFLAGS :=    -lgomp
LOCAL_C_INCLUDE := common/ evaluator/
LOCAL_STATIC_LIBRARIES := libvisual common
include $(BUILD_SHARED_LIBRARY)

