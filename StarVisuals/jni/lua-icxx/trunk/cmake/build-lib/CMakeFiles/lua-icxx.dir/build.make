# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 2.8

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canoncical targets will work.
.SUFFIXES:

# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list

# Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/scott/workspace/StarVisuals/StarVisuals/jni/lua-icxx/trunk/cmake

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/scott/workspace/StarVisuals/StarVisuals/jni/lua-icxx/trunk/cmake

# Include any dependencies generated for this target.
include build-lib/CMakeFiles/lua-icxx.dir/depend.make

# Include the progress variables for this target.
include build-lib/CMakeFiles/lua-icxx.dir/progress.make

# Include the compile flags for this target's objects.
include build-lib/CMakeFiles/lua-icxx.dir/flags.make

build-lib/CMakeFiles/lua-icxx.dir/LuaInterpreter.cpp.o: build-lib/CMakeFiles/lua-icxx.dir/flags.make
build-lib/CMakeFiles/lua-icxx.dir/LuaInterpreter.cpp.o: /home/scott/workspace/StarVisuals/StarVisuals/jni/lua-icxx/trunk/src/LuaInterpreter.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/scott/workspace/StarVisuals/StarVisuals/jni/lua-icxx/trunk/cmake/CMakeFiles $(CMAKE_PROGRESS_1)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object build-lib/CMakeFiles/lua-icxx.dir/LuaInterpreter.cpp.o"
	cd /home/scott/workspace/StarVisuals/StarVisuals/jni/lua-icxx/trunk/cmake/build-lib && /usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/lua-icxx.dir/LuaInterpreter.cpp.o -c /home/scott/workspace/StarVisuals/StarVisuals/jni/lua-icxx/trunk/src/LuaInterpreter.cpp

build-lib/CMakeFiles/lua-icxx.dir/LuaInterpreter.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/lua-icxx.dir/LuaInterpreter.cpp.i"
	cd /home/scott/workspace/StarVisuals/StarVisuals/jni/lua-icxx/trunk/cmake/build-lib && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/scott/workspace/StarVisuals/StarVisuals/jni/lua-icxx/trunk/src/LuaInterpreter.cpp > CMakeFiles/lua-icxx.dir/LuaInterpreter.cpp.i

build-lib/CMakeFiles/lua-icxx.dir/LuaInterpreter.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/lua-icxx.dir/LuaInterpreter.cpp.s"
	cd /home/scott/workspace/StarVisuals/StarVisuals/jni/lua-icxx/trunk/cmake/build-lib && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/scott/workspace/StarVisuals/StarVisuals/jni/lua-icxx/trunk/src/LuaInterpreter.cpp -o CMakeFiles/lua-icxx.dir/LuaInterpreter.cpp.s

build-lib/CMakeFiles/lua-icxx.dir/LuaInterpreter.cpp.o.requires:
.PHONY : build-lib/CMakeFiles/lua-icxx.dir/LuaInterpreter.cpp.o.requires

build-lib/CMakeFiles/lua-icxx.dir/LuaInterpreter.cpp.o.provides: build-lib/CMakeFiles/lua-icxx.dir/LuaInterpreter.cpp.o.requires
	$(MAKE) -f build-lib/CMakeFiles/lua-icxx.dir/build.make build-lib/CMakeFiles/lua-icxx.dir/LuaInterpreter.cpp.o.provides.build
.PHONY : build-lib/CMakeFiles/lua-icxx.dir/LuaInterpreter.cpp.o.provides

build-lib/CMakeFiles/lua-icxx.dir/LuaInterpreter.cpp.o.provides.build: build-lib/CMakeFiles/lua-icxx.dir/LuaInterpreter.cpp.o

