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

#define LINE_SIZE   (1024)
#define STATIC      static

STATIC void parse_file(FILE* in, FILE* out);
STATIC BOOL parse_string(char* string, FILE* out);
STATIC void create_example_parameter(void);

int main(int argc, char* argv[])
{
  // Parse through the input:
  int opt;
  char* inputFile = NULL;
  char* outputfile = NULL;

  while ((opt = getopt(argc, argv, "i:o:h")) != -1)
  {
    switch (opt)
    {
      case 'i':
        inputFile = optarg;
        break;

      case 'o':
        outputfile = optarg;
        break;

      case 'h':
      default:
        fprintf(stderr, "usage %s -i <input file> -o <output file>\n", argv[0]);
        exit(EXIT_FAILURE);
        break;
    }
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

  create_example_parameter();

  parse_file(in, out);
  fclose(in);
  fclose(out);

  return EXIT_SUCCESS;
}

STATIC void create_example_parameter(void)
{
  insert_parameter("a_ident", TYPE_STRING, (parameter_value) "TheValeur");
  insert_parameter("name", TYPE_STRING, (parameter_value) "mickael");
  insert_parameter("gre", TYPE_INT, (parameter_value) 547);
  insert_parameter("myNiceDouble", TYPE_DOUBLE, (parameter_value) 0.156844);
}



enum parse_file_mode
{
  COPY_MODE,
  DETECTION_START_DELIMITER,
  IN_TEMPLATE_SCRIPT,
  DETECTION_STOP_DELIMITER
};

static int no_line;

int getLine(void)
{
  return no_line;
}

STATIC void parse_file(FILE* in, FILE* out)
{
  ASSERT(in != NULL);
  ASSERT(out != NULL);

  //first parsing level detect {%, {{ on one line
  char line[LINE_SIZE];
  line[LINE_SIZE - 1] = '\0';
  BOOL bInError = FALSE;
  int mode;

  no_line = 0;

  while ((fgets(line, LINE_SIZE - 1, in) != NULL) && !bInError)
  {
    //parse line
    char* current;
    char* start = NULL;
    char* stop = NULL;
    current = line;
    mode = COPY_MODE;
    no_line++;

    while ((*current != '\0') && !bInError)
    {
      switch (mode)
      {
        case COPY_MODE:
          if (*current == '{')
          {
            mode = DETECTION_START_DELIMITER;
          }
          else
          {
            fputc(*current, out);
          }
          break;

        case DETECTION_START_DELIMITER:
          start = current + 1;
          switch (*current)
          {
            case '%':
              mode = IN_TEMPLATE_SCRIPT;
              break;

            case '{':
              mode = IN_TEMPLATE_SCRIPT;
              break;

            case '#':
              mode = IN_TEMPLATE_SCRIPT;
              break;

            default:
              mode = COPY_MODE;
              fputc(*current, out);
              start = NULL;
              break;
          }
          break;

        case IN_TEMPLATE_SCRIPT:
          switch (*current)
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

            default:
              // no change of state
              break;
          }
          break;

        case DETECTION_STOP_DELIMITER:
          if (*current == '}')
          {
            mode = DETECTION_START_DELIMITER;
            stop = current;
            //launch parsing
            char toParse[LINE_SIZE];
            strncpy(toParse, start, stop - start);
            toParse[stop - start - 1] = '\0'; //NOTE: -1 to remove previous char i.e }} or #}
            bInError = parse_string(toParse, out);
            mode = COPY_MODE;

          }
          else
          {
            mode = IN_TEMPLATE_SCRIPT;
          }
          break;

        default:
          ASSERT(FALSE);
          break;

      }

      current++;
    }
  }
}


STATIC BOOL parse_string(char* string, FILE* out)
{
  YY_BUFFER_STATE buffer;
  ast* astRoot;

  ast_clean();
  astRoot = getAstRoot();
  astRoot->inError = FALSE;
  fprintf(stdout, "parse = \"%s\"\n", string);

  buffer = yy_scan_string ( string );
  yyparse();

  if (!astRoot->inError)
  {
    fputs(astRoot->string, out);

    if (astRoot->string != NULL)
    {
      free(astRoot->string);
    }
    if (astRoot->identifier != NULL)
    {
      free(astRoot->identifier);
    }
    /*switch (astRoot->type)
    {
      case AST_STRING:
        fputs(astRoot->string, out);
        free(astRoot->string);
        break;


      case AST_FUNCTION:
      default:
        fprintf(stdout, "ast type %d not possible....\n", astRoot->type);
        ASSERT(FALSE);
        break;
    }*/
  }

  yy_delete_buffer(buffer);

  return astRoot->inError;
}


