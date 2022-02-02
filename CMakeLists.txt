cmake_minimum_required(VERSION 3.16)

project(parg C)

include(CTest)

if(MSVC)
  add_compile_options(/W3)
else()
  add_compile_options(-Wall -Wextra -pedantic)
endif()

add_library(parg parg.c parg.h)
target_include_directories(parg PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}>)

if(BUILD_TESTING)
  add_executable(test_parg test/test_parg.c)
  target_link_libraries(test_parg PRIVATE parg)

  add_test(test_parg test_parg)
endif()
