#ifndef _PAYSAGES_PACK_H_
#define _PAYSAGES_PACK_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef struct PackStream PackStream;

PackStream* packReadFile(const char* filepath);
PackStream* packWriteFile(const char* filepath);
void packCloseStream(PackStream* stream);

void packWriteDouble(PackStream* stream, double* value);
void packReadDouble(PackStream* stream, double* value);
void packWriteInt(PackStream* stream, int* value);
void packReadInt(PackStream* stream, int* value);
void packWriteString(PackStream* stream, char* value, int max_length);
void packReadString(PackStream* stream, char* value, int max_length);

#ifdef __cplusplus
}
#endif

#endif
