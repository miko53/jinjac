
project(jinjac)

cmake_minimum_required(VERSION 2.8.8)

find_package(BISON REQUIRED)
find_package(FLEX REQUIRED)

find_program( MEMORYCHECK_COMMAND valgrind )
set( MEMORYCHECK_COMMAND_OPTIONS "--leak-check=full" )

option(COVERAGE "to active coverage" OFF)
option(TRACE "to active trace" OFF)

if (${CMAKE_BUILD_TYPE} MATCHES "Debug")
  set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DJINJAC_DEBUG") 
endif() 

if (TRACE)
  set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DTRACE") 
endif() 

message(STATUS "COVERAGE: ${COVERAGE}")
message(STATUS "TRACE: ${TRACE}")

set (COVERAGE_OPTIONS "--coverage -UJINJAC_DEBUG")

enable_testing()

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=gnu99 -D_GNU_SOURCE -W -Wall -Wno-aggregate-return -Wno-suggest-attribute=format -Wno-undef -fms-extensions -fstack-protector-all -Wstack-protector -fno-omit-frame-pointer")

if (COVERAGE)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${COVERAGE_OPTIONS}")
endif()

set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -O0 ")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -O2")

add_subdirectory(libjinjac)
add_subdirectory(jinjac_test_app)
add_subdirectory(test)
