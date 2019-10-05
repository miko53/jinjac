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
#include "jinjac.h"

static void create_example_parameter(void);
static void delete_example_parameter(void);


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

  jinjac_init();
  create_example_parameter();

  if (test_string != NULL)
  {
    jinjac_parse_string(test_string);
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

  jinjac_destroy();

  fclose(in);
  fclose(out);

  delete_example_parameter();

  return EXIT_SUCCESS;
}

static void create_example_parameter(void)
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

static void delete_example_parameter(void)
{
  parameter_delete_all();
}
