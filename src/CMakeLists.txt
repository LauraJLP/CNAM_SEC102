# CMakeLists files in this project can
# refer to the root source directory of the project as ${HELLO_SOURCE_DIR} and
# to the root binary directory of the project as ${HELLO_BINARY_DIR}.
cmake_minimum_required (VERSION 2.8.11)

project (TOPLEVEL)

SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/../bin) 

add_subdirectory (sample)
add_subdirectory (dirlist)
add_subdirectory (proclist)
add_subdirectory (rot13)
add_subdirectory (httpclient)
