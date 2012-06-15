#ifndef _PAYSAGES_PACK_H_
#define _PAYSAGES_PACK_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef struct PackStream PackStream;

PackStream* packReadFile(char* filepath);
PackStream* packWriteFile(char* filepath);
void packCloseStream(PackStream* stream);

void packWriteFloat(PackStream* stream, float* value);
void packReadFloat(PackStream* stream, float* value);
void packWriteInt(PackStream* stream, int* value);
void packReadInt(PackStream* stream, int* value);
    
#ifdef __cplusplus
}
#endif

#endif
