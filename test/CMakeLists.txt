
function(create_test test_title test_file)
add_test(NAME ${test_title}
         COMMAND autotest.rb $<TARGET_FILE:jinja_expression> ${test_file} 0
         WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
add_test(NAME ${test_title}_memcheck
         COMMAND autotest.rb $<TARGET_FILE:jinja_expression> ${test_file} 1
         WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
endfunction()

set(TEST_LIST 
  "test_01" ; 
  "test_01b" ; 
  "test_02" ; 
  "test_03" ; 
  "test_04" ; 
  "test_05" ; 
  "test_06" ; 
  "test_07" ; 
  "test_08" ; 
  "test_09" ; 
  "test_10" ; 
  "test_11" ; 
  "test_12" ; 
  "test_13" ; 
  "test_14" ;
  "test_15" ;
  "test_16"
  )
  
set(TEST_TITLE 
  "simple_test_without_template" ;
  "simple_test_with_empty_template" ;
  "test_with_constant_string" ;
  "constant_string_and_filter_fct" ;
  "with_an_identifier_string" ;
  "with_an_identifier_double" ;
  "three_id_with_filtering" ;
  "template_do_computation" ;
  "work_with_array" ;
  "buildin_functions" ;
  "truncate_buildins_functions" ;
  "center_buildins_functions" ;
  "title_buildins_functions" ;
  "format_buildins_functions" ;
  "format_buildins_error_mngt" ;
  "argument_of_function_with_identifier_and_array"
  "argument_of_function_with_operation"
  )
  
set(INDEX 0)
list(LENGTH TEST_LIST MAX)
while(INDEX LESS MAX)
  list(GET TEST_LIST ${INDEX} TEST_NAME)
  list(GET TEST_TITLE ${INDEX} TITLE)
  
  message(STATUS "insert test ${TEST_NAME} ${TITLE}")
  
  create_test(${TITLE} ${TEST_NAME})
  
  math(EXPR T "${INDEX} + 1")
  set(INDEX "${T}")
endwhile()
