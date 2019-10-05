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
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include "parameter.h"
#include "common.h"
#include "ast.h"
#include "jinja_expression.tab.h"

typedef struct yy_buffer_state* YY_BUFFER_STATE;
extern int yyparse();
extern YY_BUFFER_STATE yy_scan_string(char* str);
extern void yy_delete_buffer(YY_BUFFER_STATE buffer);
extern int yylex_destroy(void);

#define LINE_SIZE   (1024)
typedef enum
{
  IN_TEXT,
  DETECTION_START_DELIMITER,
  IN_JINJA_SCRIPT_STRING,
  IN_JINJA_COMMENT,
  IN_JINJA_COMMENT_STOP_DELIMITER,
  DETECTION_STOP_DELIMITER,
  IN_JINJA_STATEMENT,
  IN_JINJA_EXPRESSION

} parse_file_mode;

STATIC int no_line;


STATIC void jinjac_parse_file(FILE* in, FILE* out);
STATIC BOOL jinjac_parse_string(char* string, FILE* out, FILE* in, BOOL* ignoreNextLine, parse_file_mode previousMode);
STATIC void create_example_parameter(void);
STATIC void delete_example_parameter(void);

STATIC void parse_only_string_arg(char* string)
{
  YY_BUFFER_STATE buffer;

  ast_clean();
  trace("parse only string = \"%s\"\n", string);

  buffer = yy_scan_string (string);
  yyparse();

  ast_status parserStatus;

  parserStatus = ast_getStatus();
  switch (parserStatus)
  {
    case OK_DONE:
      trace( "result: \"%s\"\n", ast_getStringResult());
      break;

    case IN_ERROR:
      trace( "in Error\n");
      break;

    case FOR_STATEMENT:
      trace("For stmt\n");
      break;

    case IF_STATEMENT:
      trace("If stmt\n");
      break;

    default:
      ASSERT(FALSE);
      break;
  }

  yy_delete_buffer(buffer);
  yylex_destroy();
  ast_clean();
}


int main(int argc, char* argv[])
{
  // Parse through the input:
  int opt;
  char* inputFile = NULL;
  char* outputfile = NULL;
  char* test_string = NULL;

  while ((opt = getopt(argc, argv, "i:o:hs:")) != -1)
  {
    switch (opt)
    {
      case 'i':
        inputFile = optarg;
        break;

      case 'o':
        outputfile = optarg;
        break;

      case 's':
        test_string = optarg;
        break;

      case 'h':
      default:
        fprintf(stderr, "usage %s -i <input file> -o <output file>\nusage %s -s \"jinja expression\"\n", argv[0], argv[0]);
        exit(EXIT_FAILURE);
        break;
    }
  }

  create_example_parameter();

  ast_init();

  if (test_string != NULL)
  {
    parse_only_string_arg(test_string);
    delete_example_parameter();
    exit(EXIT_SUCCESS);
  }

  if (inputFile == NULL)
  {
    fprintf(stderr, "input or output file not specified\n");
    exit(EXIT_FAILURE);
  }

  FILE* in = NULL;
  FILE* out = NULL;

  if (outputfile == NULL)
  {
    out = stdout;
  }
  else
  {
    out = fopen(outputfile, "w");
  }

  in = fopen(inputFile, "r");
  if (in == NULL)
  {
    fprintf(stderr, "unable to open %s file in read mode\n", inputFile);
    delete_example_parameter();
    exit(EXIT_FAILURE);
  }

  if (out == NULL)
  {
    fprintf(stderr, "unable to open %s file in write mode\n", outputfile);
    delete_example_parameter();
    exit(EXIT_FAILURE);
  }

  jinjac_parse_file(in, out);

  ast_clean();

  fclose(in);
  fclose(out);

  delete_example_parameter();

  return EXIT_SUCCESS;
}

STATIC void create_example_parameter(void)
{
  parameter param;
  param.type = TYPE_STRING;
  param.value.type_string = "TheValeur";
  parameter_insert("a_ident", &param);

  param.type = TYPE_STRING;
  param.value.type_string = "mickael";
  parameter_insert("name", &param);

  param.type = TYPE_INT;
  param.value.type_int = 547;
  parameter_insert("gre", &param);

  param.type = TYPE_DOUBLE;
  param.value.type_double = 0.156844;
  parameter_insert("myNiceDouble", &param);

  param.type = TYPE_INT;
  param.value.type_int = 2;
  parameter_insert("offset_array", &param);

  parameter_array_insert("data_value", TYPE_INT, 3, 10, 20, 15);
  parameter_array_insert("data_value_dbl", TYPE_DOUBLE, 4, 0.586, 10.45, 159.546, 3.145561);
  parameter_array_insert("data_value_str", TYPE_STRING, 3, "DES", "GTRV", "AADDEGG");
  parameter_array_insert("users", TYPE_STRING, 3, "dana", "bob", "john");
}

STATIC void delete_example_parameter(void)
{
  parameter_delete_all();
}


int getLine(void)
{
  return no_line;
}

