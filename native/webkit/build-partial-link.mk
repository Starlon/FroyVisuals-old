__object_files := \
         $(addprefix $(TARGET_OBJS)/$(patsubst lib%,%,$(LOCAL_MODULE))/,\
         $(patsubst %.cpp,%.o,\
         $(patsubst %.c,%.o,$(LOCAL_SRC_FILES))))

__ofile := $(TARGET_OBJS)/ndkwebkit/ofiles/$(LOCAL_MODULE)_o.o
__input := $(TARGET_OBJS)/ndkwebkit/ofiles/$(LOCAL_MODULE)_o.input
$(shell echo -r > $(__input))
$(shell echo -o >> $(__input))
$(shell echo $(__ofile) >> $(__input))
$(foreach i,$(__object_files),\
	$(shell echo $(i) >> $(__input)))
$(__ofile): $(__input) $(__object_files)
	@ echo Creating .o file $@
	@ mkdir -p $(dir $@)
	@ arm-linux-androideabi-ld @$<
