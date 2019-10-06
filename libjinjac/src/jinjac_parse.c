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
#include <string.h>
#include <assert.h>
#include "parameter.h"
#include "common.h"
#include "ast.h"
#include "block_statement.h"
#include "jinja_expression.tab.h"
#include "flex_decl.h"
#include "jinja_parse.h"

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

STATIC int32_t no_line;

STATIC BOOL jinjac_parse_line(char* string, FILE* out, FILE* in, BOOL* ignoreNextLine, parse_file_mode previousMode);

void jinjac_init(void)
{
  ast_init();
  parameter_init();
}

void jinjac_destroy(void)
{
  ast_clean();
}

void jinjac_parse_string(char* string)
{
  YY_BUFFER_STATE buffer;
  ast_status parserStatus;

  ast_clean();
  trace("parse only string = \"%s\"\n", string);

  buffer = yy_scan_string (string);
  yyparse();

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

int32_t getLine(void)
{
  return no_line;
}

void jinjac_parse_file(FILE* in, FILE* out)
{
  ASSERT(in != NULL);
  ASSERT(out != NULL);

  char bufferJinja[LINE_SIZE];
  int32_t bufferIndex = 0;

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
            bInError = jinjac_parse_line(bufferJinja, out, in, &bIgnoreLine, previousMode);
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

STATIC BOOL jinjac_parse_line(char* string, FILE* out, FILE* in, BOOL* ignoreNextLine, parse_file_mode previousMode)
{
  YY_BUFFER_STATE buffer;
  ast_status parserStatus;
  BOOL inError;
  BOOL bBlockActive = FALSE;
  BOOL bConditionActive = FALSE;

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
        if ((block_statement_isCurrentBlockActive() == TRUE) && (block_statement_isCurrentBlockConditionActive() == TRUE))
        {
          bBlockActive = TRUE;
          ast_setBeginOfForStatement(ftell(in));
          bConditionActive = !ast_forStmtIsLineToBeIgnored();
        }
        else
        {
          bBlockActive = FALSE;
          ast_removeLastResultItem(); // remove from executive stack

        }

        block_statement_createNewBlock(FOR_STATEMENT, bBlockActive, bConditionActive);
        break;

      case END_FOR_STATEMENT:
        trace("end for statement\n");
        if (block_statement_getCurrentBlockType() == FOR_STATEMENT)
        {
          if (block_statement_isCurrentBlockActive() == TRUE)
          {
            int64_t returnOffset;
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
                block_statement_removeCurrrent();
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
            block_statement_removeCurrrent();
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
        if ((block_statement_isCurrentBlockActive() == TRUE) && (block_statement_isCurrentBlockConditionActive() == TRUE))
        {
          bBlockActive = TRUE;
          bConditionActive = !ast_ifStmtIsLineToBeIgnored();
        }
        else
        {
          bBlockActive = FALSE;
          ast_removeLastResultItem(); // remove from executive stack
        }
        block_statement_createNewBlock(IF_STATEMENT, bBlockActive, bConditionActive);
        break;

      case ELSE_STATEMENT:
        if (block_statement_isCurrentBlockActive() == TRUE)
        {
          trace("else statement\n");
          ast_removeLastResultItem(); //NOTE: little hack to retrieve IF statement without build a new function
          block_statement_setConditionActiveOfCurrentBlock(ast_ifStmtIsLineToBeIgnored());
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
            ast_removeLastResultItem(); // remove from executive stack
          }

          block_statement_removeCurrrent();
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


