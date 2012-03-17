##
## Copyright 2009, The Android Open Source Project
##
## Redistribution and use in source and binary forms, with or without
## modification, are permitted provided that the following conditions
## are met:
##  * Redistributions of source code must retain the above copyright
##    notice, this list of conditions and the following disclaimer.
##  * Redistributions in binary form must reproduce the above copyright
##    notice, this list of conditions and the following disclaimer in the
##    documentation and/or other materials provided with the distribution.
##
## THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ``AS IS'' AND ANY
## EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
## IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
## PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
## CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
## EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
## PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
## PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
## OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
## (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
## OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
##

# From android-platform_build-c5033a3/core/definitions.mk

define transform-generated-source
@echo "target Generated: $(PRIVATE_MODULE) <= $<"
@mkdir -p $(dir $@)
$(hide) $(PRIVATE_CUSTOM_TOOL)
endef

OLD_FLEX := flex





WEBCORE_INSTRUMENTATION := false
ENABLE_AUTOFILL = false
ENABLE_SVG = true
SUPPORT_COMPLEX_SCRIPTS = true
JAVASCRIPT_ENGINE = jsc
HTTP_STACK = android

BASE_PATH := $(call my-dir)
base_intermediates := $(TARGET_OBJS)/intermediates

BUILD_PARTIAL_LINK := $(BASE_PATH)/build-partial-link.mk

# We have to use bison 2.3
include $(BASE_PATH)/bison_check.mk

SOURCE_PATH := $(BASE_PATH)/Source
WEBCORE_PATH := $(SOURCE_PATH)/WebCore
JAVASCRIPTCORE_PATH := $(SOURCE_PATH)/JavaScriptCore
WEBKIT_PATH := $(SOURCE_PATH)/WebKit
WEBCORE_INTERMEDIATES_PATH := $(WEBCORE_PATH)/intermediates
JAVASCRIPTCORE_INTERMEDIATES_PATH := $(JAVASCRIPTCORE_PATH)/intermediates
intermediates := $(WEBCORE_INTERMEDIATES_PATH)

WEBCOREDIR_V8BINDINGS_INCLUDES := \
	$(BASE_PATH)/../external/v8/include \
	\
	$(WEBCORE_PATH)/bindings/v8 \
	$(WEBCORE_PATH)/bindings/v8/custom \
	$(WEBCORE_PATH)/bindings/v8/specialization \
	$(WEBCORE_PATH)/bridge \
	$(WEBCORE_PATH)/bridge/jni \
	$(WEBCORE_PATH)/bridge/jni/v8 \
	$(WEBCORE_PATH)/bridge/jsc \
	\
	$(WEBCORE_INTERMEDIATES_PATH)/bindings \
	$(WEBCORE_INTERMEDIATES_PATH)/svg \
	$(BASE_PATH)/Source/JavaScriptCore

