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
/* scanner for jinja expression*/
%option noyywrap
%option nounput
%option noinput

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
  return STRING_LITERAL;
  }
  
\'(\\.|[^'\\])*\' { 
  yytext[strlen(yytext)-1] = '\0';
  yylval.stringData = strdup(yytext+1);
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
  yylval.stringData = strdup(yytext);
  return IDENTIFIER;
  }
%%

