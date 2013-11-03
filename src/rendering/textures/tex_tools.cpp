#include "private.h"

#include <assert.h>
#include <math.h>

/*
 * Get the base presence factor of a layer, not accounting for other layers.
 */
double texturesGetLayerBasePresence(TexturesLayerDefinition* layer, TerrainResult terrain)
{
    return zoneGetValue(layer->terrain_zone, terrain.location, terrain.normal);
}

/*
 * Get triplanar noise value, depending on the normal direction.
 */
double texturesGetTriplanarNoise(NoiseGenerator* noise, Vector3 location, Vector3 normal)
{
    /*assert(v3Norm(normal) == 1.0);*/

    double noiseXY = noiseGet2DTotal(noise, location.x, location.y);
    double noiseXZ = noiseGet2DTotal(noise, location.x, location.z);
    double noiseYZ = noiseGet2DTotal(noise, location.y, location.z);

    double mXY = fabs(normal.z);
    double mXZ = fabs(normal.y);
    double mYZ = fabs(normal.x);
    double total = mXY + mXZ + mYZ;
    mXY /= total;
    mXZ /= total;
    mYZ /= total;

    return noiseXY * mXY + noiseXZ * mXZ + noiseYZ * mYZ;
}

double texturesGetMaximalDisplacement(TexturesDefinition* textures)
{
    int i, n;
    double disp = 0.0;
    n = layersCount(textures->layers);
    for (i = 0; i < n; i++)
    {
        TexturesLayerDefinition* layer = (TexturesLayerDefinition*)layersGetLayer(textures->layers, i);

        if (layer->displacement_height > 0.0)
        {
            disp += layer->displacement_height;
        }
    }
    return disp;
}