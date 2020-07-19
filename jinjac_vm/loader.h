#ifndef LOADER_H
#define LOADER_H

#include <stdint.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif
  
typedef struct
{
  uint32_t offset;
  uint32_t size;
  uint8_t* pSection;
} section_desc;

extern int32_t jinjac_loader(FILE* inputFile);
extern void jinjac_loader_destroy(void);
extern section_desc* jinjac_loader_getDataSection(void);
extern section_desc* jinjac_loader_getCodeSection(void);

#ifdef __cplusplus
}
#endif
  

#endif /* LOADER_H */
