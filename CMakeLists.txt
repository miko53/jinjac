
project(jinjac)

cmake_minimum_required(VERSION 2.8.8)

find_package(BISON REQUIRED)
find_package(FLEX REQUIRED)

find_program( MEMORYCHECK_COMMAND valgrind )
set( MEMORYCHECK_COMMAND_OPTIONS "--leak-check=full" )

enable_testing()

set(CMAKE_BUILD_TYPE "Debug")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -O0")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -O2")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=gnu99 -D_GNU_SOURCE -W -Wall -Wno-aggregate-return -Wno-suggest-attribute=format -Wno-undef -fms-extensions -fstack-protector-all -Wstack-protector -fno-omit-frame-pointer")

add_subdirectory(libjinjac)
add_subdirectory(jinjac_test_app)
add_subdirectory(test)
