#include <stdio.h>
#include <stdlib.h>

#include "IL/il.h"
#include "IL/ilu.h"

#include "shared/types.h"
#include "shared/constants.h"

#include "auto.h"
#include "system.h"
#include "camera.h"
#include "scenery.h"
#include "render.h"

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
    scenerySetCamera(&camera);
    cameraDeleteDefinition(&camera);

    autoGenRealisticLandscape(0);
    autoSetDaytime(8, 30);

    // DEBUG
    /*double last_height, height, x;
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

void paysagesSave(char* filepath)
{
    scenerySaveToFile(filepath);
}

void paysagesLoad(char* filepath)
{
    sceneryLoadFromFile(filepath);
}
