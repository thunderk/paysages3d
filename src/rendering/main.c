#include <stdio.h>
#include <stdlib.h>

#include "tools/data.h"
#include "scenery.h"
#include "render.h"
#include "main.h"
#include "opencl.h"

#define APP_HEADER 198632.125

void paysagesInit()
{
    if (!dataInit())
    {
        /* TODO Add error callback (for interface) */
        fprintf(stderr, "ERROR : Can't locate data files.\n");
        exit(1);
    }
    openclInit();

    sceneryInit();
    renderInit();
}

void paysagesQuit()
{
    sceneryQuit();
    renderQuit();

    openclQuit();
}

FileOperationResult paysagesSave(char* filepath)
{
    PackStream* stream = packWriteFile(filepath);
    double app_header, version_header;

    if (!stream)
    {
        return FILE_OPERATION_IOERROR;
    }

    app_header = (double)APP_HEADER;
    packWriteDouble(stream, &app_header);
    version_header = (double)PAYSAGES_CURRENT_DATA_VERSION;
    packWriteDouble(stream, &version_header);

    scenerySave(stream);

    packCloseStream(stream);

    return FILE_OPERATION_OK;
}

FileOperationResult paysagesLoad(char* filepath)
{
    PackStream* stream = packReadFile(filepath);
    double app_header, version_header;

    if (!stream)
    {
        return FILE_OPERATION_IOERROR;
    }

    packReadDouble(stream, &app_header);
    if (app_header != APP_HEADER)
    {
        packCloseStream(stream);
        return FILE_OPERATION_APP_MISMATCH;
    }

    packReadDouble(stream, &version_header);
    if ((int)version_header != PAYSAGES_CURRENT_DATA_VERSION)
    {
        packCloseStream(stream);
        return FILE_OPERATION_VERSION_MISMATCH;
    }

    sceneryLoad(stream);

    packCloseStream(stream);

    return FILE_OPERATION_OK;
}
