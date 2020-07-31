#include <cstdlib>
#include <bits/getopt_core.h>
#include <iostream>
#include <fstream>
#include "jinjac_compiler.h"
#include "verbose.h"

static JinjacCompiler* jinja_compiler = nullptr;

JinjacCompiler* jinjac_getCompiler(void)
{
  return jinja_compiler;
}

static void printUsageAndExit(char* name)
{
  std::cerr << "Usage: " << name << "-i <input file> -o <output file> (-t) for printing ast (-v <verbose level>)" <<
            std::endl;
  std::cerr << "Version" << std::endl;
  exit(EXIT_FAILURE);
}

int main(int argc, char* argv[])
{
  int opt;
  uint32_t verboseLevel = 0;
  char* inputFile = NULL;
  char* outputFile = NULL;
  bool printAstTree = false;

  while ((opt = getopt(argc, argv, "i:o:tv:")) != -1)
  {
    switch (opt)
    {
      case 'i':
        inputFile = optarg;
        break;

      case 'o':
        outputFile = optarg;
        break;

      case 't':
        printAstTree = true;
        break;

      case 'v':
        verboseLevel = atoi(optarg);
        break;

      default: /* '?' */
        printUsageAndExit(argv[0]);
    }
  }

  if ((inputFile == NULL) || (outputFile == NULL))
  {
    printUsageAndExit(argv[0]);
  }

  std::ifstream in;
  in.open(inputFile);
  if (in.fail())
  {
    std::cerr << "unable to open " << inputFile << " file" << std::endl;
    exit(EXIT_FAILURE);
  }

  std::ofstream out;
  out.open(outputFile);
  if (out.fail())
  {
    std::cerr << "unable to open " << outputFile << " file" << std::endl;
    exit(EXIT_FAILURE);
  }

  verbose_setLevel(verboseLevel);
  jinja_compiler = new JinjacCompiler(&in, &out);
  jinja_compiler->printAstTree(printAstTree);
  jinja_compiler->doCompile();

  in.close();
  out.close();

  delete jinja_compiler;
  return EXIT_SUCCESS;
}
