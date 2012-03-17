
WARNING_FLAGS := -Wall -Wstrict-aliasing -Wcast-align -Waddress -Wmissing-braces -Wimplicit -Wunused -Wno-unused-variable
DEBUG_FLAGS := -g -ggdb3 
OPTIM_FLAGS := -O0 
#-O3 -ftree-vectorize -ffast-math -funsafe-math-optimizations -fsingle-precision-constant
ifeq ($(TARGET_ARCH_ABI),armeabi-v7a)
    LOCAL_CFLAGS += -DNO_MMX
    OPTIM_FLAGS += -DHAVE_ARM=1 -DHAVE_NEON=1 -mfloat-abi=softfp -mfpu=neon
endif
ifeq ($(TARGET_ARCH_ABI),armeabi)
    LOCAL_CFLAGS += -DNO_MMX
    OPTIM_FLAGS += -DHAVE_ARM=1 -mfloat-abi=softfp
endif
ifeq ($(TARGET_ARCH_ABI),x86)
    # No MMX because the MMX assembly under actor_avs is for Windows.
    LOCAL_CFLAS += -DNO_MMX
    DEBUG_FLAGS := -g
endif

include $(call all-subdir-makefiles)


