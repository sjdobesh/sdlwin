cmake_minimum_required(VERSION 3.7)
project(sdl_pxl)

# append sdl2
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/sdl2)

# GoogleTest requires at least C++11
set(CMAKE_CXX_STANDARD 11)

set(CMAKE_EXPORT_COMMANDS ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)
set(CMAKE_BUILD_TYPE Debug)

##
### Test definitions ###
##

# fuck tests, live on the edge
# configure_file(CMakeLists.txt.in
#         googletest-download/CMakeLists.txt)
# execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
#         WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/googletest-download )
# execute_process(COMMAND ${CMAKE_COMMAND} --build .
#         WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/googletest-download )

# add_subdirectory(${CMAKE_BINARY_DIR}/googletest-src
#         ${CMAKE_BINARY_DIR}/googletest-build)

# enable_testing()
# add_subdirectory(test)

##
### Source definitions ###
##

include_directories("${PROJECT_SOURCE_DIR}/include")

file(GLOB sources
  "${PROJECT_SOURCE_DIR}/include/*.h"
  "${PROJECT_SOURCE_DIR}/src/*.c")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../bin)

find_package(SDL2   REQUIRED)
find_package(SDL2_image   REQUIRED)
find_package(SDL2_ttf   REQUIRED)


add_executable(sdl_pxl ${sources})
target_link_libraries(sdl_pxl SDL2::Main SDL2_image SDL2_ttf pngz png m -static-libgcc -static-libstdc++)