build-lib/CMakeFiles/lua-icxx.dir/LuaFuncRef.cpp.o: build-lib/CMakeFiles/lua-icxx.dir/flags.make
build-lib/CMakeFiles/lua-icxx.dir/LuaFuncRef.cpp.o: /home/scott/workspace/StarVisuals/StarVisuals/jni/lua-icxx/trunk/src/LuaFuncRef.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/scott/workspace/StarVisuals/StarVisuals/jni/lua-icxx/trunk/cmake/CMakeFiles $(CMAKE_PROGRESS_2)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object build-lib/CMakeFiles/lua-icxx.dir/LuaFuncRef.cpp.o"
	cd /home/scott/workspace/StarVisuals/StarVisuals/jni/lua-icxx/trunk/cmake/build-lib && /usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/lua-icxx.dir/LuaFuncRef.cpp.o -c /home/scott/workspace/StarVisuals/StarVisuals/jni/lua-icxx/trunk/src/LuaFuncRef.cpp

build-lib/CMakeFiles/lua-icxx.dir/LuaFuncRef.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/lua-icxx.dir/LuaFuncRef.cpp.i"
	cd /home/scott/workspace/StarVisuals/StarVisuals/jni/lua-icxx/trunk/cmake/build-lib && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/scott/workspace/StarVisuals/StarVisuals/jni/lua-icxx/trunk/src/LuaFuncRef.cpp > CMakeFiles/lua-icxx.dir/LuaFuncRef.cpp.i

build-lib/CMakeFiles/lua-icxx.dir/LuaFuncRef.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/lua-icxx.dir/LuaFuncRef.cpp.s"
	cd /home/scott/workspace/StarVisuals/StarVisuals/jni/lua-icxx/trunk/cmake/build-lib && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/scott/workspace/StarVisuals/StarVisuals/jni/lua-icxx/trunk/src/LuaFuncRef.cpp -o CMakeFiles/lua-icxx.dir/LuaFuncRef.cpp.s

build-lib/CMakeFiles/lua-icxx.dir/LuaFuncRef.cpp.o.requires:
.PHONY : build-lib/CMakeFiles/lua-icxx.dir/LuaFuncRef.cpp.o.requires

build-lib/CMakeFiles/lua-icxx.dir/LuaFuncRef.cpp.o.provides: build-lib/CMakeFiles/lua-icxx.dir/LuaFuncRef.cpp.o.requires
	$(MAKE) -f build-lib/CMakeFiles/lua-icxx.dir/build.make build-lib/CMakeFiles/lua-icxx.dir/LuaFuncRef.cpp.o.provides.build
.PHONY : build-lib/CMakeFiles/lua-icxx.dir/LuaFuncRef.cpp.o.provides

build-lib/CMakeFiles/lua-icxx.dir/LuaFuncRef.cpp.o.provides.build: build-lib/CMakeFiles/lua-icxx.dir/LuaFuncRef.cpp.o

build-lib/CMakeFiles/lua-icxx.dir/LuaObjRef.cpp.o: build-lib/CMakeFiles/lua-icxx.dir/flags.make
build-lib/CMakeFiles/lua-icxx.dir/LuaObjRef.cpp.o: /home/scott/workspace/StarVisuals/StarVisuals/jni/lua-icxx/trunk/src/LuaObjRef.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/scott/workspace/StarVisuals/StarVisuals/jni/lua-icxx/trunk/cmake/CMakeFiles $(CMAKE_PROGRESS_3)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object build-lib/CMakeFiles/lua-icxx.dir/LuaObjRef.cpp.o"
	cd /home/scott/workspace/StarVisuals/StarVisuals/jni/lua-icxx/trunk/cmake/build-lib && /usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/lua-icxx.dir/LuaObjRef.cpp.o -c /home/scott/workspace/StarVisuals/StarVisuals/jni/lua-icxx/trunk/src/LuaObjRef.cpp

build-lib/CMakeFiles/lua-icxx.dir/LuaObjRef.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/lua-icxx.dir/LuaObjRef.cpp.i"
	cd /home/scott/workspace/StarVisuals/StarVisuals/jni/lua-icxx/trunk/cmake/build-lib && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/scott/workspace/StarVisuals/StarVisuals/jni/lua-icxx/trunk/src/LuaObjRef.cpp > CMakeFiles/lua-icxx.dir/LuaObjRef.cpp.i

