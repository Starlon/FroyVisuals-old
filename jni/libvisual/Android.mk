LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_C_INCLUDES := ./include
LOCAL_MODULE	:= visual
LOCAL_SRC_FILES := lv_utils.c  lv_beat.c  lv_cache.c  lv_cpu.c  lv_gl.c  lv_libvisual.c  lv_mem.c  lv_palette.c  lv_rectangle.c  lv_time.c  lv_video.c  lv_bin.c  lv_collection.c  lv_error.c  lv_hashlist.c  lv_list.c  lv_morph.c   lv_param.c   lv_ringbuffer.c  lv_transform.c  lv_video_simd.c  lv_actor.c  lv_bmp.c   lv_color.c   lv_event.c   lv_hashmap.c  lv_log.c  lv_object.c  lv_plugin.c  lv_songinfo.c  lv_ui.c lv_audio.c  lv_buffer.c  lv_config.c  lv_fourier.c  lv_input.c  lv_math.c  lv_os.c  lv_random.c  lv_thread.c
LOCAL_CFLAGS	:= -Wall -Wstrict-aliasing -Wcast-align -Waddress -g -O2  -ggdb3 -Wmissing-braces -Wimplicit -Wunused -Wmissing-prototypes -Wno-unused-variable -O0
LOCAL_LDLIBS    := -lm -ldl
include $(BUILD_SHARED_LIBRARY)

