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

  #define DBG_RULES
  
#ifdef DBG_RULES
  #define dbg_rules(...)     fprintf(stdout, __VA_ARGS__)
#else 
  #define dbg_rules(...)
#endif /* DBG_RULES*/
 
%}

%union {
  int integerData;
  double doubleData;
  char *stringData;
}

%token<stringData> STRING_CST
%token<stringData> IDENTIFIER
%token<doubleData> FLOAT
%token<integerData> INTEGER
%token '[' ']'
%right '|'
%left '('
%left ')'

%left '+' '-' '*' '/'

%token FOR END_FOR IN IF IS ELSE END_IF ELIF L_TRUE L_FALSE BLOCK END_BLOCK EXTENDS RAW END_RAW SET
%token EQUAL HIGH_AND_EQUAL_THAN LOWER_AND_EQUAL_THAN DIFFERENT

%type<integerData>  number_exp
%type<doubleData> mixed_number_exp

%%
jinja_stmt:
  jinja_for_stmt  { }
  | jinja_endfor_stmt { }
  | jinja_if_stmt { }
  | jinja_else_stmt { }
  | jinja_endif_stmt { }
  | jinja_filtered_expr { dbg_rules("a filtered expr\n");}

jinja_for_stmt:
  FOR IDENTIFIER IN jinja_filtered_expr { dbg_rules(" a FOR statement\n"); }

jinja_endfor_stmt:
  END_FOR { dbg_rules("a ENDFOR stmt\n"); }

jinja_endif_stmt:
  END_IF { dbg_rules("a ENDIF stmt\n"); }

jinja_else_stmt:
  ELSE { dbg_rules("a ELSE stmt\n"); }

jinja_if_stmt:
  IF condition_expr { dbg_rules("a IF statement\n"); }
  
jinja_filtered_expr:
  jinja_function_expr 
  |
   jinja_postfix_expr { //convert id to string
                        getAstRoot()->currentStringValue = ast_convert_to_string();
                      }
  |
  jinja_filtered_expr '|' jinja_function_expr {
                                                dbg_rules("a jinja filtered expr\n"); 
                                                getAstRoot()->currentStringValue = ast_apply_filtering();
                                              }

jinja_postfix_expr:
     jinja_primary_expr { }
   | IDENTIFIER '[' jinja_array_offset_expr ']' {
                                                  dbg_rules("an array '%s' \n", $1); 
                                                  ast_create_array_on_top($1);
                                                } 
/*   | jinja_postfix_expr  '.' IDENTIFIER { dbg_rules("a dot- identifier\n"); }*/ //Plus tard...


jinja_function_expr:
   IDENTIFIER '(' jinja_arg_list ')' { 
                                        dbg_rules("a Function '%s'\n", $1);
                                        ast_insert_function($1);
                                        free($1);
                                        ast_dump_stack();
                                     }

jinja_arg_list:
      %empty
   |  jinja_postfix_expr { 
                           dbg_rules("arg \n"); 
                           ast_create_function_args_from_top();
                           //ast_dump_stack();
                         }
   |  jinja_arg_list ',' jinja_postfix_expr { 
                                               dbg_rules("arg list\n");
                                               ast_insert_function_args();
                                               //ast_dump_stack();
                                            }

                                            
jinja_array_offset_expr:
    IDENTIFIER  { dbg_rules("1-a id '%s'\n", $1);  ast_insert_identifier($1); } 
  | number_exp { dbg_rules("an int '%d'\n", $1); ast_insert_integer($1); }
  
jinja_primary_expr:
    IDENTIFIER  { 
                  ast_insert_identifier($1);
                }
  |
    jinja_constant

jinja_constant:
   STRING_CST { 
                ast_insert_constante($1);
                }
 | mixed_number_exp {  
                      ast_insert_double($1);
                    }
 | number_exp { 
                ast_insert_integer($1);
              }
 | L_TRUE {
            ast_insert_boolean(TRUE);
            //dbg_rules("True\n");
          };
 | L_FALSE {
            ast_insert_boolean(FALSE);
            //dbg_rules("False\n");
            };

number_exp:
  INTEGER { $$ = $1; }
 | number_exp '+' number_exp  { $$ = $1+$3;}
 | number_exp '-' number_exp  { $$ = $1-$3;}
 | number_exp '*' number_exp  { $$ = $1*$3;}
 | number_exp '/' number_exp  { $$ = $1/$3;}
 | '(' number_exp ')' { $$ = $2; }
  
mixed_number_exp:
  FLOAT { $$ = $1; }
 | mixed_number_exp '+' mixed_number_exp  { $$ = $1+$3;}
 | mixed_number_exp '-' mixed_number_exp  { $$ = $1-$3;}
 | mixed_number_exp '*' mixed_number_exp  { $$ = $1*$3;}
 | mixed_number_exp '/' mixed_number_exp  { $$ = $1/$3;}
 | '(' mixed_number_exp ')' { $$ = $2; }
 | number_exp '+' mixed_number_exp  { $$ = $1+$3;}
 | number_exp '-' mixed_number_exp  { $$ = $1-$3;}
 | number_exp '*' mixed_number_exp  { $$ = $1*$3;}
 | number_exp '/' mixed_number_exp  { $$ = $1/$3;}
 | mixed_number_exp '+' number_exp  { $$ = $1+$3;}
 | mixed_number_exp '-' number_exp  { $$ = $1-$3;}
 | mixed_number_exp '*' number_exp  { $$ = $1*$3;}
 | mixed_number_exp '/' number_exp  { $$ = $1/$3;}


condition_expr:
  jinja_postfix_expr EQUAL jinja_postfix_expr { dbg_rules("equal expression\n"); }
  |
  jinja_postfix_expr DIFFERENT jinja_postfix_expr { dbg_rules("diff expression\n"); }
  |
  jinja_postfix_expr HIGH_AND_EQUAL_THAN jinja_postfix_expr { dbg_rules(">= expression\n"); }
  |
  jinja_postfix_expr LOWER_AND_EQUAL_THAN jinja_postfix_expr { dbg_rules("<= expression\n"); }
  |
  jinja_postfix_expr IS jinja_function_expr { dbg_rules("IS expression\n"); }
  | 
  jinja_postfix_expr IS IDENTIFIER { dbg_rules("IS expression\n"); }

  
%%

void yyerror(const char *s) 
{
  dbg_rules("line %d: error: '%s'\n",getLine(), s);
  getAstRoot()->inError = TRUE;
}

