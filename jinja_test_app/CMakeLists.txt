
include_directories(${LIBJINJAC_INCLUDE_DIR})

add_executable(jinja_test jinja_test_app.c)
target_link_libraries(jinja_test libjinjac_static)
