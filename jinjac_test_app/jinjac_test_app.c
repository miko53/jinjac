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
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "jinjac.h"

static void create_example_parameter(void);
static void delete_example_parameter(void);
static void parse_file(char* inputFile, char* outputfile);
static void parse_buffer(char* inputFile, char* outputfile);

static int p_search(char* key, int32_t* privKey, int* isArray);
static J_STATUS p_get(int32_t privKey, jinjac_parameter* param);
static int p_array_getProperties(int32_t privKey, jinjac_parameter_type* type, int32_t* nbItem);
static J_STATUS p_array_getValue(int32_t privKey, int32_t offset, jinjac_parameter_value* v);

jinjac_parameter_callback jinjac_specific_search_cb =
{
  .search = p_search,
  .get = p_get,
  .array_getProperties = p_array_getProperties,
  .array_getValue = p_array_getValue
};

int main(int argc, char* argv[])
{
  // Parse through the input:
  int opt;
  int useBuffer;
  useBuffer = 0;
  char* inputFile = NULL;
  char* outputfile = NULL;
  char* test_string = NULL;

  while ((opt = getopt(argc, argv, "i:o:hbs:")) != -1)
  {
    switch (opt)
    {
      case 'i':
        inputFile = optarg;
        break;

      case 'o':
        outputfile = optarg;
        break;

      case 'b':
        fprintf(stderr, "buffer mode\n");
        useBuffer = 1;
        break;

      case 's':
        test_string = optarg;
        break;

      case 'h':
      default:
        fprintf(stderr,
                "usage %s -i <input file> -o <output file> <-b> use buffer when set\nusage %s -s \"jinja expression\"\n", argv[0],
                argv[0]);
        exit(EXIT_FAILURE);
        break;
    }
  }

  jinjac_init();
  jinjac_parameter_registerUserParameter(&jinjac_specific_search_cb);
  create_example_parameter();

  if (test_string != NULL)
  {
    jinjac_dbg_parse_string(test_string);
    delete_example_parameter();
    exit(EXIT_SUCCESS);
  }

  if (inputFile == NULL)
  {
    fprintf(stderr, "input or output file not specified\n");
    delete_example_parameter();
    exit(EXIT_FAILURE);
  }

  if (useBuffer == 0)
  {
    parse_file(inputFile, outputfile);
  }
  else
  {
    parse_buffer(inputFile, outputfile);
  }

  jinjac_destroy();


  delete_example_parameter();

  return EXIT_SUCCESS;
}

static void create_example_parameter(void)
{
  jinjac_parameter param;
  param.type = TYPE_STRING;
  param.value.type_string = "TheValeur";
  jinjac_parameter_insert("a_ident", &param);

  param.type = TYPE_STRING;
  param.value.type_string = "mickael";
  jinjac_parameter_insert("name", &param);

  param.type = TYPE_INT;
  param.value.type_int = 547;
  jinjac_parameter_insert("gre", &param);

  param.type = TYPE_DOUBLE;
  param.value.type_double = 0.156844;
  jinjac_parameter_insert("myNiceDouble", &param);

  param.type = TYPE_INT;
  param.value.type_int = 2;
  jinjac_parameter_insert("offset_array", &param);

  jinjac_parameter_array_insert("data_value", TYPE_INT, 3, 10, 20, 15);
  jinjac_parameter_array_insert("data_value_dbl", TYPE_DOUBLE, 4, 0.586, 10.45, 159.546, 3.145561);
  jinjac_parameter_array_insert("data_value_str", TYPE_STRING, 3, "DES", "GTRV", "AADDEGG");
  jinjac_parameter_array_insert("users", TYPE_STRING, 3, "dana", "bob", "john");
}

static void delete_example_parameter(void)
{
  jinjac_parameter_delete_all();
}

