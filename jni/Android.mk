
WARNING_FLAGS := -Wall -Wstrict-aliasing -Wcast-align -Waddress -Wmissing-braces -Wimplicit -Wunused -Wno-unused-variable
DEBUG_FLAGS := -g -ggdb3
OPTIM_FLAGS := -O2
OPTIM_FLAGS := -O3 -mfpu=neon -mfloat-abi=softfp -ftree-vectorize -ffast-math -funsafe-math-optimizations -fsingle-precision-constant

include $(call all-subdir-makefiles)


