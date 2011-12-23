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

/*static char _filename[22];

static void _setupFilename(int number)
{
    _filename[15] = (char)(97 + number / 26);
    _filename[16] = (char)(97 + number % 26);
}

static void _doRender(int number, int postonly)
{
    _setupFilename(number);
    fprintf(stderr, "Rendering %s...\n", _filename);

    autoRenderAll(postonly);

    fprintf(stderr, "Saving %s...\n", _filename);
    remove(_filename);
    renderSaveToFile(_filename);
}

static void _renderTurnTable(int count)
{
    int i;

    for (i = 0; i < count; i++)
    {
        cameraSetLocation(sin(M_PI * 2.0 * (double)i / (double)count) * 20.0, 8.0, cos(M_PI * 2.0 * (double)i / (double)count) * 20.0);

        _doRender(i, 0);
    }
}

static void _renderFly(int count, double speed)
{
    int i;
    double x, y, z, ty1;

    x = 0.0;
    y = 8.0;
    z = 0.0;
    for (i = 0; i < count; i++)
    {
        cameraSetLocation(x, y, z);
        cameraSetTarget(x, y, z + 1.0);
        _doRender(i, 0);

        ty1 = terrainGetHeight(x, z);
        if (y > ty1 + 9.0)
        {
            y -= speed;
        }
        if (y < ty1 + 7.0)
        {
            y += speed;
        }
        z += speed;
    }
}

static void _renderDayTime(int count)
{
    int i;

    for (i = 0; i < count; i++)
    {
        autoSetDaytimeFraction(0.4 + (double)i / (double)count);

        _doRender(i, i > 0 ? 1 : 0);
    }
}

static void _renderQuality()
{
    int i;

    for (i = 1; i <= 10; i++)
    {
        autoSetRenderQuality(i);
        renderSetSize(800, 600);

        _doRender(i - 1, 0);
    }
}*/

int main(int argc, char** argv)
{
    /*strcpy(_filename, "./output/result__.png");*/

    ilInit();
    iluInit();

    cameraSetLocation(-12.0, 5.0, 2.0);
    cameraSetTarget(0.0, 5.0, 0.0);

    autoInit();
    guiInit();

    autoSetRenderQuality(5);
    autoGenRealisticLandscape(0);
    autoSetDaytime(8, 30);

    guiStart();

    //_doRender(0, 0);
    //_renderTurnTable(600);
    //_renderDayTime(600);
    //_renderFly(600, 0.1);
    //_renderQuality();

    return 0;
}

