
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(ast)

if (BISON_FOUND)
BISON_TARGET(JinjacBison jinja_expression.y ${CMAKE_CURRENT_BINARY_DIR}/jinja_expression.tab.cpp
             COMPILE_FLAGS "-d")
endif()

if (FLEX_FOUND)
FLEX_TARGET(JinjacFlex jinja_expression.l ${CMAKE_CURRENT_BINARY_DIR}/lex.yy.cpp)
endif()

ADD_FLEX_BISON_DEPENDENCY(JinjacFlex JinjacBison)

set(JINJACC_SRCS jinjacc.cpp
                 ast/astnode.cpp
                 ast/ast.cpp
                 ast/asttext.cpp ast/aststatement.cpp ast/astvariable.cpp ast/astnumber.cpp ast/astboolean.cpp ast/aststringliteral.cpp ast/astvariablearray.cpp ast/astfunction.cpp ast/astarray.cpp
                 ast/astcompute.cpp ast/astcondition.cpp
                 ast/astjinjaexpr.cpp
                 jinjac_compiler.cpp
                ${BISON_JinjacBison_OUTPUTS} ${FLEX_JinjacFlex_OUTPUTS}
                staticdata.cpp buffer.cpp 
                code.cpp
                verbose.cpp
   )

add_executable(jinjacc ${JINJACC_SRCS})

