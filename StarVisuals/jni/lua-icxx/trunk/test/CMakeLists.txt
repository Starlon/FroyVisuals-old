add_executable(test-lua-icxx main.cpp)
include_directories(
    ../src
    "/usr/include/lua5.1"
)
target_link_libraries(test-lua-icxx lua-icxx liblua5.1.so)


#set(ENV{CTEST_OUTPUT_ON_FAILURE} 1)
add_test(NAME test-lib 
         COMMAND ${CMAKE_COMMAND} -E chdir ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}/test-lua-icxx
)
