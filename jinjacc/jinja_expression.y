%code requires{
  #include <stdio.h>
  #include <stdlib.h>
  #include "common.h"
  #include "flex_decl.h"
  #include "astnode.h"
  #include "astvariable.h"
  #include "astnumber.h"
  #include "astboolean.h"
  #include "astarray.h"
  #include "aststringliteral.h"
  #include "astvariablearray.h"
  #include "astfunction.h"
  #include "astcompute.h"
  #include "aststatement.h"
  #include "astcondition.h"
  #include "astjinjaexpr.h"
  #include "jinjac_compiler.h"
  
  extern JinjacCompiler* jinjac_getCompiler(void);
}

%union {
  const AstNode *stmnt;
  int32_t integerData;
  double doubleData;
  std::string *stringData;
}

%define parse.error verbose
%type<stringData> STRING_LITERAL
%type<stringData> IDENTIFIER
%type<doubleData> FLOAT
%type<integerData> INTEGER

%token STRING_LITERAL
%token IDENTIFIER
%token FLOAT
%token INTEGER

//%token '[' ']' '.'
//%right '[' ']' '.'
%right '|'
%left '('
%left ')'

%left '+' '-' '*' '/'

%token FOR END_FOR IN IF IS ELSE END_IF ELIF L_TRUE L_FALSE BLOCK END_BLOCK EXTENDS RAW END_RAW SET
%token EQUAL HIGH_AND_EQUAL_THAN LOWER_AND_EQUAL_THAN DIFFERENT AND OR NOT
%token LOWER_THAN HIGHER_THAN

%destructor { delete($$); } IDENTIFIER STRING_LITERAL

%type<stmnt> jinja_primary_expr expression multiplicative_expr
%type<stmnt> jinja_filtered_expr function_expression
%type<stmnt> postfix_expression array array_list
%type<stmnt> jinja_stmt jinja_arg_list
%type<stmnt> condition_expr
%type<stmnt> jinja_for_stmt jinja_if_stmt
%type<stmnt> condition_unary condition_comparaison condition_equal condition_and

%%
jinja_stmt:
  jinja_for_stmt  { 
                    if (jinjac_getCompiler()->isInJinjaStatement() == true)
                    {
                      const AstFor* f =  dynamic_cast < const AstFor* > ($1);
                      f->setStripBeginBlock(jinjac_getCompiler()->isStripBeginBlock());
                      jinjac_getCompiler()->getRoot()->insert($1);
                      jinjac_getCompiler()->getRoot()->push($1);
                    }
                    else
                    {
                      dbg_print("Not in a statemement");
                      jinjac_getCompiler()->setParsingInError("Not in a statemement");
                    }
                  }
  | jinja_endfor_stmt {
                        if (jinjac_getCompiler()->isInJinjaStatement() == true)
                        {
                          jinjac_getCompiler()->getRoot()->pop(jinjac_getCompiler()->isStripEndBlock());
                        }
                        else
                        {
                          dbg_print("Not in a statemement");
                          jinjac_getCompiler()->setParsingInError("Not in a statemement");
                        }
                      }
  | jinja_if_stmt {
                        if (jinjac_getCompiler()->isInJinjaStatement() == true)
                        {
                          const AstIf* astIf =  dynamic_cast < const AstIf* > ($1);
                          astIf->setStripBeginBlock(jinjac_getCompiler()->isStripBeginBlock());
                          jinjac_getCompiler()->getRoot()->insert($1);
                          jinjac_getCompiler()->getRoot()->push($1);
                        }
                        else
                        {
                          dbg_print("Not in a statemement");
                          jinjac_getCompiler()->setParsingInError("Not in a statemement");
                        }
                  }
  | jinja_else_stmt {
                        if (jinjac_getCompiler()->isInJinjaStatement() == true)
                        {
                          const AstIf* astIf = dynamic_cast < const AstIf* > ( jinjac_getCompiler()->getRoot()->top() );
                          if (astIf != nullptr)
                          {
                            astIf->setStripEndBlock(jinjac_getCompiler()->isStripEndBlock());
                            astIf->pushToElseStmt();
                            astIf->setStripBeginBlock(jinjac_getCompiler()->isStripBeginBlock());
                          }
                          else
                          {
                            dbg_print("else without if\n");
                            assert(false);//TODO set error
                          }
                        }
                        else
                        {
                          dbg_print("Not in a statemement");
                          jinjac_getCompiler()->setParsingInError("Not in a statemement");
                        }
                    }
  | jinja_endif_stmt {
                        if (jinjac_getCompiler()->isInJinjaStatement() == true)
                        {
                          jinjac_getCompiler()->getRoot()->pop(jinjac_getCompiler()->isStripEndBlock());
                        }
                        else
                        {
                          dbg_print("Not in a statemement");
                          jinjac_getCompiler()->setParsingInError("Not in a statemement");
                        }
                     }
  | jinja_filtered_expr {  
                           if (jinjac_getCompiler()->isInJinjaStatement() == false)
                           {
                             dbg_print("a filtered expr\n");
                             jinjac_getCompiler()->getRoot()->insert(new AstJinjaExpr($1));
                           }
                           else
                           {
                             dbg_print("Not in a expression");
                             jinjac_getCompiler()->setParsingInError("Not in a expression");
                           } 
                        }

