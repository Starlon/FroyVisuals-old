LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE	:= libvisual
LOCAL_SRC_FILES := libvisual/lv_utils.c  libvisual/lv_beat.c  libvisual/lv_cache.c  libvisual/lv_cpu.c  libvisual/lv_gl.c  libvisual/lv_libvisual.c  libvisual/lv_mem.c  libvisual/lv_palette.c  libvisual/lv_rectangle.c  libvisual/lv_time.c  libvisual/lv_video.c  libvisual/lv_bin.c  libvisual/lv_collection.c  libvisual/lv_error.c  libvisual/lv_hashlist.c  libvisual/lv_list.c  libvisual/lv_morph.c   libvisual/lv_param.c   libvisual/lv_ringbuffer.c  libvisual/lv_transform.c  libvisual/lv_video_simd.c  libvisual/lv_actor.c  libvisual/lv_bmp.c   libvisual/lv_color.c   libvisual/lv_event.c   libvisual/lv_hashmap.c  libvisual/lv_log.c  libvisual/lv_object.c  libvisual/lv_plugin.c  libvisual/lv_songinfo.c  libvisual/lv_ui.c libvisual/lv_audio.c  libvisual/lv_buffer.c  libvisual/lv_config.c  libvisual/lv_fourier.c  libvisual/lv_input.c  libvisual/lv_math.c  libvisual/lv_os.c  libvisual/lv_random.c  libvisual/lv_thread.c
LOCAL_LDLIBS	:= -Wall
LOCAL_CFLAGS	:= -O0 -g
include $(BUILD_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_C_INCLUDES := ./include
LOCAL_SRC_FILES:= tinyalsa/mixer.c tinyalsa/pcm.c
LOCAL_MODULE := libtinyalsa
LOCAL_SHARED_LIBRARIES:= libcutils libutils
LOCAL_PRELINK_MODULE := false
include $(BUILD_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_C_INCLUDES := ./include jni/libvisual
LOCAL_MODULE := actor_lv_scope
LOCAL_SRC_FILES := lv_scope/actor_lv_scope.c
LOCAL_LDLIBS := -Wall
LOCAL_CFLAGS := -O0 -g
LOCAL_SHARED_LIBRARIES := libvisual
include $(BUILD_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_C_INCLUDES := ./include jni/libvisual
LOCAL_MODULE := input_alsa
LOCAL_SRC_FILES := alsa/input_alsa.c
LOCAL_LDLIBS := -Wall
LOCAL_CFLAGS := -O0 -g
LOCAL_SHARED_LIBRARIES := libtinyalsa libvisual
include $(BUILD_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_C_INCLUDES := ./include jni/libvisual
LOCAL_MODULE    := froyvisuals
LOCAL_SRC_FILES := froyvisuals.c
LOCAL_LDLIBS    := -lm -llog -ljnigraphics
LOCAL_STATIC_LIBRARIES := libvisual tinyalsa
include $(BUILD_SHARED_LIBRARY)