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

void paysagesInit()
{
    ilInit();
    iluInit();

    cameraSetLocation(-12.0, 5.0, 2.0);
    cameraSetTarget(0.0, 5.0, 0.0);

    autoInit();

    autoSetRenderQuality(5);
    autoGenRealisticLandscape(0);
    autoSetDaytime(8, 30);
}
