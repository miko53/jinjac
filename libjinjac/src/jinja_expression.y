/*
 * Copyright (c) 2019 miko53
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
%{
  #include <stdio.h>
  #include <stdlib.h>
  #include "common.h"
  #include "ast.h"
  #include "parameter.h"
  #include "convert.h"
  #include "flex_decl.h"
  #include "jinjac_parse.h"
  
  
  #define check_status(status, errorString) \
                               do {\
                               if (status != J_OK) \
                               { \
                                 ast_setInError(errorString); \
                                 YYABORT; \
                               }\
                               } while (0);
%}

%union {
  int32_t integerData;
  double doubleData;
  char *stringData;
}

%define parse.error verbose
%token<stringData> STRING_LITERAL
%token<stringData> IDENTIFIER
%token<doubleData> FLOAT
%token<integerData> INTEGER

//%token '[' ']' '.'
//%right '[' ']' '.'
%right '|'
%left '('
%left ')'

%left '+' '-' '*' '/'

%token FOR END_FOR IN IF IS ELSE END_IF ELIF L_TRUE L_FALSE BLOCK END_BLOCK EXTENDS RAW END_RAW SET
%token EQUAL HIGH_AND_EQUAL_THAN LOWER_AND_EQUAL_THAN DIFFERENT AND OR NOT
%token LOWER_THAN HIGHER_THAN

%destructor { free($$); } IDENTIFIER STRING_LITERAL

%%
jinja_stmt:
  jinja_for_stmt  { }
  | jinja_endfor_stmt { }
  | jinja_if_stmt { }
  | jinja_else_stmt { }
  | jinja_endif_stmt { }
  | jinja_filtered_expr { dbg_print("a filtered expr\n");}

jinja_for_stmt:
  FOR IDENTIFIER IN jinja_filtered_expr { 
                                           dbg_print(" a FOR statement id %s \n", $2  );
                                           J_STATUS status;
                                           status = ast_create_for_stmt($2);
                                           check_status(status, "unable to create for statement");
                                           //ast_dump_stack();
                                        }

jinja_endfor_stmt:
  END_FOR {
            dbg_print("a ENDFOR stmt\n"); 
            J_STATUS status;
            status = ast_create_end_for_stmt();
            check_status(status, "unable to create endfor statement");
          }

jinja_endif_stmt:
  END_IF { 
           dbg_print("a ENDIF stmt\n");
           J_STATUS status;
           status = ast_create_end_if_stmt();
           check_status(status, "unable to create endif statement");
         }

jinja_else_stmt:
  ELSE { 
         dbg_print("a ELSE stmt\n"); 
         J_STATUS status;
         status = ast_create_else_stmt();
         check_status(status, "unable to create else statement");
       }

jinja_if_stmt:
  IF condition_expr { 
                      dbg_print("a IF statement\n");
                      J_STATUS status;
                      status = ast_create_if_stmt();
                      check_status(status, "unable to create if statement");
                      //ast_dump_stack(); 
                    }
  
jinja_filtered_expr:
  function_expression { 
                         dbg_print("function expression...\n");
                         J_STATUS status;
                         status = ast_execute_function(); 
                         check_status(status, "unable to execute function");
                         //ast_dump_stack();
                      }
  |
   postfix_expression { //convert id to string
                        //dbg_print("postfix_expression string conversion...\n");
                        //ast_dump_stack();
                      }
  |
  jinja_filtered_expr '|' function_expression {
                                                dbg_print("a jinja filtered expr\n"); 
                                                J_STATUS status;
                                                status = ast_execute_filtered_function();
                                                check_status(status, "unable to execute filtering statement");
                                                //ast_dump_stack();
                                              }

postfix_expression:
     expression 
   | IDENTIFIER '[' expression ']' {  //TODO: Check id IDENTIFIER --> postfix_expression
                                      dbg_print("an array %s \n", $1); 
                                      J_STATUS status;
                                      status = ast_create_array_on_top($1);
                                      check_status(status, "unable to create array");
                                   } 
  /* | IDENTIFIER  '.' IDENTIFIER {
                                   //TODO: Check id IDENTIFIER --> postfix_expression
                                   dbg_print("a dot- identifier (%s)\n", $1);
                                }*/
   | array                      { 
                                  dbg_print("array-(biis)\n"); 
                                }
   
   
array:
   '[' array_list ']'      { dbg_print("array\n");
                             J_STATUS status;
                             status = ast_set_list_type(FALSE);
                             check_status(status, "unable to set list property");
                           }
   | '(' array_list ')'    { 
                              dbg_print("tuple\n");
                              J_STATUS status;
                              status = ast_set_list_type(TRUE);
                              check_status(status, "unable to set tuple property");
                           }
  
array_list:
   postfix_expression                {
                                       dbg_print("insert item-1\n");  
                                       J_STATUS status;
                                       status = ast_create_list_on_top();
                                       check_status(status, "unable to create list");
                                     }
   |
   array_list ',' postfix_expression { 
                                       dbg_print("insert item-2\n"); 
                                       J_STATUS status;
                                       status = ast_list_insert_item();
                                       check_status(status, "unable to update list");
                                     }


function_expression:
   IDENTIFIER '(' jinja_arg_list ')' { 
                                        dbg_print("a Function '%s'\n", $1);
                                        J_STATUS status;
                                        status = ast_insert_function($1);
                                       check_status(status, "unable to create function");
                                        free($1);
                                        //ast_dump_stack();
                                     }

jinja_arg_list:
      %empty
   |  postfix_expression { 
                           dbg_print("arg \n"); 
                           J_STATUS status;
                           status = ast_create_function_args_from_top();
                           check_status(status, "unable to insert argument");
                           //ast_dump_stack();
                         }
   |  jinja_arg_list ',' postfix_expression { 
                                               dbg_print("arg list\n");
                                               J_STATUS status;
                                               status = ast_insert_function_args();
                                               check_status(status, "unable to insert next argument");
                                               //ast_dump_stack();
                                            }


