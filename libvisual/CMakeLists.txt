CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
MESSAGE(STATUS "${CMAKE_SOURCE_DIR}")

SET(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

PROJECT(libvisual)

# Version numbers
SET(LV_VERSION_MAJOR  0)
SET(LV_VERSION_MINOR  5)
SET(LV_VERSION_MICRO  0)
SET(LV_VERSION        "${LV_VERSION_MAJOR}.${LV_VERSION_MINOR}.${LV_VERSION_MICRO}")
SET(LV_VERSION_SUFFIX "${LV_VERSION_MAJOR}.${LV_VERSION_MINOR}")

# Package information
SET(PACKAGE           "libvisual")
SET(PACKAGE_VERSION   "${LV_VERSION}")
SET(PACKAGE_BUGREPORT "http://libvisual.sf.net")
SET(PACKAGE_NAME      "${PACKAGE}")
SET(PACKAGE_STRING    "${PACKAGE}-${PACKAGE_VERSION}")
SET(PACKAGE_TARNAME   "${PACKAGE}-${PACKAGE_VERSION}")
SET(PACKAGE_URL       "http://libvisual.sf.net")

# Installation Paths
INCLUDE(GNUInstallDirs)
SET(LV_INCLUDE_DIR      "${CMAKE_INSTALL_INCLUDEDIR}/libvisual-${LV_VERSION_SUFFIX}")
SET(LV_INCLUDE_DIR_FULL "${CMAKE_INSTALL_FULL_INCLUDEDIR}/libvisual-${LV_VERSION_SUFFIX}")
SET(LV_PLUGIN_DIR       "${CMAKE_INSTALL_LIBDIR}/libvisual-${LV_VERSION_SUFFIX}")
SET(LV_PLUGIN_DIR_FULL  "${CMAKE_INSTALL_FULL_LIBDIR}/libvisual-${LV_VERSION_SUFFIX}")

# Include basic CMake modules
INCLUDE(CheckFunctionExists)
INCLUDE(CheckIncludeFile)
INCLUDE(CheckIncludeFiles)
INCLUDE(CheckLibraryExists)
INCLUDE(CheckTypeSize)
INCLUDE(TestBigEndian)
FIND_PACKAGE(PkgConfig REQUIRED)

# Check target processor architecture
IF(CMAKE_SYSTEM_PROCESSOR MATCHES "^(i.86|x86)$")
  SET(VISUAL_ARCH_X86 yes)
ELSEIF(CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64")
  SET(VISUAL_ARCH_X86_64 yes)
ELSEIF(CMAKE_SYSTEM_PROCESSOR MATCHES "^mips")
  SET(VISUAL_ARCH_MIPS yes)
ELSEIF(CMAKE_SYSTEM_PROCESSOR MATCHES "^alpha")
  SET(VISUAL_ARCH_ALPHA yes)
ELSEIF(CMAKE_SYSTEM_PROCESSOR MATCHES "^sparc")
  SET(VISUAL_ARCH_SPARC yes)
ELSEIF(CMAKE_SYSTEM_PROCESSOR MATCHES "^(powerpc|ppc)")
  SET(VISUAL_ARCH_POWERPC yes)
ELSE()
  SET(VISUAL_ARCH_UNKNOWN yes)
ENDIF()

# Check endianness
TEST_BIG_ENDIAN(IS_BIG_ENDIAN)
IF(IS_BIG_ENDIAN)
  SET(VISUAL_BIG_ENDIAN 1)
  SET(VISUAL_LITTLE_ENDIAN 0)
ELSE(IS_BIG_ENDIAN)
  SET(VISUAL_BIG_ENDIAN 0)
  SET(VISUAL_LITTLE_ENDIAN 1)
ENDIF(IS_BIG_ENDIAN)

# Check target platform
IF(CMAKE_SYSTEM_NAME MATCHES "Linux")
  SET(VISUAL_OS_LINUX yes)
ELSEIF(CMAKE_SYSTEM_NAME MATCHES "(FreeBSD|DragonFly)")
  SET(VISUAL_OS_FREEBSD yes)
ELSEIF(CMAKE_SYSTEM_NAME MATCHES "NetBSD")
  SET(VISUAL_OS_NETBSD yes)
ELSEIF(CMAKE_SYSTEM_NAME MATCHES "OpenBSD")
  SET(VISUAL_OS_OPENBSD yes)
ELSEIF(CMAKE_SYSTEM_NAME MATCHES "SunOS")
  SET(VISUAL_OS_SOLARIS yes)
  SET(VISUAL_OS_SUNOS yes)
ELSEIF(CMAKE_SYSTEM_NAME MATCHES "HP-UX")
  SET(VISUAL_OS_HPUX yes)
ELSEIF(CMAKE_SYSTEM_NAME MATCHES "AIX")
  SET(VISUAL_OS_AIX yes)
ELSEIF(CMAKE_SYSTEM_NAME MATCHES "BeOS")
  SET(VISUAL_OS_BEOS yes)
ELSEIF(CMAKE_SYSTEM_NAME MATCHES "IRIX")
  SET(VISUAL_OS_IRIX yes)
ELSEIF(CMAKE_SYSTEM_NAME MATCHES "Darwin")
  SET(VISUAL_OS_DARWIN yes)
ELSEIF(CMAKE_SYSTEM_NAME MATCHES "OSF")
  SET(VISUAL_OS_OSF yes)
ELSEIF(CMAKE_SYSTEM_NAME MATCHES "SCO")
  SET(VISUAL_OS_SCO yes)
ELSEIF(WIN32)
  SET(VISUAL_OS_WIN32 yes)
ELSE()
  SET(VISUAL_OS_UNKNOWN yes)
ENDIF()

SET(VISUAL_WITH_CYGWIN ${CYGWIN})
SET(VISUAL_WITH_MINGW  ${MINGW})

# Check for typedefs, structures, and compiler characteristics
INCLUDE(CheckCCompiler)
CHECK_C_COMPILER_SUPPORTS_CONST(HAVE_C_CONST)
CHECK_C_COMPILER_SUPPORTS_INLINE(INLINE HAVE_C_INLINE)
INCLUDE(CheckForVarArgs)
CHECK_FOR_ISO_C_VARARGS(LV_HAVE_ISO_C_VARARGS)
CHECK_FOR_GNU_C_VARARGS(LV_HAVE_GNU_C_VARARGS)

# Check for standard C library headers
INCLUDE(CheckStdCHeaders)
IF(NOT STDC_HEADERS)
  MESSAGE(FATAL ERROR "Libvisual requires standard C library headers to build")
ENDIF(NOT STDC_HEADERS)

# TODO: Generate error when a required header is missing
CHECK_INCLUDE_FILE(dirent.h     HAVE_DIRENT_H)
CHECK_INCLUDE_FILE(fcntl.h      HAVE_FCNTL_H)
CHECK_INCLUDE_FILE(sched.h      HAVE_SCHED_H)
CHECK_INCLUDE_FILE(sys/stat.h   HAVE_SYS_STAT_H)
CHECK_INCLUDE_FILE(sys/sched.h  HAVE_SYS_SCHED_H)
CHECK_INCLUDE_FILE(sys/socket.h HAVE_SYS_SOCKET_H)
CHECK_INCLUDE_FILE(sys/time.h   HAVE_SYS_TIME_H)
CHECK_INCLUDE_FILE(stdint.h     _LV_HAVE_STDINT_H)
CHECK_INCLUDE_FILE(inttypes.h   _LV_HAVE_INTTYPES_H)

# Check for non standard C library functions
CHECK_FUNCTION_EXISTS(gettimeofday HAVE_GETTIMEOFDAY)
CHECK_FUNCTION_EXISTS(usleep HAVE_USLEEP)
CHECK_FUNCTION_EXISTS(nanosleep HAVE_NANOSLEEP)
CHECK_FUNCTION_EXISTS(strdup HAVE_STRDUP)
CHECK_FUNCTION_EXISTS(strndup HAVE_STRNDUP)
CHECK_FUNCTION_EXISTS(sysconf HAVE_SYSCONF)
CHECK_FUNCTION_EXISTS(select HAVE_SELECT)
# TODO: Translate AC_FUNC_SELECT_ARGTYPES

# Check for dynamic linking library
CHECK_LIBRARY_EXISTS(dl dlsym "" HAVE_DL)
IF(NOT HAVE_DL)
  IF(NOT VISUAL_OS_WIN32)
    MESSAGE(FATAL_ERROR "Libvisual requires dlfcn to build")
  ENDIF(NOT VISUAL_OS_WIN32)
ENDIF(NOT HAVE_DL)

# Check for standard math library
CHECK_LIBRARY_EXISTS(m sqrt "" HAVE_STDC_MATH)
IF(NOT HAVE_STDC_MATH)
  MESSAGE(FATAL_ERROR "Libvisual requires libm to build")
ENDIF(NOT HAVE_STDC_MATH)
SET(HAVE_SQRT 1)

# Internationalization
SET(GETTEXT_PACKAGE "libvisual-${LV_VERSION_SUFFIX}")
OPTION(ENABLE_NLS "Enable localization" no)
IF(ENABLE_NLS)
  FIND_PACKAGE(Gettext REQUIRED)
  SET(HAVE_GETTEXT ${GETTEXT_FOUND})
  ADD_SUBDIRECTORY(po)
ENDIF(ENABLE_NLS)

# Check for thread implementation
OPTION(ENABLE_THREADS "Turn on basic thread support" yes)
IF(ENABLE_THREADS)
  FIND_PACKAGE(Threads)
  IF(Threads_FOUND)
	SET(VISUAL_HAVE_THREADS yes)
    SET(THREAD_INCLUDE_DIRS "")
    SET(THREAD_LIBS ${CMAKE_THREAD_LIBS_INIT})

	IF(CMAKE_USE_PTHREADS_INIT)
      SET(VISUAL_THREAD_MODEL_POSIX yes)
    ELSEIF(CMAKE_USE_WIN32_THREADS_INIT)
      SET(VISUAL_THREAD_MODEL_WIN32 yes)
    ENDIF(CMAKE_USE_PTHREADS_INIT)
  ELSE(Threads_FOUND)
    PKG_CHECK_MODULES(GTHREAD2 gthread-2.0)
    IF(GTHREAD2_FOUND)
      SET(VISUAL_HAVE_THREADS yes)
      SET(VISUAL_THREAD_MODEL_GTHREAD2 yes)
      SET(THREAD_INCLUDE_DIRS ${GTHREAD2_INCLUDE_DIRS})
      SET(THREAD_LIBS ${GTHREAD2_LIBRARIES})
    ELSE(GTHREAD2_FOUND)
	  MESSAGE(WARNING "You do not have any supported thread implementation available. Libvisual will be built without thread support.")
    ENDIF(GTHREAD2_FOUND)
  ENDIF(Threads_FOUND)
ENDIF(ENABLE_THREADS)

# Check for type sizes
CHECK_TYPE_SIZE(short  SIZEOF_SHORT)
CHECK_TYPE_SIZE(int    SIZEOF_INT)
CHECK_TYPE_SIZE(long   SIZEOF_LONG)
CHECK_TYPE_SIZE(size_t SIZEOF_SIZE_T)

# Try to figure out whether visual_size should be long or int
MESSAGE(STATUS "Checking for the appropriate definition for size_t")
IF(SIZEOF_SIZE_T EQUAL SIZEOF_SHORT)
  SET(VISUAL_SIZE_TYPE short)
  SET(VISUAL_SIZE_T_FORMAT "hu")
ELSEIF(SIZEOF_SIZE_T EQUAL SIZEOF_INT)
  SET(VISUAL_SIZE_TYPE int)
  SET(VISUAL_SIZE_T_FORMAT "u")
ELSEIF(SIZEOF_SIZE_T EQUAL SIZEOF_LONG)
  SET(VISUAL_SIZE_TYPE long)
  SET(VISUAL_SIZE_T_FORMAT "lu")
ELSE(SIZEOF_SIZE_T EQUAL SIZEOF_SHORT)
  MESSAGE(FATAL_ERROR "No type matching size_t in size")
ENDIF(SIZEOF_SIZE_T EQUAL SIZEOF_SHORT)
MESSAGE(STATUS "Size of size_t is ${VISUAL_SIZE_TYPE}")

# Build profiles

OPTION(ENABLE_PROFILING "Enable profiling" no)
OPTION(ENABLE_EXTRA_OPTIMIZATIONS "Enable extra optimizations" no)

IF(ENABLE_EXTRA_OPTIMIZATIONS)
  SET(OPT_C_FLAGS "-O3 -fexpensive-optimizations")
ELSEIF(CMAKE_BUILD_TYPE STREQUAL Debug)
  SET(OPT_C_FLAGS "-O0")
ENDIF(ENABLE_EXTRA_OPTIMIZATIONS)

IF(ENABLE_PROFILING)
  SET(PROFILE_C_FLAGS "${EXTRA_CFLAGS} -pg")
ENDIF(ENABLE_PROFILING)

# Add -Werror to debug builds?
SET(CMAKE_C_FLAGS "${OPT_C_FLAGS} ${PROFILE_C_FLAGS} -std=c99 -Wall -Wmissing-prototypes -Wunused")
SET(CMAKE_C_FLAGS_DEBUG "-ggdb3")

OPTION(ENABLE_FAST_FP_RNG "Enable faster random floating point generator" ${ENABLE_EXTRA_OPTIMIZATIONS})
SET(VISUAL_RANDOM_FAST_FP_RNG ${ENABLE_FAST_FP_RNG})

# API Documentation
OPTION(ENABLE_DOCS "Enable the creation of API documentation" no)
IF(ENABLE_DOCS)
  FIND_PACKAGE(Doxygen)
  IF(DOXYGEN_FOUND)
    IF(NOT DOXYGEN_DOT_FOUND)
      MESSAGE(WARNING "Graphviz Dot tool not found. API documentation will not have diagrams")
    ENDIF(NOT DOXYGEN_DOT_FOUND)
    CONFIGURE_FILE(${PROJECT_SOURCE_DIR}/Doxyfile.in ${PROJECT_BINARY_DIR}/Doxyfile)
    # FIXME/TODO: Figure out how to generate the docs in the first make run
    ADD_CUSTOM_TARGET(docs
      COMMAND ${DOXYGEN_EXECUTABLE} ${PROJECT_BINARY_DIR}/Doxyfile
      COMMENT "Generating API documentation.."
      VERBATIM
    )
  ELSE(DOXYGEN_FOUND)
    MESSAGE(WARNING "Doxygen not found. No API documentation will be generated")
    SET(ENABLE_DOCS no)
  ENDIF(DOXYGEN_FOUND)
ENDIF(ENABLE_DOCS)

# Pedantic checks
OPTION(ENABLE_PEDANTIC_CHECKS "Enable pedantic checks (program immediately aborts if errors occur)" no)
IF(ENABLE_PEDANTIC_CHECKS)
  ADD_DEFINITIONS(-DVISUAL_PEDANTIC_CHECKS)
ENDIF(ENABLE_PEDANTIC_CHECKS)

# Tools
OPTION(ENABLE_TOOLS "Build Libvisual tools" yes)
IF(ENABLE_TOOLS)
  FIND_PACKAGE(SDL)
  FIND_PACKAGE(OpenGL)
  FIND_PACKAGE(X11Fixed)

  SET(HAVE_SDL ${SDL_FOUND})
  SET(HAVE_GL ${OPENGL_FOUND})
  SET(HAVE_X11 ${X11_FOUND})

  IF(X11_FOUND AND X11_xf86vmode_FOUND AND OPENGL_FOUND)
	SET(HAVE_GLX yes)
  ENDIF()

  ADD_SUBDIRECTORY(tools/lv-tool)
  ADD_SUBDIRECTORY(tools/benchmarks)
ENDIF(ENABLE_TOOLS)

# Generate pkg-config file
SET(LV_PKG_CONFIG_NAME   "libvisual-${LV_VERSION_SUFFIX}")
SET(LV_PKG_CONFIG_LIBS   "-lvisual-${LV_VERSION_SUFFIX} ${CMAKE_THREAD_LIBS_INIT}")
SET(LV_PKG_CONFIG_CFLAGS "")
SET(LV_PKG_CONFIG_DEPS   "")
IF(VISUAL_THREAD_MODEL_GTHREAD2)
  SET(LV_PKG_CONFIG_DEPS "gthread-2.0")
ENDIF(VISUAL_THREAD_MODEL_GTHREAD2)
CONFIGURE_FILE(${PROJECT_SOURCE_DIR}/libvisual.pc.in ${PROJECT_BINARY_DIR}/${LV_PKG_CONFIG_NAME}.pc @ONLY)
INSTALL(FILES ${PROJECT_BINARY_DIR}/${LV_PKG_CONFIG_NAME}.pc DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig)

# Generate config headers
CONFIGURE_FILE(${PROJECT_SOURCE_DIR}/config.h.in ${PROJECT_BINARY_DIR}/config.h)
CONFIGURE_FILE(${PROJECT_SOURCE_DIR}/libvisual/lvconfig.h.in ${PROJECT_BINARY_DIR}/libvisual/lvconfig.h)

# Build libvisual
ADD_SUBDIRECTORY(libvisual)

# Uninstallation (script copied from CMake FAQ)

CONFIGURE_FILE(
  "${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
  IMMEDIATE @ONLY
)

ADD_CUSTOM_TARGET(uninstall
  "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
)

# Packaging

SET(CPACK_PACKAGE_VERSION_MAJOR ${LV_VERSION_MAJOR})
SET(CPACK_PACKAGE_VERSION_MINOR ${LV_VERSION_MINOR})
SET(CPACK_PACKAGE_VERSION_PATCH ${LV_VERSION_MICRO})
SET(CPACK_RESOURCE_FILE_LICENSE ${PROJECT_SOURCE_DIR}/COPYING)
SET(CPACK_RESOURCE_FILE_README  ${PROJECT_SOURCE_DIR}/README)

SET(CPACK_GENERATOR TGZ TBZ2)
SET(CPACK_SOURCE_GENERATOR TGZ TBZ2)

SET(CPACK_SOURCE_IGNORE_FILES
  /\\\\.git/
  /\\\\.gitignore$
  /_CPack_Packages/
  /CMakeCache.txt$
  CMakeFiles/
  /CPackConfig.cmake$
  /CPackSourceConfig.cmake$
  cmake_install.cmake$
  cmake_uninstall.cmake$
  Makefile$
  /install_manifest.txt$
  \\\\.service$
  \\\\.desktop$
  \\\\.pc$
  \\\\.o$
  \\\\.a$
  \\\\.so$
  \\\\.so\\\\..*$
  \\\\.mo$
  \\\\.tar\\\\.gz$
  \\\\.tar\\\\.bz2$
  ~$
)

INCLUDE(CPack)
