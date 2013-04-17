#ifndef _PAYSAGES_MAIN_H_
#define _PAYSAGES_MAIN_H_

#define PAYSAGES_CURRENT_DATA_VERSION 1

#ifdef __cplusplus
extern "C" {
#endif

    typedef enum {
        FILE_OPERATION_OK,
        FILE_OPERATION_IOERROR,
        FILE_OPERATION_APP_MISMATCH,
        FILE_OPERATION_VERSION_MISMATCH
    } FileOperationResult;

    void paysagesInit();
    void paysagesQuit();
    FileOperationResult paysagesSave(char* filepath);
    FileOperationResult paysagesLoad(char* filepath);

#ifdef __cplusplus
}
#endif

#endif
