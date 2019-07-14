#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>

typedef struct yy_buffer_state* YY_BUFFER_STATE;
extern int yyparse();
extern YY_BUFFER_STATE yy_scan_string(char* str);
extern void yy_delete_buffer(YY_BUFFER_STATE buffer);

#define LINE_SIZE   (1024)
#define STATIC      static
#define ASSERT      assert

typedef enum { FALSE, TRUE } BOOL;


STATIC void parse_file(FILE* in, FILE* out);
STATIC void parse_string(char* string);


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

  if ((inputFile == NULL) || (outputfile == NULL))
  {
    fprintf(stdout, "input or output file not specified\n");
    exit(EXIT_FAILURE);
  }

  FILE* in = fopen(inputFile, "r");
  if (in == NULL)
  {
    fprintf(stderr, "unable to open %s file in read mode\n", inputFile);
    exit(EXIT_FAILURE);
  }

  FILE* out = fopen(outputfile, "w");
  if (out == NULL)
  {
    fprintf(stderr, "unable to open %s file in write mode\n", outputfile);
    exit(EXIT_FAILURE);
  }

  parse_file(in, out);
  
  return EXIT_SUCCESS;
}

enum parse_file_mode
{
  COPY_MODE,
  DETECTION_START_DELIMITER,
  IN_TEMPLATE_SCRIPT,
  DETECTION_STOP_DELIMITER
};


STATIC void parse_file(FILE* in, FILE* out)
{
  ASSERT(in != NULL);
  ASSERT(out != NULL);

  //first parsing level detect {%, {{ on one line
  char line[LINE_SIZE];
  line[LINE_SIZE - 1] = '\0';
  int mode;

  while (fgets(line, LINE_SIZE - 1, in) != NULL)
  {
    //parse line
    char* current;
    char* start = NULL;
    char* stop = NULL;
    BOOL bInError = FALSE;
    current = line;
    mode = COPY_MODE;

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
            strncpy(toParse, start, stop-start);
            toParse[stop-start-1] = '\0';  //NOTE: -1 to remove previous char i.e }} or #}
            parse_string(toParse);
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

STATIC void parse_string(char* string)
{
  YY_BUFFER_STATE buffer;

  fprintf(stdout, "parse = \"%s\"\n", string);

  buffer = yy_scan_string ( string );
  yyparse();
  yy_delete_buffer(buffer);

}


