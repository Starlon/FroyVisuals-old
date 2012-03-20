LOCAL_PATH := $(call my-dir)

# include the profiler snippet
#-include android-ndk-profiler.mk

# this is your shared library
include $(CLEAR_VARS)


LOCAL_C_INCLUDES := ./include
LOCAL_MODULE	:= visual


LOCAL_CFLAGS += $(WARNING_FLAGS)
LOCAL_CFLAGS += $(DEBUG_FLAGS)
LOCAL_CFLAGS += $(OPTIM_FLAGS)

LOCAL_LDLIBS    := -lm -ldl -llog 

#profiling
#LOCAL_LDLIBS += -L$(call host-path, $(LOCAL_PATH))/$(TARGET_ARCH_ABI) -landprof
#LOCAL_CFLAGS += -pg -DVISUAL_HAVE_PROFILING -fno-omit-frame-pointer -fno-function-sections


LOCAL_SRC_FILES := lv_utils.c  lv_beat.c  lv_cache.c  lv_cpu.c  lv_gl.c  lv_libvisual.c  lv_mem.c  lv_palette.c  lv_rectangle.c  lv_time.c  lv_video.c  lv_bin.c  lv_collection.c  lv_error.c  lv_hashlist.c  lv_list.c  lv_morph.c   lv_param.c   lv_ringbuffer.c  lv_transform.c  lv_video_simd.c  lv_actor.c  lv_bmp.c   lv_color.c   lv_event.c   lv_hashmap.c  lv_log.c  lv_object.c  lv_plugin.c  lv_songinfo.c  lv_ui.c lv_audio.c  lv_buffer.c  lv_config.c  lv_fourier.c  lv_input.c  lv_math.c  lv_os.c  lv_random.c  lv_thread.c kiss_fft129/kiss_fft.c

LOCAL_SRC_FILES += lvavs_pipeline.c lvavs_preset.c

ifeq ($(TARGET_ARCH_ABI),armeabi-v7a)
    #LOCAL_CFLAGS += -DVISUAL_ARCH_ARM=1 -DHAVE_NEON=1 -mfloat-abi=softfp -mfpu=neon
    #LOCAL_SRC_FILES := lv_utils.c  lv_beat.c  lv_cache.c  lv_cpu.c  lv_gl.c  lv_libvisual.c  lv_mem.c.neon  lv_palette.c  lv_rectangle.c  lv_time.c  lv_video.c  lv_bin.c  lv_collection.c  lv_error.c  lv_hashlist.c  lv_list.c  lv_morph.c   lv_param.c   lv_ringbuffer.c  lv_transform.c  lv_video_simd.c  lv_actor.c  lv_bmp.c   lv_color.c   lv_event.c   lv_hashmap.c  lv_log.c  lv_object.c  lv_plugin.c  lv_songinfo.c  lv_ui.c lv_audio.c  lv_buffer.c  lv_config.c  lv_fourier.c.neon  lv_input.c  lv_math.c.neon  lv_os.c  lv_random.c  lv_thread.c kiss_fft129/kiss_fft.c
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

#LOCAL_CFLAGS += -fno-omit-frame-pointer -fno-function-sections

include $(BUILD_SHARED_LIBRARY)
$(call import-module,cpufeatures) 
