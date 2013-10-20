#ifndef _PAYSAGES_MAIN_H_
#define _PAYSAGES_MAIN_H_

#define PAYSAGES_CURRENT_DATA_VERSION 1

#include "rendering_global.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    FILE_OPERATION_OK,
    FILE_OPERATION_IOERROR,
    FILE_OPERATION_APP_MISMATCH,
    FILE_OPERATION_VERSION_MISMATCH
} FileOperationResult;

RENDERINGSHARED_EXPORT void paysagesInit();
RENDERINGSHARED_EXPORT void paysagesQuit();
RENDERINGSHARED_EXPORT FileOperationResult paysagesSave(char* filepath);
RENDERINGSHARED_EXPORT FileOperationResult paysagesLoad(char* filepath);

#ifdef __cplusplus
}
#endif

#endif
