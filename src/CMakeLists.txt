
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

set(SRCS_JINJAC
      ast.c jinjac.c parameter.c 
      convert.c buildin.c jobject.c 
      jfunction.c str_obj.c
      ${BISON_JinjacBison_OUTPUTS} ${FLEX_JinjacFlex_OUTPUTS}
  )

add_executable(jinja_expression ${SRCS_JINJAC} )
target_link_libraries(jinja_expression fl)
