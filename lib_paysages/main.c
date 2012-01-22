#include <stdio.h>
#include <stdlib.h>

#include "IL/il.h"
#include "IL/ilu.h"

#include "shared/types.h"
#include "shared/constants.h"
#include "shared/functions.h"
#include "shared/globals.h"
#include "shared/system.h"

#include "terrain.h"
#include "water.h"
#include "lighting.h"
#include "textures.h"
#include "sky.h"
#include "clouds.h"

void paysagesInit()
{
    systemInit();
    ilInit();
    iluInit();

    cameraSetLocation(-12.0, 5.0, 2.0);
    cameraSetTarget(0.0, 5.0, 0.0);

    autoInit();
    skyInit();
    terrainInit();
    texturesInit();
    waterInit();
    lightingInit();
    renderInit();

    autoSetRenderQuality(5);
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

void paysagesSave(char* filepath)
{
    FILE* f = fopen(filepath, "wb");

    cameraSave(f);
    cloudsSave(f);
    fogSave(f);
    renderSave(f);
    skySave(f);
    terrainSave(f);
    texturesSave(f);
    waterSave(f);

    lightingSave(f);

    fclose(f);
}

void paysagesLoad(char* filepath)
{
    FILE* f = fopen(filepath, "rb");

    cameraLoad(f);
    cloudsLoad(f);
    fogLoad(f);
    renderLoad(f);
    skyLoad(f);
    terrainLoad(f);
    texturesLoad(f);
    waterLoad(f);

    lightingLoad(f);

    fclose(f);
}
