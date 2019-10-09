
include_directories(${LIBJINJAC_INCLUDE_DIR})

add_executable(jinjac_test jinjac_test_app.c)
target_link_libraries(jinjac_test libjinjac_static)
