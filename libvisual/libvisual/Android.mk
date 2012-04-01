LOCAL_PATH := $(call my-dir)

# include the profiler snippet
#-include android-ndk-profiler.mk

# this is your shared library
include $(CLEAR_VARS)


LOCAL_C_INCLUDES := ./include
LOCAL_MODULE	:= visual
LOCAL_SHARED_LIBRARIES := lua 

LOCAL_CFLAGS += $(WARNING_FLAGS)
LOCAL_CFLAGS += $(DEBUG_FLAGS)
LOCAL_CFLAGS += $(OPTIM_FLAGS)

LOCAL_LDLIBS    := -lm -ldl -llog 

#profiling
#LOCAL_LDLIBS += -L$(call host-path, $(LOCAL_PATH))/$(TARGET_ARCH_ABI) -landprof
#LOCAL_CFLAGS += -pg -DVISUAL_HAVE_PROFILING -fno-omit-frame-pointer -fno-function-sections

LOCAL_SRC_FILES := $(addprefix /, $(notdir $(wildcard $(LOCAL_PATH)/*.c) $(wildcard $(LOCAL_PATH)/*.cpp)))
LOCAL_SRC_FILES += private/lv_video_convert.c  private/lv_video_fill.c  private/lv_video_scale.c

ifeq ($(TARGET_ARCH_ABI),armeabi-v7a)
    #LOCAL_CFLAGS += -DVISUAL_ARCH_ARM=1 -DHAVE_NEON=1 -mfloat-abi=softfp -mfpu=neon
endif

ifeq ($(TARGET_ARCH_ABI),x86)
    LOCAL_CFLAGS += -DVISUAL_ARCH_X86=1
endif

LOCAL_SHARED_LIBRARIES := cpufeatures

ifeq ($(TARGET_ARCH_ABI),armeabi)
    #LOCAL_CFLAGS += -DVISUAL_ARCH_ARM=1 
    #LOCAL_LDLIBS += -L$(call host-path, $(LOCAL_PATH))/$(TARGET_ARCH_ABI) -landprof
    #LOCAL_CFLAGS += -pg -DVISUAL_HAVE_PROFILING -fno-omit-frame-pointer -fno-function-sections
endif

ifeq ($(TARGET_ARCH_ABI),armeabi-v7a)
    #LOCAL_CFLAGS += -DVISUAL_ARCH_ARM=1
    #LOCAL_LDLIBS += -L$(call host-path, $(LOCAL_PATH))/$(TARGET_ARCH_ABI) -landprof
    #LOCAL_CFLAGS += -pg -DVISUAL_HAVE_PROFILING -fno-omit-frame-pointer -fno-function-sections
endif

include $(BUILD_SHARED_LIBRARY)
$(call import-module,cpufeatures) 