WEBKIT_C_INCLUDES := \
	$(JNI_H_INCLUDE) \
	$(WEBKIT_PATH)/android/icu \
	$(BASE_PATH)/../external/ \
	$(BASE_PATH)/../external/icu4c/common \
	$(BASE_PATH)/../external/icu4c/i18n \
	$(BASE_PATH)/../external/jpeg \
	$(BASE_PATH)/../external/libxml2/include \
	$(BASE_PATH)/../external/libxslt \
	$(BASE_PATH)/../external/hyphenation \
	$(BASE_PATH)/../external/skia/emoji \
	$(BASE_PATH)/../external/skia/gpu/include \
	$(BASE_PATH)/../external/skia/include/core \
	$(BASE_PATH)/../external/skia/include/effects \
	$(BASE_PATH)/../external/skia/include/gpu \
	$(BASE_PATH)/../external/skia/include/images \
	$(BASE_PATH)/../external/skia/include/ports \
	$(BASE_PATH)/../external/skia/include/utils \
	$(BASE_PATH)/../external/skia/src/ports \
	$(BASE_PATH)/../external/sqlite/dist \
	$(BASE_PATH)/../frameworks/base/core/jni/android/graphics \
	$(BASE_PATH)/../frameworks/base/include \
	$(NDK_ROOT)/sources/cxx-stl/stlport/stlport \
	$(BASE_PATH)/../system/core/include \
	$(SOURCE_PATH) \
	$(WEBCORE_PATH) \
	$(WEBCORE_PATH)/accessibility \
	$(WEBCORE_PATH)/bindings/ \
	$(WEBCORE_PATH)/bindings/generic \
	$(WEBCORE_PATH)/css \
	$(WEBCORE_PATH)/dom \
	$(WEBCORE_PATH)/editing \
	$(WEBCORE_PATH)/fileapi \
	$(WEBCORE_PATH)/history \
	$(WEBCORE_PATH)/history/android \
	$(WEBCORE_PATH)/html \
	$(WEBCORE_PATH)/html/canvas \
	$(WEBCORE_PATH)/html/parser \
	$(WEBCORE_PATH)/html/shadow \
	$(WEBCORE_PATH)/inspector \
	$(WEBCORE_PATH)/loader \
	$(WEBCORE_PATH)/loader/appcache \
	$(WEBCORE_PATH)/loader/archive \
	$(WEBCORE_PATH)/loader/archive/android \
	$(WEBCORE_PATH)/loader/cache \
	$(WEBCORE_PATH)/loader/icon \
	$(WEBCORE_PATH)/notifications \
	$(WEBCORE_PATH)/page \
	$(WEBCORE_PATH)/page/android \
	$(WEBCORE_PATH)/page/animation \
	$(WEBCORE_PATH)/platform \
	$(WEBCORE_PATH)/platform/android \
	$(WEBCORE_PATH)/platform/animation \
	$(WEBCORE_PATH)/platform/graphics \
	$(WEBCORE_PATH)/platform/graphics/android \
	$(WEBCORE_PATH)/platform/graphics/filters \
	$(WEBCORE_PATH)/platform/graphics/gpu \
	$(WEBCORE_PATH)/platform/graphics/network \
	$(WEBCORE_PATH)/platform/graphics/skia \
	$(WEBCORE_PATH)/platform/graphics/transforms \
	$(WEBCORE_PATH)/platform/image-decoders \
	$(WEBCORE_PATH)/platform/image-decoders/bmp \
	$(WEBCORE_PATH)/platform/image-decoders/gif \
	$(WEBCORE_PATH)/platform/image-decoders/ico \
	$(WEBCORE_PATH)/platform/image-decoders/jpeg \
	$(WEBCORE_PATH)/platform/image-decoders/png \
	$(WEBCORE_PATH)/platform/image-decoders/webp \
	$(WEBCORE_PATH)/platform/mock \
	$(WEBCORE_PATH)/platform/network \
	$(WEBCORE_PATH)/platform/network/android \
	$(WEBCORE_PATH)/platform/sql \
	$(WEBCORE_PATH)/platform/text \
	$(WEBCORE_PATH)/platform/text/transcoder \
	$(WEBCORE_PATH)/plugins \
	$(WEBCORE_PATH)/plugins/android \
	$(WEBCORE_PATH)/rendering \
	$(WEBCORE_PATH)/rendering/style \
	$(WEBCORE_PATH)/rendering/svg \
	$(WEBCORE_PATH)/rendering/mathml \
	$(WEBCORE_PATH)/storage \
	$(WEBCORE_PATH)/svg \
	$(WEBCORE_PATH)/svg/animation \
	$(WEBCORE_PATH)/svg/graphics \
	$(WEBCORE_PATH)/svg/graphics/filters \
	$(WEBCORE_PATH)/svg/properties \
	$(WEBCORE_PATH)/websockets \
	$(WEBCORE_PATH)/workers \
	$(WEBCORE_PATH)/xml \
	$(WEBKIT_PATH)/android \
	$(WEBKIT_PATH)/android/WebCoreSupport \
	$(WEBKIT_PATH)/android/jni \
	$(WEBKIT_PATH)/android/nav \
	$(WEBKIT_PATH)/android/plugins \
	$(JAVASCRIPTCORE_PATH) \
	$(JAVASCRIPTCORE_PATH)/collector/handles \
	$(JAVASCRIPTCORE_PATH)/heap \
	$(JAVASCRIPTCORE_PATH)/wtf \
	$(JAVASCRIPTCORE_PATH)/wtf/unicode \
	$(JAVASCRIPTCORE_PATH)/wtf/unicode/icu \
	$(WEBCORE_INTERMEDIATES_PATH) \
	$(WEBCORE_INTERMEDIATES_PATH)/css \
	$(WEBCORE_INTERMEDIATES_PATH)/html \
	$(WEBCORE_INTERMEDIATES_PATH)/platform \
	$(WEBCORE_INTERMEDIATES_PATH)/xml \
	$(BASE_PATH)/../external/harfbuzz/src \
	$(BASE_PATH)/../external/harfbuzz/contrib \
	$(WEBCOREDIR_V8BINDINGS_INCLUDES)

