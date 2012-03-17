# Copyright (C) 2009 The Android Open Source Project
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
include $(CLEAR_VARS)

LOCAL_SRC_FILES := EmojiFactory.cpp

LOCAL_MODULE_TAGS := optional

LOCAL_MODULE := libndkemoji

LOCAL_SHARED_LIBRARIES := \
	libndkcutils \
	libndkutils

ifeq ($(TARGET_OS)-$(TARGET_SIMULATOR),linux-true)
LOCAL_LDLIBS += -ldl
endif
ifneq ($(TARGET_SIMULATOR),true)
LOCAL_SHARED_LIBRARIES += libdl
endif

LOCAL_CFLAGS += -Wall -Werror

LOCAL_C_INCLUDES += \
	$(LOCAL_PATH)/../../../frameworks/base/include/utils \
  $(LOCAL_PATH)/../../../frameworks/base/include \
  $(LOCAL_PATH)/../../../system/core/include \
  $(LOCAL_PATH)/../../../frameworks/base/include

include $(BUILD_SHARED_LIBRARY)

$(call import-module,system/core/libcutils)
$(call import-module,frameworks/base/libs/utils)
