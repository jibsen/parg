cmake_minimum_required(VERSION 2.8.6)

project(parg C)

include(CheckCCompilerFlag)
include(CTest)

option(BUILD_COVERAGE "Code coverage" OFF)

message(STATUS "CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")

function(_add_c_compiler_flags var)
  foreach(flag ${ARGN})
    string(REGEX REPLACE "[^a-zA-Z0-9]+" "_" flag_var "CFLAG_${flag}")
    check_c_compiler_flag("${flag}" ${flag_var})
    if(${flag_var})
      set(${var} "${${var}} ${flag}")
    endif()
  endforeach()
  set(${var} "${${var}}" PARENT_SCOPE)
endfunction()

if(MSVC)
  _add_c_compiler_flags(CMAKE_C_FLAGS "/analyze")
  add_definitions(/D_CRT_SECURE_NO_WARNINGS)
elseif(CMAKE_COMPILER_IS_GNUCC)
  _add_c_compiler_flags(CMAKE_C_FLAGS
    -Wall
    -Wextra
    -pedantic
    -Wshadow
    -Wpointer-arith
    -Wcast-qual
    -Wcast-align
    -Wstrict-prototypes
    -Wmissing-prototypes
  )
elseif(CMAKE_C_COMPILER_ID MATCHES "Clang")
  _add_c_compiler_flags(CMAKE_C_FLAGS "-Weverything;-pedantic")
endif()

if(BUILD_COVERAGE)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -O0 --coverage")
endif()

include_directories(${PROJECT_SOURCE_DIR})

add_library(parg parg.c parg.h)

if(BUILD_TESTING)
  add_executable(test_parg test/test_parg.c test/greatest.h parg.h)
  target_link_libraries(test_parg parg)

  enable_testing()
  add_test(test_parg test_parg)
endif()