WEBKIT_CPPFLAGS := -Wno-sign-promo

WEBKIT_CFLAGS := \
	-Wno-endif-labels -Wno-import -Wno-format \
  -fno-strict-aliasing \
  -include "WebCorePrefix.h" \
	-DALWAYS_INLINE=inline \
	-include "assert.h" \
	-DHAVE_ENDIAN_H -DHAVE_SYS_UIO_H -DHAVE_PTHREADS \
	-DGL_GLEXT_PROTOTYPES -DEGL_EGLEXT_PROTOTYPES \
	-Darm \
	-Wno-psabi \
	-DANDROID_LARGE_MEMORY_DEVICE \
	-DENABLE_SVG=1 -DENABLE_SVG_ANIMATION=1 \
	-DWTF_USE_ACCELERATED_COMPOSITING=1 \
	-DSUPPORT_COMPLEX_SCRIPTS=1 \
	-fvisibility=hidden \
	-DWTF_USE_V8=1

include $(BASE_PATH)/Source/JavaScriptCore/Android.v8.wtf.mk
include $(BASE_PATH)/Source/WebCore/Android.mk
include $(BASE_PATH)/Source/WebCore/Android.v8bindings.mk
include $(BASE_PATH)/Source/WebCore/Android.derived.mk
include $(BASE_PATH)/Source/WebCore/Android.derived.v8bindings.mk
include $(BASE_PATH)/Source/WebKit/Android.mk

WEBCOREDIR_GEN_SOURCES := $(filter %.cpp,$(WEBCOREDIR_GENERATED_SOURCES))
WEBCOREDIR_GEN_SOURCES := $(subst $(WEBCORE_PATH)/,,$(WEBCOREDIR_GEN_SOURCES))
WTFDIR_SOURCES := $(addprefix Source/JavaScriptCore/,$(WTFDIR_SOURCES))
WEBKITDIR_SOURCES := $(addprefix Source/WebKit/,$(WEBKITDIR_SOURCES))

additional-dependencies = \
  $(eval __object_files := \
           $(addprefix $(TARGET_OBJS)/$(patsubst lib%,%,$(LOCAL_MODULE))/,\
           $(patsubst %.cpp,%.o,\
           $(patsubst %.c,%.o,$(LOCAL_SRC_FILES))))) \
	$(eval $(__object_files): $1)

####################### wtfdir

include $(CLEAR_VARS)

LOCAL_PATH := $(BASE_PATH)
LOCAL_CFLAGS := $(WEBKIT_CFLAGS)
LOCAL_CPPFLAGS := $(WEBKIT_CPPFLAGS)
LOCAL_C_INCLUDES := $(WEBKIT_C_INCLUDES) \
	$(JAVASCRIPTCORE_INTERMEDIATES_PATH) \
	$(JAVASCRIPTCORE_INTERMEDIATES_PATH)/runtime

LOCAL_MODULE := libndkwtfdir
LOCAL_SRC_FILES := $(WTFDIR_SOURCES)
LOCAL_ALLOW_UNDEFINED_SYMBOLS := true

$(call additional-dependencies,$(WTFDIR_GENERATED_SOURCES))

#include $(BUILD_PARTIAL_LINK)
include $(BUILD_STATIC_LIBRARY)

####################### webcoredir