STATIC void jinjac_parse_file(FILE* in, FILE* out)
{
  ASSERT(in != NULL);
  ASSERT(out != NULL);

  char bufferJinja[LINE_SIZE];
  int bufferIndex = 0;

  BOOL bInError = FALSE;
  BOOL bIgnoreLine = FALSE;

  char current;
  char previous;
  char stringChar;

  parse_file_mode mode;
  parse_file_mode previousMode;

  no_line = 0;

  mode = IN_TEXT;
  previousMode = IN_JINJA_EXPRESSION;
  previous = '\0';
  current = fgetc(in);

  while (!feof(in) && !bInError)
  {
    if (current == '\n')
    {
      no_line++;
      //clear buffer
      bufferIndex = 0;
      if (mode != IN_JINJA_COMMENT)
      {
        if (mode != IN_TEXT)
        {
          bInError = TRUE;
        }

        if (!bIgnoreLine)
        {
          fputc(current, out);
        }
      }
    }
    else
    {
      switch (mode)
      {
        case IN_TEXT:
          if (current == '{')
          {
            mode = DETECTION_START_DELIMITER;
          }
          else
          {
            if (!bIgnoreLine)
            {
              fputc(current, out);
            }
          }
          break;

        case DETECTION_START_DELIMITER:
          switch (current)
          {
            case '%':
              mode = IN_JINJA_STATEMENT;
              break;

            case '{':
              if (bIgnoreLine)
              {
                mode = IN_TEXT;
              }
              else
              {
                mode = IN_JINJA_EXPRESSION;
              }
              break;

            case '#':
              mode = IN_JINJA_COMMENT;
              break;

            default:
              mode = IN_TEXT;
              fputc(previous, out);
              fputc(current, out);
              bufferIndex = 0;
              break;
          }
          break;

        case IN_JINJA_COMMENT:
          if (current == '#')
          {
            mode = IN_JINJA_COMMENT_STOP_DELIMITER;
          }
          break;

        case IN_JINJA_COMMENT_STOP_DELIMITER:
          if (current == '}')
          {
            mode = IN_TEXT;
          }
          else
          {
            mode = IN_JINJA_COMMENT;
          }
          break;

        case IN_JINJA_STATEMENT:
          switch (current)
          {
            case '%':
              previousMode = IN_JINJA_STATEMENT;
              mode = DETECTION_STOP_DELIMITER;
              break;

            case '\'':
            case '\"':
              stringChar = current;
              bufferJinja[bufferIndex++] = current;
              previousMode = IN_JINJA_STATEMENT;
              mode = IN_JINJA_SCRIPT_STRING;
              break;

            default:
              // no change of state
              bufferJinja[bufferIndex++] = current;
              break;
          }

          break;

        case IN_JINJA_EXPRESSION:
          switch (current)
          {
            case '}':
              previousMode = IN_JINJA_EXPRESSION;
              mode = DETECTION_STOP_DELIMITER;
              break;

            case '\'':
            case '\"':
              stringChar = current;
              bufferJinja[bufferIndex++] = current;
              previousMode = IN_JINJA_EXPRESSION;
              mode = IN_JINJA_SCRIPT_STRING;
              break;

            default:
              // no change of state
              bufferJinja[bufferIndex++] = current;
              break;
          }
          break;

        case IN_JINJA_SCRIPT_STRING:
          if (current == stringChar)
          {
            mode = previousMode;
          }

          bufferJinja[bufferIndex++] = current;
          break;

        case DETECTION_STOP_DELIMITER:
          if (current == '}')
          {
            //launch parsing
            bufferJinja[bufferIndex++] = '\0';
            bInError = jinjac_parse_string(bufferJinja, out, in, &bIgnoreLine, previousMode);
            mode = IN_TEXT;
            bufferIndex = 0;
          }
          else
          {
            bufferJinja[bufferIndex++] = previous;
            bufferJinja[bufferIndex++] = current;
            mode = previousMode;
          }
          break;

        default:
          ASSERT(FALSE);
          break;

      }
    }

    previous = current;
    current = fgetc(in);
  }

  if (bInError)
  {
    error("Parsing error\n");
  }

}

typedef struct
{
  BOOL bIsBlockActive;
  BOOL bIsConditionActive;
  ast_status blockType;
  //int level;
} block;

//STATIC block block_current;

STATIC block block_stack[50];
STATIC int block_level;

STATIC BOOL block_statement_isCurrentBlockActive(void)
{
  BOOL b;
  b = FALSE;

  if (block_level == 0)
  {
    b = TRUE;
  }
  else
  {
    b = block_stack[block_level - 1].bIsBlockActive;
  }

  return b;
}

STATIC BOOL block_statement_isCurrentBlockConditionActive(void)
{
  BOOL b;
  b = FALSE;

  if (block_level == 0)
  {
    b = TRUE;
  }
  else
  {
    b = block_stack[block_level - 1].bIsConditionActive;
  }

  return b;
}

STATIC ast_status block_statement_getCurrentBlockType(void)
{
  ast_status s;
  s = IN_ERROR;

  if (block_level != 0)
  {
    s = block_stack[block_level - 1 ].blockType;
  }

  return s;
}

