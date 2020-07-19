
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "loader.h"
#include "vm.h"
#include "param.h"

static void insert_default_test_parameter(void);

static void print_usage(char* exeName)
{
  fprintf(stderr, "Usage: %s [-i jinjac bytecode input file ] [-p parametre file] [-o output file]\n", exeName);
  exit(EXIT_FAILURE);
}


int main(int argc, char* argv[])
{
  int opt;
  int status;
  status = EXIT_FAILURE;
  
  char* inputFile;
  char* outputFile;
  char* paramFile;
  
  inputFile = NULL;
  outputFile = NULL;
  paramFile = NULL;
  
  while ((opt = getopt(argc, argv, "i:o:p:")) != -1)
  {
      switch (opt) 
      {
      case 'i':
          inputFile = optarg;
          break;
          
      case 'o':
          outputFile = optarg;
          break;
          
      case 'p':
          paramFile = optarg;
          break;
      default: 
          print_usage(argv[0]);
      }
  }
  
  if ((inputFile == NULL) /*|| (outputFile == NULL) || (paramFile == NULL)*/)
  {
    print_usage(argv[0]);
  }
  
  FILE* fInput;
  FILE* fOutput;
  FILE* fParam;
  
  fInput = fopen(inputFile, "r");
  if (fInput == NULL)
  {
    fprintf(stderr, "unable to open %s file\n", inputFile);
    return EXIT_FAILURE;
  }
  
  if (outputFile != NULL)
  {
    fOutput = fopen(outputFile, "w");
    if (fOutput == NULL)
    {
      fprintf(stderr, "unable to create %s file\n", outputFile);
      return EXIT_FAILURE;
    }
  }
  else
  { 
    fOutput = stdout;
  }

  /*
  fParam = fopen(paramFile, "r");
  if (fParam == NULL)
  {
    fprintf(stderr, "unable to open %s file\n", paramFile);
    return EXIT_FAILURE;
  }
  */
  
  insert_default_test_parameter();
  
  status = jinjac_loader(fInput);
  if (status == 0)
  {
    status = vm_exe(fOutput);
    status = EXIT_SUCCESS;
    jinjac_loader_destroy();
  }
  
  fclose(fInput);
  if (outputFile != NULL)
    fclose(fOutput);
  
  param_destroy();
  
  return status;
}

void insert_default_test_parameter()
{
  J_STATUS rc;
  
  rc = param_initialize(0);
  assert(rc == J_OK); 
  
  rc = param_insert("a_ident", J_STRING, 1, "TheValeur");
  assert(rc == J_OK);

  /*rc = param_insert("a_ident", J_STRING, 1, "TheValeur");
  assert(rc == -1);*/
  
  rc = param_insert("name", J_STRING, 1, "mickael");
  assert(rc == J_OK);
  
  rc = param_insert("gre", J_INT, 1, 547);
  assert(rc == J_OK);

  rc = param_insert("myNiceDouble", J_DOUBLE, 1, (double) 0.156844);
  assert(rc == J_OK);

  rc = param_insert("offset_array", J_INT, 1, 2);
  assert(rc == J_OK);

  rc = param_insert("data_value", J_INT, 3, 10, 20, 15);
  assert(rc == J_OK);

  rc = param_insert("data_value_dbl", J_DOUBLE, 4, 0.586, 10.45, 159.546, 3.145561);
  assert(rc == J_OK);

  rc = param_insert("data_value_str", J_STRING, 3, "DES", "GTRV", "AADDEGG");
  assert(rc == J_OK);

  rc = param_insert("users", J_STRING, 3, "dana", "bob", "john");
  assert(rc == J_OK);
  
  
  rc = param_insert("@name", J_STRING, 1, "Tyrion");
  assert(rc == J_OK);
  
  rc = param_insert("@speed", J_DOUBLE, 1, 125.58);
  assert(rc == J_OK);

  rc = param_insert("@count", J_INT, 1, 5684);
  assert(rc == J_OK);
  
  rc = param_insert("@array_data_int", J_INT, 5, 5, 8, -159, 68, 156);
  assert(rc == J_OK);

  rc = param_insert("@array_data_dbl", J_DOUBLE, 10, 2.0, 598.1, 14.89, 144.117, 0.215, -159.178, 0.0, 157.0, 0.12984, 159618.19);
  assert(rc == J_OK);

  rc = param_insert("@array_data_string", J_STRING, 3, "good", "bad", "ugly" );
  assert(rc == J_OK);
  
}