WEBCOREDIR_SOURCES := \
	$(WEBCOREDIR_BINDINGS_SOURCES) \
	$(WEBCOREDIR_CSS_SOURCES) \
	$(WEBCOREDIR_DOM_SOURCES) \
	$(WEBCOREDIR_EDITING_SOURCES) \
	$(WEBCOREDIR_FILEAPI_SOURCES) \
	$(WEBCOREDIR_HISTORY_SOURCES) \
	$(WEBCOREDIR_HTML_SOURCES) \
	$(WEBCOREDIR_INSPECTOR_SOURCES) \
	$(WEBCOREDIR_LOADER_SOURCES) \
	$(WEBCOREDIR_MATHML_SOURCES) \
	$(WEBCOREDIR_PAGE_SOURCES) \
	$(WEBCOREDIR_PLATFORM_SOURCES) \
	$(WEBCOREDIR_PLUGINS_SOURCES) \
	$(WEBCOREDIR_RENDERING_SOURCES) \
	$(WEBCOREDIR_STORAGE_SOURCES) \
	$(WEBCOREDIR_SVG_SOURCES) \
	$(WEBCOREDIR_WORKERS_SOURCES) \
	$(WEBCOREDIR_XML_SOURCES) \
	$(WEBCOREDIR_V8BINDINGS_SOURCES) \
	$(WEBCOREDIR_GEN_SOURCES)

WEBCOREDIR_SOURCES := $(addprefix Source/WebCore/,$(WEBCOREDIR_SOURCES))

include $(CLEAR_VARS)

LOCAL_PATH := $(BASE_PATH)
LOCAL_CFLAGS := $(WEBKIT_CFLAGS)
LOCAL_CPPFLAGS := $(WEBKIT_CPPFLAGS)
LOCAL_C_INCLUDES := $(WEBKIT_C_INCLUDES)

LOCAL_MODULE := libndkwebcoredir
LOCAL_SRC_FILES := libndkwebcoredir.cpp \
	$(WEBCOREDIR_SOURCES) \
	$(WEBKITDIR_SOURCES)

$(eval $(call additional-dependencies,$(WEBCOREDIR_GENERATED_SOURCES)))

include $(BUILD_STATIC_LIBRARY)

####################### the whole thing

include $(CLEAR_VARS)

LOCAL_PATH := $(BASE_PATH)
LOCAL_CFLAGS := $(WEBKIT_CFLAGS)
LOCAL_CPPFLAGS := $(WEBKIT_CPPFLAGS)
LOCAL_C_INCLUDES := $(WEBKIT_C_INCLUDES)
LOCAL_MODULE := libndkwebkit
LOCAL_SRC_FILES := \
	Source/WebKit/android/jni/WebCoreJniOnLoad.cpp
# \
#	ofiles/libndkwebcoredir_o.o \
#	ofiles/libndkwtfdir_o.o \
#	ofiles/libndkwebkitdir_o.o
LOCAL_LDLIBS := -ldl -lGLESv2 -lEGL -llog -landroid
LOCAL_SHARED_LIBRARIES := \
	libndksqlite \
	libndkjpeg \
	libndkskia \
	libndkskiagpu \
	libndkutils \
	libndkcutils \
	libndkicuuc \
	libndkicui18n \
	libndkharfbuzz
LOCAL_STATIC_LIBRARIES := \
	libndkwebcoredir \
	libndkwtfdir \
	libndkv8 \
	libndkxslt \
	libndkxml2 \
	libndkhyphenation

$(eval $(call additional-dependencies,$(WEBCOREDIR_GENERATED_SOURCES)))

include $(BUILD_SHARED_LIBRARY)

$(call import-module,cxx-stl/stlport)
$(call import-module,system/core/libcutils)
$(call import-module,frameworks/base/libs/utils)
$(call import-module,frameworks/opt/emoji)
$(call import-module,external/icu4c)
$(call import-module,external/skia)
$(call import-module,external/libxml2)
$(call import-module,external/libxslt)
$(call import-module,external/harfbuzz)
$(call import-module,external/sqlite/android)
$(call import-module,external/sqlite/dist)
$(call import-module,external/hyphenation)
$(call import-module,external/openssl)
$(call import-module,external/v8)
$(call import-module,webkit)
