/* scanner for jinja expression*/
%option noyywrap
%option nounput
%option noinput

%{
  #include "jinja_expression.tab.hpp"  // to get the token types from Bison
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
  yylval.stringData = new std::string(yytext+1);
  return STRING_LITERAL;
  }
  
\'(\\.|[^'\\])*\' { 
  yytext[strlen(yytext)-1] = '\0';
  yylval.stringData = new std::string(yytext+1);
  return STRING_LITERAL;
  }
[|()\[\],+\-/*\.]   { return yytext[0]; }

({HEXCONSTANT}|{OCTALCONSTANT}|{DECIMALCONSTANT}){INTEGERSUFFIX}?  { yylval.integerData = strtol(yytext, NULL, 0); return INTEGER; }
([0-9]+\.[0-9]*){FLOATINGSUFFI}?                                   { yylval.doubleData = atof(yytext); return FLOAT; }
({DECIMALCONSTANT}){FLOATINGSUFFI}?                                { yylval.doubleData = atof(yytext); return FLOAT; }
and                       { return AND;}
or                        { return OR;}
not                       { return NOT;}
for                       { return FOR;}
in                        { return IN; }
endfor                    { return END_FOR; }
if                        { return IF;}
is                        { return IS; }
else                      { return ELSE; }
elif                      { return ELIF; }
endif                     { return END_IF; }
true|True                 { return L_TRUE; }
false|False               { return L_FALSE; }
block                     { return BLOCK; }
endblock                  { return END_BLOCK; }
extends                   { return EXTENDS; }
raw                       { return RAW; }
endraw                    { return END_RAW; }
set                       { return SET; }
\=\=                      { return EQUAL; }
\>\=                      { return HIGH_AND_EQUAL_THAN; }
\<\=                      { return LOWER_AND_EQUAL_THAN; }
\<                        { return LOWER_THAN; }
\>                        { return HIGHER_THAN; }
\!\=                      { return DIFFERENT;}
[_a-z\@\$A-Z][_a-zA-Z0-9]*  {
  // We have to strdup yytext because Flex will change it for the next token.
  // Note that this memory must be freed somewhere, so that's why we call
  // free() above in the Bison section.  (Aside: we use free() instead of
  // delete because strdup is a C function that uses malloc, not a C++
  // function that uses new.)
  yylval.stringData = new std::string(yytext);
  return IDENTIFIER;
  }
%%

