
project(jinjac)

cmake_minimum_required(VERSION 2.8.8)

find_package(BISON REQUIRED)
find_package(FLEX REQUIRED)

find_program( MEMORYCHECK_COMMAND valgrind )
set( MEMORYCHECK_COMMAND_OPTIONS "--leak-check=full" )

option(COVERAGE "to active coverage" OFF)
option(TRACE "to active trace" OFF)

if (${CMAKE_BUILD_TYPE} MATCHES "Debug")
  set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DJINJAC_DEBUG") 
  set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DJINJAC_DEBUG") 
endif() 

if (TRACE)
  set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DTRACE") 
  set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DTRACE") 
endif() 

message(STATUS "COVERAGE: ${COVERAGE}")
message(STATUS "TRACE: ${TRACE}")

set (COVERAGE_OPTIONS "--coverage -UJINJAC_DEBUG")

include (CTest)
enable_testing()

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=gnu99 -D_GNU_SOURCE -W -Wall -Wno-aggregate-return -Wno-suggest-attribute=format -Wno-undef -fms-extensions -fstack-protector-all -Wstack-protector -fno-omit-frame-pointer")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -W -Wall -fexceptions -fstack-protector-strong")

if (COVERAGE)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${COVERAGE_OPTIONS}")
endif()

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 ")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O2")

include_directories(include)

add_subdirectory(jinjacc)
add_subdirectory(jinjac_vm)
add_subdirectory(tests)
