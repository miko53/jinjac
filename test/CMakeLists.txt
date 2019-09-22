
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
  "test_01c" ;
  "test_01d" ;
  "test_02" ; 
  "test_03" ; 
  "test_04" ; 
  "test_05" ; 
  "test_06" ; 
  "test_07" ; 
  "test_08" ; 
  "test_08b" ;
  "test_09" ; 
  "test_10" ; 
  "test_11" ; 
  "test_12" ; 
  "test_13" ; 
  "test_14" ;
  "test_15" ;
  "test_16" ;
  "test_17" ;
  "test_18" ;
  "test_18b" ;
  "test_18c" ;
  "test_19"
  )
  
set(TEST_TITLE 
  "01_simple_test_without_template" ;
  "01b_simple_test_with_empty_template" ;
  "01c_simple_test_with_tag_inside_string" ;
  "01d_add_comment_in_script" ;
  "02_test_with_constant_string" ;
  "03_constant_string_and_filter_fct" ;
  "04_with_an_identifier_string" ;
  "05_with_an_identifier_double" ;
  "06_three_id_with_filtering" ;
  "07_template_do_computation" ;
  "08_work_with_array" ;
  "08b_array_displayed_directly" ;
  "09_buildin_functions" ;
  "10_truncate_buildins_functions" ;
  "11_center_buildins_functions" ;
  "12_title_buildins_functions" ;
  "13_format_buildins_functions" ;
  "14_format_buildins_error_mngt" ;
  "15_argument_of_function_with_identifier_and_array" ;
  "16_argument_of_function_with_operation" ;
  "17_simple_for_statement" ;
  "18_for_with_array" ;
  "18b_for_without_execution" ;
  "18c_for_with_error_of_id" ;
  "19_simple_if_statement"
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
