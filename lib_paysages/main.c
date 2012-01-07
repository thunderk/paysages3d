#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <string.h>

#include "IL/il.h"
#include "IL/ilu.h"

#include "shared/types.h"
#include "shared/constants.h"
#include "shared/functions.h"
#include "shared/globals.h"
#include "shared/system.h"
#include "terrain.h"

void paysagesInit()
{
    systemInit();
    ilInit();
    iluInit();

    cameraSetLocation(-12.0, 5.0, 2.0);
    cameraSetTarget(0.0, 5.0, 0.0);

    autoInit();

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
