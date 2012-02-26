
WARNING_FLAGS := -Wall -Wstrict-aliasing -Wcast-align -Waddress -Wmissing-braces -Wimplicit -Wunused -Wno-unused-variable
DEBUG_FLAGS := -g -ggdb3
OPTIM_FLAGS := -O2

include $(call all-subdir-makefiles)


