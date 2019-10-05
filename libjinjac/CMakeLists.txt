
set(LIBJINJAC_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include CACHE INTERNAL "jinjac include dir")

include_directories(${LIBJINJAC_INCLUDE_DIR})

add_subdirectory(src)
