LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_C_INCLUDES := ./include
LOCAL_MODULE	:= actor_corona
LOCAL_SRC_FILES := actor_corona.cpp  corona.cpp  palette.cpp
LOCAL_CFLAGS	:= -Wall -O0 -g -Wstrict-aliasing -Wcast-align -Wpointer-arith -Waddress
LOCAL_STATIC_LIBRARIES := libvisual
#include $(BUILD_SHARED_LIBRARY)

