# Copyright (C) 2008 The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

LOCAL_PATH:= $(call my-dir)

# libutils is a little unique: It's built twice, once for the host
# and once for the device.

commonSources:= \
	Asset.cpp \
	AssetDir.cpp \
	AssetManager.cpp \
	BufferedTextOutput.cpp \
	CallStack.cpp \
	Debug.cpp \
	FileMap.cpp \
	Flattenable.cpp \
	ObbFile.cpp \
	Pool.cpp \
	RefBase.cpp \
	ResourceTypes.cpp \
	SharedBuffer.cpp \
	Static.cpp \
	StopWatch.cpp \
	StreamingZipInflater.cpp \
	String8.cpp \
	String16.cpp \
	StringArray.cpp \
	SystemClock.cpp \
	TextOutput.cpp \
	Threads.cpp \
	Timers.cpp \
	VectorImpl.cpp \
	ZipFileCRO.cpp \
	ZipFileRO.cpp \
	ZipUtils.cpp \
	misc.cpp




# For the device
# =====================================================
include $(CLEAR_VARS)


# we have the common sources, plus some device-specific stuff
LOCAL_SRC_FILES:= \
	$(commonSources) \
	BackupData.cpp \
	BackupHelpers.cpp \
	Looper.cpp

ifeq ($(TARGET_OS),linux)
LOCAL_LDLIBS += -lrt -ldl
endif

LOCAL_C_INCLUDES += \
    $(LOCAL_PATH)/../../include \
    $(LOCAL_PATH)/../../../../system/core/include

LOCAL_SHARED_LIBRARIES := \
	libndkcutils

LOCAL_MODULE:= libndkutils
LOCAL_CFLAGS += -DHAVE_PTHREADS -DHAVE_SYS_UIO_H -DHAVE_POSIX_FILEMAP -DOS_PATH_SEPARATOR=\'/\' -DHAVE_ENDIAN_H
LOCAL_LDLIBS += -lz -llog
include $(BUILD_SHARED_LIBRARY)

# Include subdirectory makefiles
# ============================================================

# If we're building with ONE_SHOT_MAKEFILE (mm, mmm), then what the framework
# team really wants is to build the stuff defined by this makefile.
ifeq (,$(ONE_SHOT_MAKEFILE))
include $(call first-makefiles-under,$(LOCAL_PATH))
endif

$(call import-module,system/core/libcutils)
