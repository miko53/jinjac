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
  
  
  #define stop_on_error(errorString)   \
                            do { \
                              ast_setInError(errorString); \
                              YYABORT; \
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
                                           ast_create_for_stmt($2);
                                           //ast_dump_stack();
                                        }

jinja_endfor_stmt:
  END_FOR {
            dbg_print("a ENDFOR stmt\n"); 
            ast_create_end_for_stmt();
          }

jinja_endif_stmt:
  END_IF { 
           dbg_print("a ENDIF stmt\n");
           ast_create_end_if_stmt();
         }

jinja_else_stmt:
  ELSE { 
          dbg_print("a ELSE stmt\n"); 
          ast_create_else_stmt();
       }

jinja_if_stmt:
  IF condition_expr { 
                      dbg_print("a IF statement\n");
                      ast_create_if_stmt();
                      //ast_dump_stack(); 
                    }
  
jinja_filtered_expr:
  function_expression { 
                         dbg_print("function expression...\n");
                         ast_execute_function(); 
                         //ast_dump_stack();
                      }
  |
   postfix_expression { //convert id to string
                        dbg_print("postfix_expression string conversion...\n");
                        //ast_dump_stack();
                        
                      }
  |
  jinja_filtered_expr '|' function_expression {
                                                dbg_print("a jinja filtered expr\n"); 
                                                ast_execute_filtered_function();
                                                //ast_dump_stack();
                                              }

postfix_expression:
     expression 
   | IDENTIFIER '[' expression ']' {  //TODO: Check id IDENTIFIER --> postfix_expression
                                      dbg_print("an array %s \n", $1); 
                                      ast_create_array_on_top($1);
                                   } 
  /* | IDENTIFIER  '.' IDENTIFIER {
                                   //TODO: Check id IDENTIFIER --> postfix_expression
                                   dbg_print("a dot- identifier (%s)\n", $1);
                                }*/
   | array                      { dbg_print("array-(biis)\n"); }
   
   
array:
   '[' array_list ']'      { dbg_print("array\n");
                             ast_set_list_type(FALSE);
                           }
   | '(' array_list ')'    { 
                              dbg_print("tuple\n");
                              ast_set_list_type(TRUE);
                           }
  
array_list:
   postfix_expression                { dbg_print("insert item-1\n");  
                                       ast_create_list_on_top();
                                     }
   |
   array_list ',' postfix_expression { dbg_print("insert item-2\n"); 
                                       ast_list_insert_item();
                                     }


function_expression:
   IDENTIFIER '(' jinja_arg_list ')' { 
                                        dbg_print("a Function '%s'\n", $1);
                                        ast_insert_function($1);
                                        free($1);
                                        //ast_dump_stack();
                                     }

jinja_arg_list:
      %empty
   |  postfix_expression { 
                           dbg_print("arg \n"); 
                           ast_create_function_args_from_top();
                           //ast_dump_stack();
                         }
   |  jinja_arg_list ',' postfix_expression { 
                                               dbg_print("arg list\n");
                                               ast_insert_function_args();
                                               //ast_dump_stack();
                                            }


expression:
  multiplicative_expr
  | 
  expression '+' multiplicative_expr { 
                                        dbg_print("ADD\n");
                                        ast_do_operation('+');
                                     }
  |
  expression '-' multiplicative_expr { 
                                        dbg_print("SUB\n");
                                        ast_do_operation('-'); 
                                     }


multiplicative_expr:
  jinja_primary_expr
  |
  multiplicative_expr '*' jinja_primary_expr { 
                                                dbg_print("MUL\n");
                                                ast_do_operation('*');
                                             }
  |
  multiplicative_expr '/' jinja_primary_expr { 
                                                dbg_print("DIV\n");
                                                ast_do_operation('/');  
                                             }
  
jinja_primary_expr:
  IDENTIFIER  { 
                ast_insert_identifier($1);
                dbg_print("Identifier '%s'\n", $1);
              }
  |
  STRING_LITERAL { 
                    ast_insert_constante($1);
                    dbg_print("String Literal '%s'\n", $1);
                 }
 | FLOAT {  
           ast_insert_double($1);
           dbg_print("Double '%f'\n", $1);
         }
 | INTEGER { 
             ast_insert_integer($1);
             dbg_print("Integer '%d'\n", $1);
           }
 | L_TRUE  {
             ast_insert_boolean(TRUE);
             dbg_print("Boolean 'True'\n");
           }
 | L_FALSE {
             ast_insert_boolean(FALSE);
             dbg_print("Boolean 'False'\n");
           }
 | '(' expression ')' {
                        dbg_print("EXP WITH '(' ')'\n");
                      }

condition_expr:
 condition_or 
 |
 condition_expr AND condition_or { dbg_print("and condition\n"); }

condition_or:
 condition_equal
 |
 condition_or OR condition_equal { dbg_print("or condition\n"); }

condition_equal:
  condition_comparaison
  |
  condition_equal EQUAL condition_comparaison { dbg_print("equal expression\n");ast_do_condition(AST_EQUAL); }
  |
  condition_equal DIFFERENT condition_comparaison { dbg_print("different expression\n");ast_do_condition(AST_DIFFERENT); }

condition_comparaison:
  condition_unary
  |
  condition_comparaison HIGH_AND_EQUAL_THAN condition_unary { dbg_print("'>=' expression\n");ast_do_condition(AST_HIGH_AND_EQUAL_THAN);}
  |
  condition_comparaison HIGHER_THAN condition_unary { dbg_print("'>' expression\n");ast_do_condition(AST_HIGH_THAN);}
  |
  condition_comparaison LOWER_AND_EQUAL_THAN condition_unary { dbg_print("'<=' expression\n");ast_do_condition(AST_LOWER_AND_EQUAL_THAN); }
  |
  condition_comparaison LOWER_THAN condition_unary { dbg_print("'<' expression\n");ast_do_condition(AST_LOWER_THAN);}

  
condition_unary:
  postfix_expression { dbg_print("postfix expression in condition\n");}
  |
  function_expression { dbg_print("alone fct expression\n"); } //TODO
  |
  '(' condition_expr ')' { dbg_print("condition with parenthese\n"); }
  |
  NOT postfix_expression { dbg_print("NOT condition \n"); }
  
%%

void yyerror(const char *s) 
{
  dbg_print("line %d: error: '%s'\n",jinja_parse_getNoLine(), s);
  ast_setInError("global error");
}

