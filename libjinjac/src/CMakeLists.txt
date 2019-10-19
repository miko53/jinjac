
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

if (BISON_FOUND)
BISON_TARGET(JinjacBison jinja_expression.y ${CMAKE_CURRENT_BINARY_DIR}/jinja_expression.tab.c
             COMPILE_FLAGS "-d")
endif()

if (FLEX_FOUND)
FLEX_TARGET(JinjacFlex jinja_expression.l ${CMAKE_CURRENT_BINARY_DIR}/lex.yy.c)
endif()

ADD_FLEX_BISON_DEPENDENCY(JinjacFlex JinjacBison)

set(LIBJINJAC_SRC_FILES
      ast.c jinjac_parse.c jinjac_stream.c parameter.c 
      convert.c buildin.c jobject.c jlist.c
      jfunction.c str_obj.c buffer.c
      trace.c block_statement.c
      ${BISON_JinjacBison_OUTPUTS} ${FLEX_JinjacFlex_OUTPUTS}
  )

add_library(libjinjac_shared SHARED ${LIBJINJAC_SRC_FILES})
add_library(libjinjac_static STATIC ${LIBJINJAC_SRC_FILES})
