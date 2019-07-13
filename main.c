

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

typedef struct yy_buffer_state * YY_BUFFER_STATE;
extern int yyparse();
extern YY_BUFFER_STATE yy_scan_string(char * str);
extern void yy_delete_buffer(YY_BUFFER_STATE buffer);




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
  
  
  
  YY_BUFFER_STATE buffer;
  buffer = yy_scan_string ( argv[1] );
  yyparse();
  yy_delete_buffer(buffer);
  
  //fprintf(stdout, "argv[1] = %s\n", argv[1]);

  return EXIT_SUCCESS;
}
