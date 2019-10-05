
include_directories(${LIBJINJAC_INCLUDE_DIR})

add_executable(jinja_expression jinja_expression.c)
target_link_libraries(jinja_expression libjinjac_static)
