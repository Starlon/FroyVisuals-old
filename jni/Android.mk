LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE	:= libvisual
LOCAL_SRC_FILES := libvisual/lv_utils.c  libvisual/lv_beat.c  libvisual/lv_cache.c  libvisual/lv_cpu.c  libvisual/lv_gl.c  libvisual/lv_libvisual.c  libvisual/lv_mem.c  libvisual/lv_palette.c  libvisual/lv_rectangle.c  libvisual/lv_time.c  libvisual/lv_video.c  libvisual/lv_bin.c  libvisual/lv_collection.c  libvisual/lv_error.c  libvisual/lv_hashlist.c  libvisual/lv_list.c  libvisual/lv_morph.c   libvisual/lv_param.c   libvisual/lv_ringbuffer.c  libvisual/lv_transform.c  libvisual/lv_video_simd.c  libvisual/lv_actor.c  libvisual/lv_bmp.c   libvisual/lv_color.c   libvisual/lv_event.c   libvisual/lv_hashmap.c  libvisual/lv_log.c  libvisual/lv_object.c  libvisual/lv_plugin.c  libvisual/lv_songinfo.c  libvisual/lv_ui.c libvisual/lv_audio.c  libvisual/lv_buffer.c  libvisual/lv_config.c  libvisual/lv_fourier.c  libvisual/lv_input.c  libvisual/lv_math.c  libvisual/lv_os.c  libvisual/lv_random.c  libvisual/lv_thread.c
LOCAL_CFLAGS	:= -Wall -O0 -g
LOCAL_LDLIBS    := -lm -llog
include $(BUILD_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_C_INCLUDES := ./include
LOCAL_SRC_FILES:= tinyalsa/mixer.c tinyalsa/pcm.c
LOCAL_MODULE := libtinyalsa
LOCAL_LDLIBS := -llog
LOCAL_CFLAGS := -Wall -O0 -g
LOCAL_SHARED_LIBRARIES:= libcutils libutils
LOCAL_PRELINK_MODULE := false
include $(BUILD_STATIC_LIBRARY)

#### ACTOR PLUGINS

include $(CLEAR_VARS)
LOCAL_C_INCLUDES := ./include jni/libvisual
LOCAL_MODULE := actor_lv_scope
LOCAL_SRC_FILES := actor/lv_scope/actor_lv_scope.c
LOCAL_LDLIBS := -llog
LOCAL_CFLAGS := -Wall -O0 -g
LOCAL_SHARED_LIBRARIES := libvisual
#include $(BUILD_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_C_INCLUDES := ./include ./jni/libvisual
LOCAL_MODULE := actor_corona
LOCAL_SRC_FILES := actor/corona/actor_corona.cpp actor/corona/corona.cpp actor/corona/palette.cpp
LOCAL_LDLIBS := -llog
LOCAL_CFLAGS := -Wall -O0 -g
LOCAL_SHARED_LIBRARIES := libvisual
#include $(BUILD_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_C_INCLUDES := ./include jni/libvisual
LOCAL_MODULE := actor_bumpscope
LOCAL_SRC_FILES := actor/bumpscope/actor_bumpscope.c actor/bumpscope/bump_scope.c
LOCAL_LDLIBS := -llog
LOCAL_CFLAGS := -Wall -O0 -g
LOCAL_SHARED_LIBRARIES := libvisual
include $(BUILD_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_C_INCLUDES := ./include jni/libvisual
LOCAL_MODULE := actor_infinite
LOCAL_SRC_FILES := actor/infinite/compute.c  actor/infinite/display.c  actor/infinite/file.c  actor/infinite/lv_infinite.c  actor/infinite/renderer.c
LOCAL_LDLIBS := -llog 
LOCAL_CFLAGS := -Wall -O0 -g
LOCAL_SHARED_LIBRARIES := libvisual
#include $(BUILD_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_C_INCLUDES := ./include jni/libvisual
LOCAL_MODULE := actor_jakdaw
LOCAL_SRC_FILES := actor/jakdaw/actor_jakdaw.c  actor/jakdaw/feedback.c  actor/jakdaw/plotter.c
LOCAL_LDLIBS := -llog
LOCAL_CFLAGS := -Wall -O0 -g
LOCAL_SHARED_LIBRARIES := libvisual
#include $(BUILD_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_C_INCLUDES := ./include jni/libvisual jni/actor/oinksie
LOCAL_MODULE := actor_oinksie
LOCAL_SRC_FILES := actor/oinksie/actor_oinksie.c  actor/oinksie/gfx-analyzer.c    actor/oinksie/gfx-misc.c     actor/oinksie/misc.c     actor/oinksie/screen.c actor/oinksie/audio.c          actor/oinksie/gfx-background.c  actor/oinksie/gfx-palette.c  actor/oinksie/oinksie.c  actor/oinksie/table.c actor/oinksie/config.c         actor/oinksie/gfx-blur.c        actor/oinksie/gfx-scope.c    actor/oinksie/scene.c
LOCAL_LDLIBS := -llog
LOCAL_CFLAGS := -Wall -O0 -g
LOCAL_SHARED_LIBRARIES := libvisual
#include $(BUILD_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_C_INCLUDES := ./include jni/libvisual
LOCAL_MODULE := actor_JESS
LOCAL_SRC_FILES := actor/JESS/analyser.c    actor/JESS/draw.c            actor/JESS/jess.c  actor/JESS/projection.c actor/JESS/distorsion.c  actor/JESS/draw_low_level.c  actor/JESS/pal.c   actor/JESS/renderer.c
LOCAL_LDLIBS := -llog
LOCAL_CFLAGS := -Wall -O0 -g
LOCAL_SHARED_LIBRARIES := libvisual
#include $(BUILD_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_C_INCLUDES := ./include jni/libvisual
LOCAL_MODULE := actor_madspin
LOCAL_SRC_FILES := actor/madspin/madspin.c
LOCAL_LDLIBS := -lGLESv2 -llog
LOCAL_CFLAGS := -Wall -O0 -g
LOCAL_SHARED_LIBRARIES := libvisual 
#include $(BUILD_SHARED_LIBRARY)

#### MORPH PLUGINS

include $(CLEAR_VARS)
LOCAL_C_INCLUDES := ./include jni/libvisual
LOCAL_MODULE := morph_alphablend
LOCAL_SRC_FILES := morph/alphablend/morph_alphablend.c
LOCAL_LDLIBS := -llog
LOCAL_CFLAGS := -Wall -O0 -g
LOCAL_SHARED_LIBRARIES := libvisual 
#include $(BUILD_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_C_INCLUDES := ./include jni/libvisual
LOCAL_MODULE := morph_flash
LOCAL_SRC_FILES := morph/flash/morph_flash.c
LOCAL_LDLIBS := -llog
LOCAL_CFLAGS := -Wall -O0 -g
LOCAL_SHARED_LIBRARIES := libvisual 
#include $(BUILD_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_C_INCLUDES := ./include jni/libvisual
LOCAL_MODULE := morph_slide
LOCAL_SRC_FILES := morph/slide/morph_slide.c
LOCAL_LDLIBS := --llog
LOCAL_CFLAGS := -Wall -O0 -g
LOCAL_SHARED_LIBRARIES := libvisual 
#include $(BUILD_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_C_INCLUDES := ./include jni/libvisual
LOCAL_MODULE := morph_tentacle
LOCAL_SRC_FILES := morph/tentacle/morph_tentacle.c
LOCAL_LDLIBS := -llog
LOCAL_CFLAGS := -Wall -O0 -g
LOCAL_SHARED_LIBRARIES := libvisual 
#include $(BUILD_SHARED_LIBRARY)

#### INPUT PLUGINS

include $(CLEAR_VARS)
LOCAL_C_INCLUDES := ./include jni/libvisual
LOCAL_MODULE := input_alsa
LOCAL_SRC_FILES := input/alsa/input_alsa.c
LOCAL_LDLIBS := -llog
LOCAL_CFLAGS := -Wall -O0 -g
LOCAL_SHARED_LIBRARIES := libtinyalsa libvisual
include $(BUILD_SHARED_LIBRARY)

####################

include $(CLEAR_VARS)
LOCAL_C_INCLUDES := ./include jni/libvisual
LOCAL_MODULE    := froyvisuals
LOCAL_SRC_FILES := froyvisuals.c
LOCAL_LDLIBS    := -lm -llog -ljnigraphics
LOCAL_CFLAGS    := -Wall -O0 -g
LOCAL_STATIC_LIBRARIES := libvisual tinyalsa
include $(BUILD_SHARED_LIBRARY)

#### Utilities

include $(CLEAR_VARS)
LOCAL_C_INCLUDES := ./include
LOCAL_MODULE := tinycap
LOCAL_SRC_FILES := tinyalsa/tinycap.c
LOCAL_CFLAGS := -Wall
LOCAL_STATIC_LIBRARIES := tinyalsa
include $(BUILD_EXECUTABLE)

include $(CLEAR_VARS)
LOCAL_C_INCLUDES := ./include
LOCAL_MODULE := tinyplay
LOCAL_SRC_FILES := tinyalsa/tinyplay.c
LOCAL_CFLAGS := -Wall
LOCAL_STATIC_LIBRARIES := tinyalsa
include $(BUILD_EXECUTABLE)

include $(CLEAR_VARS)
LOCAL_C_INCLUDES := ./include
LOCAL_MODULE := tinymix
LOCAL_SRC_FILES := tinyalsa/tinymix.c
LOCAL_CFLAGS := -Wall
LOCAL_STATIC_LIBRARIES := tinyalsa
include $(BUILD_EXECUTABLE)