STATIC BOOL jinjac_parse_string(char* string, FILE* out, FILE* in, BOOL* ignoreNextLine, parse_file_mode previousMode)
{
  YY_BUFFER_STATE buffer;
  ast_status parserStatus;
  BOOL inError;

  ASSERT(string != NULL);
  ASSERT(out != NULL);
  ASSERT(ignoreNextLine != NULL);

  inError = FALSE;

  trace("line %d: string to parse = \"%s\"\n", no_line, string);
  trace("Previous mode = %d\n", previousMode);

  buffer = yy_scan_string ( string );
  yyparse();

  parserStatus = ast_getStatus();

  if (previousMode == IN_JINJA_STATEMENT)
  {
    switch (parserStatus)
    {
      case IN_ERROR:
        trace("error %d\n", __LINE__);
        inError = TRUE;
        break;

      case OK_DONE:
        error("a statement is expected not an expression\n");
        inError = TRUE;
        break;

      case FOR_STATEMENT:
        trace("for statement\n");
        //create a new block (statement) level
        block_stack[block_level].blockType = FOR_STATEMENT;
        if ((block_statement_isCurrentBlockActive() == TRUE) && (block_statement_isCurrentBlockConditionActive() == TRUE))
        {
          block_stack[block_level].bIsBlockActive = TRUE;

          ast_setBeginOfForStatement(ftell(in));
          //*ignoreNextLine =
          block_stack[block_level].bIsConditionActive = !ast_forStmtIsLineToBeIgnored();
        }
        else
        {
          block_stack[block_level].bIsBlockActive = FALSE;
          ast_removeLastResultItem(); // remove from executive stack

        }
        block_level++;
        break;

      case END_FOR_STATEMENT:
        trace("end for statement\n");
        if (block_statement_getCurrentBlockType() == FOR_STATEMENT)
        {
          if (block_statement_isCurrentBlockActive() == TRUE)
          {
            long int returnOffset;
            BOOL bOk;
            bOk = ast_executeEndForStmt(&returnOffset);
            if (bOk)
            {
              if (returnOffset != -1)
              {
                fseek(in, returnOffset, SEEK_SET);
              }
              else
              {
                ast_removeLastResultItem();
                ASSERT(block_level >= 1);
                block_level--;
              }

              ast_removeLastResultItem();
            }
            else
            {
              trace("end For execution Error %d\n", __LINE__);
              inError = TRUE;
            }
          }
          else
          {
            ast_removeLastResultItem(); // remove from executive stack
            ASSERT(block_level >= 1);
            block_level--;
          }
        }
        else
        {
          trace("error, not in for stmt %d\n", __LINE__);
          inError = TRUE;
        }
        break;

      case IF_STATEMENT:
        trace("if statement\n");
        //create a new block (statement) level
        block_stack[block_level].blockType = IF_STATEMENT;
        if (block_statement_isCurrentBlockActive() == TRUE)
        {
          block_stack[block_level].bIsBlockActive = TRUE;
          block_stack[block_level].bIsConditionActive = !ast_ifStmtIsLineToBeIgnored();
        }
        else
        {
          block_stack[block_level].bIsBlockActive = FALSE;
          ast_removeLastResultItem(); // remove from executive stack
        }
        block_level++;
        break;

      case ELSE_STATEMENT:
        if (block_statement_isCurrentBlockActive() == TRUE)
        {
          trace("else statement\n");
          ast_removeLastResultItem(); //NOTE: little hack to retrieve IF statement without build a new function
          ASSERT(block_level >= 1);
          block_stack[block_level - 1].bIsConditionActive = ast_ifStmtIsLineToBeIgnored();
        }
        else
        {
          ast_removeLastResultItem(); // remove from executive stack
        }
        break;

      case END_IF_STATEMENT:
        trace("end if statement\n");
        if (block_statement_getCurrentBlockType() == IF_STATEMENT)
        {
          if (block_statement_isCurrentBlockActive() == TRUE)
          {
            ASSERT(block_level >= 1);
            block_level--;
            ast_removeLastResultItem(); // remove from executive stack
          }

          ast_removeLastResultItem(); // remove from executive stack
        }
        else
        {
          trace("error, not in IF stmt %d\n", __LINE__);
          inError = TRUE;
        }
        break;

      default:
        break;
    }

    *ignoreNextLine = !block_statement_isCurrentBlockConditionActive();
  }
  else if (previousMode == IN_JINJA_EXPRESSION)
  {
    switch (parserStatus)
    {
      case OK_DONE:
        fputs(ast_getStringResult(), out);
        ast_removeLastResultItem();
        break;

      case IN_ERROR:
        error("parsing error %d \n", __LINE__);
        inError = TRUE;
        break;

      default:
        ASSERT(FALSE);
        break;
    }
  }
  else
  {
    ASSERT(FALSE);
  }

  ast_dump_stack();

  yy_delete_buffer(buffer);
  yylex_destroy();

  return inError;
}


