
WARNING_FLAGS := -Wall -Wstrict-aliasing -Wcast-align -Waddress -Wmissing-braces -Wimplicit -Wunused -Wno-unused-variable
DEBUG_FLAGS := -g -ggdb3 
OPTIM_FLAGS := -O2 -ftree-vectorize -ffast-math -funsafe-math-optimizations -fsingle-precision-constant

include $(call all-subdir-makefiles)


