
#include <cstdlib>
#include "common.h"
#include <bits/getopt_core.h>
#include <iostream>
#include <fstream>
#include "jinjac_compiler.h"

static JinjacCompiler* jinja_compiler = nullptr;

JinjacCompiler* jinjac_getCompiler(void) 
{
  return jinja_compiler;
}

static void printUsageAndExit(char* name)
{
    std::cerr << "Usage: " << name << "-i <input file> -o <output file>" << std::endl;
    exit(EXIT_FAILURE);
}

int main(int argc, char* argv[])
{
  
    int opt;
    char* inputFile = NULL;
    char* outputFile = NULL;

    while ((opt = getopt(argc, argv, "i:o:")) != -1)
    {
        switch (opt)
        {
            case 'i':
                inputFile = optarg;
                break;

            case 'o':
                outputFile = optarg;
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
  
  
    jinja_compiler = new JinjacCompiler(&in, &out);
    
    jinja_compiler->doCompile();
    
    in.close();
    out.close();
  
    delete jinja_compiler;
  return EXIT_SUCCESS;
}
