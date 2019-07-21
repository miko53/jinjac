/* scanner for jinja expression*/
%{
  #include <stdio.h>
  #include "jinja_expression.tab.h"  // to get the token types from Bison
  
%}

FLOATINGSUFFI ([flFL])
INTEGERSUFFIX ([uU][lL]|[lL][uU]|[uUlL])
DECIMALCONSTANT ([1-9][0-9]*)
OCTALCONSTANT ([0][0-7]*)
HEXCONSTANT ([0][xX][0-9A-Fa-f]+)


%%
[ \t]           ;
\"(\\.|[^"\\])*\" { 
  yytext[strlen(yytext)-1] = '\0';
  yylval.stringData = strdup(yytext+1);
  return STRING_CST;
  }
  
\'(\\.|[^"\\])*\' { 
  yytext[strlen(yytext)-1] = '\0';
  yylval.stringData = strdup(yytext+1);
  return STRING_CST;
  }
[|()\[\],+\-/*\.]   { return yytext[0]; }

([0-9]+\.[0-9]*){FLOATINGSUFFI}?                                   { yylval.doubleData = atof(yytext); return FLOAT; }
({DECIMALCONSTANT}){FLOATINGSUFFI}?                                { yylval.doubleData = atof(yytext); return FLOAT; }
({HEXCONSTANT}|{OCTALCONSTANT}|{DECIMALCONSTANT}){INTEGERSUFFIX}?  { yylval.integerData = strtol(yytext, NULL, 0); return INTEGER; }
for                       { return FOR;}
in                        { return IN; }
endfor                    { return END_FOR; }
if                        { return IF;}
is                        { return IS; }
else                      { return ELSE; }
elif                      { return ELIF; }
endif                     { return END_IF; }
true                      { return L_TRUE; }
false                     { return L_FALSE; }
block                     { return BLOCK; }
endblock                  { return END_BLOCK; }
extends                   { return EXTENDS; }
raw                       { return RAW; }
endraw                    { return END_RAW; }
set                       { return SET; }
\=\=                      { return EQUAL; }
\>\=                      { return HIGH_AND_EQUAL_THAN; }
\<\=                      { return LOWER_AND_EQUAL_THAN; }
\!\=                      { return DIFFERENT;}
[_a-zA-Z][_a-zA-Z0-9]*  {
  // We have to strdup yytext because Flex will change it for the next token.
  // Note that this memory must be freed somewhere, so that's why we call
  // free() above in the Bison section.  (Aside: we use free() instead of
  // delete because strdup is a C function that uses malloc, not a C++
  // function that uses new.)
  yylval.stringData = strdup(yytext);
  return IDENTIFIER;
  }
%%

