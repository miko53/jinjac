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
  /*
  typedef struct yy_buffer_state * YY_BUFFER_STATE;
  extern int yyparse();
  extern YY_BUFFER_STATE yy_scan_string(char * str);
  extern void yy_delete_buffer(YY_BUFFER_STATE buffer);*/
 
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
  | jinja_filtered_expr { fprintf(stdout, "a filtered expr\n");}

jinja_for_stmt:
  FOR IDENTIFIER IN jinja_filtered_expr { fprintf(stdout, " a FOR statement\n"); }

jinja_endfor_stmt:
  END_FOR { fprintf(stdout, "a ENDFOR stmt\n"); }

jinja_endif_stmt:
  END_IF { fprintf(stdout, "a ENDIF stmt\n"); }

jinja_else_stmt:
  ELSE { fprintf(stdout, "a ELSE stmt\n"); }

jinja_if_stmt:
  IF condition_expr { fprintf(stdout, "a IF statement\n"); }
  
jinja_filtered_expr:
  jinja_function_expr 
  |
   jinja_postfix_expr { //convert id to string
                        getAstRoot()->currentStringValue = ast_convert_to_string();
                      }
  |
  jinja_filtered_expr '|' jinja_function_expr {
                                                fprintf(stdout, "a jinja filtered expr\n"); 
                                                getAstRoot()->currentStringValue = ast_apply_filtering();
                                              }

jinja_postfix_expr:
     jinja_primary_expr { }
   | IDENTIFIER '[' jinja_array_offset_expr ']' {
                                                  fprintf(stdout, "an array '%s' \n", $1); 
                                                  ast_create_array_on_top($1);
                                                } 
/*   | jinja_postfix_expr  '.' IDENTIFIER { fprintf(stdout, "a dot- identifier\n"); }*/ //Plus tard...


jinja_function_expr:
   IDENTIFIER '(' jinja_arg_list ')' { 
                                        ast_insert_function($1);
                                        free($1);
                                     }

jinja_arg_list:
      %empty
   |  jinja_postfix_expr { 
                           fprintf(stdout, "arg \n"); 
                         }
   |  jinja_arg_list ',' jinja_postfix_expr { 
                                               fprintf(stdout, "arg list\n");
                                            }

                                            
jinja_array_offset_expr:
    IDENTIFIER  { fprintf(stdout, "1-a id '%s'\n", $1);  ast_insert_identifier($1); } 
  | number_exp { fprintf(stdout, "an int '%d'\n", $1); ast_insert_integer($1); }
  
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
  jinja_postfix_expr EQUAL jinja_postfix_expr { fprintf(stdout, "equal expression\n"); }
  |
  jinja_postfix_expr DIFFERENT jinja_postfix_expr { fprintf(stdout, "diff expression\n"); }
  |
  jinja_postfix_expr HIGH_AND_EQUAL_THAN jinja_postfix_expr { fprintf(stdout, ">= expression\n"); }
  |
  jinja_postfix_expr LOWER_AND_EQUAL_THAN jinja_postfix_expr { fprintf(stdout, "<= expression\n"); }
  |
  jinja_postfix_expr IS jinja_function_expr { fprintf(stdout, "IS expression\n"); }
  | 
  jinja_postfix_expr IS IDENTIFIER { fprintf(stdout, "IS expression\n"); }

  
%%

void yyerror(const char *s) 
{
  fprintf(stdout, "line %d: error: '%s'\n",getLine(), s);
  getAstRoot()->inError = TRUE;
}

