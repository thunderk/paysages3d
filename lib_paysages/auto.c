#include "auto.h"

#include <math.h>
#include <stdlib.h>
#include <time.h>

#include "render.h"
#include "scenery.h"
#include "system.h"
#include "zone.h"

void autoGenRealisticLandscape(int seed)
{
    if (!seed)
    {
        seed = time(NULL);
    }
    srand(seed);

    sceneryAutoPreset();
}
