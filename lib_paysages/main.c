#include <stdio.h>
#include <stdlib.h>

#include "IL/il.h"
#include "IL/ilu.h"

#include "shared/types.h"

#include "auto.h"
#include "color.h"
#include "system.h"
#include "camera.h"
#include "scenery.h"
#include "render.h"
#include "main.h"

#define APP_HEADER 198632.125

void paysagesInit()
{
    CameraDefinition camera;

    systemInit();
    ilInit();
    iluInit();

    sceneryInit();
    renderInit();

    camera = cameraCreateDefinition();
    cameraSetLocation(&camera, -12.0, 5.0, 2.0);
    cameraSetTarget(&camera, 0.0, 5.0, 0.0);
    cameraRotateYaw(&camera, 1.0);
    scenerySetCamera(&camera);
    cameraDeleteDefinition(&camera);

    autoGenRealisticLandscape(0);
    autoSetDaytime(8, 30);

    // DEBUG
    /*float last_height, height, x;
    last_height = height = 0.0;
    x = 0.0;
    while (height <= 1.0 || height >= last_height || last_height < 0.1)
    {
        last_height = height;
        height = terrainGetHeight(x, 0.0);
        x += 0.1;
    }
    cameraSetLocation(x - 2.0, height, 0.0);
    cameraSetTarget(x - 1.0, height, 0.0);*/
}

void paysagesQuit()
{
    sceneryQuit();
    renderQuit();
}

FileOperationResult paysagesSave(char* filepath)
{
    PackStream* stream = packWriteFile(filepath);
    float app_header, version_header;
    
    if (!stream)
    {
        return FILE_OPERATION_IOERROR;
    }
    
    app_header = (float)APP_HEADER;
    packWriteFloat(stream, &app_header);
    version_header = (float)PAYSAGES_CURRENT_DATA_VERSION;
    packWriteFloat(stream, &version_header);
    
    scenerySave(stream);
    
    packCloseStream(stream);
    
    return FILE_OPERATION_OK;
}

FileOperationResult paysagesLoad(char* filepath)
{
    PackStream* stream = packReadFile(filepath);
    float app_header, version_header;

    if (!stream)
    {
        return FILE_OPERATION_IOERROR;
    }

    packReadFloat(stream, &app_header);
    if (app_header != APP_HEADER)
    {
        packCloseStream(stream);
        return FILE_OPERATION_APP_MISMATCH;
    }
    
    packReadFloat(stream, &version_header);
    if ((int)version_header != PAYSAGES_CURRENT_DATA_VERSION)
    {
        packCloseStream(stream);
        return FILE_OPERATION_VERSION_MISMATCH;
    }

    sceneryLoad(stream);

    packCloseStream(stream);
    
    return FILE_OPERATION_OK;
}
