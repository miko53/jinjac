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

  // Declare stuff from Flex that Bison needs to know about:
  extern int yylex();
  extern int yyparse();
  extern FILE *yyin;
 
  extern int getLine(void);
  
  void yyerror(const char *s);
  
  #define stop_on_error(errorString)   \
                            do { \
                              ast_setInError(errorString); \
                              YYABORT; \
                            } while (0);

%}

%union {
  int integerData;
  double doubleData;
  char *stringData;
}

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
                                                ast_execute_function();
                                                //ast_dump_stack();
                                              }

postfix_expression:
     expression 
   | IDENTIFIER '[' expression ']' {  //TODO: Check id IDENTIFIER --> postfix_expression
                                      dbg_print("an array %s \n", $1); 
                                      ast_create_array_on_top($1);
                                   } 
   | IDENTIFIER  '.' IDENTIFIER {
                                   //TODO: Check id IDENTIFIER --> postfix_expression
                                   dbg_print("a dot- identifier (%s)\n", $1);
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
  postfix_expression { dbg_print("alone expression\n"); ast_convert_to_condition(); }
  |
  function_expression { dbg_print("alone fct expression\n"); /*ast_convert_to_condition();*/ } //TODO
  |
  postfix_expression EQUAL postfix_expression { dbg_print("equal expression\n"); ast_do_condition(AST_EQUAL); }
  |
  postfix_expression DIFFERENT postfix_expression { dbg_print("diff expression\n"); ast_do_condition(AST_DIFFERENT);}
  |
  postfix_expression HIGH_AND_EQUAL_THAN postfix_expression { dbg_print(">= expression\n"); ast_do_condition(AST_HIGH_AND_EQUAL_THAN);}
  |
  postfix_expression HIGHER_THAN postfix_expression { dbg_print("> expression\n");  ast_do_condition(AST_HIGH_THAN);}
  |
  postfix_expression LOWER_AND_EQUAL_THAN postfix_expression { dbg_print("<= expression\n"); ast_do_condition(AST_LOWER_AND_EQUAL_THAN); }
  |
  postfix_expression LOWER_THAN postfix_expression { dbg_print("< expression\n"); ast_do_condition(AST_LOWER_THAN);}
  |
  postfix_expression IS function_expression { dbg_print("IS expression with fct\n"); }
  | 
  postfix_expression IS IDENTIFIER { dbg_print("IS expression with id\n"); }
  |
  '(' condition_expr ')' { dbg_print("condition with parenthese\n"); }
  
%%

void yyerror(const char *s) 
{
  dbg_print("line %d: error: '%s'\n",getLine(), s);
  ast_setInError("global error");
}

