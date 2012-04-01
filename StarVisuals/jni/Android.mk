
WARNING_FLAGS := -Wall -Wstrict-aliasing -Wcast-align -Waddress -Wmissing-braces -Wimplicit -Wunused -Wno-unused-variable


ifeq ($(TARGET_ARCH_ABI),armeabi)
#    ARCH_CFLAGS := -march=armv6 -mfp=vfp -D__ARM_ARCH_6__ $(WARNING_FLAGS)
        ARCH_CFLAGS := $(WARNING_FLAGS)
endif
    
ifeq ($(TARGET_ARCH_ABI),armeabi-v7a)
    ARCH_CFLAGS += -DHAVE_NEON=1 -mfloat-abi=softfp -mfpu=neon $(WARNING_FLAGS)
endif
        
ifeq ($(TARGET_ARCH_ABI),x86)
    ARCH_CFLAGS += -DVISUAL_ARCH_X86=1 $(WARNING_FLAGS)
endif


include $(call all-subdir-makefiles)