jinja_for_stmt:
  FOR IDENTIFIER IN jinja_filtered_expr { 
                                           dbg_print(" a FOR statement id '%s' \n", $2->c_str());
                                           $$ = new AstFor(*$2, $4);
                                           delete $2;
                                        }

jinja_endfor_stmt:
  END_FOR {
            dbg_print("a ENDFOR stmt\n"); 
          }

jinja_endif_stmt:
  END_IF { 
           dbg_print("a ENDIF stmt\n");
         }

jinja_else_stmt:
  ELSE { 
         dbg_print("a ELSE stmt\n"); 
       }

jinja_if_stmt:
  IF condition_expr { 
                      dbg_print("a IF statement\n");
                      $$ = new AstIf($2);
                    }
  
jinja_filtered_expr:
  function_expression { 
                         dbg_print("function expression...\n");
                         $$ = $1;
                      }
  |
   postfix_expression { //convert id to string
                        dbg_print("postfix_expression string conversion...\n");
                        //ast_dump_stack();
                        $$ = $1;
                      }
  |
  jinja_filtered_expr '|' function_expression {
                                                dbg_print("a jinja filtered expr\n"); 
                                                $$ = $3;
                                                $$->push($1);
                                              }

postfix_expression:
     expression { $$ = $1; }
   | IDENTIFIER '[' expression ']' {  //TODO: Check id IDENTIFIER --> postfix_expression
                                      dbg_print("an array '%s' \n", $1->c_str()); 
                                      $$ = new AstVariableArray(*$1, $3);
                                      delete $1;
                                   } 
  /* | IDENTIFIER  '.' IDENTIFIER {
                                   //TODO: Check id IDENTIFIER --> postfix_expression
                                   dbg_print("a dot- identifier (%s)\n", $1);
                                }*/
   | array                      { 
                                  dbg_print("array-(biis)\n");
                                  $$ = $1;
                                }
   
   
array:
   '[' array_list ']'      { dbg_print("array\n");
                             $$ = $2;
                           }
   | '(' array_list ')'    { 
                              dbg_print("tuple\n");                              
                              $$ = $2;
                              const AstArray* astArray = dynamic_cast < const AstArray* > ( $$ );
                              if (astArray != nullptr)
                              {
                                astArray->setTuple(true);
                              }
                              else
                              {
                                assert(false);//TODO set error
                              }
                           }
  
array_list:
   postfix_expression                {
                                       dbg_print("insert item-1\n");  
                                       $$ = new AstArray($1);
                                     }
   |
   array_list ',' postfix_expression { 
                                       dbg_print("insert item-2\n"); 
                                       $$ = $1;
                                       $$->push($3);
                                     }


function_expression:
   IDENTIFIER '(' jinja_arg_list ')' { 
                                        dbg_print("a Function '%s'\n", $1->c_str());
                                        $$ = new AstFunction(*$1, nullptr, $3);
                                        delete $1;
                                     }

jinja_arg_list:
      %empty { $$ = new AstFunctionArgument(); }
   |  postfix_expression { 
                           dbg_print("arg \n"); 
                           $$ = new AstFunctionArgument($1);
                         }
   |  jinja_arg_list ',' postfix_expression { 
                                               dbg_print("arg list\n");
                                               $$->push($3);
                                            }


