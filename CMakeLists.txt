cmake_minimum_required(VERSION 2.8)

project(parg C)

include(CheckCCompilerFlag)
include(CTest)

option(BUILD_COVERAGE "Code coverage" OFF)

message(STATUS "CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")

if(MSVC)
  add_definitions(/D_CRT_SECURE_NO_WARNINGS)
  if(NOT MSVC_VERSION LESS 1700)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /analyze")
  endif()
elseif(CMAKE_COMPILER_IS_GNUCC)
  set(gcc_warning_flags
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
  foreach(flag ${gcc_warning_flags})
    string(REGEX REPLACE "[^a-zA-Z0-9]+" "_" flag_var "CFLAG_${flag}")
    CHECK_C_COMPILER_FLAG("${flag}" ${flag_var})
    if(${flag_var})
      set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${flag}")
    endif()
    unset(flag_var)
  endforeach()
  unset(gcc_warning_flags)
elseif(CMAKE_C_COMPILER_ID MATCHES "Clang")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Weverything -pedantic")
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
