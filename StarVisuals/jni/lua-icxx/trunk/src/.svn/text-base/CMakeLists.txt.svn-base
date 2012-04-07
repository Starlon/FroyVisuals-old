
# build:
file(GLOB srcfiles "*.cpp")
include_directories("/usr/include/lua5.1")
add_library(lua-icxx SHARED ${srcfiles})
set_target_properties(lua-icxx PROPERTIES SOVERSION 1.0.2)

# install:
install(DIRECTORY .
        DESTINATION "include/lua-icxx"
        FILES_MATCHING PATTERN "*.h"
                       PATTERN ".svn" EXCLUDE
       )
install(TARGETS lua-icxx LIBRARY 
        DESTINATION lib)

# package:
set(CPACK_PACKAGE_DESCRIPTION "Lua-icxx library")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Lua interpreter for C++ apps")
set(CPACK_PACKAGE_NAME "lua-icxx")
set(CPACK_DEBIAN_PACKAGE_DEPENDS "liblua5.1 (>= 5.1)")

set(CPACK_PACKAGE_CONTACT "Oliver Schoenborn")
set(CPACK_PACKAGE_VERSION_MAJOR "1")
set(CPACK_PACKAGE_VERSION_MINOR "0")
set(CPACK_PACKAGE_VERSION_PATCH "2")

set(CPACK_GENERATOR "DEB;RPM;")

include(CPack)