expression:
  multiplicative_expr
  | 
  expression '+' multiplicative_expr { 
                                        dbg_print("ADD\n");
                                        $$ = new AstCompute($1, $3, '+');
                                     }
  |
  expression '-' multiplicative_expr { 
                                        dbg_print("SUB\n");
                                        $$ = new AstCompute($1, $3, '-');
                                     }


multiplicative_expr:
  jinja_primary_expr
  |
  multiplicative_expr '*' jinja_primary_expr { 
                                                dbg_print("MUL\n");
                                                $$ = new AstCompute($1, $3, '*');
                                             }
  |
  multiplicative_expr '/' jinja_primary_expr { 
                                                dbg_print("DIV\n");
                                                $$ = new AstCompute($1, $3, '/');
                                             }
  
jinja_primary_expr:
  IDENTIFIER  { 
                 dbg_print("Identifier '%s'\n", $1->c_str());
                  $$ = new AstVariable(*$1);
                  delete $1;
              }
  |
  STRING_LITERAL { 
                    $$ = new AstStringLiteral(*$1);
                    dbg_print("String Literal '%s'\n", $1->c_str());
                    delete $1;
                 }
 | FLOAT {  
            dbg_print("Double '%f'\n", $1);
            $$ = new AstNumber($1);
         }
 | INTEGER { 
             dbg_print("Integer '%d'\n", $1);
             $$ = new AstNumber($1);
           }
 | L_TRUE  {
             dbg_print("Boolean 'True'\n");
             $$ = new AstBoolean(true);
           }
 | L_FALSE {
             dbg_print("Boolean 'False'\n");
             $$ = new AstBoolean(false);
           }
 | '(' expression ')' {
                        dbg_print("EXP WITH '(' ')'\n");
                        $$ = $2;
                      }

condition_expr:
 condition_and 
 |
 condition_expr OR condition_and { 
                                   dbg_print("or condition\n");
                                   $$ = new AstCondition($1, $3, AstCondition::AST_OR);
                                 }

condition_and:
 condition_equal
 |
 condition_and AND condition_equal { 
                                     dbg_print("and condition\n");
                                     $$ = new AstCondition($1, $3, AstCondition::AST_AND);
                                   }

condition_equal:
  condition_comparaison
  |
  condition_equal EQUAL condition_comparaison { 
                                                 dbg_print("equal expression\n");
                                                 $$ = new AstCondition($1, $3, AstCondition::AST_EQUAL);
                                              }
  |
  condition_equal DIFFERENT condition_comparaison { 
                                                     dbg_print("different expression\n");
                                                     $$ = new AstCondition($1, $3, AstCondition::AST_DIFFERENT);
                                                  }

condition_comparaison:
  condition_unary
  |
  condition_comparaison HIGH_AND_EQUAL_THAN condition_unary { 
                                                               dbg_print("'>=' expression\n");
                                                               $$ = new AstCondition($1, $3, AstCondition::AST_HIGH_AND_EQUAL_THAN);
                                                            }
  |
  condition_comparaison HIGHER_THAN condition_unary { 
                                                       dbg_print("'>' expression\n");
                                                       $$ = new AstCondition($1, $3, AstCondition::AST_HIGH_THAN);
                                                    }
  |
  condition_comparaison LOWER_AND_EQUAL_THAN condition_unary {
                                                                dbg_print("'<=' expression\n");
                                                                $$ = new AstCondition($1, $3, AstCondition::AST_LOWER_AND_EQUAL_THAN);
                                                             }
  |
  condition_comparaison LOWER_THAN condition_unary { 
                                                      dbg_print("'<' expression\n");
                                                      $$ = new AstCondition($1, $3, AstCondition::AST_LOWER_THAN);
                                                   }

  
condition_unary:
  postfix_expression { 
                        dbg_print("postfix expression in condition\n");
                        $$ = $1;
                     }
  |
  function_expression { 
                         dbg_print("alone fct expression\n"); //TODO
                         $$ = $1;
                      } 
  |
  '(' condition_expr ')' { 
                           dbg_print("condition with parenthese\n"); 
                           $$ = $2;
                         }
  |
  NOT postfix_expression { 
                            dbg_print("NOT condition \n");
                            $$ = new AstCondition($2, nullptr, AstCondition::AST_NOT);
                         }
  
%%

void yyerror(const char *s) 
{
  dbg_print("line %d: error: '%s'\n", jinjac_getCompiler()->getNoLine(), s);
  jinjac_getCompiler()->setParsingInError("Parsing Error");
}

