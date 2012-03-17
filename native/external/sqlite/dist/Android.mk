##
##
## Build the library
##
##

LOCAL_PATH:= $(call my-dir)

common_src_files := sqlite3.c

# NOTE the following flags,
#   SQLITE_TEMP_STORE=3 causes all TEMP files to go into RAM. and thats the behavior we want
#   SQLITE_ENABLE_FTS3   enables usage of FTS3 - NOT FTS1 or 2.
#   SQLITE_DEFAULT_AUTOVACUUM=1  causes the databases to be subject to auto-vacuum
sqlite_cflags :=  -DHAVE_USLEEP=1 -DSQLITE_DEFAULT_JOURNAL_SIZE_LIMIT=1048576 -DSQLITE_THREADSAFE=1 -DNDEBUG=1 -DSQLITE_ENABLE_MEMORY_MANAGEMENT=1 -DSQLITE_DEFAULT_AUTOVACUUM=1 -DSQLITE_TEMP_STORE=3 -DSQLITE_ENABLE_FTS3 -DSQLITE_ENABLE_FTS3_BACKWARDS

# the device library
include $(CLEAR_VARS)

LOCAL_SRC_FILES := $(common_src_files)

ifneq ($(TARGET_ARCH),arm)
LOCAL_LDLIBS += -lpthread -ldl
endif

LOCAL_CFLAGS += $(sqlite_cflags)

ifneq ($(TARGET_SIMULATOR),true)
LOCAL_SHARED_LIBRARIES := libdl
endif

LOCAL_MODULE:= libndksqlite
LOCAL_C_INCLUDES += $(call include-path-for, system-core)/cutils
LOCAL_SHARED_LIBRARIES += liblog \
            libndkicuuc \
            libndkicui18n \
            libndkutils
LOCAL_LDLIBS += -llog

# include android specific methods
LOCAL_WHOLE_STATIC_LIBRARIES := libndksqlite3_android

## Choose only one of the allocator systems below
# new sqlite 3.5.6 no longer support external allocator 
#LOCAL_SRC_FILES += mem_malloc.c
#LOCAL_SRC_FILES += mem_mspace.c


include $(BUILD_SHARED_LIBRARY)


