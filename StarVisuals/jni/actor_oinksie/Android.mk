LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_C_INCLUDES := ./include
LOCAL_MODULE	:= actor_oinksie
LOCAL_SRC_FILES := actor_oinksie.c audio.c config.c gfx-analyzer.c gfx-background.c gfx-blur.c gfx-misc.c gfx-palette.c gfx-scope.c misc.c oinksie.c scene.c screen.c table.c
LOCAL_CFLAGS    += $(ARCH_CFLAGS)
LOCAL_STATIC_LIBRARIES := libvisual
#include $(BUILD_SHARED_LIBRARY)