static void parse_file(char* inputFile, char* outputfile)
{
  FILE* in = NULL;
  FILE* out = NULL;

  if (outputfile == NULL)
  {
    out = stdout;
  }
  else
  {
    out = fopen(outputfile, "w+");
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

  jinjac_render_with_file(in, out);

  fclose(in);
  fclose(out);

}

void parse_buffer(char* inputFile, char* outputfile)
{
  struct stat statInputFile;
  int rc;
  rc = stat(inputFile, &statInputFile);
  if (rc == 0)
  {
    uint8_t* inputFileBuffer;
    int32_t sizeInputFile;
    uint8_t* outputFileBuffer;
    int32_t sizeOutputFile;

    inputFileBuffer = malloc(statInputFile.st_size);
    if (inputFileBuffer != NULL)
    {
      sizeInputFile = statInputFile.st_size;
      outputFileBuffer = NULL;
      sizeOutputFile = 0;

      FILE* in = fopen(inputFile, "r");
      fread(inputFileBuffer, sizeInputFile, 1, in);

      jinjac_render_with_buffer((char*) inputFileBuffer, sizeInputFile, (char**) &outputFileBuffer, &sizeOutputFile);
      FILE* out = fopen(outputfile, "w");
      fwrite(outputFileBuffer, sizeOutputFile, 1, out);
      //fputc('\n', out);
      fclose(out);
      fclose(in);
      free(inputFileBuffer);
      free(outputFileBuffer);
    }
  }
  else
  {
    fprintf(stderr, "unable to open %s file\n", inputFile);
  }
}


int p_search(char* key, int32_t* privKey, int* isArray)
{
  //simulate another array of data to test
  int r = 1;
  int bIsArray;
  bIsArray = 0;

  if (strcmp(key, "@name") == 0)
  {
    *privKey = 0;
  }
  else if (strcmp(key, "@speed") == 0)
  {
    *privKey = 1;
  }
  else if (strcmp(key, "@count") == 0)
  {
    *privKey = 2;
  }
  else if (strcmp(key, "@array_data_int") == 0)
  {
    bIsArray = 1;
    *privKey = 3;
  }
  else if (strcmp(key, "@array_data_dbl") == 0)
  {
    bIsArray = 1;
    *privKey = 4;
  }
  else if (strcmp(key, "@array_data_string") == 0)
  {
    bIsArray = 1;
    *privKey = 5;
  }
  else
  {
    r = 0;
  }

  if (isArray != NULL)
  {
    *isArray = bIsArray;
  }


  return r;
}

J_STATUS p_get(int32_t privKey, jinjac_parameter* param)
{
  J_STATUS s;
  s = J_OK;

  switch (privKey)
  {
    case 0: //@name
      param->type = TYPE_STRING;
      param->value.type_string = "Tyrion";
      break;
    case 1: //@speed
      param->type = TYPE_DOUBLE;
      param->value.type_double = 125.58;
      break;
    case 2: //@count
      param->type = TYPE_INT;
      param->value.type_int = 5684;
      break;

    default:
      s = J_ERROR;
      break;
  }

  return s;
}

int p_array_getProperties(int32_t privKey, jinjac_parameter_type* type, int32_t* nbItem)
{
  int s;
  int32_t nItems;
  s = 1;
  nItems = 0;

  switch (privKey)
  {
    case 3:
      *type = TYPE_INT;
      nItems = 5;
      break;

    case 4:
      *type = TYPE_DOUBLE;
      nItems = 10;
      break;

    case 5:
      *type = TYPE_STRING;
      nItems = 3;
      break;

    default:
      s = 0;

      break;
  }

  if (nbItem != NULL)
  {
    *nbItem = nItems;
  }

  return s;
}

static int32_t array_data_int[5] =
{ 5, 8, -159, 68, 156 };

static double array_data_dbl[10] =
{ 2.0, 598.1, 14.89, 144.117, 0.215, -159.178, 0, 157, 0.12984, 159618.19 };

static char* array_data_string[3] =
{ "good", "bad", "ugly" };


J_STATUS p_array_getValue(int32_t privKey, int32_t offset, jinjac_parameter_value* v)
{
  J_STATUS s;
  s = J_ERROR;

  switch (privKey)
  {
    case 3: // @array_data_int
      if (offset < 5)
      {
        v->type_int = array_data_int[offset];
        s = J_OK;
      }
      break;

    case 4: // @array_data_dbl
      if (offset < 10)
      {
        v->type_double = array_data_dbl[offset];
        s = J_OK;
      }
      break;

    case 5: // @array_data_string
      if (offset < 3)
      {
        v->type_string = array_data_string[offset];
        s = J_OK;
      }
      break;

    default:
      break;
  }

  return s;
}

