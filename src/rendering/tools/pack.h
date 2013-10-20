#ifndef _PAYSAGES_TOOLS_PACK_H_
#define _PAYSAGES_TOOLS_PACK_H_

#include "../rendering_global.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct PackStream PackStream;

RENDERINGSHARED_EXPORT PackStream* packReadFile(const char* filepath);
RENDERINGSHARED_EXPORT PackStream* packWriteFile(const char* filepath);
RENDERINGSHARED_EXPORT void packCloseStream(PackStream* stream);

RENDERINGSHARED_EXPORT void packWriteDouble(PackStream* stream, double* value);
RENDERINGSHARED_EXPORT void packReadDouble(PackStream* stream, double* value);
RENDERINGSHARED_EXPORT void packWriteInt(PackStream* stream, int* value);
RENDERINGSHARED_EXPORT void packReadInt(PackStream* stream, int* value);
RENDERINGSHARED_EXPORT void packWriteString(PackStream* stream, char* value, int max_length);
RENDERINGSHARED_EXPORT void packReadString(PackStream* stream, char* value, int max_length);

#ifdef __cplusplus
}
#endif

#endif
