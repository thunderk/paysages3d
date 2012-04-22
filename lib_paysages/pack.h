#ifndef _PAYSAGES_PACK_H_
#define _PAYSAGES_PACK_H_

#include <stdio.h>
#include "shared/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct PackStream PackStream;

PackStream* packReadFile(char* filepath);
PackStream* packWriteFile(char* filepath);
void packCloseStream(PackStream* stream);

void packWriteDouble(PackStream* stream, double* value);
void packReadDouble(PackStream* stream, double* value);
void packWriteInt(PackStream* stream, int* value);
void packReadInt(PackStream* stream, int* value);
    
#ifdef __cplusplus
}
#endif

#endif
