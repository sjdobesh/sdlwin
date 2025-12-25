include_directories("${PROJECT_SOURCE_DIR}/include")

set(CMAKE_BUILD_TYPE Debug)

file(GLOB sources "${PROJECT_SOURCE_DIR}/src/*.c")
list(REMOVE_ITEM sources "${PROJECT_SOURCE_DIR}/src/main.c")

file(GLOB tests "${PROJECT_SOURCE_DIR}/test/*.cpp")
list(REMOVE_ITEM tests "${PROJECT_SOURCE_DIR}/test/main.cpp")

foreach(file ${tests})
  set(name)
  get_filename_component(name ${file} NAME_WE)
  add_executable("${name}_tests"
    ${sources}
    ${file}
    "${PROJECT_SOURCE_DIR}/test/main.cpp")
  find_package(SDL2 REQUIRED)
  target_link_libraries("${name}_tests" gtest_main SDL2::Main m -static-libgcc -static-libstdc++)
  add_test(NAME ${name} COMMAND "${name}_tests")
endforeach()