expression:
  multiplicative_expr
  | 
  expression '+' multiplicative_expr { 
                                        dbg_print("ADD\n");
                                        J_STATUS status;
                                        status = ast_do_operation('+');
                                        check_status(status, "unable to execute operation '+'");
                                     }
  |
  expression '-' multiplicative_expr { 
                                        dbg_print("SUB\n");
                                        J_STATUS status;
                                        status = ast_do_operation('-'); 
                                        check_status(status, "unable to execute operation '-'");
                                     }


multiplicative_expr:
  jinja_primary_expr
  |
  multiplicative_expr '*' jinja_primary_expr { 
                                                dbg_print("MUL\n");
                                                J_STATUS status;
                                                status = ast_do_operation('*');
                                                check_status(status, "unable to execute operation '*'");
                                             }
  |
  multiplicative_expr '/' jinja_primary_expr { 
                                                dbg_print("DIV\n");
                                                J_STATUS status;
                                                status = ast_do_operation('/');  
                                                check_status(status, "unable to execute operation '/'");
                                             }
  
jinja_primary_expr:
  IDENTIFIER  { 
                J_STATUS status;
                status = ast_insert_identifier($1);
                check_status(status, "unable to insert identifier");
                dbg_print("Identifier '%s'\n", $1);
              }
  |
  STRING_LITERAL { 
                    J_STATUS status;
                    status = ast_insert_constante($1);
                    check_status(status, "unable to insert constante");
                    dbg_print("String Literal '%s'\n", $1);
                 }
 | FLOAT {  
           dbg_print("Double '%f'\n", $1);
           J_STATUS status;
           status = ast_insert_double($1);
           check_status(status, "unable to insert double");
         }
 | INTEGER { 
             dbg_print("Integer '%d'\n", $1);
             J_STATUS status;
             status = ast_insert_integer($1);
             check_status(status, "unable to insert integer");
           }
 | L_TRUE  {
             dbg_print("Boolean 'True'\n");
             J_STATUS status;
             status = ast_insert_boolean(TRUE);
             check_status(status, "unable to insert boolean 'true'");
           }
 | L_FALSE {
             dbg_print("Boolean 'False'\n");
             J_STATUS status;
             status = ast_insert_boolean(FALSE);
             check_status(status, "unable to insert boolean 'false'");
           }
 | '(' expression ')' {
                        dbg_print("EXP WITH '(' ')'\n");
                      }

condition_expr:
 condition_and 
 |
 condition_expr OR condition_and { 
                                   dbg_print("or condition\n");
                                   J_STATUS status;
                                   status = ast_do_logical_condition(AST_OR);
                                   check_status(status, "unable to do logical 'or'");
                                 }

condition_and:
 condition_equal
 |
 condition_and AND condition_equal { 
                                     dbg_print("and condition\n");
                                     J_STATUS status;
                                     status = ast_do_logical_condition(AST_AND); 
                                     check_status(status, "unable to do logical 'and'");
                                   }

condition_equal:
  condition_comparaison
  |
  condition_equal EQUAL condition_comparaison { 
                                                 dbg_print("equal expression\n");
                                                 J_STATUS status;
                                                 status = ast_do_condition(AST_EQUAL);
                                                 check_status(status, "unable to do logical 'equal'");
                                              }
  |
  condition_equal DIFFERENT condition_comparaison { 
                                                     dbg_print("different expression\n");
                                                     J_STATUS status;
                                                     status = ast_do_condition(AST_DIFFERENT);
                                                     check_status(status, "unable to do logical 'different'");
                                                  }

condition_comparaison:
  condition_unary
  |
  condition_comparaison HIGH_AND_EQUAL_THAN condition_unary { 
                                                               dbg_print("'>=' expression\n");
                                                               J_STATUS status;
                                                               status = ast_do_condition(AST_HIGH_AND_EQUAL_THAN);
                                                               check_status(status, "unable to do operation '>='");
                                                            }
  |
  condition_comparaison HIGHER_THAN condition_unary { 
                                                       dbg_print("'>' expression\n");
                                                       J_STATUS status;
                                                       status = ast_do_condition(AST_HIGH_THAN);
                                                       check_status(status, "unable to do operation '>'");
                                                    }
  |
  condition_comparaison LOWER_AND_EQUAL_THAN condition_unary {
                                                                dbg_print("'<=' expression\n");
                                                                J_STATUS status;
                                                                status = ast_do_condition(AST_LOWER_AND_EQUAL_THAN);
                                                                check_status(status, "unable to do operation '<='");
                                                             }
  |
  condition_comparaison LOWER_THAN condition_unary { 
                                                      dbg_print("'<' expression\n");
                                                      J_STATUS status;
                                                      status = ast_do_condition(AST_LOWER_THAN);
                                                      check_status(status, "unable to do operation '<'");
                                                   }

  
condition_unary:
  postfix_expression { 
                        dbg_print("postfix expression in condition\n");
                     }
  |
  function_expression { 
                         dbg_print("alone fct expression\n"); //TODO
                      } 
  |
  '(' condition_expr ')' { 
                           dbg_print("condition with parenthese\n"); 
                         }
  |
  NOT postfix_expression { 
                            dbg_print("NOT condition \n");
                            J_STATUS status;
                            status = ast_do_logical_condition(AST_NOT);
                            check_status(status, "unable to do operation 'not'");
                         }
  
%%

void yyerror(const char *s) 
{
  dbg_print("line %d: error: '%s'\n",jinja_parse_getNoLine(), s);
  ast_setInError("global error");
}

