#ifndef _META_H
#define _META_H

#include "ressource.h"

#define META_TOTAL_PROBES 0
#define META_SAMPLE_FREQ 1
#define META_PROBE 2
#define META_UNIT_SIZE 3

#define META_PROBE_NAME_LEN 16


SrMetaData* meta_InitStruct(void);
int meta_GetData(char *filepath, SrMetaData *md);
void meta_FreeData(SrMetaData *md);


#endif
