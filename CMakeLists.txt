cmake_minimum_required(VERSION 3.16)

project(parg C)

include(CTest)

if(MSVC)
  add_compile_options(/W3)
elseif(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
  add_compile_options(-Wall -Wextra -pedantic)
endif()

add_library(parg parg.c parg.h)
target_include_directories(parg PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}>)

if(BUILD_TESTING)
  add_executable(test_parg test/test_parg.c)
  target_link_libraries(test_parg PRIVATE parg)
  if(MSVC)
    target_compile_definitions(test_parg PRIVATE _CRT_SECURE_NO_WARNINGS)
  endif()

  add_test(test_parg test_parg)
endif()