build-lib/CMakeFiles/lua-icxx.dir/LuaObjRef.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/lua-icxx.dir/LuaObjRef.cpp.s"
	cd /home/scott/workspace/StarVisuals/StarVisuals/jni/lua-icxx/trunk/cmake/build-lib && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/scott/workspace/StarVisuals/StarVisuals/jni/lua-icxx/trunk/src/LuaObjRef.cpp -o CMakeFiles/lua-icxx.dir/LuaObjRef.cpp.s

build-lib/CMakeFiles/lua-icxx.dir/LuaObjRef.cpp.o.requires:
.PHONY : build-lib/CMakeFiles/lua-icxx.dir/LuaObjRef.cpp.o.requires

build-lib/CMakeFiles/lua-icxx.dir/LuaObjRef.cpp.o.provides: build-lib/CMakeFiles/lua-icxx.dir/LuaObjRef.cpp.o.requires
	$(MAKE) -f build-lib/CMakeFiles/lua-icxx.dir/build.make build-lib/CMakeFiles/lua-icxx.dir/LuaObjRef.cpp.o.provides.build
.PHONY : build-lib/CMakeFiles/lua-icxx.dir/LuaObjRef.cpp.o.provides

build-lib/CMakeFiles/lua-icxx.dir/LuaObjRef.cpp.o.provides.build: build-lib/CMakeFiles/lua-icxx.dir/LuaObjRef.cpp.o

build-lib/CMakeFiles/lua-icxx.dir/LuaTempResult.cpp.o: build-lib/CMakeFiles/lua-icxx.dir/flags.make
build-lib/CMakeFiles/lua-icxx.dir/LuaTempResult.cpp.o: /home/scott/workspace/StarVisuals/StarVisuals/jni/lua-icxx/trunk/src/LuaTempResult.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/scott/workspace/StarVisuals/StarVisuals/jni/lua-icxx/trunk/cmake/CMakeFiles $(CMAKE_PROGRESS_4)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object build-lib/CMakeFiles/lua-icxx.dir/LuaTempResult.cpp.o"
	cd /home/scott/workspace/StarVisuals/StarVisuals/jni/lua-icxx/trunk/cmake/build-lib && /usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/lua-icxx.dir/LuaTempResult.cpp.o -c /home/scott/workspace/StarVisuals/StarVisuals/jni/lua-icxx/trunk/src/LuaTempResult.cpp

build-lib/CMakeFiles/lua-icxx.dir/LuaTempResult.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/lua-icxx.dir/LuaTempResult.cpp.i"
	cd /home/scott/workspace/StarVisuals/StarVisuals/jni/lua-icxx/trunk/cmake/build-lib && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/scott/workspace/StarVisuals/StarVisuals/jni/lua-icxx/trunk/src/LuaTempResult.cpp > CMakeFiles/lua-icxx.dir/LuaTempResult.cpp.i

build-lib/CMakeFiles/lua-icxx.dir/LuaTempResult.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/lua-icxx.dir/LuaTempResult.cpp.s"
	cd /home/scott/workspace/StarVisuals/StarVisuals/jni/lua-icxx/trunk/cmake/build-lib && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/scott/workspace/StarVisuals/StarVisuals/jni/lua-icxx/trunk/src/LuaTempResult.cpp -o CMakeFiles/lua-icxx.dir/LuaTempResult.cpp.s

build-lib/CMakeFiles/lua-icxx.dir/LuaTempResult.cpp.o.requires:
.PHONY : build-lib/CMakeFiles/lua-icxx.dir/LuaTempResult.cpp.o.requires

build-lib/CMakeFiles/lua-icxx.dir/LuaTempResult.cpp.o.provides: build-lib/CMakeFiles/lua-icxx.dir/LuaTempResult.cpp.o.requires
	$(MAKE) -f build-lib/CMakeFiles/lua-icxx.dir/build.make build-lib/CMakeFiles/lua-icxx.dir/LuaTempResult.cpp.o.provides.build
.PHONY : build-lib/CMakeFiles/lua-icxx.dir/LuaTempResult.cpp.o.provides

build-lib/CMakeFiles/lua-icxx.dir/LuaTempResult.cpp.o.provides.build: build-lib/CMakeFiles/lua-icxx.dir/LuaTempResult.cpp.o

