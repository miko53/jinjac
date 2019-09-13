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
  IN_TEXTE,
  DETECTION_START_DELIMITER,
  IN_JINJA_SCRIPT,
  IN_JINJA_SCRIPT_STRING,
  DETECTION_STOP_DELIMITER
} parse_file_mode;

STATIC int no_line;


STATIC void jinjac_parse_file(FILE* in, FILE* out);
STATIC BOOL jinjac_parse_string(char* string, FILE* out, FILE* in, BOOL* ignoreNextLine);
STATIC void create_example_parameter(void);
STATIC void delete_example_parameter(void);

STATIC void parse_only_string_arg(char* string)
{
  YY_BUFFER_STATE buffer;

  ast_clean();
  fprintf(stdout, "parse only string = \"%s\"\n", string);

  buffer = yy_scan_string (string);
  yyparse();

  ast_status parserStatus;

  parserStatus = ast_getStatus();
  switch (parserStatus)
  {
    case OK_DONE:
      fprintf(stdout, "result: \"%s\"\n", ast_getStringResult());
      break;

    case IN_ERROR:
      fprintf(stdout, "in Error\n");
      break;

    case FOR_STATEMENT:
      fprintf(stdout, "For stmt\n");
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
        fprintf(stderr, "usage %s -i <input file> -o <output file>\n", argv[0]);
        exit(EXIT_FAILURE);
        break;
    }
  }

  create_example_parameter();

  ast_init();

  if (test_string != NULL)
  {
    parse_only_string_arg(test_string);
    exit(EXIT_SUCCESS);
  }

  if (inputFile == NULL)
  {
    fprintf(stdout, "input or output file not specified\n");
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
    exit(EXIT_FAILURE);
  }

  if (out == NULL)
  {
    fprintf(stderr, "unable to open %s file in write mode\n", outputfile);
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
  insert_parameter("a_ident", TYPE_STRING, (parameter_value) "TheValeur");
  insert_parameter("name", TYPE_STRING, (parameter_value) "mickael");
  insert_parameter("gre", TYPE_INT, (parameter_value) 547);
  insert_parameter("myNiceDouble", TYPE_DOUBLE, (parameter_value) 0.156844);
  insert_parameter("offset_array", TYPE_INT, (parameter_value) 2);
  insert_array_parameter("data_value", TYPE_INT, 3, 10, 20, 15);
  insert_array_parameter("data_value_dbl", TYPE_DOUBLE, 4, 0.586, 10.45, 159.546, 3.145561);
  insert_array_parameter("data_value_str", TYPE_STRING, 3, "DES", "GTRV", "AADDEGG");
  insert_array_parameter("users", TYPE_STRING, 3, "dana", "bob", "john");
}

STATIC void delete_example_parameter(void)
{
  param_delete_all();
}


int getLine(void)
{
  return no_line;
}

STATIC void jinjac_parse_file(FILE* in, FILE* out)
{
  ASSERT(in != NULL);
  ASSERT(out != NULL);

  //first parsing level detect {%, {{ on one line
  BOOL bInError = FALSE;
  BOOL bIgnoreLine = FALSE;
  char current;
  char previous;
  char stringChar;
  parse_file_mode mode;

  no_line = 0;
  mode = IN_TEXTE;
  char bufferJinja[LINE_SIZE];
  int bufferIndex = 0;

  previous = '\0';
  current = fgetc(in);

  while (!feof(in) && !bInError)
  {
    if (current == '\n')
    {
      no_line++;
      //clear buffer
      bufferIndex = 0;
      if (mode != IN_TEXTE)
      {
        bInError = TRUE;
      }
      if (!bIgnoreLine)
      {
        fputc(current, out);
      }
    }
    else
    {
      switch (mode)
      {
        case IN_TEXTE:
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
              mode = IN_JINJA_SCRIPT;
              break;

            case '{':
              if (bIgnoreLine)
              {
                mode = IN_TEXTE;
              }
              else
              {
                mode = IN_JINJA_SCRIPT;
              }
              break;

            case '#':
              mode = IN_JINJA_SCRIPT;
              break;

            default:
              mode = IN_TEXTE;
              fputc(previous, out);
              fputc(current, out);
              bufferIndex = 0;
              break;
          }
          break;

        case IN_JINJA_SCRIPT:
          switch (current)
          {
            case '%':
              mode = DETECTION_STOP_DELIMITER;
              break;

            case '}':
              mode = DETECTION_STOP_DELIMITER;
              break;

            case '#':
              mode = DETECTION_STOP_DELIMITER;
              break;

            case '\'':
            case '\"':
              stringChar = current;
              bufferJinja[bufferIndex++] = current;
              mode = IN_JINJA_SCRIPT_STRING;
              break;

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
            mode = IN_JINJA_SCRIPT;
          }

          bufferJinja[bufferIndex++] = current;
          break;

        case DETECTION_STOP_DELIMITER:
          if (current == '}')
          {
            mode = DETECTION_START_DELIMITER;
            //launch parsing
            bufferJinja[bufferIndex++] = '\0';
            bInError = jinjac_parse_string(bufferJinja, out, in, &bIgnoreLine);
            mode = IN_TEXTE;
            bufferIndex = 0;
          }
          else
          {
            bufferJinja[bufferIndex++] = previous;
            bufferJinja[bufferIndex++] = current;
            mode = IN_JINJA_SCRIPT;
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
}


STATIC BOOL jinjac_parse_string(char* string, FILE* out, FILE* in, BOOL* ignoreNextLine)
{
  YY_BUFFER_STATE buffer;
  ast_status parserStatus;
  BOOL inError;

  ASSERT(string != NULL);
  ASSERT(out != NULL);
  ASSERT(ignoreNextLine != NULL);

  inError = FALSE;
  *ignoreNextLine = FALSE;

  fprintf(stdout, "parse = \"%s\"\n", string);

  buffer = yy_scan_string ( string );
  yyparse();

  parserStatus = ast_getStatus();
  switch (parserStatus)
  {
    case OK_DONE:
      fputs(ast_getStringResult(), out);
      ast_removeLastResultItem();
      break;

    case FOR_STATEMENT:
      fprintf(stdout, "FOR stmt\n");
      ast_setBeginOfForStatement(ftell(in));
      *ignoreNextLine = ast_forStmtIsLineToBeIgnored();
      break;

    case IN_ERROR:
      fprintf(stdout, "parsing Error\n");
      inError = TRUE;
      break;

    case END_FOR_STATEMENT:
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

          ast_removeLastResultItem();
        }
        else
        {
          inError = TRUE;
        }
      }
      break;

    default:
      ASSERT(FALSE);
      break;
  }

  ast_dump_stack();

  yy_delete_buffer(buffer);
  yylex_destroy();

  return inError;
}


