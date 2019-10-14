include_directories(${LIBJINJAC_INCLUDE_DIR})

add_executable(example_jinjac example.c)
target_link_libraries(example_jinjac libjinjac_static)

