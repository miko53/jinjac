#include "loader.h"
#include <string.h>
#include <endian.h>
#include <stdlib.h>
#include "common.h"

static section_desc data_section_desc;
static section_desc code_section_desc;

static J_STATUS loader_read_section(section_desc* desc, FILE* inputFile)
{
  int32_t nbItemRead;
  J_STATUS rc;
  rc = J_ERROR;

  nbItemRead = fread(&desc->offset, sizeof(uint32_t), 1, inputFile);
  desc->offset = be32toh(desc->offset);
  if (nbItemRead != 1)
  {
    fprintf(stderr, "input file size too short\n");
  }
  else 
    rc = J_OK;
  
  if (rc == J_OK)
  {
    nbItemRead = fread(&desc->size, sizeof(uint32_t), 1, inputFile);
    desc->size = be32toh(desc->size);
    if (nbItemRead != 1)
    {
      rc = J_ERROR;
      fprintf(stderr, "input file size too short\n");
    }
  }
  
  return rc;
}

J_STATUS jinjac_loader(FILE* inputFile)
{
  J_STATUS rc;
  int32_t nbItemRead;
  uint16_t version;
  uint16_t pad;
  char magic[5];
  rc = J_ERROR;
  
  magic[4] = '\0';
  nbItemRead = fread(&magic, 4 , 1, inputFile);
  if ((nbItemRead == 1) && (strncmp(magic, "JOBJ", 4) == 0))
  {
    rc = J_OK;
  }
  else
  {
    fprintf(stderr, "input file is not a jinjac compiled file (wrong magic number '%s' != 'JOBJ')\n", magic);  
  }
  
  if (rc == J_OK)
  {
    nbItemRead = fread(&version, sizeof(uint16_t), 1, inputFile);
    version = be16toh(version);
    if (!((nbItemRead == 1) && (version == 0x0100)))
    {
      rc = J_ERROR;
      fprintf(stderr, "version not compatible (version => 0x%x != 0x0100\n)", version);
    }
  }
  
  if (rc == J_OK)
  {
    nbItemRead = fread(&pad, sizeof(uint16_t), 1, inputFile);
    if (nbItemRead != 1)
    {
      rc = J_ERROR;
      fprintf(stderr, "input file size too short\n");
    }
  }
  
  rc = loader_read_section(&data_section_desc, inputFile);
  if (rc == J_OK)
  {
    rc = loader_read_section(&code_section_desc, inputFile);
  }
  
  if (rc == J_OK)
  {
    fprintf(stderr, "[info] datasection abs offset = 0x%x, size = %d\n", data_section_desc.offset, data_section_desc.size);
    fprintf(stderr, "[info] codesection abs offset = 0x%x, size = %d\n", code_section_desc.offset, code_section_desc.size);
    
    data_section_desc.pSection = malloc(data_section_desc.size);
    if (data_section_desc.pSection == NULL)
    {
      fprintf(stderr, "wrong data section size %d\n", data_section_desc.size);
      rc = J_ERROR;
    }
    
  }
  
  if (rc == J_OK) 
  {
    code_section_desc.pSection = malloc(code_section_desc.size);
    if (code_section_desc.pSection == NULL)
    {
      fprintf(stderr, "wrong code section size %d\n", code_section_desc.size);
      rc = J_ERROR;
    }
  }
  
  if (rc == J_OK)
  {
    fseek(inputFile, data_section_desc.offset, SEEK_SET);
    nbItemRead = fread(data_section_desc.pSection, data_section_desc.size, 1, inputFile);
    if (nbItemRead != 1)
    {
      fprintf(stderr, "unable to read data section\n");
      rc = J_ERROR;
    }
  }
  
  if (rc == J_OK)
  {
    fseek(inputFile, code_section_desc.offset, SEEK_SET);
    nbItemRead = fread(code_section_desc.pSection, code_section_desc.size, 1, inputFile);
    if (nbItemRead != 1)
    {
      fprintf(stderr, "unable to read code section\n");
      rc = J_ERROR;
    }
  }
  
  if (rc != J_OK)
  {
    jinjac_loader_destroy();
  }
  
  return rc;
}

section_desc * jinjac_loader_getCodeSection()
{
  return &code_section_desc;
}

section_desc * jinjac_loader_getDataSection()
{
  return &data_section_desc;
}


void jinjac_loader_destroy()
{
  if (data_section_desc.pSection != NULL)
  {
    free(data_section_desc.pSection);
    data_section_desc.pSection = NULL;
  }
  
  if (code_section_desc.pSection != NULL)
  {
    free(code_section_desc.pSection);
    code_section_desc.pSection = NULL;
  }
}