# Object files for target lua-icxx
lua__icxx_OBJECTS = \
"CMakeFiles/lua-icxx.dir/LuaInterpreter.cpp.o" \
"CMakeFiles/lua-icxx.dir/LuaFuncRef.cpp.o" \
"CMakeFiles/lua-icxx.dir/LuaObjRef.cpp.o" \
"CMakeFiles/lua-icxx.dir/LuaTempResult.cpp.o"

# External object files for target lua-icxx
lua__icxx_EXTERNAL_OBJECTS =

build-lib/liblua-icxx.so.1.0.2: build-lib/CMakeFiles/lua-icxx.dir/LuaInterpreter.cpp.o
build-lib/liblua-icxx.so.1.0.2: build-lib/CMakeFiles/lua-icxx.dir/LuaFuncRef.cpp.o
build-lib/liblua-icxx.so.1.0.2: build-lib/CMakeFiles/lua-icxx.dir/LuaObjRef.cpp.o
build-lib/liblua-icxx.so.1.0.2: build-lib/CMakeFiles/lua-icxx.dir/LuaTempResult.cpp.o
build-lib/liblua-icxx.so.1.0.2: build-lib/CMakeFiles/lua-icxx.dir/build.make
build-lib/liblua-icxx.so.1.0.2: build-lib/CMakeFiles/lua-icxx.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --red --bold "Linking CXX shared library liblua-icxx.so"
	cd /home/scott/workspace/StarVisuals/StarVisuals/jni/lua-icxx/trunk/cmake/build-lib && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/lua-icxx.dir/link.txt --verbose=$(VERBOSE)
	cd /home/scott/workspace/StarVisuals/StarVisuals/jni/lua-icxx/trunk/cmake/build-lib && $(CMAKE_COMMAND) -E cmake_symlink_library liblua-icxx.so.1.0.2 liblua-icxx.so.1.0.2 liblua-icxx.so

build-lib/liblua-icxx.so: build-lib/liblua-icxx.so.1.0.2

# Rule to build all files generated by this target.
build-lib/CMakeFiles/lua-icxx.dir/build: build-lib/liblua-icxx.so
.PHONY : build-lib/CMakeFiles/lua-icxx.dir/build

build-lib/CMakeFiles/lua-icxx.dir/requires: build-lib/CMakeFiles/lua-icxx.dir/LuaInterpreter.cpp.o.requires
build-lib/CMakeFiles/lua-icxx.dir/requires: build-lib/CMakeFiles/lua-icxx.dir/LuaFuncRef.cpp.o.requires
build-lib/CMakeFiles/lua-icxx.dir/requires: build-lib/CMakeFiles/lua-icxx.dir/LuaObjRef.cpp.o.requires
build-lib/CMakeFiles/lua-icxx.dir/requires: build-lib/CMakeFiles/lua-icxx.dir/LuaTempResult.cpp.o.requires
.PHONY : build-lib/CMakeFiles/lua-icxx.dir/requires

build-lib/CMakeFiles/lua-icxx.dir/clean:
	cd /home/scott/workspace/StarVisuals/StarVisuals/jni/lua-icxx/trunk/cmake/build-lib && $(CMAKE_COMMAND) -P CMakeFiles/lua-icxx.dir/cmake_clean.cmake
.PHONY : build-lib/CMakeFiles/lua-icxx.dir/clean

build-lib/CMakeFiles/lua-icxx.dir/depend:
	cd /home/scott/workspace/StarVisuals/StarVisuals/jni/lua-icxx/trunk/cmake && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/scott/workspace/StarVisuals/StarVisuals/jni/lua-icxx/trunk/cmake /home/scott/workspace/StarVisuals/StarVisuals/jni/lua-icxx/trunk/src /home/scott/workspace/StarVisuals/StarVisuals/jni/lua-icxx/trunk/cmake /home/scott/workspace/StarVisuals/StarVisuals/jni/lua-icxx/trunk/cmake/build-lib /home/scott/workspace/StarVisuals/StarVisuals/jni/lua-icxx/trunk/cmake/build-lib/CMakeFiles/lua-icxx.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : build-lib/CMakeFiles/lua-icxx.dir/depend
