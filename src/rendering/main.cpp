#include <stdio.h>
#include <stdlib.h>

#include "tools/data.h"
#include "Scenery.h"
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
    PackStream stream;
    double app_header, version_header;

    if (!stream.bindToFile(filepath, true))
    {
        return FILE_OPERATION_IOERROR;
    }

    app_header = (double)APP_HEADER;
    stream.write(&app_header);
    version_header = (double)PAYSAGES_CURRENT_DATA_VERSION;
    stream.write(&version_header);

    scenerySave(&stream);

    return FILE_OPERATION_OK;
}

FileOperationResult paysagesLoad(char* filepath)
{
    PackStream stream;
    double app_header, version_header;

    if (!stream.bindToFile(filepath, false))
    {
        return FILE_OPERATION_IOERROR;
    }

    stream.read(&app_header);
    if (app_header != APP_HEADER)
    {
        return FILE_OPERATION_APP_MISMATCH;
    }

    stream.read(&version_header);
    if ((int)version_header != PAYSAGES_CURRENT_DATA_VERSION)
    {
        return FILE_OPERATION_VERSION_MISMATCH;
    }

    sceneryLoad(&stream);

    return FILE_OPERATION_OK;
}
